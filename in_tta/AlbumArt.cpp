/*
The ttaplugin-winamp project.
Copyright (C) 2005-2016 Yamagata Fumihiro

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wasabi/api/service/api_service.h>
#include <Agave/Config/api_config.h>
#include <Wasabi/api/memmgr/api_memmgr.h>
#include <Winamp/in2.h>
#include <Winamp/wa_ipc.h>
#include <nu/servicebuilder.h>
#include <Wasabi/api/service/waservicefactory.h>
#include <Agave/AlbumArt/svc_albumArtProvider.h>
#include "AlbumArt.h"
#include <taglib/trueaudio/trueaudiofile.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>
#include <taglib/tag.h>
#include <loader/loader/utils.h>

#include "MediaLibrary.h"

static const int MIME_LENGTH = 64;

class AlbumArtFactory : public waServiceFactory
{
public:
	FOURCC GetServiceType();
	const char *GetServiceName();
	GUID GetGUID();
	void *GetInterface(int global_lock);
	int ReleaseInterface(void *ifc);
	int ServiceNotify(int msg, int param1, int param2);

protected:
	RECVS_DISPATCH;
};

extern In_Module plugin; // TODO: change if you called yours something else

static AlbumArtFactory albumArtFactory;

void Wasabi_Init()
{
	plugin.service->service_register(&albumArtFactory);
}

void Wasabi_Quit()
{
	plugin.service->service_deregister(&albumArtFactory);
}

void *Wasabi_Malloc(size_t size_in_bytes)
{
	return plugin.memmgr->sysMalloc(size_in_bytes);
}

void Wasabi_Free(void *memory_block)
{
	plugin.memmgr->sysFree(memory_block);
}

class TTA_AlbumArtProvider : public svc_albumArtProvider
{
public:
	TTA_AlbumArtProvider();
	virtual ~TTA_AlbumArtProvider();
	bool IsMine(const wchar_t *filename);
	int ProviderType(void);
	// implementation note: use WASABI_API_MEMMGR to alloc bits and mimetype, so that the recipient can free through that
	int GetAlbumArtData(const wchar_t *filename, const wchar_t *type, void **bits, size_t *len, wchar_t **mimeType);
	int SetAlbumArtData(const wchar_t *filename, const wchar_t *type, void *bits, size_t len, const wchar_t *mimeType);
	int DeleteAlbumArt(const wchar_t *filename, const wchar_t *type);

protected:
	RECVS_DISPATCH;
	CRITICAL_SECTION	CriticalSection;
	std::wstring			FileName;
	bool					isSucceed;
	TagLib::ByteVector		AlbumArt;
	TagLib::String			extension;
};

TTA_AlbumArtProvider::TTA_AlbumArtProvider() : svc_albumArtProvider()
{
	::InitializeCriticalSection(&CriticalSection);
	isSucceed = false;
	FileName = L"";
	AlbumArt = TagLib::ByteVector();
	extension = TagLib::String();
}

TTA_AlbumArtProvider::~TTA_AlbumArtProvider()
{
	::DeleteCriticalSection(&CriticalSection);
	isSucceed = false;
	FileName = L"";
	AlbumArt = TagLib::ByteVector();
	extension = TagLib::String();
}

static const wchar_t *GetLastCharactercW(const wchar_t *string)
{
	if (!string || !*string)
	{
		return string;
	}

	return CharPrevW(string, string + lstrlenW(string));
}

static const wchar_t *scanstr_backW(const wchar_t *str, const wchar_t *toscan, const wchar_t *defval)
{
	const wchar_t *s = GetLastCharactercW(str);

	if (!str[0])
	{
		return defval;
	}

	if (!toscan || !toscan[0])
	{
		return defval;
	}

	while (1)
	{
		const wchar_t *t = toscan;

		while (*t)
		{
			if (*t == *s)
			{
				return s;
			}

			t = CharNextW(t);
		}

		t = CharPrevW(str, s);

		if (t == s)
		{
			return defval;
		}

		s = t;
	}
}

static const wchar_t *extensionW(const wchar_t *fn)
{
	const wchar_t *end = scanstr_backW(fn, L"./\\", 0);
	if (!end)
	{
		return (fn + lstrlenW(fn));
	}

	if (*end == L'.')
	{
		return end + 1;
	}

	return (fn + lstrlenW(fn));
}

bool TTA_AlbumArtProvider::IsMine(const wchar_t *filename)
{
	const wchar_t *extension = extensionW(filename);
	if (extension && *extension)
	{
		return (SameStr(extension, L"TTA")) ? true : false;
	}
	return false;
}

int TTA_AlbumArtProvider::ProviderType(void)
{
	return ALBUMARTPROVIDER_TYPE_EMBEDDED;
}

int TTA_AlbumArtProvider::GetAlbumArtData(const wchar_t *filename, const wchar_t *type, void **bits, size_t *len, wchar_t **mime_type)
{
	size_t tag_size = 0;
	int retval = ALBUMARTPROVIDER_FAILURE;
	size_t string_len = 0;
	TagLib::String mimeType;

	::EnterCriticalSection(&CriticalSection);

	if (!SameStr(type, L"cover"))
	{
		::LeaveCriticalSection(&CriticalSection);
		return retval;
	}

	if (!bits || !len || !mime_type)
	{
		::LeaveCriticalSection(&CriticalSection);
		return retval;
	}

	if (!isSucceed || !SameStr(FileName.c_str(), filename))
	{
		FileName = filename;

		TagLib::TrueAudio::File TagFile(FileName.c_str());

		if (!TagFile.isValid())
		{
			isSucceed = false;
			::LeaveCriticalSection(&CriticalSection);
			return retval;
		}
		else
		{
			isSucceed = true;
		}

		// read Album Art
		AlbumArt =
			TagFile.ID3v2Tag()->albumArt(TagLib::ID3v2::AttachedPictureFrame::FrontCover, mimeType);
		extension = mimeType.substr(mimeType.find("/") + 1);
	}

	if (!AlbumArt.isEmpty())
	{
		*len = AlbumArt.size();
		*bits = (char *)Wasabi_Malloc(*len);
		if (NULL == *bits)
		{
			::LeaveCriticalSection(&CriticalSection);
			return retval;
		}

		errno_t err = memcpy_s(*bits, AlbumArt.size(), AlbumArt.data(), AlbumArt.size());
		if (err)
		{
			::LeaveCriticalSection(&CriticalSection);
			return retval;
		}

		*mime_type = (wchar_t *)Wasabi_Malloc(extension.size() * 2 + 2);
		if (NULL == *mime_type)
		{
			if (NULL != *bits)
			{
				Wasabi_Free(*bits);
			}
			::LeaveCriticalSection(&CriticalSection);
			return retval;
		}
		else
		{
			mbstowcs_s(&string_len, *mime_type, extension.size() + 1, extension.toCString(), _TRUNCATE);
			retval = ALBUMARTPROVIDER_SUCCESS;
		}

		if (retval)
		{
			if (NULL != *bits)
			{
				Wasabi_Free(*bits);
			}

			if (NULL != *mime_type)
			{
				Wasabi_Free(*mime_type);
			}
		}
	}

	::LeaveCriticalSection(&CriticalSection);
	return retval;
}

int TTA_AlbumArtProvider::SetAlbumArtData(const wchar_t *filename, const wchar_t *type, void *bits, size_t len, const wchar_t *mime_type)
{
	int retval = ALBUMARTPROVIDER_FAILURE;
	TagLib::String mimeType(L"");
	int size = 0;
	TagLib::ID3v2::AttachedPictureFrame::Type artType = TagLib::ID3v2::AttachedPictureFrame::Other;
	size_t string_len = 0;

	::EnterCriticalSection(&CriticalSection);

	if (std::wstring(filename) == L"")
	{
		::LeaveCriticalSection(&CriticalSection);
		return retval;
	}

	size_t convertedChars = 0;

	TagLib::ByteVector AlbumArt;

	if (!bits)
	{
		//delete AlbumArt
		AlbumArt.setData(NULL, 0);

	}
	else if (len == 0 || wcscmp(mime_type, L"") == 0)
	{
		::LeaveCriticalSection(&CriticalSection);
		return retval;
	}
	else
	{
		mimeType = L"image/";
		mimeType += mime_type;
		size = (int)len;
		artType = TagLib::ID3v2::AttachedPictureFrame::FrontCover;
		AlbumArt.setData((const char *)bits, (TagLib::uint)size);
	}

	TagLib::TrueAudio::File TTAFile(filename);

	if (TTAFile.isValid())
	{
		TTAFile.ID3v2Tag()->setAlbumArt(AlbumArt, artType, mimeType);
		TTAFile.save();
		isSucceed = false;
		retval = ALBUMARTPROVIDER_SUCCESS;
	}

	::LeaveCriticalSection(&CriticalSection);

	return retval;
}

int TTA_AlbumArtProvider::DeleteAlbumArt(const wchar_t *filename, const wchar_t *type)
{
	return SetAlbumArtData(filename, type, NULL, 0, L"jpeg");
}

#define CBCLASS TTA_AlbumArtProvider
START_DISPATCH;
CB(SVC_ALBUMARTPROVIDER_PROVIDERTYPE, ProviderType);
CB(SVC_ALBUMARTPROVIDER_GETALBUMARTDATA, GetAlbumArtData);
CB(SVC_ALBUMARTPROVIDER_ISMINE, IsMine);
CB(SVC_ALBUMARTPROVIDER_SETALBUMARTDATA, SetAlbumArtData);
CB(SVC_ALBUMARTPROVIDER_DELETEALBUMART, DeleteAlbumArt);
END_DISPATCH;
#undef CBCLASS

TTA_AlbumArtProvider *albumArtProvider = NULL;

// {bb653840-6dab-4867-9f42-A772E4068C81}
static const GUID TTA_albumArtproviderGUID =
{ 0xbb653840, 0x6dab, 0x4867, { 0x9f, 0x42, 0xa7, 0x72, 0xe4, 0x05, 0x8c, 0x81 } };

FOURCC AlbumArtFactory::GetServiceType()
{
	return svc_albumArtProvider::SERVICETYPE;
}

const char *AlbumArtFactory::GetServiceName()
{
	return "TTA Album Art Provider";
}

GUID AlbumArtFactory::GetGUID()
{
	return TTA_albumArtproviderGUID;
}

void *AlbumArtFactory::GetInterface(int global_lock)
{
	if (!albumArtProvider)
	{
		albumArtProvider = new TTA_AlbumArtProvider();
	}
	return albumArtProvider;
}

int AlbumArtFactory::ReleaseInterface(void *ifc)
{
	return 1;
}

int AlbumArtFactory::ServiceNotify(int msg, int param1, int param2)
{
	return 1;
}

#define CBCLASS AlbumArtFactory
START_DISPATCH;
CB(WASERVICEFACTORY_GETSERVICETYPE, GetServiceType)
CB(WASERVICEFACTORY_GETSERVICENAME, GetServiceName)
CB(WASERVICEFACTORY_GETGUID, GetGUID)
CB(WASERVICEFACTORY_GETINTERFACE, GetInterface)
CB(WASERVICEFACTORY_RELEASEINTERFACE, ReleaseInterface)
CB(WASERVICEFACTORY_SERVICENOTIFY, ServiceNotify)
END_DISPATCH;
#undef CBCLASS
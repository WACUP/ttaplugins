/*
The ttaplugins-winamp project.
Copyright (C) 2005-2015 Yamagata Fumihiro

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

#if !defined(AFX_MediaLibrary_H__997DC726_50DB_46B4_A156_DB5E92EC2BE8__INCLUDED_)
#define AFX_MediaLibrary_H__997DC726_50DB_46B4_A156_DB5E92EC2BE8__INCLUDED_

#include <winamp\wa_ipc.h>

#include <taglib/toolkit/tstring.h>
#include <taglib/trueaudio/trueaudiofile.h>
#include <taglib/mpeg/id3v2/frames/attachedpictureframe.h>

static const __int32 MAX_MUSICTEXT = 512;
static const __int32 MAX_YEAR = 10;

struct TagInfo
{
	unsigned long	Length;
	std::wstring	Format;
	std::wstring	Title;
	std::wstring	Artist;
	std::wstring	AlbumArtist;
	std::wstring	Comment;
	std::wstring	Album;
	std::wstring	Year;
	std::wstring	Genre;
	std::wstring	Track;
	std::wstring	Composer;
	std::wstring	Publisher;
	std::wstring	Disc;
	std::wstring	BPM;
	std::wstring    Bitrate;
	std::wstring    Samplerate;
};

class CMediaLibrary
{
public:
	CMediaLibrary();
	virtual ~CMediaLibrary();
	__int32  GetExtendedFileInfo(const wchar_t *fn, const char *Metadata, wchar_t *dest, const size_t destlen);
	__int32  SetExtendedFileInfo(const wchar_t *fn, const char *Metadata, const wchar_t *val);
	__int32  WriteExtendedFileInfo();
	void FlushCache(const bool skipLock = false);
	std::wstring GetCurrentFileName() { return FileName; };
	bool	isValid() { return isValidFile; };

private:

	CRITICAL_SECTION	CriticalSection;
	TagInfo				TagDataW;
	DWORD				GetTagTime;
	std::wstring		FileName;
	bool				isValidFile;

	bool GetTagInfo(const std::wstring fn);

};


#endif // !defined(AFX_MediaLibrary_H__997DC726_50DB_46B4_A156_DB5E92EC2BE8__INCLUDED_)

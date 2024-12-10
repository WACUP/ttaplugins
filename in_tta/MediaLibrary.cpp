/*
The ttaplugin-winamp project.
Copyright (C) 2005-2011 Yamagata Fumihiro

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

// MediaLibrary.cpp: Implementation of CMediaLibrary class
//
//////////////////////////////////////////////////////////////////////
#include "MediaLibrary.h"
#include "resource.h"
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/id3v1/id3v1tag.h>
#include <sstream>
#include <iomanip>
#include <strsafe.h>
#include <loader/loader/utils.h>

//////////////////////////////////////////////////////////////////////
// Create / Destroy
//////////////////////////////////////////////////////////////////////

CMediaLibrary::CMediaLibrary()
{
	::InitializeCriticalSection(&CriticalSection);

	FlushCache();
}

CMediaLibrary::~CMediaLibrary()
{
	FlushCache();

	::DeleteCriticalSection(&CriticalSection);

}

void CMediaLibrary::FlushCache(const bool skipLock)
{
	if (!skipLock)
	{
		::EnterCriticalSection(&CriticalSection);
	}

	GetTagTime = 0;

	TagDataW.Length = 0;
	TagDataW.Format = L"";
	TagDataW.Title = L"";
	TagDataW.Artist = L"";
	TagDataW.Comment = L"";
	TagDataW.Album = L"";
	TagDataW.AlbumArtist = L"";
	TagDataW.Year = L"";
	TagDataW.Genre = L"";
	TagDataW.Track = L"";
	TagDataW.Composer = L"";
	TagDataW.Publisher = L"";
	TagDataW.Disc = L"";
	TagDataW.BPM = L"";
	TagDataW.BitDepth = L"";

	FileName = L"";

	isValidFile = false;

	if (!skipLock)
	{
		::LeaveCriticalSection(&CriticalSection);
	}
}

bool CMediaLibrary::GetTagInfo(const std::wstring fn)
{
	if (FileName != fn)
	{
		// just make sure that we've got things
		// in a known state to avoid quirks...
		FlushCache(true);

		TagLib::TrueAudio::File TTAFile(fn.c_str());

		if (!TTAFile.isValid())
		{
			return false;
		}
		else
		{
			isValidFile = true;
		}
		TagDataW.Length = (unsigned long)(TTAFile.audioProperties()->lengthInMilliseconds());

		int Lengthbysec = ((TagDataW.Length > 0) ? (TagDataW.Length / 1000) : 0);
		int hour = Lengthbysec / 3600;
		int min = Lengthbysec / 60;
		int sec = Lengthbysec % 60;

		std::wstringstream second;
		if (hour > 0)
		{
			second << std::setw(2) << std::setfill(L'0') << hour << L":" << std::setw(2)
				<< std::setfill(L'0') << min << L":" << std::setw(2) << std::setfill(L'0') << sec;
		}
		else if (min > 0)
		{
			second << std::setw(2) << std::setfill(L'0') << min << L":" << std::setw(2)
				<< std::setfill(L'0') << sec;
		}
		else
		{
			second << std::setw(2) << std::setfill(L'0') << sec;
		}

		TagDataW.Bitrate = std::to_wstring((long long)TTAFile.audioProperties()->bitrate());
		TagDataW.Samplerate = std::to_wstring((long long)TTAFile.audioProperties()->sampleRate());
		TagDataW.BitDepth = std::to_wstring((long long)TTAFile.audioProperties()->bitsPerSample());

		std::wstring channel_designation = (TTAFile.audioProperties()->channels() == 2) ? L"Stereo" : L"Monoral";

		std::wstringstream ttainfo_temp;
		ttainfo_temp << L"Format\t\t: TTA" << TTAFile.audioProperties()->ttaVersion()
			<< L"\nSample\t\t: " << TagDataW.BitDepth
			<< L"-bit\nSample Rate\t: " << TagDataW.Samplerate
			<< L" Hz\nBit Rate\t\t: " << TagDataW.Bitrate
			<< L" kbit/s\n# of Channels\t: " << TTAFile.audioProperties()->channels()
			<< L" (" << channel_designation
			<< L")\nLength\t\t: " << second.str();
		TagDataW.Format = ttainfo_temp.str();

		std::wstring temp;
		if (NULL != TTAFile.ID3v2Tag())
		{
			temp = TTAFile.ID3v2Tag()->title().toCWString();
			TagDataW.Title = temp;
			temp = TTAFile.ID3v2Tag()->artist().toCWString();
			TagDataW.Artist = temp;
			temp = TTAFile.ID3v2Tag()->albumArtist().toCWString();
			TagDataW.AlbumArtist = temp;
			temp = TTAFile.ID3v2Tag()->comment().toCWString();
			TagDataW.Comment = temp;
			temp = TTAFile.ID3v2Tag()->album().toCWString();
			TagDataW.Album = temp;
			temp = TTAFile.ID3v2Tag()->stringYear().toCWString();
			TagDataW.Year = temp;
			temp = TTAFile.ID3v2Tag()->genre().toCWString();
			TagDataW.Genre = temp;
			temp = TTAFile.ID3v2Tag()->stringTrack().toCWString();
			TagDataW.Track = temp;
			temp = TTAFile.ID3v2Tag()->composers().toCWString();
			TagDataW.Composer = temp;
			temp = TTAFile.ID3v2Tag()->publisher().toCWString();
			TagDataW.Publisher = temp;
			temp = TTAFile.ID3v2Tag()->disc().toCWString();
			TagDataW.Disc = temp;
			temp = TTAFile.ID3v2Tag()->BPM().toCWString();
			TagDataW.BPM = temp;
		}
		else if (NULL != TTAFile.ID3v1Tag())
		{
			std::wstringstream temp_year;
			std::wstringstream temp_track;
			temp = TTAFile.ID3v1Tag()->title().toCWString();
			TagDataW.Title = temp;
			temp = TTAFile.ID3v1Tag()->artist().toCWString();
			TagDataW.Artist = temp;
			temp = TTAFile.ID3v1Tag()->comment().toCWString();
			TagDataW.Comment = temp;
			temp = TTAFile.ID3v1Tag()->album().toCWString();
			TagDataW.Album = temp;
			temp_year << TTAFile.ID3v1Tag()->year();
			TagDataW.Year = temp_year.str();
			temp = TTAFile.ID3v1Tag()->genre().toCWString();
			TagDataW.Genre = temp;
			temp_track << TTAFile.ID3v1Tag()->track();
			TagDataW.Track = temp_track.str();

		}
	}

	return true;
}

int CMediaLibrary::GetExtendedFileInfo(const wchar_t *fn, const char *Metadata, wchar_t *dest, const size_t destlen)
{
	bool FindTag;
	int RetCode;

	if (_stricmp(Metadata, "type") == 0 ||
		_stricmp(Metadata, "streammetadata") == 0)
	{
		dest[0] = L'0';
		dest[1] = L'\0';
		return 1;
	}
	else if (_stricmp(Metadata, "family") == 0)
	{
		// TODO localise
		wcsncpy_s(dest, destlen, L"The True Audio File", _TRUNCATE);
		return 1;
	}
	else if (_stricmp(Metadata, "lossless") == 0)
	{
		dest[0] = L'1';
		dest[1] = L'\0';
		return 1;
	}
	else if (_stricmp(Metadata, "streamgenre") == 0 ||
			 _stricmp(Metadata, "streamtype") == 0 ||
			 _stricmp(Metadata, "streamurl") == 0 ||
			 _stricmp(Metadata, "streamname") == 0)
	{
		return 0;
	}
	else if (_stricmp(Metadata, "reset") == 0)
	{
		// this might sometimes mess up so we'll see if what's
		// being requested is a reset & if it is then we'll do
		// a check to see if something else has the lock to do
		// a quick bail to try to avoid a hang related failure
		if (!::TryEnterCriticalSection(&CriticalSection))
		{
			return 0;
		}

		FileName = L"";
		FlushCache(true);

		::LeaveCriticalSection(&CriticalSection);
		return 0;
	}

	::EnterCriticalSection(&CriticalSection);

	if (std::wstring(fn) != FileName)
	{
		FindTag = GetTagInfo(fn);
		if (FindTag)
		{
			FileName = std::wstring(fn);
		}
	}
	else
	{
		FindTag = true;
	}

	if (FindTag) {
		RetCode = 1;

		if (_stricmp(Metadata, "length") == 0)
		{
			I2WStr(TagDataW.Length, dest, destlen);
		}
		else if (_stricmp(Metadata, "formatinformation") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Format.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "title") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Title.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "artist") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Artist.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "albumartist") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.AlbumArtist.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "comment") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Comment.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "album") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Album.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "year") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Year.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "genre") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Genre.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "track") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Track.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "tracks") == 0)
		{
			size_t slash_pos = TagDataW.Track.find_first_of(L'/');
			if (slash_pos != std::wstring::npos)
			{
				wcsncpy_s(dest, destlen, TagDataW.Track.substr(slash_pos + 1).c_str(), _TRUNCATE);
			}
		}
		else if (_stricmp(Metadata, "composer") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Composer.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "publisher") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Publisher.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "disc") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Disc.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "discs") == 0)
		{
			size_t slash_pos = TagDataW.Disc.find_first_of(L'/');
			if (slash_pos != std::wstring::npos)
			{
				wcsncpy_s(dest, destlen, TagDataW.Disc.substr(slash_pos + 1).c_str(), _TRUNCATE);
			}
		}
		else if (_stricmp(Metadata, "bpm") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.BPM.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "bitrate") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Bitrate.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "samplerate") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.Samplerate.c_str(), _TRUNCATE);
		}
		else if (_stricmp(Metadata, "bitdepth") == 0)
		{
			wcsncpy_s(dest, destlen, TagDataW.BitDepth.c_str(), _TRUNCATE);
		}
		else
		{
			RetCode = 0;
		}

	}
	else {
		FileName = L"";
		RetCode = 0;
	}

	::LeaveCriticalSection(&CriticalSection);
	return RetCode;
}

int CMediaLibrary::SetExtendedFileInfo(const wchar_t *fn, const char *Metadata, const wchar_t *val)
{
	bool FindTag = false;
	int RetCode = 0;

	::EnterCriticalSection(&CriticalSection);

	if (std::wstring(fn) != FileName)
	{
		FindTag = GetTagInfo(fn);
		FileName = std::wstring(fn);
	}
	else
	{
		FindTag = true;
	}

	if (FindTag)
	{
		const char *MetaData = reinterpret_cast<const char*>(Metadata);

		if (_stricmp(MetaData, "title") == 0)
		{
			TagDataW.Title = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "artist") == 0)
		{
			TagDataW.Artist = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "albumartist") == 0)
		{
			TagDataW.AlbumArtist = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "comment") == 0)
		{
			TagDataW.Comment = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "album") == 0)
		{
			TagDataW.Album = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "year") == 0)
		{
			TagDataW.Year = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "genre") == 0)
		{
			TagDataW.Genre = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "track") == 0)
		{
			TagDataW.Track = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "composer") == 0)
		{
			TagDataW.Composer = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "publisher") == 0)
		{
			TagDataW.Publisher = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "disc") == 0)
		{
			TagDataW.Disc = val;
			RetCode = 1;
		}
		else if (_stricmp(MetaData, "bpm") == 0)
		{
			TagDataW.BPM = val;
			RetCode = 1;
		}
		else
		{
			RetCode = 0;
		}

	}
	else
	{
		RetCode = 0;
	}

	::LeaveCriticalSection(&CriticalSection);
	return RetCode;
}

int CMediaLibrary::WriteExtendedFileInfo()
{
	::EnterCriticalSection(&CriticalSection);

	if (FileName.empty())
	{
		::LeaveCriticalSection(&CriticalSection);
		return 0;
	}
	else
	{
		TagLib::TrueAudio::File TTAFile(FileName.c_str());

		if (!TTAFile.isValid())
		{
			::LeaveCriticalSection(&CriticalSection);
			return 0;
		}

		if (NULL != TTAFile.ID3v2Tag(true))
		{
			TagLib::String temp;
			temp = TagLib::String(TagDataW.Title);
			TTAFile.ID3v2Tag()->setTitle(temp);
			temp = TagLib::String(TagDataW.Artist);
			TTAFile.ID3v2Tag()->setArtist(temp);
			temp = TagLib::String(TagDataW.AlbumArtist);
			TTAFile.ID3v2Tag()->setAlbumArtist(temp);
			temp = TagLib::String(TagDataW.Comment);
			TTAFile.ID3v2Tag()->setComment(temp);
			temp = TagLib::String(TagDataW.Album);
			TTAFile.ID3v2Tag()->setAlbum(temp);
			temp = TagLib::String(TagDataW.Year);
			TTAFile.ID3v2Tag()->setStringYear(temp);
			temp = TagLib::String(TagDataW.Genre);
			TTAFile.ID3v2Tag()->setGenre(temp);
			temp = TagLib::String(TagDataW.Track);
			TTAFile.ID3v2Tag()->setStringTrack(temp);
			temp = TagLib::String(TagDataW.Composer);
			TTAFile.ID3v2Tag()->setComposers(temp);
			temp = TagLib::String(TagDataW.Publisher);
			TTAFile.ID3v2Tag()->setPublisher(temp);
			temp = TagLib::String(TagDataW.Disc);
			TTAFile.ID3v2Tag()->setDisc(temp);
			temp = TagLib::String(TagDataW.BPM);
			TTAFile.ID3v2Tag()->setBPM(temp);

		}
		else if (NULL != TTAFile.ID3v1Tag(true))
		{
			TTAFile.ID3v1Tag()->setTitle(TagDataW.Title);
			TTAFile.ID3v1Tag()->setArtist(TagDataW.Artist);
			TTAFile.ID3v1Tag()->setAlbum(TagDataW.Album);
			TTAFile.ID3v1Tag()->setComment(TagDataW.Comment);
			TTAFile.ID3v1Tag()->setYear(WStr2I(TagDataW.Year.c_str()));
			TTAFile.ID3v1Tag()->setTrack(WStr2I(TagDataW.Track.c_str()));
			TTAFile.ID3v1Tag()->setGenre(TagDataW.Genre);
		}

		TTAFile.save();
	}

	::LeaveCriticalSection(&CriticalSection);

	return 1;
}

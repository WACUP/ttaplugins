/*
The ttaplugin-winamp project.
Copyright (C) 2005-2025 Yamagata Fumihiro

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
		auto id3v2tag = TTAFile.ID3v2Tag();
		if (NULL != id3v2tag)
		{
			temp = id3v2tag->title().toCWString();
			TagDataW.Title = temp;
			temp = id3v2tag->artist().toCWString();
			TagDataW.Artist = temp;
			temp = id3v2tag->getTextFrame("TPE2").toCWString();
			TagDataW.AlbumArtist = temp;
			temp = id3v2tag->comment().toCWString();
			TagDataW.Comment = temp;
			temp = id3v2tag->album().toCWString();
			TagDataW.Album = temp;
			temp = id3v2tag->getTextFrame("TDRC").toCWString();
			TagDataW.Year = temp;
			temp = id3v2tag->genre().toCWString();
			TagDataW.Genre = temp;
			temp = id3v2tag->getTextFrame("TRCK").toCWString();
			TagDataW.Track = temp;
			temp = id3v2tag->getTextFrame("TCOM").toCWString();
			TagDataW.Composer = temp;
			temp = id3v2tag->getTextFrame("TPUB").toCWString();
			TagDataW.Publisher = temp;
			temp = id3v2tag->getTextFrame("TPOS").toCWString();
			TagDataW.Disc = temp;
			temp = id3v2tag->getTextFrame("TBPM").toCWString();
			TagDataW.BPM = temp;
		}
		else
		{
			auto id3v1tag = TTAFile.ID3v1Tag();
			if (NULL != id3v1tag)
			{
				std::wstringstream temp_year;
				std::wstringstream temp_track;
				temp = id3v1tag->title().toCWString();
				TagDataW.Title = temp;
				temp = id3v1tag->artist().toCWString();
				TagDataW.Artist = temp;
				temp = id3v1tag->comment().toCWString();
				TagDataW.Comment = temp;
				temp = id3v1tag->album().toCWString();
				TagDataW.Album = temp;
				temp_year << id3v1tag->year();
				TagDataW.Year = temp_year.str();
				temp = id3v1tag->genre().toCWString();
				TagDataW.Genre = temp;
				temp_track << id3v1tag->track();
				TagDataW.Track = temp_track.str();
			}
		}
	}

	return true;
}

int CMediaLibrary::GetExtendedFileInfo(const wchar_t *fn, const char *Metadata, wchar_t *dest, const size_t destlen)
{
	if (SameStrA(Metadata, "type") ||
		SameStrA(Metadata, "streammetadata"))
	{
		dest[0] = L'0';
		dest[1] = L'\0';
		return 1;
	}
	else if (SameStrA(Metadata, "family"))
	{
		// TODO localise
		return (int)CopyCchStrEx(dest, destlen, L"The True Audio File");
	}
	else if (SameStrA(Metadata, "lossless"))
	{
		dest[0] = L'1';
		dest[1] = L'\0';
		return 1;
	}
    else if (SameStrNA(Metadata, "stream", 6) &&
             (SameStrA((Metadata + 6), "type") ||
              SameStrA((Metadata + 6), "genre") ||
              SameStrA((Metadata + 6), "url") ||
              SameStrA((Metadata + 6), "name") ||
			  SameStrA((Metadata + 6), "title")))
	{
		return 0;
	}
	else if (SameStrA(Metadata, "reset"))
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

	bool FindTag;
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

	int RetCode = 0;
	if (FindTag)
	{
		const bool length_seconds = SameStrA(Metadata, "length_seconds");
		if (length_seconds || SameStrA(Metadata, "length"))
		{
			I2WStrLen((!length_seconds ? TagDataW.Length : (TagDataW.Length / 1000)), dest, destlen, &RetCode);
		}
		else if (SameStrA(Metadata, "formatinformation"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Format.c_str());
		}
		else if (SameStrA(Metadata, "title"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Title.c_str());
		}
		else if (SameStrA(Metadata, "artist"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Artist.c_str());
		}
		else if (SameStrA(Metadata, "albumartist"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.AlbumArtist.c_str());
		}
		else if (SameStrA(Metadata, "comment"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Comment.c_str());
		}
		else if (SameStrA(Metadata, "album"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Album.c_str());
		}
		else if (SameStrA(Metadata, "year"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Year.c_str());
		}
		else if (SameStrA(Metadata, "genre"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Genre.c_str());
		}
		else if (SameStrA(Metadata, "track"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Track.c_str());
		}
		else if (SameStrA(Metadata, "tracks"))
		{
			const size_t slash_pos = TagDataW.Track.find_first_of(L'/');
			if (slash_pos != std::wstring::npos)
			{
				RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Track.substr(slash_pos + 1).c_str());
			}
		}
		else if (SameStrA(Metadata, "composer"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Composer.c_str());
		}
		else if (SameStrA(Metadata, "publisher"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Publisher.c_str());
		}
		else if (SameStrA(Metadata, "disc"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Disc.c_str());
		}
		else if (SameStrA(Metadata, "discs"))
		{
			const size_t slash_pos = TagDataW.Disc.find_first_of(L'/');
			if (slash_pos != std::wstring::npos)
			{
				RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Disc.substr(slash_pos + 1).c_str());
			}
		}
		else if (SameStrA(Metadata, "bpm"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.BPM.c_str());
		}
		else if (SameStrA(Metadata, "bitrate"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Bitrate.c_str());
		}
		else if (SameStrA(Metadata, "samplerate"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.Samplerate.c_str());
		}
		else if (SameStrA(Metadata, "bitdepth"))
		{
			RetCode = (int)CopyCchStrEx(dest, destlen, TagDataW.BitDepth.c_str());
		}
		else
		{
			RetCode = 0;
		}
	}
	else
	{
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

		if (SameStrA(MetaData, "title"))
		{
			TagDataW.Title = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "artist"))
		{
			TagDataW.Artist = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "albumartist"))
		{
			TagDataW.AlbumArtist = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "comment"))
		{
			TagDataW.Comment = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "album"))
		{
			TagDataW.Album = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "year"))
		{
			TagDataW.Year = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "genre"))
		{
			TagDataW.Genre = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "track"))
		{
			TagDataW.Track = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "composer"))
		{
			TagDataW.Composer = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "publisher"))
		{
			TagDataW.Publisher = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "disc"))
		{
			TagDataW.Disc = val;
			RetCode = 1;
		}
		else if (SameStrA(MetaData, "bpm"))
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

		auto id3v2tag = TTAFile.ID3v2Tag(true);
		if (NULL != id3v2tag)
		{
			TagLib::String temp;
			temp = TagLib::String(TagDataW.Title);
			id3v2tag->setTitle(temp);
			temp = TagLib::String(TagDataW.Artist);
			id3v2tag->setArtist(temp);
			temp = TagLib::String(TagDataW.AlbumArtist);
			id3v2tag->setTextFrame("TPE2", temp);
			temp = TagLib::String(TagDataW.Comment);
			id3v2tag->setComment(temp);
			temp = TagLib::String(TagDataW.Album);
			id3v2tag->setAlbum(temp);
			temp = TagLib::String(TagDataW.Year);
			id3v2tag->setTextFrame("TDRC", temp);
			temp = TagLib::String(TagDataW.Genre);
			id3v2tag->setGenre(temp);
			temp = TagLib::String(TagDataW.Track);
			id3v2tag->setTextFrame("TRCK", temp);
			temp = TagLib::String(TagDataW.Composer);
			id3v2tag->setTextFrame("TCOM", temp);
			temp = TagLib::String(TagDataW.Publisher);
			id3v2tag->setTextFrame("TPUB", temp);
			temp = TagLib::String(TagDataW.Disc);
			id3v2tag->setTextFrame("TPOS", temp);
			temp = TagLib::String(TagDataW.BPM);
			id3v2tag->setTextFrame("TBPM", temp);
		}
		else
		{
			auto id3v1tag = TTAFile.ID3v1Tag(true);
			if (NULL != id3v1tag)
			{
				id3v1tag->setTitle(TagDataW.Title);
				id3v1tag->setArtist(TagDataW.Artist);
				id3v1tag->setAlbum(TagDataW.Album);
				id3v1tag->setComment(TagDataW.Comment);
				id3v1tag->setYear(WStr2I(TagDataW.Year.c_str()));
				id3v1tag->setTrack(WStr2I(TagDataW.Track.c_str()));
				id3v1tag->setGenre(TagDataW.Genre);
			}
		}

		TTAFile.save();
	}

	::LeaveCriticalSection(&CriticalSection);

	return 1;
}

// in_tta.cpp : Defines the initialization routines for the DLL.
//
/* Description:	 TTA input plug-in for upper Winamp 2.91
 *               with MediaLibrary Extension version
 * Developed by: Alexander Djourik <ald@true-audio.com>
 *               Pavel Zhilin <pzh@true-audio.com>
 *               (MediaLibrary Extension Yamagata Fumihiro <bunbun042000@gmail.com> )
 *
 * Copyright (c) 2005 Alexander Djourik. All rights reserved.
 *
 */

 /*
 The ttaplugin-winamp project.
 Copyright (C) 2005-2017 Yamagata Fumihiro

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

#include <Shlwapi.h>
#include <type_traits>
#include <strsafe.h>

#include "AlbumArt.h"
#include "MediaLibrary.h"
#include "in_tta.h"

#include <Winamp/in2.h>
#include <Agave/Language/api_language.h>

#include <taglib/tag.h>
#include <taglib/trueaudiofile.h>
#include <taglib/tstring.h>

#include "DecodeFile.h"
#include "..\common\VersionNo.h"
#include "resource.h"

const static int MAX_MESSAGE_LENGTH = 1024;
const static __int32 PLAYING_BUFFER_LENGTH = 576;
const static __int32 TRANSCODING_BUFFER_LENGTH = 5120;

// for playing static variables
static __declspec(align(16)) CDecodeFile playing_ttafile;

static HANDLE decoder_handle = INVALID_HANDLE_VALUE;
static DWORD WINAPI __stdcall DecoderThread(void *p);
static volatile int killDecoderThread = 0;

// for transcoding static variable
static __declspec(align(16)) CDecodeFile transcode_ttafile;

// for MetaData static variables
CMediaLibrary m_ReadTag;
CMediaLibrary m_WriteTag;

void config(HWND hwndParent);
void about(HWND hwndParent);
void init();
void quit();
void getfileinfo(const wchar_t *file, wchar_t *title, int *length_in_ms);
int  infodlg(const wchar_t *file, HWND hwndParent);
int  isourfile(const wchar_t *fn);
int  play(const wchar_t *fn);
void pause();
void unpause();
int  ispaused();
void stop();
int  getlength();
int  getoutputtime();
void setoutputtime(int time_in_ms);
void setvolume(int volume);
void setpan(int pan);
void eq_set(int on, char data[10], int preamp);

In_Module mod = {
	IN_VER,
	"TTA Audio Decoder " PLUGIN_VERSION_CHAR,
	NULL,		// hMainWindow
	NULL,		// hDllInstance
	"TTA\0TTA Audio File (*.TTA)\0",
	1,			// is_seekable
	1,			// uses output
	config,
	about,
	init,
	quit,
	getfileinfo,
	infodlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	getlength,
	getoutputtime,
	setoutputtime,
	setvolume,
	setpan,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // vis stuff
	NULL, NULL,	// dsp
	eq_set,
	NULL,		// setinfo
	NULL		// out_mod
};

static void tta_error_message(int error, const wchar_t *filename)
{
	wchar_t message[MAX_MESSAGE_LENGTH];

	std::wstring name(filename);

	switch (error)
	{
	case TTA_OPEN_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Can't open file:\n%ls", name.c_str());
		break;
	case TTA_FORMAT_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Unknown TTA format version:\n%ls", name.c_str());
		break;
	case TTA_NOT_SUPPORTED:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Not supported file format:\n%ls", name.c_str());
		break;
	case TTA_FILE_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"File is corrupted:\n%ls", name.c_str());
		break;
	case TTA_READ_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Can't read from file:\n%ls", name.c_str());
		break;
	case TTA_WRITE_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Can't write to file:\n%ls", name.c_str());
		break;
	case TTA_MEMORY_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Insufficient memory available");
		break;
	case TTA_SEEK_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"file seek error");
		break;
	case TTA_PASSWORD_ERROR:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"password protected file");
		break;
	default:
		StringCbPrintf(message, MAX_MESSAGE_LENGTH, L"Unknown TTA decoder error");
		break;
	}

	MessageBox(mod.hMainWindow, message, L"TTA Decoder Error",
		MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

}

static BOOL CALLBACK about_dialog(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(dialog, IDC_PLUGIN_VERSION,
			IN_TTA_PLUGIN_VERSION_CREADIT);
		SetDlgItemText(dialog, IDC_PLUGIN_CREADIT,
			IN_TTA_PLUGIN_COPYRIGHT_CREADIT);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(dialog, wparam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void config(HWND hwndParent)
{
	DialogBox(mod.hDllInstance, MAKEINTRESOURCE(IDD_ABOUT),
		hwndParent, about_dialog);
}

void about(HWND hwndParent)
{
	DialogBox(mod.hDllInstance, MAKEINTRESOURCE(IDD_ABOUT),
		hwndParent, about_dialog);
}


void init()
{
	Wasabi_Init();
}

void quit()
{
	Wasabi_Quit();
}


void getfileinfo(const wchar_t *file, wchar_t *title, int *length_in_ms)
{

	title = L"";

	if (!file || !*file)
	{
		// invalid filename may be playing file
		if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
		{
			*length_in_ms = playing_ttafile.GetLengthbymsec();
		}
		else
		{
			*length_in_ms = 0;
		}
	}
	else
	{
		TagLib::FileName fn(file);
		TagLib::TrueAudio::File f(fn);
		if (f.isValid() == true)
		{
			*length_in_ms = f.audioProperties()->lengthInMilliseconds();
		}
		else
		{
			// cannot get fileinfo
			*length_in_ms = 0;
		}
	}
}

int infodlg(const wchar_t *filename, HWND parent)
{
	return 0;
}

int isourfile(const wchar_t *filename)
{
	return 0;
}

int play(const wchar_t *filename)
{
	int maxlatency;
	unsigned long decoder_thread_id;
	int return_number;

	if (!playing_ttafile.isValid())
	{
		return 1;
	}
	else
	{
		// Do nothing
	}

	try
	{
		return_number = playing_ttafile.SetFileName(filename);
	}

	catch (CDecodeFile_exception &ex)
	{
		tta_error_message(ex.code(), filename);
		return -1;
	}

	maxlatency = mod.outMod->Open(playing_ttafile.GetSampleRate(),
		playing_ttafile.GetNumberofChannel(), playing_ttafile.GetOutputBPS(), -1, -1);
	if (maxlatency < 0)
	{
		stop();
		return 1;
	}
	else
	{
		// Do nothing
	}

	// setup information display
	mod.SetInfo(playing_ttafile.GetBitrate(), playing_ttafile.GetSampleRate() / 1000, playing_ttafile.GetNumberofChannel(), 1);

	// initialize vis stuff
	mod.SAVSAInit(maxlatency, playing_ttafile.GetSampleRate());
	mod.VSASetInfo(playing_ttafile.GetNumberofChannel(), playing_ttafile.GetSampleRate());

	// set the output plug-ins default volume
	mod.outMod->SetVolume(-666);

	killDecoderThread = 0;

	decoder_handle = CreateThread(NULL, 0, DecoderThread, NULL, 0, &decoder_thread_id);
	if (!decoder_handle)
	{
		stop();
		return 1;
	}
	else
	{
		// Do nothing
	}

	return 0;
}

void pause()
{
	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		playing_ttafile.SetPaused(1);
	}
	else
	{
		// do nothing
	}

	mod.outMod->Pause(1);
}

void unpause()
{
	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		playing_ttafile.SetPaused(0);
	}
	else
	{
		// do nothing
	}

	mod.outMod->Pause(0);
}

int ispaused()
{
	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		return playing_ttafile.GetPaused();
	}
	else
	{
		return 0;
	}

}

void stop()
{
	if (INVALID_HANDLE_VALUE != decoder_handle)
	{
		killDecoderThread = 1;
		WaitForSingleObject(decoder_handle, INFINITE);
		CloseHandle(decoder_handle);
		decoder_handle = INVALID_HANDLE_VALUE;
	}
	else
	{
		// Do nothing
	}

	mod.SetInfo(0, 0, 0, 1);
	mod.outMod->Close();
	mod.SAVSADeInit();

}

int getlength()
{
	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		return playing_ttafile.GetLengthbymsec();
	}
	else
	{
		return 0;
	}
}

int getoutputtime()
{
	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		return (int)(playing_ttafile.GetDecodePosMs())
			+ mod.outMod->GetOutputTime() - mod.outMod->GetWrittenTime();
	}
	else
	{
		return 0;
	}
}

void setoutputtime(int time_in_ms)
{
	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		playing_ttafile.SetSeekNeeded(time_in_ms);
	}
	else
	{
		// do nothing
	}

}

void setvolume(int volume)
{
	mod.outMod->SetVolume(volume);
}

void setpan(int pan)
{
	mod.outMod->SetPan(pan);
}

void eq_set(int on, char data[10], int preamp)
{
	// Do nothing
}

static void do_vis(unsigned char *data, int count, int bps, long double position)
{

	if (playing_ttafile.isValid() && playing_ttafile.isDecodable())
	{
		mod.SAAddPCMData(data, playing_ttafile.GetNumberofChannel(), bps, (int)position);
		mod.VSAAddPCMData(data, playing_ttafile.GetNumberofChannel(), bps, (int)position);
	}
	else
	{
		// Do nothing
	}
}


DWORD WINAPI __stdcall DecoderThread(void *p)
{

	int done = 0;
	int decoded_samples;
	static const __int32 PLAYING_BUFFER_SIZE = TTA_FIFO_BUFFER_SIZE;
	static BYTE pcm_buffer[PLAYING_BUFFER_SIZE];

	if (!playing_ttafile.isValid() || !playing_ttafile.isDecodable())
	{
		tta_error_message(-1, L"");
		done = 1;
		return 0;
	}
	else
	{
		// do nothing
	}

	int bitrate = playing_ttafile.GetBitrate();

	while (!killDecoderThread)
	{
		if (!playing_ttafile.isDecodable())
		{
			tta_error_message(-1, L"");
			PostMessage(mod.hMainWindow, WM_WA_MPEG_EOF, 0, 0);
			return 0;
		}
		else
		{
			// Do nothing
		}

		if (playing_ttafile.GetSeekNeeded() != -1)
		{
			mod.outMod->Flush((int)playing_ttafile.SeekPosition(&done));
		}
		else
		{
			// do nothing
		}

		if (done)
		{
			if (!mod.outMod->IsPlaying())
			{
				PostMessage(mod.hMainWindow, WM_WA_MPEG_EOF, 0, 0);
				return 0;
			}
			else
			{
				mod.SetInfo(bitrate, playing_ttafile.GetSampleRate() / 1000, playing_ttafile.GetNumberofChannel(), 1);
			}
		}
		else if (mod.outMod->CanWrite() >=
			((PLAYING_BUFFER_LENGTH * playing_ttafile.GetNumberofChannel() *
				playing_ttafile.GetByteSize()) << (mod.dsp_isactive() ? 1 : 0)))
		{
			try
			{
				decoded_samples = playing_ttafile.GetSamples(pcm_buffer, PLAYING_BUFFER_SIZE, &bitrate);
			}
			catch (CDecodeFile_exception &ex)
			{
				tta_error_message(ex.code(), playing_ttafile.GetFileName());
				PostMessage(mod.hMainWindow, WM_WA_MPEG_EOF, 0, 0);
				mod.SetInfo(0, 0, 0, 1);
				mod.outMod->Close();
				mod.SAVSADeInit();
				return 0;
			}

			if (decoded_samples == 0)
			{
				done = 1;
			}
			else
			{
				do_vis(pcm_buffer, decoded_samples, playing_ttafile.GetOutputBPS(), playing_ttafile.GetDecodePosMs());
				if (mod.dsp_isactive())
				{
					decoded_samples = mod.dsp_dosamples(reinterpret_cast<short*>(pcm_buffer), decoded_samples, playing_ttafile.GetOutputBPS(),
						playing_ttafile.GetNumberofChannel(), playing_ttafile.GetSampleRate());
				}
				else
				{
					// Do nothing
				}
				mod.outMod->Write(reinterpret_cast<char *>(pcm_buffer), decoded_samples * playing_ttafile.GetNumberofChannel()
					* (playing_ttafile.GetOutputBPS() >> 3));
			}

			mod.SetInfo(bitrate, playing_ttafile.GetSampleRate() / 1000, playing_ttafile.GetNumberofChannel(), 1);
		}
		else
		{
			mod.SetInfo(bitrate, playing_ttafile.GetSampleRate() / 1000, playing_ttafile.GetNumberofChannel(), 1);

			Sleep(1);
		}

	}

	return 0;
}

extern "C"
{
	__declspec(dllexport) In_Module* __cdecl winampGetInModule2(void)
	{
		return &mod;
	}


	__declspec(dllexport) int __cdecl
		winampGetExtendedFileInfoW(const wchar_t *fn, const wchar_t *data, wchar_t *dest, size_t destlen)
	{

		return m_ReadTag.GetExtendedFileInfo(fn, data, dest, destlen);
	}

	__declspec(dllexport) int __cdecl winampUseUnifiedFileInfoDlg(const char * fn)
	{
		// this will be called when Winamp is requested to show a File Info dialog for the selected file(s)
		// and this will allow you to override or forceable ignore the handling of a file or format
		// e.g. this will allow streams/urls to be ignored
		if (!_strnicmp(fn, "file://", 7))
		{
			fn += 7;
		}

		if (PathIsURLA(fn))
		{
			return 0;
		}
		else
		{
			// Do nothing
		}

		return 1;
	}


	__declspec(dllexport) int __cdecl
		winampSetExtendedFileInfoW(const wchar_t *fn, const wchar_t *data, const wchar_t *val)
	{
		return m_WriteTag.SetExtendedFileInfo(fn, data, val);
	}

	__declspec(dllexport) int __cdecl winampWriteExtendedFileInfo()
	{
		m_ReadTag.FlushCache();
		return m_WriteTag.WriteExtendedFileInfo();
	}

	__declspec(dllexport) intptr_t __cdecl
		winampGetExtendedRead_openW(const wchar_t *filename, int *size, int *bps, int *nch, int *srate)
	{

		CDecodeFile *dec = &transcode_ttafile;
		if (!dec->isValid())
		{
			return (intptr_t)0;
		}
		else
		{
			// do nothing
		}

		try
		{
			dec->SetFileName(filename);
		}

		catch (CDecodeFile_exception &ex)
		{
			tta_error_message(ex.code(), filename);
			return (intptr_t)0;
		}

		*bps = dec->GetBitsperSample();
		*nch = dec->GetNumberofChannel();
		*srate = dec->GetSampleRate();
		*size = dec->GetDataLength() * (*bps / 8) * (*nch);

		return (intptr_t)dec;
	}

	__declspec(dllexport) intptr_t __cdecl winampGetExtendedRead_getData(intptr_t handle, char *dest, int len, int *killswitch)
	{
		CDecodeFile *dec = &transcode_ttafile;
		int dest_used = 0;
		int n = 0;
		int bitrate;
		int32_t decoded_samples = 0;
		int32_t decoded_bytes = 0;

		if (!dec->isDecodable())
		{
			return (intptr_t)-1;
		}
		else
		{
			// do nothing
		}

		try
		{
			decoded_samples = dec->GetSamples((BYTE *)dest, len, &bitrate);
		}
		catch (CDecodeFile_exception &ex)
		{
			tta_error_message(ex.code(), dec->GetFileName());
			dest_used = -1;
		}

		if (0 != decoded_samples)
		{
			decoded_bytes = decoded_samples * dec->GetBitsperSample() / 8 * dec->GetNumberofChannel();
		}
		else
		{
			// Do nothing
		}


		return (intptr_t)decoded_bytes;
	}

	// return nonzero on success, zero on failure
	__declspec(dllexport) int __cdecl winampGetExtendedRead_setTime(intptr_t handle, int millisecs)
	{
		int done = 0;
		CDecodeFile *dec = &transcode_ttafile;
		if (NULL != dec && dec->isValid() && dec->isDecodable())
		{
			dec->SetSeekNeeded(millisecs);
			dec->SeekPosition(&done);
		}
		else
		{
			return 0;
		}
		return 1;
	}

	__declspec(dllexport) void __cdecl winampGetExtendedRead_close(intptr_t handle)
	{
		// Do nothing
	}
}

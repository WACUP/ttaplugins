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

#include <Winamp/in2.h>
#include <Agave/Language/api_language.h>
#include <Agave/Config/api_config.h>

#include <taglib/trueaudio/trueaudiofile.h>
#include <taglib/toolkit/tstring.h>

#include "DecodeFile.h"
#include "..\common\VersionNo.h"
#include "resource.h"

#include <loader/loader/utils.h>
#include <loader/loader/delay_load_helper.h>
#include <loader/loader/runtime_helper.h>
#include <loader/hook/squash.h>

const static int MAX_MESSAGE_LENGTH = 1024;
const static __int32 PLAYING_BUFFER_LENGTH = 576;
const static __int32 TRANSCODING_BUFFER_LENGTH = 5120;

// for playing static variables
static /*__declspec(align(16))*/ CDecodeFile* playing_ttafile;

static HANDLE decoder_handle = INVALID_HANDLE_VALUE;
static DWORD WINAPI __stdcall DecoderThread(void *p);
static volatile int killDecoderThread = 0;

// for MetaData static variables
CMediaLibrary *m_ReadTag = NULL;
CMediaLibrary *m_WriteTag = NULL;

void about(HWND hwndParent);
int init(void);
void quit(void);
void getfileinfo(const wchar_t *file, wchar_t *title, int *length_in_ms);
/*int  infodlg(const wchar_t *file, HWND hwndParent);
int  isourfile(const wchar_t *fn);*/
int  play(const wchar_t *fn);
void pause(void);
void unpause(void);
int  ispaused(void);
void stop(void);
int  getlength(void);
int  getoutputtime(void);
void setoutputtime(int time_in_ms);
void setvolume(int volume);
void setpan(int pan);
void GetFileExtensions(void);

In_Module plugin = {
	IN_VER_WACUP,
	"TTA Audio Decoder " PLUGIN_VERSION_CHAR,
	NULL,		// hMainWindow
	NULL,		// hDllInstance
	NULL,		// filled in later
	1,			// is_seekable
	1,			// uses output
	NULL,
	about,
	init,
	quit,
	getfileinfo,
	0/*infodlg*/,
	0/*isourfile*/,
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
	IN_INIT_VIS_RELATED_CALLS,
	NULL, NULL,	// dsp
	IN_INIT_WACUP_EQSET_EMPTY
	NULL,		// setinfo
	NULL,		// out_mod
	NULL,       // api_service
	INPUT_HAS_READ_META | INPUT_HAS_WRITE_META | INPUT_USES_UNIFIED_ALT3 |
	INPUT_HAS_FORMAT_CONVERSION_UNICODE | INPUT_HAS_FORMAT_CONVERSION_SET_TIME_MODE,
	GetFileExtensions,	// loading optimisation
	IN_INIT_WACUP_END_STRUCT
};

void GetFileExtensions(void)
{
	if (!plugin.FileExtensions)
	{
		// TODO localise
		plugin.FileExtensions = BuildInputFileListString(L"TTA", L"TTA Audio File (*.TTA)");
	}
}

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

	TimedMessageBox(plugin.hMainWindow, message, L"True Audio Decoder Error",
										MB_ICONERROR | MB_SYSTEMMODAL, 3000);

}

void about(HWND hwndParent)
{
	const unsigned char* output = DecompressResourceText(plugin.hDllInstance, plugin.hDllInstance, IDR_ABOUT_TEXT_GZ);
	// TODO localise
	wchar_t message[2048]/* = { 0 }*/;
	PrintfCch(message, ARRAYSIZE(message), (LPCWSTR)output,
			  (LPCWSTR)plugin.description, WACUP_Author(),
			  WACUP_Copyright(), TEXT(__DATE__));
	AboutMessageBox(hwndParent, message, L"True Audio Decoder");
	SafeFree((void*)output);
}

int init(void)
{
	Wasabi_Init();
	// TODO localise
	plugin.description = (char *)TEXT("True Audio Decoder " PLUGIN_VERSION_CHAR);
	return IN_INIT_SUCCESS;
}

void quit(void)
{
	Wasabi_Quit();
}

void getfileinfo(const wchar_t *file, wchar_t *title, int *length_in_ms)
{
	//title = L"";

	if (!file || !*file)
	{
		// invalid filename may be playing file
		if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
		{
			*length_in_ms = playing_ttafile->GetLengthbymsec();
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

/*int infodlg(const wchar_t *filename, HWND parent)
{
	return INFOBOX_UNCHANGED;
}

int isourfile(const wchar_t *filename)
{
	return 0;
}*/

int play(const wchar_t *filename)
{
	if (playing_ttafile && !playing_ttafile->isValid())
	{
		return 1;
	}

	try
	{
		if (!playing_ttafile)
		{
			playing_ttafile = new CDecodeFile();
		}

		if (playing_ttafile)
		{
			playing_ttafile->SetFileName(filename);
		}
	}
	catch (CDecodeFile_exception &ex)
	{
		tta_error_message(ex.code(), filename);
		return -1;
	}

	if (!playing_ttafile)
	{
		return 1;
	}

	const int samplerate = playing_ttafile->GetSampleRate(),
			  channels = playing_ttafile->GetNumberofChannel(),
			  maxlatency = (plugin.outMod && plugin.outMod->Open && samplerate &&
							channels ? plugin.outMod->Open(samplerate, channels,
							playing_ttafile->GetOutputBPS(), -1, -1) : -1);
	if (maxlatency < 0)
	{
		stop();
		return 1;
	}

	// setup information display
	plugin.SetInfo(playing_ttafile->GetBitrate(), (samplerate / 1000), channels, 1);

	// initialize vis stuff
	plugin.SAVSAInit(maxlatency, samplerate);
	plugin.VSASetInfo(channels, samplerate);

	// set the output plug-ins default volume
	plugin.outMod->SetVolume(-666);

	killDecoderThread = 0;

	decoder_handle = StartThread(DecoderThread, 0, static_cast<int>(plugin.
								 config->GetInt(playbackConfigGroupGUID,
								 L"priority", THREAD_PRIORITY_HIGHEST)), 0, NULL);

	if (!decoder_handle)
	{
		stop();
		return 1;
	}

	return 0;
}

void pause(void)
{
	if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
	{
		playing_ttafile->SetPaused(1);
	}

	if (plugin.outMod)
	{
		plugin.outMod->Pause(1);
	}
}

void unpause(void)
{
	if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
	{
		playing_ttafile->SetPaused(0);
	}

	if (plugin.outMod)
	{
		plugin.outMod->Pause(0);
	}
}

int ispaused(void)
{
	if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
	{
		return playing_ttafile->GetPaused();
	}
	return 0;
}

void stop(void)
{
	killDecoderThread = 1;
	WaitForThreadToClose(&decoder_handle, INFINITE);

	if (plugin.outMod && plugin.outMod->Close)
	{
		// there's a user out there using out_sqr
		// & it'll crash when tta files are being
		// played which has been going on forever
		__try
		{
			plugin.outMod->Close();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	if (plugin.outMod)
	{
		plugin.SAVSADeInit();
	}
}

int getlength(void)
{
	if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
	{
		return playing_ttafile->GetLengthbymsec();
	}
	return 0;
}

int getoutputtime(void)
{
	if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
	{
		return (plugin.outMod ? (int)(playing_ttafile->GetDecodePosMs() +
			   (plugin.outMod->GetOutputTime() - plugin.outMod->GetWrittenTime())) : 0);
	}
	return 0;
}

void setoutputtime(int time_in_ms)
{
	if (playing_ttafile && playing_ttafile->isValid() && playing_ttafile->isDecodable())
	{
		playing_ttafile->SetSeekNeeded(time_in_ms);
	}
}

void setvolume(int volume)
{
	if (plugin.outMod && plugin.outMod->SetVolume)
	{
		plugin.outMod->SetVolume(volume);
	}
}

void setpan(int pan)
{
	if (plugin.outMod && plugin.outMod->SetPan)
	{
		plugin.outMod->SetPan(pan);
	}
}

DWORD WINAPI __stdcall DecoderThread(void *p)
{
	int done = 0, decoded_samples;
	static const __int32 PLAYING_BUFFER_SIZE = TTA_FIFO_BUFFER_SIZE;
	static BYTE pcm_buffer[PLAYING_BUFFER_SIZE];

	if (!playing_ttafile || !playing_ttafile->isValid() || !playing_ttafile->isDecodable())
	{
		tta_error_message(-1, L"");
		done = 1;
		return 0;
	}

	const int channels = playing_ttafile->GetNumberofChannel();
	while (!killDecoderThread)
	{
		if (!playing_ttafile->isDecodable())
		{
			tta_error_message(-1, L"");
			PostEOF();
			return 0;
		}

		if (playing_ttafile->GetSeekNeeded() != -1)
		{
			plugin.outMod->Flush((int)playing_ttafile->SeekPosition(&done));
		}

		if (done)
		{
			PostEOF();
			if (plugin.outMod && plugin.outMod->Close)
			{
				plugin.outMod->Close();
			}

			if (plugin.outMod)
			{
				plugin.SAVSADeInit();
			}
			break;
		}
		else if (plugin.outMod->CanWrite() >= ((PLAYING_BUFFER_LENGTH * channels *
				playing_ttafile->GetByteSize()) << (plugin.dsp_isactive() ? 1 : 0)))
		{
			try
			{
				decoded_samples = playing_ttafile->GetSamples(pcm_buffer, PLAYING_BUFFER_SIZE);
			}
			catch (CDecodeFile_exception &ex)
			{
				tta_error_message(ex.code(), playing_ttafile->GetFileName());
				PostEOF();

				if (plugin.outMod && plugin.outMod->Close)
				{
					plugin.outMod->Close();
				}

				if (plugin.outMod)
				{
					plugin.SAVSADeInit();
				}
				break;
			}

			if (decoded_samples == 0)
			{
				done = 1;
			}
			else
			{
				const int bps = playing_ttafile->GetOutputBPS();
				if (playing_ttafile->isValid() && playing_ttafile->isDecodable())
				{
					plugin.SAAddPCMData(pcm_buffer, channels, bps, (int)playing_ttafile->GetDecodePosMs());
					/*plugin.VSAAddPCMData(pcm_buffer, channels, bps, (int)playing_ttafile->GetDecodePosMs());*/
				}

				if (plugin.dsp_isactive())
				{
					decoded_samples = plugin.dsp_dosamples(reinterpret_cast<short*>(pcm_buffer), decoded_samples,
																bps, channels, playing_ttafile->GetSampleRate());
				}

				plugin.outMod->Write(reinterpret_cast<char *>(pcm_buffer), decoded_samples * channels * (bps >> 3));
			}
		}
		else
		{
			Sleep(1);
		}
	}

	return 0;
}

extern "C"
{
	__declspec(dllexport) In_Module* __cdecl winampGetInModule2(void)
	{
		return &plugin;
	}

	__declspec(dllexport) int __cdecl
		winampGetExtendedFileInfoW(const wchar_t *fn, const char *data, wchar_t *dest, const size_t destlen)
	{
		if (m_ReadTag == NULL)
		{
			if (!SameStrA(data, "reset"))
			{
				m_ReadTag = new CMediaLibrary();
			}
		}
		return ((m_ReadTag != NULL) ? m_ReadTag->GetExtendedFileInfo(fn, data, dest, destlen) : 0);
	}

	__declspec(dllexport) int __cdecl winampUseUnifiedFileInfoDlg(const wchar_t * fn)
	{
		// this will be called when Winamp is requested to show a File Info dialog for the selected file(s)
		// and this will allow you to override or forceable ignore the handling of a file or format
		// e.g. this will allow streams/urls to be ignored
		/*if (SameStrNA(fn, "file://", 7))
		{
			fn += 7;
		}*/
		return !IsPathURL(fn);
	}

	// should return a child window of 513x271 pixels (341x164 in msvc dlg units), or return NULL for no tab.
	// Fill in name (a buffer of namelen characters), this is the title of the tab (defaults to "Advanced").
	// filename will be valid for the life of your window. n is the tab number. This function will first be 
	// called with n == 0, then n == 1 and so on until you return NULL (so you can add as many tabs as you like).
	// The window you return will recieve WM_COMMAND, IDOK/IDCANCEL messages when the user clicks OK or Cancel.
	// when the user edits a field which is duplicated in another pane, do a SendMessage(GetParent(hwnd),WM_USER,(WPARAM)L"fieldname",(LPARAM)L"newvalue");
	// this will be broadcast to all panes (including yours) as a WM_USER.
	extern "C" __declspec(dllexport) HWND winampAddUnifiedFileInfoPane(int n, const wchar_t * filename,
																	   HWND parent, wchar_t *name, size_t namelen)
	{
		/*if (n == 0)
		{
			// add first pane
			SetPropW(parent, L"INBUILT_NOWRITEINFO", (HANDLE)1);
			Info *info = new Info(filename);
			if (info)
			{
				if (info->Error())
				{
					delete info;
					return NULL;
				}
				return LangCreateDialog(IDD_INFO, parent, ChildProc_Advanced, (LPARAM)info);
			}
		}*/
		return NULL;
	}

	__declspec(dllexport) int __cdecl
		winampSetExtendedFileInfoW(const wchar_t *fn, const char *data, const wchar_t *val)
	{
		if (m_WriteTag == NULL)
		{
			m_WriteTag = new CMediaLibrary();
		}
		return ((m_WriteTag != NULL) ? m_WriteTag->SetExtendedFileInfo(fn, data, val) : 0);
	}

	__declspec(dllexport) int __cdecl winampWriteExtendedFileInfo()
	{
		if (m_ReadTag != NULL)
		{
			m_ReadTag->FlushCache();
		}
		return ((m_WriteTag != NULL) ? m_WriteTag->WriteExtendedFileInfo() : 0);
	}

	__declspec(dllexport) intptr_t __cdecl
		winampGetExtendedRead_openW(const wchar_t *filename, int *size, int *bps, int *nch, int *srate)
	{
		CDecodeFile *dec = new CDecodeFile();
		if (!dec->isValid())
		{
			return (intptr_t)0;
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

		if (bps)
		{
			*bps = dec->GetBitsperSample();
		}
		if (nch)
		{
			*nch = dec->GetNumberofChannel();
		}
		if (srate)
		{
			*srate = dec->GetSampleRate();
		}
		if (size && bps && nch)
		{
			*size = dec->GetDataLength() * (*bps / 8) * (*nch);
		}

		return reinterpret_cast<intptr_t>(dec);
	}

	__declspec(dllexport) intptr_t __cdecl winampGetExtendedRead_getData(intptr_t handle, char *dest, int len, int *killswitch)
	{
		CDecodeFile *dec = (CDecodeFile *)handle;
		int dest_used = 0;
		int n = 0;
		int32_t decoded_samples = 0;
		int32_t decoded_bytes = 0;

		if (!dec->isDecodable())
		{
			return (intptr_t)-1;
		}

		try
		{
			decoded_samples = dec->GetSamples((BYTE *)dest, len);
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

		return (intptr_t)decoded_bytes;
	}

	// return nonzero on success, zero on failure
	__declspec(dllexport) int __cdecl winampGetExtendedRead_setTime(intptr_t handle, int millisecs)
	{
		int done = 0;
		CDecodeFile *dec = (CDecodeFile *)handle;
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
		CDecodeFile *dec = (CDecodeFile *)handle;
		if (dec)
		{
			delete dec;
		}
	}

	extern "C" __declspec(dllexport) int winampUninstallPlugin(HINSTANCE /*hDllInst*/, HWND /*hwndDlg*/, int /*param*/)
	{
		return IN_PLUGIN_UNINSTALL_NOW;
	}
}

DLL_DELAY_LOAD_HANDLER

RUNTIME_HELPER_HANDLER
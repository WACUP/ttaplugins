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

#include "stdafx.h"

#include "AudioCoderTTA.h"
#include "enc_tta.h"
#include "..\common\VersionNo.h"

// wasabi based services for localisation support
#include <Agave/Language/api_language.h>
#include <Winamp/wa_ipc.h>
#include <mmsystem.h>
#include <Wasabi/bfc/platform/platform.h>
#define WA_UTILS_SIMPLE
#include <../loader/loader/utils.h>
#include <../loader/loader/runtime_helper.h>

#include <strsafe.h>

HWND winampwnd = 0;

SETUP_API_LNG_VARS;

const static int MAX_MESSAGE_LENGTH = 1024;

typedef struct
{
	//	configtype cfg;
	char configfile[MAX_PATH];
}
configwndrec;

/*BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}*/

//void readconfig(char *configfile, configtype *cfg)
//{
//	cfg->compression = 8;
//	if (configfile) GetPrivateProfileStruct("audio_flake", "conf", cfg, sizeof(configtype), configfile);
//}

//void writeconfig(char *configfile, configtype *cfg)
//{
//	if (configfile) WritePrivateProfileStruct("audio_flake", "conf", cfg, sizeof(configtype), configfile);
//}

#ifndef _WIN64
static HINSTANCE GetMyInstance()
{
	MEMORY_BASIC_INFORMATION mbi/* = { 0 }*/;
	if (VirtualQuery(GetMyInstance, &mbi, sizeof(mbi)))
		return (HINSTANCE)mbi.AllocationBase;
	return NULL;
}
#endif

void GetLocalisationApiService(HINSTANCE hinst)
{
	if (WASABI_API_ORIG_HINST == NULL)
	{
		// loader so that we can get the localisation service api for use
		// need to have this initialised before we try to do anything with localisation features
		StartPluginLangOnly(hinst, EncFlakeLangGUID);
	}
}

extern "C"
{
	unsigned int __declspec(dllexport) GetAudioTypes3(const int idx, GETAUDIOTYPEPARAMS)
	{
		if (idx == 0)
		{
#ifndef _WIN64
			GetLocalisationApiService(GetMyInstance());
#else
			GetLocalisationApiService(hinst);
#endif

			ConfigBuildName(WASABI_API_LNG_HINST, WASABI_API_ORIG_HINST,
#ifndef _WIN64
							desc, IDS_ENC_TTA_DESC, VERSION_CHAR, "");
#else
							desc, IDS_ENC_TTA_DESC, TEXT(VERSION_CHAR), "");
#endif
			return mmioFOURCC('T', 'T', 'A', ' ');
		}
		return 0;
	}

	AudioCoder __declspec(dllexport) *CreateAudio3(int nch, int srate, int bps, unsigned int srct, unsigned int *outt, char *configfile)
	{
		if (srct == mmioFOURCC('P', 'C', 'M', ' ') && *outt == mmioFOURCC('T', 'T', 'A', ' '))
		{
			//			configtype cfg;
			//			readconfig(configfile, &cfg);

			AudioCoderTTA *t = 0;
			t = new AudioCoderTTA(nch, srate, bps);
			if (t->GetLastError())
			{
				delete t;
				return NULL;
			}
			return t;
		}
		return NULL;
	}

	void __declspec(dllexport) FinishAudio3(const char *filename, AudioCoder *coder)
	{
		wchar_t wfilename[MAX_PATHLEN + 1]/* = { 0 }*/;
		ConvertANSI(filename, -1, CP_ACP, wfilename, ARRAYSIZE(wfilename), NULL);
		((AudioCoderTTA*)coder)->FinishAudio(wfilename);
	}

	void __declspec(dllexport) FinishAudio3W(const wchar_t *filename, AudioCoder *coder)
	{
		((AudioCoderTTA*)coder)->FinishAudio(filename);
	}

	void __declspec(dllexport) PrepareToFinish(const char *filename, AudioCoder *coder)
	{
		((AudioCoderTTA*)coder)->PrepareToFinish();
	}

	void __declspec(dllexport) PrepareToFinishW(const wchar_t *filename, AudioCoder *coder)
	{
		((AudioCoderTTA*)coder)->PrepareToFinish();
	}

	/*BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_INITDIALOG:
			configwndrec* wc = (configwndrec*)lParam;
			if (wc)
			{
				//			SendDlgItemMessage(hwndDlg, IDC_COMPRESSIONSLIDER, TBM_SETRANGE, TRUE, MAKELONG(0, 12));
				//			SendDlgItemMessage(hwndDlg, IDC_COMPRESSIONSLIDER, TBM_SETPOS, TRUE, wc->cfg.compression);

				SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG)lParam);
			}
			break;

		case WM_NOTIFY:
			//			if (wParam == IDC_COMPRESSIONSLIDER)
			//			{
			//				LPNMHDR l = (LPNMHDR)lParam;
			//				if (l->idFrom == IDC_COMPRESSIONSLIDER)
			//					wr->cfg.compression = SendDlgItemMessage(hwndDlg, IDC_COMPRESSIONSLIDER, TBM_GETPOS, 0, 0);
			//			}
			break;

		case WM_DESTROY:
			configwndrec* wc = (configwndrec*)GetWindowLongPtr(hwndDlg, GWLP_USERDATA, 0);
			if (wc)
			{
				//			writeconfig(wc->configfile, &wc->cfg);
				free(wc);
				wc = NULL;
			}
			break;
		}
		return 0;
	}*/

	HWND __declspec(dllexport) ConfigAudio3(HWND hwndParent, HINSTANCE hinst, unsigned int outt, char *configfile)
	{
		if (outt == mmioFOURCC('T', 'T', 'A', ' '))
		{
			//configwndrec *wc = (configwndrec*)malloc(sizeof(configwndrec));
			//			if (configfile) lstrcpyn(wc->configfile, configfile, MAX_PATH);
			//			else wc->configfile[0] = 0;
			//			readconfig(configfile, &wc->cfg);
			//GetLocalisationApiService(hinst);
			//			return LangCreateDialog(IDD_CONFIG, hwndParent, DlgProc, (LPARAM)wc);
		}
		return NULL;
	}

	int __declspec(dllexport) SetConfigItem(unsigned int outt, char *item, char *data, char *configfile)
	{
		// nothing yet
		return 0;
	}

	int __declspec(dllexport) GetConfigItem(unsigned int outt, char *item, char *data, int len, char *configfile)
	{
		if (outt == mmioFOURCC('T', 'T', 'A', ' '))
		{
			ConfigItemCopy(item, "extension", data, len, "tta");
			return 1;
		}
		return 0;
	}

	void __declspec(dllexport) SetWinampHWND(HWND hwnd)
	{
		winampwnd = hwnd;
	}
};

static void tta_error_message(int error, const wchar_t *filename)
{
	wchar_t message[MAX_MESSAGE_LENGTH];

	std::wstring name(filename);
	switch (error) {
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

	MessageBox(winampwnd, message, L"TTA Decoder Error",
						  MB_ICONERROR | MB_SYSTEMMODAL);
}

RUNTIME_LEN_HELPER_HANDLER
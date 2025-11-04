/*
The ttaplugins-winamp project.
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

#ifndef VERSIONNO_H
#define VERSIONNO_H

#define /*
!define /**/ /*\
/**/MAJOR_VERSION 3
#define /*
!define /**/ /*\
/**/MINOR_VERSION 72
#define /*
!define /**/ /*\
/**/EXTENDED_VERSION 23


#ifndef MAKESTR
#define _MAKESTR(s) #s
#define MAKESTR(s) _MAKESTR(s)
#define _MAKESTRW(s) L#s
#define MAKESTRW(s) _MAKESTRW(s)
#endif

#ifdef _DEBUG
#define BASE_VERSION_CHAR "v" MAKESTR(MAJOR_VERSION) "." MAKESTR(MINOR_VERSION) "debug"
#define BASE_VERSION_WCHAR L"v" MAKESTRW(MAJOR_VERSION) L"." MAKESTRW(MINOR_VERSION) L"debug"
#else
#define BASE_VERSION_CHAR "v" MAKESTR(MAJOR_VERSION) "." MAKESTR(MINOR_VERSION)
#define BASE_VERSION_WCHAR L"v" MAKESTRW(MAJOR_VERSION) L"." MAKESTRW(MINOR_VERSION)
#endif

#define COPYRIGHT_YEARS L"2005-2025"
#define VERSION_CHAR BASE_VERSION_CHAR
#define VERSION_WCHAR BASE_VERSION_WCHAR

#define FILEVER    MAJOR_VERSION,MINOR_VERSION,EXTENDED_VERSION,0
#define PRODUCTVER MAJOR_VERSION,MINOR_VERSION,EXTENDED_VERSION,0
#define STRFILEVER    VERSION_CHAR "\0"
#define STRPRODUCTVER STRFILEVER
#define COPYRIGHT_IN_TTA     BASE_VERSION_CHAR ", (C)2005 Alexander Djourik. All rights reserved.  (C)" COPYRIGHT_YEARS " Yamagta Fumihiro. All right reserved."
#define COPYRIGHT_ENC_TTA    BASE_VERSION_CHAR " (C)" COPYRIGHT_YEARS " Yamagata Fumihiro. All right reserved."
#define PLUGIN_VERSION_CHAR VERSION_CHAR
#define PLUGIN_VERSION_WCHAR VERSION_WCHAR
#define LIBTTA_VERSION_WCHAR L"libtta C++ Ver.2.3"

#define PROJECT_URL_WCHAR L"<https://github.com/bunbun042000/ttaplugin-winamp>"
#define ORIGINAL_CREADIT01 L"Plugin is written by Alexander Djourik, Pavel Zhilin and Anton Gorbunov.\n"
#define ORIGINAL_CREADIT02 L"Copyright (c) 2003 Alexander Djourik.\n"
#define ORIGINAL_CREADIT03 L"All rights reserved.\n"
#define LIBTTA_ORIGINAL_CREADIT LIBTTA_VERSION_WCHAR L"Copyright(c) 1999 - 2015 Aleksander Djuric.All rights reserved.\n"
#define CREADIT01 L"Modified by Yamagata Fumihiro, " COPYRIGHT_YEARS L"\n"
#define CREADIT02 L"Copyright (C)" COPYRIGHT_YEARS L" Yamagata Fumihiro.\n"

#define IN_TTA_PLUGIN_VERSION_CREADIT L"Winamp plug-in version " PLUGIN_VERSION_WCHAR L"\nbased on " LIBTTA_VERSION_WCHAR L"\n" PROJECT_URL_WCHAR
#define IN_TTA_PLUGIN_COPYRIGHT_CREADIT ORIGINAL_CREADIT01 ORIGINAL_CREADIT02 ORIGINAL_CREADIT03 CREADIT01 CREADIT02
#endif  /* VERSIONNO_H */

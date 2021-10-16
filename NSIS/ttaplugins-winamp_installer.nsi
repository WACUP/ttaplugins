; This script generates an installer for a Winamp 2.x / 5.x plug-in.
;
; The installer will automatically close Winamp if it's running and then if
; successful, ask the user whether or not they would like to run Winamp with
; the newly installed plug-in.
;
; This is a single section installer but is easily altered for multiple
; sections and is based of the original Winamp installer script but tweaked
; to be easier to use i think :o)

;--------------------------------

Unicode true

; Header Files
; not used in this case but handy when scaling up to multiple sections
; !include "Sections.nsh"
!include ..\common\VersionNo.h

; common defines for a generic DrO installer :o)
!define BASE_VERSION "v${MAJOR_VERSION}.${MINOR_VERSION}"
!define VERSION "${BASE_VERSION}"
!define ALT_VER "${MAJOR_VERSION}_${MINOR_VERSION}"
!define PLUG "ttaplugins-winamp"
!define PLUG_ALT "ttaplugins-winamp"
!define IN_PLUG_FILE "in_tta"
!define ENC_PLUG_FILE "enc_tta"
;!define LIBTTA_DLL "libtta"
;!define TAGLIB_DLL "tag"
!define VC_REDIST "vcredist_2019_x86.exe"

!include x64.nsh

; use leet compression
SetCompressor lzma

; The name of the installer based on the filename and version
Name "${PLUG} ${VERSION}"

; The file to write based on the filename and version
OutFile "${PLUG_ALT}_v${ALT_VER}.exe"
; you could alter it to output you plugin installers into a common location
; to make it easier to maintain them
; OutFile "../_Installers/${PLUG_ALT}_v${ALT_VER}.exe"

; license file
LicenseData "../lgpl.txt"

; The default installation directory
InstallDir $PROGRAMFILES\Winamp
InstProgressFlags smooth

; detect Winamp path from uninstall string if available
InstallDirRegKey HKLM \
          "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp" \
          "UninstallString"

; The text to prompt the user to enter a directory
DirText "Please select your Winamp path below (you will be able to proceed \
         when Winamp is detected):"

; automatically close the installer when done.
AutoCloseWindow true

; adds xp style support
XPStyle on

; hide the "show details" box
ShowInstDetails nevershow

;--------------------------------

;Pages

Page license

PageEx directory
Caption " "
PageExEnd

; enable this line if you have extra sections and want to choose what's
; installed
;Page components

Page instfiles

;--------------------------------

; CloseWinamp: this will in a loop send the Winamp window the WM_CLOSE
; message until it does not find a valid Winamp window
; (should really protect against Winamp failing to exit!)
;
Function CloseWinamp
  Push $5
  loop:
    FindWindow $5 "Winamp v1.x"
    IntCmp $5 0 done
    SendMessage $5 16 0 0
    Sleep 100
    Goto loop
  done:
  Pop $5
FunctionEnd


; The stuff to install
Section ""
  ; attempt to close winamp if it's running
  Call CloseWinamp
  ; add a small delay to allow any file operations to happen once Winamp
  ; is closed
  Sleep 100

  SetOverwrite on
  SetOutPath "$INSTDIR\Plugins"
  ; File to extract
  ; File "${PLUG_FILE}.dll"
  ; if you're script is in the project folder then the following file path is
  ; likely to apply otherwise just alter the path as needed
  File "..\Release\${IN_PLUG_FILE}.dll"
  File "..\Release\${ENC_PLUG_FILE}.dll"
  SetOutPath "$INSTDIR"
;  File "..\..\libraries_vc2017\bin\${LIBTTA_DLL}.dll"
;  File "..\..\libraries\bin\${TAGLIB_DLL}.dll"

  SetOverwrite off
SectionEnd

Section "Microsoft Visual C++ 2019 Redist" SEC_CRT2019

  ; Make this required on the web installer, since it has a fully reliable check to
  ; see if it needs to be downloaded and installed or not.
  SectionIn RO

    SetRegView 64 
    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Version"
    DetailPrint "Found version $0"

    ${If} $0 >= "v14.16.27024.01"
    	DetailPrint "Visual C++ 2017 Redistributable registry key was not found; assumed to be uninstalled."
	Goto done
     ${Else}

  SetOutPath "$TEMP"

  DetailPrint "Downloading Visual C++ 2019 Redistributable Setup..."
  DetailPrint "Contacting Microsoft.com..."
;  NSISdl::download /TIMEOUT=15000 "https://download.microsoft.com/download/7/a/6/7a68af9f-3761-4781-809b-b6df0f56d24c/vc_redist.x86.exe" ${VC_REDIST}
  inetc::get /CAPTION "Visual C++ 2019 Redistributable Setup..." /CANCELTEXT "Canceled" "https://aka.ms/vs/16/release/vc_redist.x86.exe" "$TEMP\${VC_REDIST}"
  
  Pop $0 ;Get the return value
  StrCmp $0 "OK" OnSuccess
  MessageBox MB_OK "Could not download Visual Studio 2019 Redist; none of the mirrors appear to be functional."
  Goto done
${EndIf}

OnSuccess:
  DetailPrint "Running Visual C++ 2019 Redistributable Setup..."
  ExecWait '"$TEMP\${VC_REDIST}" /qb'
  DetailPrint "Finished Visual C++ 2019 Redistributable Setup"
  
  Delete "$TEMP\${VC_REDIST}"

done:
SectionEnd

;--------------------------------

; Success, now prompt the user if they want to run Winamp again
Function .onInstSuccess
  MessageBox MB_YESNO \
             '${PLUG} was installed. Do you want to run Winamp now?' \
	 IDNO end
    ExecShell open "$INSTDIR\Winamp.exe"
  end:
FunctionEnd

; here we check to see if this a valid location ie is there a Winamp.exe
; in the directory?
Function .onVerifyInstDir
  ;Check for Winamp installation
  IfFileExists $INSTDIR\Winamp.exe Good
    Abort
  Good:
FunctionEnd

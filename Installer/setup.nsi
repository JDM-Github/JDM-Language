!include MUI2.nsh

!define APP_NAME "JDM"
!define APP_VERSION "1.0"
!define APP_EXE "JDM.exe"
!define MUI_ICON "icon.ico"
Icon "icon.ico"

;-----------------------------------------
;General
	Name "JDM Language"
	Outfile "jdm-setup.exe"
	Unicode True
	InstallDir "$PROGRAMFILES\${APP_NAME}"
;-----------------------------------------


;-----------------------------------------
;Pages
	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
	;!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH
;-----------------------------------------


;-----------------------------------------
;Language
	!insertmacro MUI_LANGUAGE "English"
;-----------------------------------------


;-----------------------------------------
;Installer Sections

	Section "JDM" SEC01
		SetOutPath "$INSTDIR\bin"
		File "..\Build\*.dll"
		File "..\Build\*.exe"

		SetOutPath "$INSTDIR\bin\scripts"
		File "..\Build\scripts\*.jdm"

		SetOutPath "$INSTDIR"
		File "..\*.bat"
		File "..\*.md"
		CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
		WriteUninstaller "$INSTDIR\uninstall.exe"
	SectionEnd

;-----------------------------------------


;-----------------------------------------
;Uninstaller Section

	Section "Uninstall"
		Delete "$INSTDIR\bin\*.dll"
		Delete "$INSTDIR\bin\*.exe"
		Delete "$INSTDIR\bin\scripts\*.jdm"
		Delete "$INSTDIR\*.bat"
		Delete "$INSTDIR\*.md"
		Delete "$INSTDIR\uninstall.exe"
		RMDir "$INSTDIR"
	SectionEnd
;-----------------------------------------
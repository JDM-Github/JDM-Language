!define APP_NAME "JDM"
!define APP_VERSION "1.0"
!define APP_EXE "JDM.exe"

Outfile "jdm-setup.exe"
InstallDir "$PROGRAMFILES\${APP_NAME}"

Page Directory
Page InstFiles

Section "MainSection" SEC01
  SetOutPath "$INSTDIR\bin"
  File "Build\*.dll"
  File "Build\*.exe"

  SetOutPath "$INSTDIR\bin\scripts"
  File "Build\scripts\*.jdm"

  SetOutPath "$INSTDIR"
  File "*.bat"
  File "*.md"
  CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
SectionEnd

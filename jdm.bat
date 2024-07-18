@echo OFF

:checkJDMPath
if not defined JDM_PATH (
    echo JDM_PATH environment variable is not set.
    goto promptUser
)

if exist "%JDM_PATH%\JDM.exe" (
    "%JDM_PATH%\JDM.exe" -f "%JDM_PATH%" %1 %2 %3 %4
    goto end
) else (
    echo JDM.exe not found at %JDM_PATH%.
    goto promptUser
)

:promptUser
set /p userInput="Do you want to define/update JDM_PATH now? (yes/no): "
if /I "%userInput%"=="yes" (
    set /p JDM_PATH="Please enter the path for JDM: "

    if exist "%JDM_PATH%\JDM.exe" (
        setx JDM_PATH "%JDM_PATH%"
        echo JDM_PATH has been set to %JDM_PATH%.
        echo Please restart your command prompt for changes to take effect.
        exit /b 0
    ) else (
        echo JDM.exe not found at %JDM_PATH%. Please try again.
        goto promptUser
    )
) else (
    echo JDM_PATH not set. Exiting.
    exit /b 1
)

:end
exit /b 0
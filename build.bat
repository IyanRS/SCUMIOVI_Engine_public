@echo off
:: MSBuild wrapper used by the VS Code tasks (.vscode/tasks.json).
:: Bootstraps the MSVC environment (msbuild isn't on the PATH of a normal
:: shell) and forwards every argument to msbuild, e.g.:
::   build.bat SCUMIOVI_Engine.sln /p:Configuration=Debug /p:Platform=x64 /m

set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"

if exist "%VCVARS%" (
    call "%VCVARS%" x64 >nul
) else (
    echo Error: Could not find vcvarsall.bat at %VCVARS%
    echo Please make sure Visual Studio 2022 Community is installed.
    exit /b 1
)

msbuild %*
exit /b %ERRORLEVEL%

@echo off
setlocal EnableDelayedExpansion

:: Initialize MSVC environment
set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"

if exist "%VCVARS%" (
    call "%VCVARS%" x64 >nul
) else (
    echo Error: Could not find vcvarsall.bat at %VCVARS%
    echo Please make sure Visual Studio 2022 Community is installed.
    exit /b 1
)

:: Create build directory for tests
if not exist tests\build mkdir tests\build

cd tests\build

:: Gather all engine source files except Main.cpp and ScriptLoader.cpp
set "ENGINE_SRC="
for %%f in (..\..\SCUMIOVI_Engine\*.cpp) do (
    if /I not "%%~nxf"=="Main.cpp" if /I not "%%~nxf"=="ScriptLoader.cpp" (
        set "ENGINE_SRC=!ENGINE_SRC! "%%f""
    )
)

:: Compiling Catch2 tests...
cl.exe /nologo /EHsc /std:c++20 /I..\..\SCUMIOVI_Engine /I..\..\SCUMIOVI_Game /I..\..\SDL3\include /I..\..\SDL3_image\include /I..\..\SDL3_ttf\include ..\test_main.cpp ..\test_vector2.cpp ..\test_delay.cpp ..\test_walkable_area.cpp ..\test_verb.cpp ..\test_inventory.cpp ..\test_event.cpp ..\test_gameplay.cpp ..\test_dialogue.cpp ..\test_ui.cpp ..\test_audio.cpp ..\test_gamestate.cpp ..\test_examine.cpp ..\test_dialogue_conditions.cpp ..\test_fade.cpp !ENGINE_SRC! /Fe:run_tests.exe /link /LIBPATH:..\..\SDL3\lib\x64 /LIBPATH:..\..\SDL3_image\lib\x64 /LIBPATH:..\..\SDL3_ttf\lib\x64 SDL3.lib SDL3_image.lib SDL3_ttf.lib

copy /Y ..\..\SCUMIOVI_Engine\SDL3*.dll . >nul

if %ERRORLEVEL% equ 0 (
    echo.
    echo ----------------------------------------------------------------------
    echo Build successful. Running tests...
    echo ----------------------------------------------------------------------
    run_tests.exe
) else (
    echo.
    echo ----------------------------------------------------------------------
    echo Compilation FAILED.
    echo ----------------------------------------------------------------------
)

cd ..\..
endlocal
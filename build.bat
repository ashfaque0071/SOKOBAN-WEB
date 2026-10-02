@echo off
setlocal EnableExtensions EnableDelayedExpansion

pushd "%~dp0" || exit /b 1

where gcc >nul 2>&1
if errorlevel 1 (
    echo Error: GCC for Windows is required.
    echo Install MinGW-w64, then add its bin directory to PATH.
    popd
    exit /b 1
)

if not exist "build" mkdir "build"

set "SOURCES="
for %%D in (src\core src\audio src\screens src\render) do (
    for %%F in ("%%D\*.c") do set "SOURCES=!SOURCES! "%%F""
)

echo Building Sokoban for Windows...
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -DUSE_LIBTYPE_SHARED !SOURCES! ^
    -o "build\sokoban.exe" ^
    -Isrc\core -Isrc\audio -Isrc\screens -Isrc\render -Iinclude ^
    -Llib -lraylibdll -lopengl32 -lgdi32 -lwinmm -lm ^
    -mwindows

if errorlevel 1 (
    echo Build failed.
    popd
    exit /b 1
)

copy /Y "lib\raylib.dll" "build\raylib.dll" >nul

if /I "%~1"=="--build-only" (
    echo Build complete: %CD%\build\sokoban.exe
    popd
    exit /b 0
)

echo Starting Sokoban...
"build\sokoban.exe"
set "GAME_EXIT=%ERRORLEVEL%"
popd
exit /b %GAME_EXIT%

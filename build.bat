@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

echo "THIS SCRIPT IS LARGELY AI-GENERATED AND NOT TESTED AT ALL. PROCED WITH CAUTION!"

:: Check if CMake is installed
where cmake >nul 2>nul
IF ERRORLEVEL 1 (
    echo "CMake is not installed. Download and install from https://cmake.org/download/"
    exit /b 1
)

:: Check if Visual Studio is installed
IF NOT EXIST "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    echo "Visual Studio is not installed. Please install Visual Studio with C++ support."
    exit /b 1
)

:: Set Visual Studio environment
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
IF ERRORLEVEL 1 (
    echo "Failed to set up Visual Studio environment. Check your installation."
    exit /b 1
)

:: Set the build and binary directories
set "BUILD_DIR=build"
set "BIN_DIR=bin"

:: Clean up if specified
if "%~1"=="clean" (
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
    )
    if exist "%BIN_DIR%" (
        rmdir /s /q "%BIN_DIR%"
    )
    mkdir "%BIN_DIR%\lib"
)

:: Create build directory and move to it
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

:: Run CMake to configure the project
cmake .. -G "Visual Studio 16 2019"
IF ERRORLEVEL 1 (
    echo "CMake configuration failed."
    exit /b 1
)

:: Build the project using Visual Studio
cmake --build . --config Release
IF ERRORLEVEL 1 (
    echo "Build failed."
    exit /b 1
)

:: Copy scripts
::if exist "..\scripts\run.bat" (
::    copy ..\scripts\run.bat "%BIN_DIR%\"
::) ELSE (
::    echo "run.bat not found."
::)
::
::if exist "..\scripts\cleanup.bat" (
::    copy ..\scripts\cleanup.bat "%BUILD_DIR%\"
::) ELSE (
::    echo "cleanup.bat not found."
::)

echo "Build completed successfully."
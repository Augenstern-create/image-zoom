@echo off

set ROOT_DIR=%~dp0
set BUILD_DIR=%ROOT_DIR%build

rem 检查是否存在 build 文件夹
if exist "%BUILD_DIR%" (
    echo %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

cmake  -A x64 ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_SYSTEM_NAME=Windows ^
-DINTTYPES_FORMAT=C99 ^
..

cmake --build . --config Release -j8

if %errorlevel% neq 0 (
    echo CMake build failed.
    exit /b %errorlevel%
)

xcopy /E /I /Y /S /D %ROOT_DIR%photograph %BUILD_DIR%\Release\photograph

cd %BUILD_DIR%\Release
"ImageZoom.exe"
cd ..\..
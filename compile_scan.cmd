@echo off
setlocal enabledelayedexpansion
set ROOT=D:\Rcube\rubikscube
set VCPKG=D:\Rcube\vcpkg\installed\x64-windows
set MSVC=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30133

set PATH=!MSVC!\bin\Hostx64\x64;!MSVC!\bin\Hostx86\x86;%PATH%
set INCLUDE=!MSVC!\include;!MSVC!\atlmfc\include
set LIB=!MSVC!\lib\x64;!MSVC!\atlmfc\lib\x64

mkdir "%ROOT%\build" 2>nul

cl.exe /EHsc /std:c++17 /I"%ROOT%\OpenCV\include" /I"%ROOT%\Model\Representation" /I"%VCPKG%\include\opencv4" /MD /O2 /Fe"%ROOT%\build\scan_cube.exe" "%ROOT%\OpenCV\src\ColorScanner.cpp" "%ROOT%\OpenCV\apps\scan_cube.cpp" /link /LIBPATH:"%VCPKG%\lib" opencv_core4.lib opencv_highgui4.lib opencv_imgproc4.lib opencv_videoio4.lib opencv_imgcodecs4.lib

if %errorlevel% equ 0 (
  echo.
  echo === Build Success ===
  copy "%VCPKG%\bin\opencv_core4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_highgui4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_imgproc4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_videoio4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_imgcodecs4.dll" "%ROOT%\build\" >nul 2>&1
  echo.
  echo Running: scan_cube.exe
  echo Press 'q' to quit after scanning all 6 faces
  echo.
  cd /d "%ROOT%\build"
  set PATH=%VCPKG%\bin;!PATH!
  scan_cube.exe 0
) else (
  echo === Build Failed ===
)

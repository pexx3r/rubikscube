@echo off
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
set ROOT=D:\Rcube\rubikscube
set VCPKG=D:\Rcube\vcpkg\installed\x64-windows
mkdir "%ROOT%\build" 2>nul
echo Starting build... > "%ROOT%\build\build.log"
cl.exe /EHsc /std:c++17 /I"%ROOT%\OpenCV\include" /I"%ROOT%\Model\Representation" /I"%VCPKG%\include\opencv4" /MD /O2 /Fe"%ROOT%\build\scan_cube.exe" "%ROOT%\OpenCV\src\ColorScanner.cpp" "%ROOT%\OpenCV\apps\scan_cube.cpp" "%ROOT%\Model\Representation\RubiksCube.cpp" /link /LIBPATH:"%VCPKG%\lib" opencv_core4.lib opencv_highgui4.lib opencv_imgproc4.lib opencv_videoio4.lib opencv_imgcodecs4.lib >> "%ROOT%\build\build.log" 2>&1
if %errorlevel% equ 0 (
  echo Build Success >> "%ROOT%\build\build.log"
  copy "%VCPKG%\bin\opencv_core4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_highgui4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_imgproc4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_videoio4.dll" "%ROOT%\build\" >nul 2>&1
  copy "%VCPKG%\bin\opencv_imgcodecs4.dll" "%ROOT%\build\" >nul 2>&1
  echo All DLLs copied >> "%ROOT%\build\build.log"
) else (
  echo Build Failed - errorlevel %errorlevel% >> "%ROOT%\build\build.log"
)

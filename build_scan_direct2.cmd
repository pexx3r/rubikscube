@echo off
set ROOT=D:\Rcube\rubikscube
set VCPKG=D:\Rcube\vcpkg\installed\x64-windows
set MSVC=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64
set BINDIR=%VCPKG%\bin
mkdir "%ROOT%\build" 2>nul
"%MSVC%\cl.exe" /EHsc /std:c++17 /I"%ROOT%\OpenCV\include" /I"%ROOT%\MoveTranslator\include" /I"%ROOT%\Model\Representation" /I"%VCPKG%\include\opencv4" /MD /O2 /Fe"%ROOT%\build\scan_cube.exe" "%ROOT%\OpenCV\src\ColorScanner.cpp" "%ROOT%\OpenCV\apps\scan_cube.cpp" "%ROOT%\MoveTranslator\src\MoveTranslator.cpp" "%ROOT%\MoveTranslator\src\OpenCVRenderer.cpp" /link /LIBPATH:"%VCPKG%\lib" opencv_core4.lib opencv_highgui4.lib opencv_imgproc4.lib opencv_videoio4.lib opencv_imgcodecs4.lib user32.lib gdi32.lib winmm.lib version.lib
if errorlevel 1 exit /b %errorlevel%
copy "%BINDIR%\opencv_core4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_highgui4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_imgproc4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_videoio4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_imgcodecs4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_core4.dll" "%ROOT%\build\" >nul
echo build complete
cd /d "%ROOT%\build"
set PATH=%BINDIR%;%PATH%
scan_cube.exe 0

@echo off
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
set ROOT=D:\Rcube\rubikscube
set VCPKG=D:\Rcube\vcpkg\installed\x64-windows
set INCLUDE=%ROOT%\OpenCV\include;%ROOT%\MoveTranslator\include;%ROOT%\Model\Representation;%VCPKG%\include\opencv4
set LIBPATH=%VCPKG%\lib
set BINDIR=%VCPKG%\bin
mkdir "%ROOT%\build" 2>nul
cl.exe /EHsc /std:c++17 /I"%ROOT%\OpenCV\include" /I"%ROOT%\MoveTranslator\include" /I"%ROOT%\Model\Representation" /I"%VCPKG%\include\opencv4" /MD /O2 /Fe"%ROOT%\build\scan_cube.exe" "%ROOT%\OpenCV\src\ColorScanner.cpp" "%ROOT%\OpenCV\apps\scan_cube.cpp" "%ROOT%\MoveTranslator\src\MoveTranslator.cpp" "%ROOT%\MoveTranslator\src\OpenCVRenderer.cpp"
if errorlevel 1 exit /b %errorlevel%
copy "%BINDIR%\opencv_core4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_highgui4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_imgproc4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_videoio4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_imgcodecs4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_imgcodecs4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_core4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_video4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_calib3d4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_imgcodecs4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_highgui4.dll" "%ROOT%\build\" >nul
copy "%BINDIR%\opencv_videoio4.dll" "%ROOT%\build\" >nul
echo build complete
cd /d "%ROOT%\build"
set PATH=%BINDIR%;%PATH%
scan_cube.exe 0

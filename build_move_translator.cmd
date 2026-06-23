@echo off
call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d D:\Rcube\rubikscube
cmake -G Ninja -S .\MoveTranslator -B .\build\MoveTranslator_build_msvc -DCMAKE_TOOLCHAIN_FILE=D:/Rcube/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b %errorlevel%
cmake --build .\build\MoveTranslator_build_msvc --config Release

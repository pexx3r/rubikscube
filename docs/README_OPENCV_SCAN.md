# OpenCV Cube Scanner

This module provides a simple webcam-based scanner that captures the six faces of a Rubik's Cube and constructs a `RubiksCube3dArray` instance.

Build (example using vcpkg):

```powershell
git clone https://github.com/microsoft/vcpkg.git
.\n+.\bootstrap-vcpkg.bat
vcpkg install opencv:x64-windows
mkdir build; cd build
cmake -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake ..\OpenCV
cmake --build . --config Release
```

Run:

```powershell
.\scan_cube.exe 0
```

Controls:
- SPACE: capture current face
- C: show calibration preview
- R: retake last face (not implemented in skeleton)
- ESC/Q: quit

Notes:
- Tweak HSV centroids in `ColorScanner` or implement a persistent calibration UI.
- Ensure include directories allow `Model/RubiksCube.h` and `Representation/RubiksCube3dArray.h` to be found.

@echo off
cd /d D:\Rcube\rubikscube\build
set PATH=D:\Rcube\vcpkg\installed\x64-windows\bin;%PATH%
echo.
echo ===============================================
echo OpenCV Rubik's Cube Scanner
echo ===============================================
echo.
echo Instructions:
echo 1. Point your camera at each cube face
echo 2. The window shows which face to scan (UP, LEFT, RIGHT, FRONT, DOWN, BACK)
echo 3. Press SPACE to scan - you'll see "DETECTED!" when captured
echo 4. Repeat for all 6 faces
echo 5. The solver will show the solution moves
echo.
echo ===============================================
echo.
scan_cube.exe 0

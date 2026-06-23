cmake_minimum_required(VERSION 3.10)
project(OpenCVScanner)

find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(ColorScanner STATIC src/ColorScanner.cpp)
target_include_directories(ColorScanner PUBLIC include)
target_link_libraries(ColorScanner PRIVATE ${OpenCV_LIBS})

add_executable(scan_cube apps/scan_cube.cpp)
target_link_libraries(scan_cube PRIVATE ColorScanner ${OpenCV_LIBS})

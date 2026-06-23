cmake_minimum_required(VERSION 3.10)
project(MoveTranslator)

find_package(OpenCV REQUIRED)

add_library(MoveTranslator STATIC src/MoveTranslator.cpp src/OpenCVRenderer.cpp)
target_include_directories(MoveTranslator PUBLIC include ${CMAKE_SOURCE_DIR}/..)
target_link_libraries(MoveTranslator PRIVATE ${OpenCV_LIBS})

add_executable(instructions_cli apps/instructions_cli.cpp)
target_link_libraries(instructions_cli PRIVATE MoveTranslator ${OpenCV_LIBS})

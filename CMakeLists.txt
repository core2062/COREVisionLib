cmake_minimum_required(VERSION 3.6)
project(COREVisionLib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

find_package(OpenCV COMPONENTS core imgproc highgui videoio)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cpp src/CORECapture.cpp src/CORECapture.h)
add_executable(COREVisionLib ${SOURCE_FILES} src/CORECapture.cpp src/CORECapture.h)
target_link_libraries(COREVisionLib ${OpenCV_LIBS})
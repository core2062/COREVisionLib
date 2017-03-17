cmake_minimum_required(VERSION 3.6)
project(COREVisionLib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV COMPONENTS core imgproc highgui videoio)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/cscore/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/NetworkTables/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/llvm/include)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    link_directories(${PROJECT_SOURCE_DIR}/3rdparty/cscore/Linux/arm-raspbian)
    link_directories(${PROJECT_SOURCE_DIR}/3rdparty/NetworkTables/Linux/arm-raspbian)
else()
    link_directories(${PROJECT_SOURCE_DIR}/3rdparty/cscore/Linux/amd64)
    link_directories(${PROJECT_SOURCE_DIR}/3rdparty/NetworkTables/Linux/amd64)
endif()

set(SOURCE_FILES main.cpp #[[src/CORECapture.cpp src/CORECapture.h]])
add_executable(COREVisionLib ${SOURCE_FILES})

target_link_libraries(COREVisionLib cscore.so ntcore.so ${OpenCV_LIBS})
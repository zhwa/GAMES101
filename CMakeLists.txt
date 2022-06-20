cmake_minimum_required(VERSION 3.20)
project(GAMES101)

include(FetchContent)

cmake_path(APPEND THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR} thirdparty)

# Set up Eigen
FetchContent_Declare(eigen
    URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip
    SOURCE_DIR ${THIRDPARTY_DIR}/eigen)
FetchContent_Populate(eigen) # Do not use FetchContext_MakeAvailable, otherwise Eigen's test programs will be added into the Solution

# Set up OpenCV
FetchContent_Declare(opencv
    URL https://github.com/opencv/opencv/releases/download/4.6.0/opencv-4.6.0-vc14_vc15.exe
    SOURCE_DIR ${THIRDPARTY_DIR}/opencv)
FetchContent_MakeAvailable(opencv)

# OpenCV Path
set(OPENCV_INCLUDE_DIR ${opencv_SOURCE_DIR}/build/include)
set(OPENCV_BIN_DIR ${opencv_SOURCE_DIR}/build/x64/vc15/bin)
set(OPENCV_LIB_DIR ${opencv_SOURCE_DIR}/build/x64/vc15/lib)

set(USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # Statically link to MSVC run-time library
cmake_path(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" "bin") # Use the same output directory for the whole project

add_subdirectory(hw0)
add_subdirectory(hw1)
add_subdirectory(hw2)
add_subdirectory(hw3)
add_subdirectory(hw4)
add_subdirectory(hw5)
add_subdirectory(hw6)
add_subdirectory(hw7)
add_subdirectory(hw8)
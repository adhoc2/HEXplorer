# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(HEXplorer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#display compiler name
message(STATUS "Compiler being used: ${CMAKE_CXX_COMPILER_ID} - ${CMAKE_CXX_COMPILER} ")
get_filename_component(compiler_path ${CMAKE_CXX_COMPILER} DIRECTORY)
message(STATUS "Compiler directory: ${compiler_path}")

# Find OpenMP
find_package(OpenMP REQUIRED)
message(STATUS "OpenMP_CXX_FOUND: ${OpenMP_CXX_FOUND}")
message(STATUS "OpenMP_CXX_FLAGS: ${OpenMP_CXX_FLAGS}")
message(STATUS "OpenMP_CXX_LIBRARIES: ${OpenMP_CXX_LIBRARIES}")

# Add the source directory
add_subdirectory(src)

# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(HEXplorer VERSION 1.1.0 LANGUAGES CXX)

# Configure a header file to pass the version number to the source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.ini
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h
    @ONLY
)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Specify the required C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source directory
add_subdirectory(src)
#add_subdirectory(installer)

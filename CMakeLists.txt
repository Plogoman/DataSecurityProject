cmake_minimum_required(VERSION 4.3)
project(DataSecurityProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DataSecurityProject main.cpp
        src/DES.cpp
        src/HexUtilities.cpp
        src/CLI.cpp
)

target_include_directories(DataSecurityProject PRIVATE include)

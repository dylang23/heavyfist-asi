cmake_minimum_required(VERSION 3.15)
project(HeavyFist CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(HeavyFist SHARED main.cpp)

# Incluye la carpeta local del proyecto donde guardaste los archivos de lemon
target_include_directories(HeavyFist PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(HeavyFist PROPERTIES 
    SUFFIX ".asi"
    OUTPUT_NAME "HeavyFist"
)

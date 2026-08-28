cmake_minimum_required(VERSION 3.15)
project(HeavyFist CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(HeavyFist SHARED main.cpp)

set_target_properties(HeavyFist PROPERTIES 
    SUFFIX ".asi"
    OUTPUT_NAME "HeavyFist"
)

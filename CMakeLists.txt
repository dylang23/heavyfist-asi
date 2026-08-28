
cmake_minimum_required(VERSION 3.10)
project(HeavyFist CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    lemon
    GIT_REPOSITORY https://github.com/Zeex/lemon.git
    GIT_TAG master
)
FetchContent_MakeAvailable(lemon)

add_library(HeavyFist SHARED main.cpp)
target_include_directories(HeavyFist PRIVATE ${lemon_SOURCE_DIR}/include)
set_target_properties(HeavyFist PROPERTIES SUFFIX ".asi")

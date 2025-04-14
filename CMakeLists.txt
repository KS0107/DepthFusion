cmake_minimum_required(VERSION 3.16)
project(DepthFusion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(include/se_orderbook) 

file(GLOB_RECURSE SOURCES
    "src/orderbook/*.cpp"
    "src/utils/*.cpp"
    "src/main.cpp"
)

add_executable(depthfusion ${SOURCES})

enable_testing()

add_subdirectory(external/googletest)
add_subdirectory(tests)
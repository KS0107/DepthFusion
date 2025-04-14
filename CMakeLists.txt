cmake_minimum_required(VERSION 3.16)
project(DepthFusion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include path for headers
include_directories(include)
include_directories(include/se_orderbook) 
# Source files
file(GLOB_RECURSE SOURCES
    "src/orderbook/*.cpp"
    "src/utils/*.cpp"
    "src/main.cpp"
)

add_executable(depthfusion ${SOURCES})
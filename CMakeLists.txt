cmake_minimum_required(VERSION 3.16)
project(DepthFusion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include project headers
include_directories(include)
include_directories(include/se_orderbook)
include_directories(include/se_orderbook/websocket)
include_directories(include/se_orderbook/feedhandlers)
include_directories(include/se_orderbook/core)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)
include_directories(${CMAKE_SOURCE_DIR}/src)
# Homebrew includes for WebSocket++ and OpenSSL
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

# WebSocket++ and OpenSSL
find_package(OpenSSL REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES
    "src/orderbook/*.cpp"
    "src/utils/*.cpp"
    "src/websocket/*.cpp"
    "src/main.cpp"
    "src/parsers/*.cpp" 
    "src/core/*.cpp"
    "src/feedhandlers/*.cpp"
)

# Build main binary
add_executable(depthfusion ${SOURCES})
target_link_libraries(depthfusion OpenSSL::SSL OpenSSL::Crypto pthread)

# Enable testing
enable_testing()
add_subdirectory(external/googletest)
add_subdirectory(tests)
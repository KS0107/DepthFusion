cmake_minimum_required(VERSION 3.16)
project(DepthFusion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include project headers
include_directories(include)
include_directories(include/se_orderbook)
include_directories(include/se_orderbook/websocket)

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
)

# Build main binary
add_executable(depthfusion ${SOURCES})
target_link_libraries(depthfusion OpenSSL::SSL OpenSSL::Crypto pthread)

# Enable testing
enable_testing()
add_subdirectory(external/googletest)
add_subdirectory(tests)
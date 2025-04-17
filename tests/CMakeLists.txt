file(GLOB TEST_SOURCES "*.cpp")

set(CORE_SOURCES
    ${PROJECT_SOURCE_DIR}/src/orderbook/OrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/orderbook/OrderBookSide.cpp
    ${PROJECT_SOURCE_DIR}/src/orderbook/AggregatedOrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/orderbook/Tick.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/Logger.cpp
    ${PROJECT_SOURCE_DIR}/src/core/OrderBookManager.cpp
    ${PROJECT_SOURCE_DIR}/src/parsers/BinanceDepthParser.cpp
    ${PROJECT_SOURCE_DIR}/src/websocket/BinanceWebSocketClient.cpp
)

add_executable(runTests ${TEST_SOURCES} ${CORE_SOURCES})

find_package(OpenSSL REQUIRED)

target_link_libraries(runTests gtest gtest_main OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(runTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(runTests)
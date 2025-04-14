file(GLOB TEST_SOURCES "*.cpp")

set(CORE_SOURCES
    ${PROJECT_SOURCE_DIR}/src/orderbook/OrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/orderbook/OrderBookSide.cpp
    ${PROJECT_SOURCE_DIR}/src/orderbook/AggregatedOrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/orderbook/Tick.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/Logger.cpp
)

add_executable(runTests ${TEST_SOURCES} ${CORE_SOURCES})
target_link_libraries(runTests gtest gtest_main)
target_include_directories(runTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(runTests)
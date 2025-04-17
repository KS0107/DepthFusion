#include <gtest/gtest.h>
#include "se_orderbook/core/OrderBookManager.hpp"
#include <string>
#include <chrono>
#include <thread>

TEST(OrderBookManagerTest, RegistersCorrectBooksForPairs) {
    OrderBookManager manager;
    manager.add_pair("ethusdt");
    manager.add_pair("btcusdt");

    testing::internal::CaptureStdout();
    manager.start();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    manager.stop();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("OrderBook [Binance] ETHUSDT"), std::string::npos);
    EXPECT_NE(output.find("OrderBook [Binance] BTCUSDT"), std::string::npos);
}
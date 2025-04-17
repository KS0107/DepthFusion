#include "AggregatedOrderBook.hpp"
#include "se_orderbook/websocket/BinanceWebSocketClient.hpp"
#include "se_orderbook/parsers/BinanceDepthParser.hpp"
#include "OrderBook.hpp"
#include "se_orderbook/core/OrderBookManager.hpp"
#include <iostream>
#include <memory>
#include <csignal>

std::atomic<bool> stop_signal = false;

void signal_handler(int) {
    stop_signal = true;
}

int main() {
    std::signal(SIGINT, signal_handler);
    OrderBookManager manager;

    manager.add_pair("btcusdt");
    manager.add_pair("ethusdt");
    manager.add_pair("solusdt");

    manager.start();

    while (!stop_signal) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    std::cout << "\n[Main] Shutting down...\n";
    manager.stop();

    return 0;
}
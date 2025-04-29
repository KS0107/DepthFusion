#include "AggregatedOrderBook.hpp"
#include "se_orderbook/core/OrderBookManager.hpp"
#include "se_orderbook/feedhandlers/BinanceFeedHandler.hpp"
#include <iostream>
#include <memory>
#include <csignal>
#include <thread>

std::atomic<bool> stop_signal = false;

void signal_handler(int) {
    stop_signal = true;
}

int main() {
    std::signal(SIGINT, signal_handler);

    OrderBookManager manager;

    manager.add_feed_handler(std::make_unique<BinanceFeedHandler>(
        std::vector<std::string>{"btcusdt", "ethusdt", "solusdt"}, 
        manager.get_aggregated_order_book()
    ));

    manager.start();

    std::thread print_thread([&]() {
        while (!stop_signal) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "\n=== Aggregated Order Books ===\n";
            manager.print_all();
        }
    });
    while (!stop_signal) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\n[Main] Shutting down...\n";
    manager.stop();
    print_thread.join();

    std::cout << "[Main] Clean exit.\n";
    return 0;
}
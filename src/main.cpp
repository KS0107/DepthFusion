#include "AggregatedOrderBook.hpp"
#include "se_orderbook/core/OrderBookManager.hpp"
#include "se_orderbook/feedhandlers/BinanceFeedHandler.hpp"
#include "se_orderbook/feedhandlers/KrakenFeedHandler.hpp"
#include <iostream>
#include <memory>
#include <csignal>
#include <optional>
#include <thread>

std::atomic<bool> stop_signal = false;

void signal_handler(int) {
    stop_signal = true;
}

int main() {
    std::signal(SIGINT, signal_handler);

    OrderBookManager manager;

    // Attach Binance Feed Handler
    manager.add_feed_handler(std::make_unique<BinanceFeedHandler>(
        std::vector<std::string>{"btcusdt"},
        manager.get_aggregated_order_book()
    ));

    // Attach Kraken Feed Handler
    manager.add_feed_handler(std::make_unique<KrakenFeedHandler>(
        std::vector<std::string>{"BTC/USDT"},  
        manager.get_aggregated_order_book()
    ));

    manager.start();

    while (!stop_signal) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\n================ Aggregated Order Book Snapshot ================\n";
        manager.print_all();
    }

    std::cout << "\n[Main] Shutting down...\n";
    manager.stop();

    return 0;
}
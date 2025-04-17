#include "se_orderbook/core/OrderBookManager.hpp"
#include "se_orderbook/parsers/BinanceDepthParser.hpp"
#include <algorithm>
#include <iostream>
#include "OrderBook.hpp"

OrderBookManager::OrderBookManager() : running_(false) {}

OrderBookManager:: ~OrderBookManager() {
    stop();
}

void OrderBookManager::add_pair(const std::string& pair) {
    std::string upper = pair;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    pairs_.push_back(pair);
    agg_.register_orderbook("Binance_" + upper, std::make_unique<OrderBook>("Binance", upper));
}

void OrderBookManager::start() {
    if (pairs_.empty()) return;

    std::string combined_stream;
    for (const auto& p : pairs_) {
        if (!combined_stream.empty()) combined_stream += "/";
        combined_stream += p + "@depth@100ms";
    }

    std::string uri = "wss://stream.binance.com:9443/stream?streams=" + combined_stream;

    client_ = std::make_unique<BinanceWebSocketClient>(uri, [&](const std::string& msg) {
        auto updates = BinanceDepthParser::parse(msg);
        for (const auto& u : updates) {
            agg_.apply_update(u);
        }
        static int counter = 0;
        if ((++counter % 10) == 0 && !updates.empty()) {
            print_all();
        }
    });

    client_->connect();
    running_ = true;
    ws_thread_ = std::thread([&]() { client_->run(); });
}

void OrderBookManager::stop() {
    if (running_) {
        client_->disconnect();
        if (ws_thread_.joinable()) ws_thread_.join();
        running_ = false;
    }
}

void OrderBookManager::print_all() const {
    std::cout << agg_;
}
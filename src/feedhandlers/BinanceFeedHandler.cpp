// src/websocket/BinanceFeedHandler.cpp
#include "se_orderbook/feedhandlers/BinanceFeedHandler.hpp"
#include "se_orderbook/parsers/BinanceDepthParser.hpp"
#include "OrderBook.hpp"
#include <algorithm>
#include <iostream>

BinanceFeedHandler::BinanceFeedHandler(const std::vector<std::string>& pairs, AggregatedOrderBook& agg)
    : agg_(agg), pairs_(pairs), running_(false) {
    for (const auto& pair : pairs_) {
        std::string upper = pair;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        agg_.register_orderbook("Binance_" + upper, std::make_unique<OrderBook>("Binance", upper));
    }
}

void BinanceFeedHandler::start() {
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
    });

    client_->connect();
    running_ = true;
    ws_thread_ = std::thread([&]() { client_->run(); });
}

void BinanceFeedHandler::stop() {
    if (running_) {
        client_->disconnect();
        if (ws_thread_.joinable()) ws_thread_.join();
        running_ = false;
    }
}
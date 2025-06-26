#include "se_orderbook/feedhandlers/KrakenFeedHandler.hpp"
#include "se_orderbook/parsers/KrakenDepthParser.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <OrderBook.hpp>

KrakenFeedHandler::KrakenFeedHandler(const std::vector<std::string>& pairs, AggregatedOrderBook& agg)
    : agg_(agg), pairs_(pairs), running_(false) {
        for (const auto& pair : pairs_) {
            std::string upper = pair;
            std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
            agg_.register_orderbook("Kraken_" + upper, std::make_unique<OrderBook>("Kraken", upper));
        }
    }

void KrakenFeedHandler::start() {
    if (pairs_.empty()) return;

    client_ = std::make_unique<KrakenWebSocketClient>(
        "wss://ws.kraken.com/v2",
        [&](const std::string& msg) {
            auto updates = KrakenDepthParser::parse(msg);
            for (const auto& u : updates) {
                agg_.apply_update(u);
            }
        }
    );
    client_->set_open_handler([this]() {
        this->subscribe_to_pairs();
    });

    client_->connect();
    running_ = true;

    ws_thread_ = std::thread([this]() {
        client_->run();
    });

}

void KrakenFeedHandler::stop() {
    if (running_) {
        client_->disconnect();
        if (ws_thread_.joinable()) {
            ws_thread_.join();
        }
        running_ = false;
    }
}

void KrakenFeedHandler::subscribe_to_pairs() {
    if (!client_->is_connected()) {
        std::cerr << "[KrakenFeedHandler] Tried to subscribe, but not connected yet.\n";
        return;
    }

    nlohmann::json subscribe_message;
    subscribe_message["method"] = "subscribe";

    nlohmann::json params;
    params["channel"] = "book";
    params["symbol"] = pairs_; 

    subscribe_message["params"] = params;

    std::string msg = subscribe_message.dump();
    client_->subscribe(msg);

    std::cout << "[KrakenFeedHandler] Sent subscription: " << msg << std::endl;
}
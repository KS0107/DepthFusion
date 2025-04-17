#include "AggregatedOrderBook.hpp"
#include "se_orderbook/websocket/BinanceWebSocketClient.hpp"
#include "se_orderbook/parsers/BinanceDepthParser.hpp"
#include "OrderBook.hpp"
#include <iostream>
#include <memory>
#include <csignal>

std::atomic<bool> stop_signal = false;

void signal_handler(int) {
    stop_signal = true;
}

int main() {
    std::signal(SIGINT, signal_handler);
    AggregatedOrderBook agg;
    agg.register_orderbook("Binance", std::make_unique<OrderBook>("Binance", "BTCUSDT"));
    static int counter = 0;
    BinanceWebSocketClient client(
        "wss://stream.binance.com:9443/ws/btcusdt@depth@100ms",
        [&](const std::string& msg) {
            auto updates = BinanceDepthParser::parse(msg);
            for (const auto& u : updates) {
                agg.apply_update(u);
            }
            if ((++counter % 10) == 0) {
                if (!updates.empty()) {
                    std::cout << "C\n";
                    std::cout << agg;
                }
            }
            
        }
    );

    client.subscribe("btcusdt@depth@100ms");
    client.connect();

    std::thread ws_thread([&]() {
        client.run();
    });

    while (!stop_signal) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\n[Main] Shutting down...\n";
    client.disconnect();
    ws_thread.join();
    std::cout << "[Main] Clean exit.\n";

    return 0;
}
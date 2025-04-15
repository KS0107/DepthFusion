#include "AggregatedOrderBook.hpp"
#include "se_orderbook/websocket/BinanceWebSocketClient.hpp"
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

    BinanceWebSocketClient client(
        "wss://stream.binance.com:9443/ws",
        [](const std::string& msg) {
            std::cout << "[Binance WS] " << msg << "\n";
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
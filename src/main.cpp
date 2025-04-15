#include "AggregatedOrderBook.hpp"
#include "se_orderbook/websocket/BinanceWebSocketClient.hpp"
#include "OrderBook.hpp"
#include <iostream>
#include <memory>

int main() {
    BinanceWebSocketClient client(
        "wss://stream.binance.com:9443/ws",
        [](const std::string& msg) {
            std::cout << "[Binance WS] " << msg << "\n";
        }
    );
    client.subscribe("btcusdt@depth@100ms");
    client.connect();
    

    client.run(); 

    return 0;
}
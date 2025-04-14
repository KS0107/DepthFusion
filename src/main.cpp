#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"
#include <iostream>

int main() {
    AggregatedOrderBook agg;
    auto binance = std::make_unique<OrderBook>("Binance", "BTCUSDT");
    auto deribit = std::make_unique<OrderBook>("Deribit", "BTCUSDT");

    agg.register_orderbook("Binance", std::move(binance));
    agg.register_orderbook("Deribit", std::move(deribit));

    agg.apply_update({"Binance", "BTCUSDT", Side::Bid, 27300.0, 1.5, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Bid, 27250.0, 2.0, false});
    agg.apply_update({"Binance", "BTCUSDT", Side::Ask, 27350.0, 0.8, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Ask, 27340.0, 1.2, false});

    std::cout << "---- Aggregated Bids ----\n";
    auto top_bids = agg.get_top_n(Side::Bid, 5);
    for (const auto& level : top_bids) {
        std::cout << "Price: " << level.price << ", Qty: " << level.quantity << "\n";
    }

    std::cout << "\n---- Aggregated Asks ----\n";
    auto top_asks = agg.get_top_n(Side::Ask, 5);
    for (const auto& level : top_asks) {
        std::cout << "Price: " << level.price << ", Qty: " << level.quantity << "\n";
    }

    return 0;
}
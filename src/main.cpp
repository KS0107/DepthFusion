#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"
#include <iostream>
#include <memory>

int main() {
    AggregatedOrderBook agg;

    // Create and register two books for BTCUSDT
    auto binance_btc = std::make_unique<OrderBook>("Binance", "BTCUSDT");
    auto deribit_btc = std::make_unique<OrderBook>("Deribit", "BTCUSDT");

    agg.register_orderbook("Binance", std::move(binance_btc));
    agg.register_orderbook("Deribit", std::move(deribit_btc));

    // Apply updates to both exchanges
    agg.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.2, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Bid, 26950.0, 1.8, false});
    agg.apply_update({"Binance", "BTCUSDT", Side::Ask, 27100.0, 0.9, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Ask, 27200.0, 1.1, false});

    // Print individual books
    std::cout << "=== INDIVIDUAL BOOKS ===\n";
    std::cout << agg << "\n";

    // Get and print aggregated view for BTCUSDT
    std::cout << "=== AGGREGATED TOP BIDS ===\n";
    auto top_bids = agg.get_top_n(Side::Bid, 3);
    for (const auto& entry : top_bids) {
        std::cout << "Bid: " << entry.price << " @ " << entry.quantity << "\n";
    }

    std::cout << "=== AGGREGATED TOP ASKS ===\n";
    auto top_asks = agg.get_top_n(Side::Ask, 3);
    for (const auto& entry : top_asks) {
        std::cout << "Ask: " << entry.price << " @ " << entry.quantity << "\n";
    }

    return 0;
}
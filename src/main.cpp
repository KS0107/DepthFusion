#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"
#include <iostream>

bool check_level(const OrderEntry& entry, double expected_price, double expected_qty) {
    return entry.price == expected_price && entry.quantity == expected_qty;
}

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
    bool bids_correct = top_bids.size() == 2 &&
                        check_level(top_bids[0], 27300.0, 1.5) &&
                        check_level(top_bids[1], 27250.0, 2.0);

    for (const auto& level : top_bids) {
        std::cout << "Price: " << level.price << ", Qty: " << level.quantity << "\n";
    }
    std::cout << (bids_correct ? "[PASS] get_top_n(Bid)\n" : "[FAIL] get_top_n(Bid)\n");

    std::cout << "\n---- Aggregated Asks ----\n";
    auto top_asks = agg.get_top_n(Side::Ask, 5);
    bool asks_correct = top_asks.size() == 2 &&
                        check_level(top_asks[0], 27340.0, 1.2) &&
                        check_level(top_asks[1], 27350.0, 0.8);

    for (const auto& level : top_asks) {
        std::cout << "Price: " << level.price << ", Qty: " << level.quantity << "\n";
    }
    std::cout << (asks_correct ? "[PASS] get_top_n(Ask)\n" : "[FAIL] get_top_n(Ask)\n");

    std::cout << "\n---- Per-exchange Top-N ----\n";
    auto all_bids = agg.get_all_top_n(Side::Bid, 1);
    bool per_exchange_ok = all_bids["Binance"].size() == 1 &&
                           check_level(all_bids["Binance"][0], 27300.0, 1.5) &&
                           all_bids["Deribit"].size() == 1 &&
                           check_level(all_bids["Deribit"][0], 27250.0, 2.0);

    for (const auto& [exchange, entries] : all_bids) {
        std::cout << exchange << " Top Bid: ";
        for (const auto& entry : entries) {
            std::cout << entry.price << "@" << entry.quantity << " ";
        }
        std::cout << "\n";
    }

    std::cout << (per_exchange_ok ? "[PASS] get_all_top_n(Bid)\n" : "[FAIL] get_all_top_n(Bid)\n");

    return 0;
}
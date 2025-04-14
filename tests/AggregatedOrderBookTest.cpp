#include <gtest/gtest.h>
#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"

TEST(AggregatedOrderBookTest, RegistersAndUpdatesMultipleBooks) {
    AggregatedOrderBook agg;

    auto binance = std::make_unique<OrderBook>("Binance", "BTCUSDT");
    auto deribit = std::make_unique<OrderBook>("Deribit", "BTCUSDT");

    agg.register_orderbook("Binance", std::move(binance));
    agg.register_orderbook("Deribit", std::move(deribit));

    agg.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.0, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Bid, 27100.0, 2.0, false});
    agg.apply_update({"Binance", "BTCUSDT", Side::Ask, 27200.0, 1.5, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Ask, 27300.0, 2.5, false});

    auto top_bids = agg.get_top_n(Side::Bid, 2);
    ASSERT_EQ(top_bids.size(), 2);
    EXPECT_EQ(top_bids[0].price, 27100.0);  // Deribit
    EXPECT_EQ(top_bids[1].price, 27000.0);  // Binance

    auto top_asks = agg.get_top_n(Side::Ask, 2);
    ASSERT_EQ(top_asks.size(), 2);
    EXPECT_EQ(top_asks[0].price, 27200.0);  // Binance
    EXPECT_EQ(top_asks[1].price, 27300.0);  // Deribit
}

TEST(AggregatedOrderBookTest, GetAllTopNReturnsPerExchangeData) {
    AggregatedOrderBook agg;

    auto binance = std::make_unique<OrderBook>("Binance", "BTCUSDT");
    auto deribit = std::make_unique<OrderBook>("Deribit", "BTCUSDT");

    agg.register_orderbook("Binance", std::move(binance));
    agg.register_orderbook("Deribit", std::move(deribit));

    agg.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.0, false});
    agg.apply_update({"Deribit", "BTCUSDT", Side::Bid, 27100.0, 2.0, false});

    auto all_bids = agg.get_all_top_n(Side::Bid, 1);

    ASSERT_EQ(all_bids.size(), 2);
    EXPECT_EQ(all_bids["Binance"][0].price, 27000.0);
    EXPECT_EQ(all_bids["Deribit"][0].price, 27100.0);
}

TEST(AggregatedOrderBookTest, AggregatedTopNTruncatesCorrectly) {
    AggregatedOrderBook agg;

    auto binance = std::make_unique<OrderBook>("Binance", "BTCUSDT");
    auto deribit = std::make_unique<OrderBook>("Deribit", "BTCUSDT");

    agg.register_orderbook("Binance", std::move(binance));
    agg.register_orderbook("Deribit", std::move(deribit));

    for (int i = 0; i < 3; ++i) {
        agg.apply_update({"Binance", "BTCUSDT", Side::Bid, static_cast<double>(27000 + i), 1.0, false});
        agg.apply_update({"Deribit", "BTCUSDT", Side::Bid, static_cast<double>(27010 + i), 2.0, false});
    }

    auto top = agg.get_top_n(Side::Bid, 4);
    ASSERT_EQ(top.size(), 4);
    EXPECT_EQ(top[0].price, 27012);
    EXPECT_EQ(top[1].price, 27011);
    EXPECT_EQ(top[2].price, 27010);
    EXPECT_EQ(top[3].price, 27002);
}

TEST(AggregatedOrderBookTest, IgnoresUnknownExchangeUpdate) {
    AggregatedOrderBook agg;

    auto binance = std::make_unique<OrderBook>("Binance", "BTCUSDT");
    agg.register_orderbook("Binance", std::move(binance));

    // Deribit not registered — should be ignored
    agg.apply_update({"Deribit", "BTCUSDT", Side::Bid, 27100.0, 2.0, false});

    auto bids = agg.get_top_n(Side::Bid, 1);
    EXPECT_TRUE(bids.empty());
}
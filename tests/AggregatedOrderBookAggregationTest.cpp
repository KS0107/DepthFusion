#include <gtest/gtest.h>
#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"

TEST(AggregatedOrderBookAggregationTest, AggregatesAcrossExchangesCorrectly) {
    AggregatedOrderBook agg;

    agg.register_orderbook("Binance_btcusdt", std::make_unique<OrderBook>("Binance", "btcusdt"));
    agg.register_orderbook("Kraken_btcusdt", std::make_unique<OrderBook>("Kraken", "btcusdt"));

    agg.apply_update({"Binance_btcusdt", "btcusdt", Side::Bid, 27000.0, 1.0, false});
    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Bid, 27050.0, 1.5, false});

    auto bids = agg.get_aggregated_top_n("btcusdt", Side::Bid, 2);
    ASSERT_EQ(bids.size(), 2);
    EXPECT_EQ(bids[0].price, 27050.0);
    EXPECT_EQ(bids[1].price, 27000.0);
}

TEST(AggregatedOrderBookAggregationTest, AggregatedAsksSortedAscending) {
    AggregatedOrderBook agg;

    agg.register_orderbook("Binance_btcusdt", std::make_unique<OrderBook>("Binance", "btcusdt"));
    agg.register_orderbook("Kraken_btcusdt", std::make_unique<OrderBook>("Kraken", "btcusdt"));

    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Ask, 27100.0, 1.0, false});
    agg.apply_update({"Binance_btcusdt", "btcusdt", Side::Ask, 27090.0, 2.0, false});

    auto asks = agg.get_aggregated_top_n("btcusdt", Side::Ask, 2);
    ASSERT_EQ(asks.size(), 2);
    EXPECT_EQ(asks[0].price, 27090.0);
    EXPECT_EQ(asks[1].price, 27100.0);
}

TEST(AggregatedOrderBookAggregationTest, TruncatesToTopNLevels) {
    AggregatedOrderBook agg;

    agg.register_orderbook("Binance_btcusdt", std::make_unique<OrderBook>("Binance", "btcusdt"));
    agg.register_orderbook("Kraken_btcusdt", std::make_unique<OrderBook>("Kraken", "btcusdt"));

    // Add 6 total levels, but ask for top 3
    agg.apply_update({"Binance_btcusdt", "btcusdt", Side::Bid, 100.0, 1.0, false});
    agg.apply_update({"Binance_btcusdt", "btcusdt", Side::Bid, 99.0, 1.0, false});
    agg.apply_update({"Binance_btcusdt", "btcusdt", Side::Bid, 98.0, 1.0, false});
    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Bid, 97.0, 1.0, false});
    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Bid, 96.0, 1.0, false});
    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Bid, 95.0, 1.0, false});

    auto bids = agg.get_aggregated_top_n("btcusdt", Side::Bid, 3);
    ASSERT_EQ(bids.size(), 3);
    EXPECT_EQ(bids[0].price, 100.0);
    EXPECT_EQ(bids[2].price, 98.0);
}

TEST(AggregatedOrderBookAggregationTest, FiltersBySymbol) {
    AggregatedOrderBook agg;

    agg.register_orderbook("Binance_btcusdt", std::make_unique<OrderBook>("Binance", "btcusdt"));
    agg.register_orderbook("Binance_ethusdt", std::make_unique<OrderBook>("Binance", "ethusdt"));

    agg.apply_update({"Binance_btcusdt", "btcusdt", Side::Ask, 100.0, 1.0, false});
    agg.apply_update({"Binance_ethusdt", "ethusdt", Side::Ask, 200.0, 1.0, false});

    auto btc = agg.get_aggregated_top_n("btcusdt", Side::Ask, 1);
    auto eth = agg.get_aggregated_top_n("ethusdt", Side::Ask, 1);

    ASSERT_EQ(btc.size(), 1);
    ASSERT_EQ(eth.size(), 1);
    EXPECT_EQ(btc[0].price, 100.0);
    EXPECT_EQ(eth[0].price, 200.0);
}

TEST(AggregatedOrderBookAggregationTest, SkipsZeroQuantityLevels) {
    AggregatedOrderBook agg;

    agg.register_orderbook("Kraken_btcusdt", std::make_unique<OrderBook>("Kraken", "btcusdt"));
    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Bid, 100.0, 1.0, false});
    agg.apply_update({"Kraken_btcusdt", "btcusdt", Side::Bid, 100.0, 0.0, false}); // Cancel

    auto bids = agg.get_aggregated_top_n("btcusdt", Side::Bid, 5);
    EXPECT_TRUE(bids.empty());
}

TEST(AggregatedOrderBookAggregationTest, IgnoresUnregisteredExchangeInAggregation) {
    AggregatedOrderBook agg;

    // No books registered
    agg.apply_update({"FakeExchange_btcusdt", "btcusdt", Side::Ask, 9000.0, 1.0, false});

    auto asks = agg.get_aggregated_top_n("btcusdt", Side::Ask, 3);
    EXPECT_TRUE(asks.empty());
}
#include <gtest/gtest.h>
#include "se_orderbook/core/OrderBookManager.hpp"
#include "se_orderbook/feedhandlers/BinanceFeedHandler.hpp"
#include "OrderBook.hpp"

TEST(OrderBookManagerTest, RegistersCorrectBooksForPairs) {
    AggregatedOrderBook agg;
    BinanceFeedHandler handler(
        std::vector<std::string>{"ethusdt", "btcusdt"}, 
        agg
    );

    // Check that books were registered
    auto all_bids = agg.get_all_top_n(Side::Bid, 1);
    EXPECT_TRUE(all_bids.find("Binance_ethusdt") != all_bids.end());
    EXPECT_TRUE(all_bids.find("Binance_btcusdt") != all_bids.end());

    auto all_asks = agg.get_all_top_n(Side::Ask, 1);
    EXPECT_TRUE(all_asks.find("Binance_ethusdt") != all_asks.end());
    EXPECT_TRUE(all_asks.find("Binance_btcusdt") != all_asks.end());
}
#include <gtest/gtest.h>
#include "OrderBook.hpp"

TEST(OrderBookTest, ApplyUpdateAndGetTopN) {
    OrderBook book("Binance", "BTCUSDT");

    NormalizedOrderUpdate update1{"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.5, false};
    NormalizedOrderUpdate update2{"Binance", "BTCUSDT", Side::Bid, 26900.0, 2.0, false};

    book.apply_update(update1);
    book.apply_update(update2);

    auto top_bids = book.get_top_n(Side::Bid, 2);

    ASSERT_EQ(top_bids.size(), 2);
    EXPECT_EQ(top_bids[0].price, 27000.0);
    EXPECT_EQ(top_bids[0].quantity, 1.5);
    EXPECT_EQ(top_bids[1].price, 26900.0);
    EXPECT_EQ(top_bids[1].quantity, 2.0);
}
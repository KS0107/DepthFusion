#include <gtest/gtest.h>
#include "OrderBook.hpp"

TEST(OrderBookTest, ApplyIncrementalUpdatesBidAsk) {
    OrderBook book("Binance", "BTCUSDT");

    // Apply updates
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.5, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Ask, 27100.0, 0.8, false});

    auto bids = book.get_top_n(Side::Bid, 1);
    auto asks = book.get_top_n(Side::Ask, 1);

    ASSERT_EQ(bids.size(), 1);
    EXPECT_EQ(bids[0].price, 27000.0);
    EXPECT_EQ(bids[0].quantity, 1.5);

    ASSERT_EQ(asks.size(), 1);
    EXPECT_EQ(asks[0].price, 27100.0);
    EXPECT_EQ(asks[0].quantity, 0.8);
}

TEST(OrderBookTest, DeleteLevelWithZeroQuantity) {
    OrderBook book("Binance", "BTCUSDT");

    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.5, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 0.0, false});

    auto bids = book.get_top_n(Side::Bid, 1);
    EXPECT_TRUE(bids.empty());
}

TEST(OrderBookTest, SortOrderBidDescAskAsc) {
    OrderBook book("Binance", "BTCUSDT");

    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.0, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27100.0, 2.0, false});

    book.apply_update({"Binance", "BTCUSDT", Side::Ask, 27200.0, 1.0, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Ask, 27150.0, 2.0, false});

    auto bids = book.get_top_n(Side::Bid, 2);
    auto asks = book.get_top_n(Side::Ask, 2);

    ASSERT_EQ(bids[0].price, 27100.0);
    ASSERT_EQ(bids[1].price, 27000.0);

    ASSERT_EQ(asks[0].price, 27150.0);
    ASSERT_EQ(asks[1].price, 27200.0);
}

TEST(OrderBookTest, TopNTruncation) {
    OrderBook book("Binance", "BTCUSDT");

    for (int i = 0; i < 10; ++i) {
        book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0 + i, 1.0 + i, false});
    }

    auto top5 = book.get_top_n(Side::Bid, 5);
    ASSERT_EQ(top5.size(), 5);
    EXPECT_EQ(top5[0].price, 27009.0);
    EXPECT_EQ(top5[4].price, 27005.0);
}

TEST(OrderBookTest, EmptyBookReturnsNothing) {
    OrderBook book("Binance", "BTCUSDT");

    auto bids = book.get_top_n(Side::Bid, 3);
    EXPECT_TRUE(bids.empty());

    auto asks = book.get_top_n(Side::Ask, 3);
    EXPECT_TRUE(asks.empty());
}

TEST(OrderBookTest, SnapshotUpdateJustBehavesLikeReplace) {
    OrderBook book("Binance", "BTCUSDT");

    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 2.0, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 3.0, true}); // snapshot

    auto bids = book.get_top_n(Side::Bid, 1);
    ASSERT_EQ(bids[0].price, 27000.0);
    ASSERT_EQ(bids[0].quantity, 3.0);
}

TEST(OrderBookTest, TopNEdgeCases) {
    OrderBook book("Binance", "BTCUSDT");
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27000.0, 1.0, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 26900.0, 1.0, false});

    EXPECT_TRUE(book.get_top_n(Side::Bid, 0).empty());
    EXPECT_TRUE(book.get_top_n(Side::Bid, -1).empty());
    EXPECT_EQ(book.get_top_n(Side::Bid, 10).size(), 2);
}

TEST(OrderBookTest, MultipleUpdatesSamePriceLevel) {
    OrderBook book("Binance", "BTCUSDT");

    book.apply_update({"Binance", "BTCUSDT", Side::Ask, 27500.0, 1.0, false});
    book.apply_update({"Binance", "BTCUSDT", Side::Ask, 27500.0, 2.5, false});

    auto asks = book.get_top_n(Side::Ask, 1);
    ASSERT_EQ(asks.size(), 1);
    EXPECT_EQ(asks[0].price, 27500.0);
    EXPECT_EQ(asks[0].quantity, 2.5);
}

TEST(OrderBookTest, SnapshotThenIncrementalUpdates) {
    OrderBook book("Binance", "BTCUSDT");

    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27100.0, 3.0, true});
    
    book.apply_update({"Binance", "BTCUSDT", Side::Bid, 27100.0, 1.0, false});

    auto bids = book.get_top_n(Side::Bid, 1);
    ASSERT_EQ(bids.size(), 1);
    EXPECT_EQ(bids[0].price, 27100.0);
    EXPECT_EQ(bids[0].quantity, 1.0);
}
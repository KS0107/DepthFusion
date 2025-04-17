#include <gtest/gtest.h>
#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"
#include "se_orderbook/parsers/BinanceDepthParser.hpp"

TEST(BinanceDepthParserTest, ParsesValidDepthMessage) {
    std::string message = R"({
        "stream":"btcusdt@depth@100ms",
        "data":{
            "e":"depthUpdate",
            "s":"BTCUSDT",
            "b":[["50000.00", "1.0"]],
            "a":[["50010.00", "2.5"]]
        }
    })";

    auto updates = BinanceDepthParser::parse(message);
    ASSERT_EQ(updates.size(), 2);

    EXPECT_EQ(updates[0].side, Side::Bid);
    EXPECT_EQ(updates[0].price, 50000.0);
    EXPECT_EQ(updates[0].quantity, 1.0);
    EXPECT_EQ(updates[0].symbol, "BTCUSDT");

    EXPECT_EQ(updates[1].side, Side::Ask);
    EXPECT_EQ(updates[1].price, 50010.0);
    EXPECT_EQ(updates[1].quantity, 2.5);
}
// include/se_orderbook/websocket/BinanceFeedHandler.hpp
#ifndef SE_ORDERBOOK_BINANCEFEEDHANDLER_HPP
#define SE_ORDERBOOK_BINANCEFEEDHANDLER_HPP

#include "se_orderbook/core/IFeedHandler.hpp"
#include "BinanceWebSocketClient.hpp"
#include "se_orderbook/AggregatedOrderBook.hpp"

class BinanceFeedHandler : public IFeedHandler {
public:
    BinanceFeedHandler(const std::vector<std::string>& pairs, AggregatedOrderBook& agg);
    void start() override;
    void stop() override;
    std::string get_exchange_name() const override { return "Binance"; }

private:
    AggregatedOrderBook& agg_;
    std::unique_ptr<BinanceWebSocketClient> client_;
    std::vector<std::string> pairs_;
    std::thread ws_thread_;
    std::atomic<bool> running_;
};

#endif
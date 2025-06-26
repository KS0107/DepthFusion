#ifndef SE_ORDERBOOK_KRAKENFEEDHANDLER_HPP
#define SE_ORDERBOOK_KRAKENFEEDHANDLER_HPP

#include "se_orderbook/core/IFeedHandler.hpp"
#include "KrakenWebSocketClient.hpp"
#include "se_orderbook/AggregatedOrderBook.hpp"

class KrakenFeedHandler : public IFeedHandler {
public:
    KrakenFeedHandler(const std::vector<std::string>& pairs, AggregatedOrderBook& agg);
    void start() override;
    void stop() override;
    std::string get_exchange_name() const override { return "Kraken"; }

private:
    void subscribe_to_pairs(); 
    AggregatedOrderBook& agg_;
    std::unique_ptr<KrakenWebSocketClient> client_;
    std::vector<std::string> pairs_;
    std::thread ws_thread_;
    std::atomic<bool> running_;
};

#endif
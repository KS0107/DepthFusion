#ifndef SE_ORDERBOOK_MANAGER_HPP
#define SE_ORDERBOOK_MANAGER_HPP

#include "AggregatedOrderBook.hpp"
#include "BinanceWebSocketClient.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <thread>

class OrderBookManager {
public:
    OrderBookManager();
    ~OrderBookManager();

    void add_pair(const std::string& symbol);
    void start();
    void stop();
    void print_all() const;

private: 
    AggregatedOrderBook agg_;
    std::vector<std::string> pairs_;
    std::unique_ptr<BinanceWebSocketClient> client_;
    std::thread ws_thread_;
    std::atomic<bool> running_;
};

#endif
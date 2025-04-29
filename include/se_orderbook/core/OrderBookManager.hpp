#ifndef SE_ORDERBOOK_MANAGER_HPP
#define SE_ORDERBOOK_MANAGER_HPP

#include "AggregatedOrderBook.hpp"
#include "IFeedHandler.hpp"
#include <memory>
#include <vector>
#include <string>

class OrderBookManager {
public:
    OrderBookManager();
    ~OrderBookManager();

    void add_feed_handler(std::unique_ptr<IFeedHandler> handler);
    AggregatedOrderBook& get_aggregated_order_book();
    void start();
    void stop();
    void print_all() const;

private:
    AggregatedOrderBook agg_;
    std::vector<std::unique_ptr<IFeedHandler>> handlers_;
};

#endif
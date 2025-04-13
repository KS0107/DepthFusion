#ifndef SE_ORDERBOOK_ORDERBOOKUPDATE_HPP
#define SE_ORDERBOOK_ORDERBOOKUPDATE_HPP

#include <string>
#include <Side.hpp>
struct OrderBookUpdate {
    std::string exchange_name;
    std::string symbol;
    Side side;
    double price;
    double quantity;
    bool is_snapshot;
};

#endif
#ifndef SE_ORDERBOOK_NORMALIZEDORDERUPDATE_HPP
#define SE_ORDERBOOK_NORMALIZEDORDERUPDATE_HPP

#include <string>
#include "Side.hpp"
struct NormalizedOrderUpdate {
    std::string exchange_name;
    std::string symbol;
    Side side;
    double price;
    double quantity;
    bool is_snapshot;
};

#endif
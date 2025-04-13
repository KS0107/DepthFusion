#ifndef SE_ORDERBOOK_TICK_HPP
#define SE_ORDERBOOK_TICK_HPP

#include <chrono>
#include <string>
#include <Side.hpp>

struct Tick {
    std::chrono::system_clock::time_point timestamp;
    std::string exchange;
    std::string symbol;
    Side side;
    double price;
    double quantity;
};

#endif
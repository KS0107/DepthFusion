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

    Tick(const std::string& exchange,
         const std::string& symbol,
         Side side,
         double price,
         double quantity,
         std::chrono::system_clock::time_point ts = std::chrono::system_clock::now())
        : timestamp(ts), exchange(exchange), symbol(symbol),
          side(side), price(price), quantity(quantity) {}
};

#endif
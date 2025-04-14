#ifndef SE_ORDERBOOK_ORDERBOOKSIDE_HPP
#define SE_ORDERBOOK_ORDERBOOKSIDE_HPP

#include <map>
#include <vector>
#include "Side.hpp"
#include "OrderEntry.hpp"

class OrderBookSide {
public: 
    OrderBookSide(Side side);

    void update(double price, double quantity);
    std::vector<OrderEntry> get_top_n(int n) const;
    const std::map<double, double>& ask_levels() const;
    const std::map<double, double, std::greater<>>& bid_levels() const;

private:
    Side side;
    std::map<double, double, std::greater<>> bids_;
    std::map<double, double> asks_;
};
#endif
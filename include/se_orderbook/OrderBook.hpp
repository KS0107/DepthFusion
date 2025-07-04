#ifndef SE_ORDERBOOK_ORDERBOOK_HPP
#define SE_ORDERBOOK_ORDERBOOK_HPP

#include <string>
#include <ostream>
#include "IOrderBook.hpp"
#include "OrderBookSide.hpp"
#include "NormalizedOrderUpdate.hpp"

class OrderBook : public IOrderBook {
friend std::ostream& operator<<(std::ostream& os, const OrderBook& ob);
public:
    OrderBook(std::string exchange_name, std::string symbol);

    void apply_update(const NormalizedOrderUpdate& update) override;
    std::vector<OrderEntry> get_top_n(Side side, int n) const override;
    std::string get_exchange_name() const override;
    std::string get_symbol() const override;

private:
    std::string exchange_name_;
    std::string symbol_;
    OrderBookSide bids_;
    OrderBookSide asks_;
};

#endif
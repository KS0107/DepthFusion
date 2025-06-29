#ifndef SE_ORDERBOOK_IORDERBOOK_HPP
#define SE_ORDERBOOK_IORDERBOOK_HPP

#include <vector>
#include <string>
#include "Side.hpp"
#include "OrderEntry.hpp"

struct NormalizedOrderUpdate;

class IOrderBook {
public:
    virtual void apply_update(const NormalizedOrderUpdate& update) = 0;
    virtual std::vector<OrderEntry> get_top_n(Side side, int n) const = 0;
    virtual std::string get_exchange_name() const = 0;
    virtual std::string get_symbol() const = 0;
    virtual ~IOrderBook() = default;    
};

#endif
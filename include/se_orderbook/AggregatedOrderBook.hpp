#ifndef SE_ORDERBOOK_AGGREGATEDORDERBOOK_HPP
#define SE_ORDERBOOK_AGGREGATEDORDERBOOK_HPP

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "Side.hpp"
#include "OrderEntry.hpp"
#include "IOrderBook.hpp"

class AggregatedOrderBook {
public:
    void register_orderbook(const std::string& exchange, std::unique_ptr<IOrderBook> book);
    void apply_update(const NormalizedOrderUpdate& update);

    std::vector<OrderEntry> get_top_n(Side side, int n) const;
    std::unordered_map<std::string, std::vector<OrderEntry>> get_all_top_n(Side side, int n) const;
private:
    std::unordered_map<std::string, std::unique_ptr<IOrderBook>> books_;
};

#endif
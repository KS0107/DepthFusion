#ifndef SE_ORDERBOOK_AGGREGATEDORDERBOOK_HPP
#define SE_ORDERBOOK_AGGREGATEDORDERBOOK_HPP

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "Side.hpp"
#include "OrderEntry.hpp"
#include "IOrderBook.hpp"
#include <ostream>

class AggregatedOrderBook {
friend std::ostream& operator<<(std::ostream& os, const AggregatedOrderBook& aob);
public:
    void register_orderbook(const std::string& exchange, std::unique_ptr<IOrderBook> book);
    void apply_update(const NormalizedOrderUpdate& update);

    std::vector<OrderEntry> get_top_n(Side side, int n) const;
    std::unordered_map<std::string, std::vector<OrderEntry>> get_all_top_n(Side side, int n) const;
    std::vector<OrderEntry> get_aggregated_top_n(const std::string& symbol, Side side, int n) const;
    void print_aggregated_book(const std::string& symbol, int n) const;
private:
    std::unordered_map<std::string, std::unique_ptr<IOrderBook>> books_;
    std::unordered_map<std::string, std::vector<IOrderBook*>> symbol_to_books_;
};

#endif
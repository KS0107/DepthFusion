#include "AggregatedOrderBook.hpp"
#include "NormalizedOrderUpdate.hpp"

void AggregatedOrderBook::register_orderbook(const std::string& exchange, std::unique_ptr<IOrderBook> book) {
    books_[exchange] = std::move(book);
}

void AggregatedOrderBook::apply_update(const NormalizedOrderUpdate& update) {
    auto it = books_.find(update.exchange_name);
    if (it != books_.end()) {
        it->second->apply_update(update);
    }
}

std::vector<OrderEntry> AggregatedOrderBook::get_top_n(Side side, int n) const {
    std::vector<OrderEntry> merged;

    for (const auto& [_, book] : books_) {
        const auto top = book->get_top_n(side, n);
        merged.insert(merged.end(), top.begin(), top.end());
    }   

    if (side == Side::Bid) {
        std::sort(merged.begin(), merged.end(), [](const auto& a, const auto& b) {
            return a.price > b.price;
        });
    } else {
        std::sort(merged.begin(), merged.end(), [](const auto& a, const auto& b) {
            return a.price < b.price;
        });
    }

    if (merged.size() > static_cast<size_t>(n)) {
        merged.resize(n);
    }
    return merged;
} 

std::unordered_map<std::string, std::vector<OrderEntry>> AggregatedOrderBook::get_all_top_n(Side side, int n) const {
    std::unordered_map<std::string, std::vector<OrderEntry>> result;

    for (const auto& [exchange, book] : books_) {
        result[exchange] = book->get_top_n(side, n);
    }
    return result;
}


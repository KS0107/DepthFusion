#include "AggregatedOrderBook.hpp"
#include "NormalizedOrderUpdate.hpp"
#include "OrderBook.hpp"
#include <iostream>
void AggregatedOrderBook::register_orderbook(const std::string& key, std::unique_ptr<IOrderBook> book) {
    std::string symbol = book->get_symbol();
    symbol_to_books_[symbol].push_back(book.get());
    books_[key] = std::move(book);
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

std::vector<OrderEntry> AggregatedOrderBook::get_aggregated_top_n(const std::string& symbol, Side side, int n) const {
    if (side == Side::Bid) {
        std::map<double, double, std::greater<double>> merged;

        for (const auto& [_, book] : books_) {
            if (const auto* ob = dynamic_cast<const OrderBook*>(book.get())) {
                if (ob->get_symbol() == symbol) {
                    for (const auto& entry : ob->get_top_n(Side::Bid, n)) {
                        merged[entry.price] += entry.quantity;
                    }
                }
            }
        }

        std::vector<OrderEntry> result;
        for (const auto& [price, qty] : merged) {
            result.push_back({price, qty});
            if (result.size() >= static_cast<size_t>(n)) break;
        }
        return result;

    } else {
        std::map<double, double, std::less<double>> merged;

        for (const auto& [_, book] : books_) {
            if (const auto* ob = dynamic_cast<const OrderBook*>(book.get())) {
                if (ob->get_symbol() == symbol) {
                    for (const auto& entry : ob->get_top_n(Side::Ask, n)) {
                        merged[entry.price] += entry.quantity;
                    }
                }
            }
        }

        std::vector<OrderEntry> result;
        for (const auto& [price, qty] : merged) {
            result.push_back({price, qty});
            if (result.size() >= static_cast<size_t>(n)) break;
        }
        return result;
    }
}

std::unordered_map<std::string, std::vector<OrderEntry>> AggregatedOrderBook::get_all_top_n(Side side, int n) const {
    std::unordered_map<std::string, std::vector<OrderEntry>> result;
    

    for (const auto& [exchange, book] : books_) {
        result[exchange] = book->get_top_n(side, n);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const AggregatedOrderBook& aob) {
    for (const auto& [exchange, book] : aob.books_) { 
        if (const auto* concrete = dynamic_cast<const OrderBook*>(book.get())) {
            os << *concrete << "\n";
        } else {
            os << "[Unknown IOrderBook impl for " << exchange << "]\n";
        }
    }
    return os;
}

void AggregatedOrderBook::print_aggregated_book(const std::string& symbol, int n) const {
    auto bids = get_aggregated_top_n(symbol, Side::Bid, n);
    auto asks = get_aggregated_top_n(symbol, Side::Ask, n);

    std::cout << "===== Aggregated Order Book: " << symbol << " =====\n";
    std::cout << "--- Bids ---\n";
    for (const auto& b : bids) {
        std::cout << b.price << " @ " << b.quantity << "\n";
    }
    std::cout << "--- Asks ---\n";
    for (const auto& a : asks) {
        std::cout << a.price << " @ " << a.quantity << "\n";
    }
}
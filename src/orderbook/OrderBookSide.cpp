#include "OrderBookSide.hpp"

OrderBookSide::OrderBookSide(Side side) : side(side) {}

void OrderBookSide::update(double price, double quantity) {
    if (side == Side::Bid) {
        if (quantity > 0) {
            bids_[price] = quantity;
        } else {
            bids_.erase(price);
        }
    } else {
        if (quantity > 0) {
            asks_[price] = quantity;
        } else {
            asks_.erase(price);
        }
    }
}

std::vector<OrderEntry> OrderBookSide::get_top_n(int n) const {
    std::vector<OrderEntry> result;
    if (n <= 0) return result;
    if (side == Side::Bid) {
        for (const auto& [price, quantity] : bids_) {
            if (result.size() >= static_cast<size_t>(n)) break;
            result.push_back(OrderEntry{price, quantity});
        }
    } else {
        for (const auto& [price, quantity] : asks_) {
            if (result.size() >= static_cast<size_t>(n)) break;
            result.push_back(OrderEntry{price, quantity});
        }
    }
    return result;
}

const std::map<double, double>& OrderBookSide::ask_levels() const {
    return asks_;
}

const std::map<double, double, std::greater<>>& OrderBookSide::bid_levels() const {
    return bids_;
}
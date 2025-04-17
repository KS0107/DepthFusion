#include "OrderBookSide.hpp"

OrderBookSide::OrderBookSide(Side side) : side(side) {}

void OrderBookSide::update(double price, double quantity) {
    if (side == Side::Bid) {
        if (quantity <= 0.0) {
            bids_.erase(price);
        } else {
            bids_[price] = quantity;
        }
    } else {
        if (quantity <= 0.0) {
            asks_.erase(price);
        } else {
            asks_[price] = quantity;
        }
    }
}

std::vector<OrderEntry> OrderBookSide::get_top_n(int n) const {
    std::vector<OrderEntry> result;
    if (n <= 0) return result;
    if (side == Side::Bid) {
        for (const auto& [price, quantity] : bids_) {
            if (quantity <= 0.0) continue; 
            if (result.size() >= static_cast<size_t>(n)) break;
            result.push_back(OrderEntry{price, quantity});
        }
    } else {
        for (const auto& [price, quantity] : asks_) {
            if (quantity <= 0.0) continue; 
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
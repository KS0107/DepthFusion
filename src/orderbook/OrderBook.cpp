#include "NormalizedOrderUpdate.hpp"
#include "OrderBook.hpp"
#include <iomanip>

OrderBook::OrderBook(std::string exchange_name, std::string symbol) :
exchange_name_(std::move(exchange_name)), 
symbol_(std::move(symbol)),
bids_(Side::Bid),
asks_(Side::Ask) {}


void OrderBook::apply_update(const NormalizedOrderUpdate& update) {
    if (update.side == Side::Bid) {
        bids_.update(update.price, update.quantity);
    } else {
        asks_.update(update.price, update.quantity);
    }
};

std::vector<OrderEntry> OrderBook::get_top_n(Side side, int n) const {
    return (side == Side::Bid) ? bids_.get_top_n(n) : asks_.get_top_n(n);
};


std::string OrderBook::get_exchange_name() const {
    return exchange_name_;
};

std::ostream& operator<<(std::ostream& os, const OrderBook& ob) {
    os << "OrderBook [" << ob.exchange_name_ << "] " << ob.symbol_ << "\n";

    auto bids = ob.get_top_n(Side::Bid, 10);
    auto asks = ob.get_top_n(Side::Ask, 10);

    os << std::fixed;

    os << "--- Bids ---\n";
    for (const auto& entry : bids)
        os << entry.price << " @ " << entry.quantity << "\n";

    os << "--- Asks ---\n";
    for (const auto& entry : asks)
        os << entry.price << " @ " << entry.quantity << "\n";

    return os;
};

std::string OrderBook::get_symbol() const {
    return symbol_;
}
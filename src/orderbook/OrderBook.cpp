#include "NormalizedOrderUpdate.hpp"
#include "OrderBook.hpp"

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
#include "AggregatedOrderBook.hpp"
#include "OrderBook.hpp"
#include <iostream>

bool check_level(const OrderEntry& entry, double expected_price, double expected_qty) {
    return entry.price == expected_price && entry.quantity == expected_qty;
}

int main() {

    return 0;
}
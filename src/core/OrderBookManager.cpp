// src/core/OrderBookManager.cpp
#include "se_orderbook/core/OrderBookManager.hpp"
#include <iostream>

OrderBookManager::OrderBookManager() {}

OrderBookManager::~OrderBookManager() {
    stop();
}

void OrderBookManager::add_feed_handler(std::unique_ptr<IFeedHandler> handler) {
    handlers_.emplace_back(std::move(handler));
}

void OrderBookManager::start() {
    for (auto& h : handlers_) {
        h->start();
    }
}

void OrderBookManager::stop() {
    for (auto& h : handlers_) {
        h->stop();
    }
}

void OrderBookManager::print_all() const {
    std::cout << agg_;
}

AggregatedOrderBook& OrderBookManager::get_aggregated_order_book() {
    return agg_;
}
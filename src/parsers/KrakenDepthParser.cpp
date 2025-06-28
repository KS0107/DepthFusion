#include "se_orderbook/parsers/KrakenDepthParser.hpp"
#include "utils/SymbolNormaliser.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

std::vector<NormalizedOrderUpdate> KrakenDepthParser::parse(const std::string& json_msg) {

    std::vector<NormalizedOrderUpdate> updates;

    try {
        auto json = nlohmann::json::parse(json_msg);
        
        if (!json.contains("channel") || json["channel"] != "book") {
            return updates;
        }
        
        if (!json.contains("type") || !json.contains("data")) {
            return updates;
        }
        
        std::string type = json["type"];
        const auto& data_array = json["data"];
        for (const auto& entry : data_array) {
            if (!entry.contains("symbol")) continue;
            
            std::string symbol = entry["symbol"];
            std::string normalised_symbol = SymbolNormaliser::kraken(symbol);

            auto side_parser = [&](const std::string& side_key, Side side) {
                if (!entry.contains(side_key)) return;
                for (const auto& level : entry[side_key]) {
                    if (!level.contains("price") || !level.contains("qty")) continue;
                    
                    double price = level["price"];
                    double qty = level["qty"];
                    NormalizedOrderUpdate update = NormalizedOrderUpdate{
                        "Kraken_" + normalised_symbol, 
                        normalised_symbol, 
                        side, 
                        price, 
                        qty, 
                        (type == "snapshot")
                    };
                    updates.push_back(update);
                    
                }
            };
            side_parser("bids", Side::Bid);
            side_parser("asks", Side::Ask);
        }
    
    } catch (const std::exception& e) {
        std::cerr << "[Kraken Parser] JSON parse error: " << e.what() << "\n";
    }
    
    return updates;
}
#include "se_orderbook/parsers/BinanceDepthParser.hpp"
#include "utils/SymbolNormaliser.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
using json  = nlohmann::json;
std::vector<NormalizedOrderUpdate> BinanceDepthParser::parse(const std::string& json_msg) {
    std::vector<NormalizedOrderUpdate> updates;

    json parsed = json::parse(json_msg, nullptr, false);  
    if (parsed.is_discarded()) {
        std::cerr << "[Parser] Malformed JSON, skipping.\n";
        return updates;
    }

    if (parsed.contains("stream")) {
        parsed = parsed["data"];  
    }

    if (!parsed.contains("s") || parsed["s"].is_null()) {
        std::cerr << "[Parser] Missing or null symbol field.\n";
        return updates;
    }

    std::string symbol = parsed["s"];
    std::string normalised_symbol = SymbolNormaliser::binance(symbol);

    if (parsed.contains("b")) {
        for (const auto& bid : parsed["b"]) {
            if (bid.size() < 2) continue;
            double price = std::stod(bid[0].get<std::string>());
            double qty   = std::stod(bid[1].get<std::string>());
            updates.push_back({
                "Binance_" + normalised_symbol,
                normalised_symbol,
                Side::Bid, 
                price, 
                qty, 
                false
            });
        }
    }

    if (parsed.contains("a")) {
        for (const auto& ask : parsed["a"]) {
            if (ask.size() < 2) continue;
            double price = std::stod(ask[0].get<std::string>());
            double qty   = std::stod(ask[1].get<std::string>());
            updates.push_back({
                "Binance_" + normalised_symbol,
                normalised_symbol,
                Side::Ask, 
                price, 
                qty, 
                false
            });
        }
    }

    return updates;
}

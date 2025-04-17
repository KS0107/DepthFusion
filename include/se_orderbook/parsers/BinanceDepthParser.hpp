#ifndef SE_ORDERBOOK_BINANCEDEPTHPARSER_HPP
#define SE_ORDERBOOK_BINANCEDEPTHPARSER_HPP

#include "NormalizedOrderUpdate.hpp"
#include <string>
#include <vector>

class BinanceDepthParser {
public:
    static std::vector<NormalizedOrderUpdate> parse(const std::string& json_msg);
};

#endif
#ifndef SE_ORDERBOOK_KRAKENDEPTHPARSER_HPP
#define SE_ORDERBOOK_KRAKENDEPTHPARSER_HPP

#include "NormalizedOrderUpdate.hpp"
#include <string>
#include <vector>

class KrakenDepthParser {
public:
    static std::vector<NormalizedOrderUpdate> parse(const std::string& json_msg);
};

#endif
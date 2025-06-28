#ifndef SYMBOLNORMALISER
#define SYMBOLNORMALISER

#include <string>
#include <algorithm>

namespace SymbolNormaliser {
    
inline std::string kraken(const std::string& symbol) {
    std::string result = symbol;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    result.erase(std::remove(result.begin(), result.end(), '/'), result.end());
    return result;
}
    
inline std::string binance(const std::string& symbol) {
    std::string result = symbol;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
}
#endif
#ifndef SE_ORDERBOOK_IFEEDHANDLER_HPP
#define SE_ORDERBOOK_IFEEDHANDLER_HPP

#include <string>
class IFeedHandler {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string get_exchange_name() const = 0;
    virtual ~IFeedHandler() = default;
};

#endif

#ifndef SE_ORDERBOOK_IWEBSOCKETCLIENT_HPP
#define SE_ORDERBOOK_IWEBSOCKETCLIENT_HPP

#include <string>

class IWebSocketClient {
public:
    virtual void connect() = 0;
    virtual void subscribe(const std::string& channel) = 0;
    virtual void run() = 0;
    virtual bool is_connected() const = 0;
    virtual ~IWebSocketClient() = default;
};
#endif
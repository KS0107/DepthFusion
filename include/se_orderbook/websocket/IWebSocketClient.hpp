#ifndef SE_ORDERBOOK_IWEBSOCKETCLIENT_HPP
#define SE_ORDERBOOK_IWEBSOCKETCLIENT_HPP

#include <string>

class IWebSocketClient {
public:
    virtual void connect() = 0;
    virtual void subscribe(const std::vector<std::string>& channels) = 0;
    virtual void unsubscribe(const std::vector<std::string>& channels) = 0;
    virtual void run() = 0;
    virtual bool is_connected() const = 0;
    virtual void disconnect() = 0;
    virtual ~IWebSocketClient() = default;
};
#endif
#ifndef SE_ORDERBOOK_BINANCEWEBSOCKETCLIENT_HPP
#define SE_ORDERBOOK_BINANCEWEBSOCKETCLIENT_HPP

#include "IWebSocketClient.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <functional>
#include <thread>
#include <atomic>

class BinanceWebSocketClient : public IWebSocketClient {
public:
    using WebSocketClient = websocketpp::client<websocketpp::config::asio_tls_client>;
    using MessageHandler = std::function<void(const std::string&)>;

    BinanceWebSocketClient(const std::string& uri, MessageHandler on_message);

    void connect() override;
    void subscribe(const std::vector<std::string>& channels) override;
    void unsubscribe(const std::vector<std::string>& channels) override;
    void run() override;
    void disconnect() override;
    bool is_connected() const override;

private:
    std::string uri_;
    std::atomic<bool> connected_;
    std::atomic<bool> manual_close_;
    std::atomic<int> next_request_id_{1};
    std::vector<std::string> pending_subscriptions_;
    WebSocketClient client_;
    websocketpp::connection_hdl hdl_;
    MessageHandler message_callback_;
    void reconnect();
};

#endif
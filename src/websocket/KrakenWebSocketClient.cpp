#include "se_orderbook/websocket/KrakenWebSocketClient.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <thread>
#include <sstream>
#include <chrono>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl.hpp>

static std::shared_ptr<boost::asio::ssl::context> kraken_tls_init() {
    auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::no_tlsv1 |
                         boost::asio::ssl::context::no_tlsv1_1 |
                         boost::asio::ssl::context::single_dh_use);
    } catch (std::exception& e) {
        std::cerr << "[Kraken TLS] TLS context setup failed: " << e.what() << std::endl;
    }
    return ctx;
}

KrakenWebSocketClient::KrakenWebSocketClient(const std::string& uri, MessageHandler on_message)
    : uri_(uri), connected_(false), manual_close_(false), message_callback_(std::move(on_message))
{
    client_.init_asio();

    client_.set_tls_init_handler([](websocketpp::connection_hdl) {
        return kraken_tls_init();
    });

    client_.set_open_handler([this](websocketpp::connection_hdl hdl) {
        hdl_ = hdl;
        connected_ = true;
        std::cout << "[Kraken WS] Connected.\n";
        if (open_handler_) {
            open_handler_();
        }

        if (!pending_channel_.empty()) {
            websocketpp::lib::error_code ec;
            client_.send(hdl_, pending_channel_, websocketpp::frame::opcode::text, ec);
            if (ec) {
                std::cerr << "[Kraken WS] Subscription error: " << ec.message() << std::endl;
            } else {
                std::cout << "[Kraken WS] Subscribed: " << pending_channel_ << std::endl;
            }
        }
    });

    client_.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg) {
        if (message_callback_) {
            message_callback_(msg->get_payload());
        }
    });

    client_.set_close_handler([this](websocketpp::connection_hdl) {
        connected_ = false;
        std::cout << "[Kraken WS] Connection closed.\n";
        if (!manual_close_) {
            reconnect();
        }
    });

    client_.set_fail_handler([this](websocketpp::connection_hdl) {
        connected_ = false;
        std::cout << "[Kraken WS] Connection failed.\n";
        reconnect();
    });

    client_.set_pong_handler([](websocketpp::connection_hdl, std::string) {
        std::cout << "[Kraken WS] Pong received.\n";
        return true;
    });
}

void KrakenWebSocketClient::connect() {
    manual_close_ = false;
    websocketpp::lib::error_code ec;
    auto con = client_.get_connection(uri_, ec);
    if (ec) {
        std::cerr << "[Kraken WS] Connect initialization error: " << ec.message() << std::endl;
        return;
    }
    client_.connect(con);
}

void KrakenWebSocketClient::run() {
    client_.run();
}

void KrakenWebSocketClient::disconnect() {
    manual_close_ = true;
    websocketpp::lib::error_code ec;
    client_.close(hdl_, websocketpp::close::status::going_away, "Client shutdown", ec);
    if (ec) {
        std::cerr << "[Kraken WS] Close error: " << ec.message() << std::endl;
    }
    client_.stop();
}

void KrakenWebSocketClient::subscribe(const std::string& channel) {
    pending_channel_ = channel;

    if (connected_) {
        websocketpp::lib::error_code ec;
        client_.send(hdl_, channel, websocketpp::frame::opcode::text, ec);
        if (ec) {
            std::cerr << "[Kraken WS] Subscription error: " << ec.message() << std::endl;
        } else {
            std::cout << "[Kraken WS] Sent subscription.\n";
        }
    } else {
        std::cout << "[Kraken WS] Subscription deferred until connected.\n";
    }
}

bool KrakenWebSocketClient::is_connected() const {
    return connected_;
}

void KrakenWebSocketClient::reconnect() {
    if (manual_close_) return;
    int reconnect_timer = 1;
    std::cout << "[Kraken WS] Reconnecting in " << reconnect_timer << " second...\n";
    std::this_thread::sleep_for(std::chrono::seconds(reconnect_timer));
    connect();

    std::thread([this]() {
        try {
            run();
        } catch (const std::exception& e) {
            std::cerr << "[Kraken WS] Reconnect run failed: " << e.what() << "\n";
        }
    }).detach();
}

void KrakenWebSocketClient::set_open_handler(std::function<void()> handler) {
    open_handler_ = std::move(handler);
}
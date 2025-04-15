#include "se_orderbook/websocket/BinanceWebSocketClient.hpp"
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <openssl/ssl.h>

static std::shared_ptr<boost::asio::ssl::context> on_tls_init() {
    auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::single_dh_use);
    } catch (std::exception& e) {
        std::cerr << "TLS context setup failed: " << e.what() << std::endl;
    }
    return ctx;
}

BinanceWebSocketClient::BinanceWebSocketClient(const std::string& uri, MessageHandler on_message) 
    : uri_(uri), connected_(false), message_callback_(std::move(on_message)) {
        client_.clear_access_channels(websocketpp::log::alevel::all);
        client_.init_asio();

        client_.set_tls_init_handler([this](websocketpp::connection_hdl) {
            return on_tls_init();
        });

        client_.set_open_handler([this](websocketpp::connection_hdl hdl) {
            connected_ = true;
            hdl_  = hdl;
            std::cout << "[WebSocket] Connected to Binance\n";
        });

        client_.set_fail_handler([this](websocketpp::connection_hdl) {
            std::cout << "[WebSocket] Connected failed\n";
            connected_ = false;
        });

        client_.set_close_handler([this](websocketpp::connection_hdl) {
            std::cout << "[WebSocket] Connected closed\n";
            connected_ = false;
        });

        client_.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg){
            if (message_callback_) {
                message_callback_(msg->get_payload());
            }
        });

        client_.set_ping_handler([](websocketpp::connection_hdl, std::string) {
            std::cout << "[WebSocket] Ping received\n";
            return true;
        });

        client_.set_pong_handler([](websocketpp::connection_hdl, std::string) {
            std::cout << "[WebSocket] Pong received\n";
            return true;
        });

        client_.set_pong_timeout_handler([](websocketpp::connection_hdl, std::string) {
            std::cerr << "[WebSocket] Pong timeout\n";
        });

        client_.set_open_handler([this] (websocketpp::connection_hdl hdl) {
            connected_ = true;
            hdl_ = hdl;
            std::cout << "[WebSocket] Connected to Binance\n";

            if (!pending_channel_.empty()) {
                std::stringstream ss;
                ss << R"({"method":"SUBSCRIBE", "params":[")" << pending_channel_ << R"("],"id":1})";
                websocketpp::lib::error_code ec;

                client_.send(hdl_, ss.str(), websocketpp::frame::opcode::text, ec);
                if (ec) {
                    std::cerr << "[WebSocket] Subscription failed: " << ec.message() << "\n";
                } else {
                    std::cout << "[WebSocket] Subscribed to: " << pending_channel_ << "\n";
                }
            }
        });
}

void BinanceWebSocketClient::connect() {
    websocketpp::lib::error_code ec;
    auto con = client_.get_connection(uri_, ec);
    if (ec) {
        std::cerr << "[WebSocket] Connection creation failed: " << ec.message() << std::endl;
        return;
    }
    client_.connect(con);
}

void BinanceWebSocketClient::subscribe(const std::string& channel) {
    pending_channel_ = channel;
}

void BinanceWebSocketClient::run() {
    client_.run();
}

bool BinanceWebSocketClient::is_connected() const {
    return connected_;
}

void BinanceWebSocketClient::disconnect() {
    if (!connected_) return;
    websocketpp::lib::error_code ec;
    client_.close(hdl_, websocketpp::close::status::going_away, "Client shutdown", ec);
    if (ec) {
        std::cerr << "[WebSocket] Error during disconnect: " << ec.message() << "\n";
    } else {
        std::cout << "[WebSocket] Sent close frame.\n";
    }

    client_.stop();
}
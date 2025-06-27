#include "se_orderbook/websocket/BinanceWebSocketClient.hpp"
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>

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
    : uri_(uri), connected_(false), message_callback_(std::move(on_message)), next_request_id_{1} {
        client_.clear_access_channels(websocketpp::log::alevel::all);
        client_.init_asio();

        client_.set_tls_init_handler([this](websocketpp::connection_hdl) {
            return on_tls_init();
        });

        client_.set_open_handler([this](websocketpp::connection_hdl hdl) {
            connected_ = true;
            hdl_  = hdl;
            std::cout << "[Binance WS] Connected to Binance\n";
        });

        client_.set_fail_handler([this](websocketpp::connection_hdl hdl) {
            connected_ = false;
            auto con = client_.get_con_from_hdl(hdl);
            std::cerr << "[Binance WS] Connection failed : " << con->get_ec().message() << "\n";
            reconnect();
        });

        client_.set_close_handler([this](websocketpp::connection_hdl) {
            connected_ = false;
            std::cerr << "[Binance WS] Connected closed\n";

            if (!manual_close_) {
                std::cerr << "[Binance WS] Unexpected close - attempting to reconnect...\n";
                reconnect();
            } else {
                std::cout << "[Binance WS] Manual close. No reconnect.\n";
            }
        });

        client_.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg){
            if (message_callback_) {
                message_callback_(msg->get_payload());
            }
        });

        client_.set_ping_handler([this](websocketpp::connection_hdl hdl, std::string payload) {
            websocketpp::lib::error_code ec;
            client_.pong(hdl, payload);
            std::cout << "[Binance WS] Ping received, sent pong with payload: " << payload << "\n";
            return true;
        });

        client_.set_pong_handler([](websocketpp::connection_hdl, std::string) {
            std::cout << "[Binance WS] Pong received\n";
            return true;
        });

        client_.set_pong_timeout_handler([](websocketpp::connection_hdl, std::string) {
            std::cerr << "[Binance WS] Pong timeout\n";
        });

        client_.set_open_handler([this] (websocketpp::connection_hdl hdl) {
            connected_ = true;
            hdl_ = hdl;
            std::cout << "[Binance WS] Connected to Binance\n";

            for (const auto& msg : pending_subscriptions_) {
                websocketpp::lib::error_code ec;
                client_.send(hdl_, msg, websocketpp::frame::opcode::text, ec);
                if (ec) {
                    std::cerr << "[Binance WS] Error sending deferred subscription: " << ec.message() << "\n";
                } else {
                    std::cout << "[Binance WS] Sent deferred subscription: " << msg << "\n";
                }
            }
            pending_subscriptions_.clear();
        });
}

void BinanceWebSocketClient::connect() {
    manual_close_ = false;
    websocketpp::lib::error_code ec;
    auto con = client_.get_connection(uri_, ec);
    if (ec) {
        std::cerr << "[Binance WS] Connection creation failed: " << ec.message() << std::endl;
        return;
    }
    client_.connect(con);
}

void BinanceWebSocketClient::subscribe(const std::vector<std::string>& channels) {
    if (channels.empty()) return;

    int req_id = next_request_id_++;

    nlohmann::json sub_msg;
    sub_msg["method"] = "SUBSCRIBE";
    sub_msg["params"] = channels;
    sub_msg["id"] = req_id;

    std::string msg = sub_msg.dump();

    if (connected_) {
        websocketpp::lib::error_code ec;
        client_.send(hdl_, msg, websocketpp::frame::opcode::text, ec);
        if (ec) {
            std::cerr << "[Binance WS] Subscription failed: " << ec.message() << "\n";
        } else {
            std::cout << "[Binance WS] Subscribed (id=" << req_id << "): " << msg << "\n";
        }
    } else {
        pending_subscriptions_.push_back(msg);
        std::cout << "[Binance WS] Subscription deferred (id=" << req_id << ").\n";
    }
}

void BinanceWebSocketClient::unsubscribe(const std::vector<std::string>& channels) {
    if (!connected_) {
        std::cerr << "[Binance WS] Cannot unsubscribe: not connected." << std::endl;
        return;
    }
    
    if (channels.empty()) {
        std::cerr << "[Binance WS] Not currently subscribed to any channels." << std::endl;
        return;
    }
    int req_id = next_request_id_++;
    
    nlohmann::json unsubscribe_msg;
    unsubscribe_msg["method"] = "UNSUBSCRIBE";
    unsubscribe_msg["params"] = channels;
    unsubscribe_msg["id"] = req_id;
    
    std::string msg = unsubscribe_msg.dump();
    websocketpp::lib::error_code ec;
    client_.send(hdl_, msg, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "[Binance WS] Unsubscribe failed: " << ec.message() << "\n";
    } else {
        std::cout << "[Binance WS] Sent unsubscribe for " << channels.size() << " stream(s).\n";
    }
}

void BinanceWebSocketClient::run() {
    client_.run();
}

bool BinanceWebSocketClient::is_connected() const {
    return connected_;
}

void BinanceWebSocketClient::reconnect() {
    int reconnect_timer = 1;
    std::cout << "[Binance WS] Reconnecting in " << reconnect_timer << " second...\n";
    std::this_thread::sleep_for(std::chrono::seconds(reconnect_timer));
    connect();

    std::thread([this]() {
        try {
            run();
        } catch (const std::exception& e) {
            std::cerr << "[Binance WS] Reconnect failed: " << e.what() << "\n";
        }
    }).detach();
}

void BinanceWebSocketClient::disconnect() {
    if (!connected_) return;
    manual_close_ = true;
    websocketpp::lib::error_code ec;
    client_.close(hdl_, websocketpp::close::status::going_away, "Client shutdown", ec);
    if (ec) {
        std::cerr << "[Binance WS] Error during disconnect: " << ec.message() << "\n";
    } else {
        std::cout << "[Binance WS] Sent close frame.\n";
    }

    client_.stop();
}
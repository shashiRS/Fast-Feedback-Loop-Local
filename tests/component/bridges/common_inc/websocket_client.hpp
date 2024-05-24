#ifndef BRIDGESDK_WEBSERVER_WEB_SOCKET_CLIENT_HPP
#define BRIDGESDK_WEBSERVER_WEB_SOCKET_CLIENT_HPP

#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/logger/logger.hpp>

using tcp = boost::asio::ip::tcp;              // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket; // from <boost/beast/websocket.hpp>

const std::string kSupportedFoxgloveProtocol{"foxglove.websocket.v1"};

namespace next {
namespace bridgesdk {
namespace test {

// Report a failure
void fail(boost::system::error_code ec, char const *what) {
  next::sdk::logger::error(__FILE__, "{}:{}", what, ec.message());
}

// Sends a WebSocket message and prints the response
class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {

public:
  // Resolver and socket require an io_context
  explicit WebSocketClient(boost::asio::io_context &ioc, const bool foxglove) : resolver_(ioc), ws_(ioc), ioc_(ioc) {
    if (foxglove) {
      sdk::logger::debug(__FILE__, "[WebSocket client] adding decorator for foxglove WebSocket upgrade request");
      ws_.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type &req) {
        // set the foxglove protocol in the WebSocket upgrade request
        req.set(boost::beast::http::field::sec_websocket_protocol, kSupportedFoxgloveProtocol);
      }));
    } else {
      sdk::logger::debug(__FILE__, "[WebSocket client] no special decorator added");
    }
  }
  ~WebSocketClient() {}

  // Start the asynchronous operation
  void Run(char const *host, char const *port) {
    // Save these for later
    host_ = host;

    // Look up the domain name
    resolver_.async_resolve(
        host, port,
        std::bind(&WebSocketClient::OnResolve, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }

  void OnResolve(boost::system::error_code ec, tcp::resolver::results_type results) {
    if (ec)
      return fail(ec, "resolve");

    // Make the connection on the IP address we get from a lookup
    boost::asio::async_connect(ws_.next_layer(), results.begin(), results.end(),
                               std::bind(&WebSocketClient::OnConnect, shared_from_this(), std::placeholders::_1));
  }

  void OnConnect(boost::system::error_code ec) {
    if (ec)
      return fail(ec, "connect");

    // Perform the websocket handshake
    ws_.async_handshake(host_, "/",
                        std::bind(&WebSocketClient::OnHandshake, shared_from_this(), std::placeholders::_1));
  }

  void OnHandshake(boost::system::error_code ec) {
    if (ec)
      return fail(ec, "handshake");
    DoRead();
  }

  void DoWrite(const std::vector<uint8_t> &data, bool binary) {
    // Send the message
    if (binary)
      ws_.binary(true);
    else
      ws_.binary(false);
    boost::system::error_code ec;
    const size_t bytes_transferred = ws_.write(boost::asio::buffer(data), ec);
    if (bytes_transferred != data.size() || ec) {
      return fail(ec, "DoWrite failed");
    }
  }

  void DoRead() {
    // Read a message into our buffer
    ws_.async_read(
        buffer_, std::bind(&WebSocketClient::OnRead, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }

  void OnRead(boost::system::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the Session was closed
    if (ec == websocket::error::closed) {
      return;
    }

    if (ec)
      return;

    received_data_.clear();
    received_data_.resize(buffer_.size());
    buffer_.prepare(buffer_.size());

    for (auto it : buffer_.data()) {
      std::copy(static_cast<const uint8_t *>(it.data()), static_cast<const uint8_t *>(it.data()) + it.size(),
                received_data_.begin());
    }
    buffer_.commit(buffer_.size());
    buffer_.consume(buffer_.size());
    const bool received_binary_ = ws_.got_binary();
    if (reader_callback_)
      reader_callback_(received_data_, received_binary_);
    DoRead();
  }

  std::vector<uint8_t> get_received_data() { return received_data_; }

  void DoClose() {
    // Close the WebSocket connection
    ws_.async_close(websocket::close_code::normal,
                    std::bind(&WebSocketClient::OnClose, shared_from_this(), std::placeholders::_1));
  }

  void OnClose(boost::system::error_code ec) {
    if (ec)
      return fail(ec, "close");

    // If we get here then the connection is closed gracefully
  }

  void SetReaderCallback(std::function<void(std::vector<uint8_t>, bool)> callback) { reader_callback_ = callback; }

private:
  tcp::resolver resolver_;
  websocket::stream<tcp::socket> ws_;
  boost::beast::multi_buffer buffer_;
  std::string host_;
  std::vector<uint8_t> received_data_;
  std::function<void(std::vector<uint8_t>, bool)> reader_callback_;
  boost::asio::io_context &ioc_;
};
} // namespace test
} // namespace bridgesdk
} // namespace next

#endif // BRIDGESDK_WEBSERVER_WEB_SOCKET_CLIENT_HPP

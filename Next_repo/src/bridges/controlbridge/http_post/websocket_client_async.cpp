// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     websocket_client_async.cpp
 * @brief    Asynchronous http client class for sending messages.
 *           Based on https://www.boost.org/doc/libs/master/libs/beast/example/http/client/async/http_client_async.cpp
 *           which is distributed under the Boost Software License, Version 1.0.
 *           (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 **/
#include "websocket_client_async.hpp"

#include <boost/lexical_cast.hpp>

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace controlbridge {

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

constexpr unsigned int http_version_number_ = 11; // set http version to 1.1

WebsocketSession::WebsocketSession(boost::asio::io_context &ioc, const std::string &address,
                                   const std::string &url_path, const std::string &scheme, std::chrono::seconds timeout,
                                   const std::string &message)
    : resolver_(boost::asio::make_strand(ioc)), stream_(boost::asio::make_strand(ioc)), host_(address), scheme_(scheme),
      url_path_(url_path), timeout_(timeout) {
  if (Setup())
    PostJsonString(message);
}

bool WebsocketSession::Setup() {
  post_successfull_ = false;

  // Look up the domain name
  boost::beast::error_code ec;
  results_ = resolver_.resolve(host_, scheme_, ec);

  if (ec) {
    error(__FILE__, " Failed during resolve: {0}", ec.message());
    setup_successfull_ = false;
    return false;
  }
  setup_successfull_ = true;
  return true;
}

void WebsocketSession::PostJsonString(const std::string &message) {
  post_successfull_ = true; // will be set to false in case of an error
  message_ = message;

  if (!setup_successfull_) {
    post_successfull_ = false;
    return;
  }

  // Set a timeout on the operation
  stream_.expires_after(timeout_);

  // Make the connection on the IP address we get from a lookup
  auto binded_callback = std::bind(&WebsocketSession::OnConnect, this, std::placeholders::_1, std::placeholders::_2);
  stream_.async_connect(results_, binded_callback);
}

void WebsocketSession::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
  if (ec)
    return FailedPostString(ec, "connect");

  // Set a timeout on the operation
  stream_.expires_after(timeout_);

  // Set up an HTTP GET request message
  req_.version(http_version_number_);
  req_.method(http::verb::post);
  req_.target(url_path_);
  req_.set(http::field::host, host_);
  req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req_.set(http::field::content_type, "application/json");
  req_.body() = message_;
  req_.prepare_payload();

  // Send the HTTP request to the remote host
  auto binded_callback = std::bind(&WebsocketSession::OnWrite, this, std::placeholders::_1, std::placeholders::_2);
  http::async_write(stream_, req_, binded_callback);
}

void WebsocketSession::OnWrite(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return FailedPostString(ec, "write");

  // Receive the HTTP response
  auto binded_callback = std::bind(&WebsocketSession::OnRead, this, std::placeholders::_1, std::placeholders::_2);
  http::async_read(stream_, buffer_, res_, binded_callback);
}

void WebsocketSession::OnRead(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return FailedPostString(ec, "read");

  debug(__FILE__, " Return value header: {0}", boost::lexical_cast<std::string>(res_.base()));
  debug(__FILE__, " Return value body: {0}", boost::lexical_cast<std::string>(res_.body()));

  if (http::status::ok != res_.result()) {
    post_successfull_ = false;
    warn(__FILE__, " http post request failed with error code: {0}", res_.result_int());
  } else {
    debug(__FILE__, " http post was successful");
  }

  // Gracefully close the socket
  stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

  // not_connected happens sometimes so don't bother reporting it.
  if (ec && ec != beast::errc::not_connected)
    return FailedPostString(ec, "shutdown");

  // If we get here then the connection is closed gracefully
}

// Report a failure
void WebsocketSession::FailedPostString(beast::error_code ec, char const *what) {
  post_successfull_ = false;
  warn(__FILE__, " Failed: {0}: {1}", what, ec.message());
}

} // namespace controlbridge
} // namespace next

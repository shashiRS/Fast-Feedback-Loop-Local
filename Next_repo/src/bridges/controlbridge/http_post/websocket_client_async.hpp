//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     websocket_client_async.hpp
 * @brief    Asynchronous http client class for sending messages.
 *           Based on https://www.boost.org/doc/libs/master/libs/beast/example/http/client/async/http_client_async.cpp
 *           which is distributed under the Boost Software License, Version 1.0.
 *           (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 **/

#include <chrono>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>

#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

namespace next {
namespace controlbridge {

// Performs an HTTP GET and prints the response
// Hint: For each send request a new websocket session needs to be created
class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {
public:
  // Objects are constructed with a strand to ensure that handlers do not execute concurrently.
  explicit WebsocketSession(boost::asio::io_context &ioc, const std::string &address, const std::string &url_path,
                            const std::string &scheme, std::chrono::seconds timeout, const std::string &message);

  // return result of post message
  bool GetPostResult() { return post_successfull_; }

private:
  bool Setup();
  void PostJsonString(const std::string &message);
  void OnConnect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
  void OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred);
  void OnRead(boost::beast::error_code ec, std::size_t bytes_transferred);

  // Report a failure
  void FailedPostString(boost::beast::error_code ec, char const *what);

  boost::asio::ip::tcp::resolver resolver_;
  boost::beast::tcp_stream stream_;
  boost::beast::flat_buffer buffer_; // (Must persist between reads)
  boost::beast::http::request<boost::beast::http::string_body> req_;
  boost::beast::http::response<boost::beast::http::string_body> res_;

  boost::asio::ip::tcp::resolver::results_type results_;

  std::string host_;
  std::string scheme_;
  std::string url_path_;
  std::string message_;
  std::chrono::seconds timeout_;

  bool post_successfull_{false};
  bool setup_successfull_{false};
};

} // namespace controlbridge
} // namespace next

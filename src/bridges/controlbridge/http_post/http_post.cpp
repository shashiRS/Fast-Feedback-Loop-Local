/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     http_post.cpp
 * @brief    http post webserver
 *
 **/

#include "http_post.hpp"

#include <next/sdk/logger/logger.hpp>

#include "websocket_client_async.hpp"

namespace next {
namespace controlbridge {

HttpPost::HttpPost(const std::string &host, const std::string &url_path, const std::string &scheme,
                   std::chrono::seconds timeout)
    : host_(host), scheme_(scheme), url_path_(url_path), timeout_(timeout) {}

bool HttpPost::PostJsonString(const std::string &message) {
  bool result{false};
  try {
    ioc_ = std::make_shared<boost::asio::io_context>();
    // create new websocket session to send message
    websocket_session_ = std::make_shared<WebsocketSession>(*ioc_, host_, url_path_, scheme_, timeout_, message);

    // Run the I/O service. The call will return when the socket is closed.
    ioc_->run();

    // get results of http post in websocket session
    result = websocket_session_->GetPostResult();

    websocket_session_.reset();
    ioc_.reset();
  } catch (std::exception const &e) {
    error(__FILE__, "Error during PostJsonString: {}", e.what());
    return false;
  }

  return result;
}

} // namespace controlbridge
} // namespace next

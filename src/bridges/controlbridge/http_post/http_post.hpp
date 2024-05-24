/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     http_post.hpp
 * @brief    Simple web client to send JSON data as HTTP POST to a web server.
 *
 **/

#ifndef NEXT_CONTROLBRIDGE_HTTP_POST_H_
#define NEXT_CONTROLBRIDGE_HTTP_POST_H_

#include <chrono>
#include <memory>
#include <string>

namespace boost {
namespace asio {
class io_context;
} // namespace asio
} // namespace boost

namespace next {
namespace controlbridge {

class WebsocketSession;

//! Simple web client to send JSON data as HTTP POST to a web server.
/**
 * This class resolves the host, scheme and url path in the setup.
 * For every send request a new connection is established and afterwards closed again.
 * The operations "connect" and "write + read response" from web server during the send request
 * are aborted after the specified timeout.
 * As data strings in JSON format can be sent.
 * The header content type of the message send to the web server is set to "application/json"
 * The response from the web server is checked for errors.
 */
class HttpPost {
public:
  /*!setup http post message
   *
   * @param host host address for posting the message, e.g. "example.com"
   *             Hint: shall not contain any prefix scheme like "https://" or "www." and no postfix path like /path/to
   * @param url_path path of the url, e.g. /path/to (see https://url.spec.whatwg.org/#concept-url-path)
   * @param schema schema of the host address, e.g. "http" (see https://url.spec.whatwg.org/#concept-url-path)
   * @param timeout time after which http post message is aborted
   */
  HttpPost(const std::string &host, const std::string &url_path, const std::string &scheme = "http",
           std::chrono::seconds timeout = std::chrono::seconds(10));

  /*!
   * @brief send http post message to host as content_type "application/json"
   *
   * @param message string which is sent to the host. String shall be in JSON format.
   * @return true if successful, otherwise false
   */
  bool PostJsonString(const std::string &message);

private:
  std::shared_ptr<boost::asio::io_context> ioc_; // The io_context is required for all I/O
  std::shared_ptr<WebsocketSession> websocket_session_;

  std::string host_;
  std::string scheme_;
  std::string url_path_;
  std::chrono::seconds timeout_;
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_CONTROLBRIDGE_HTTP_POST_H_

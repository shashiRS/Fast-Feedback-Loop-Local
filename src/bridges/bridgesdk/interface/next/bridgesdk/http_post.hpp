/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     http_post.hpp
 * @brief    http post webserver
 *
 **/

#ifndef NEXT_BRIDGESDK_HTTP_POST_H_
#define NEXT_BRIDGESDK_HTTP_POST_H_

#include <string>

namespace next {
namespace bridgesdk {

class HttpPostImpl;

class HttpPost {
public:
  /*!setup http post message
   *
   * @param host host address for posting the message, e.g. "example.com"
   *             Hint: shall not contain any prefix scheme like "https:://" or "www." and no postfix path like /path/to
   * @param port port of the host address
   * @param http_version protocol version http request
   */
  HttpPost(const std::string &host, uint16_t port = 80, std::string http_version = "1.1");

  /*!
   * @brief send http post message to host
   *
   * @param target target of the post message, e.g. /post
   * @param message string which is sent to the host
   * @return true if successful, otherwise false
   */
  bool PostString(const std::string &target, const std::string &message);

private:
  HttpPostImpl *p_impl_ = nullptr;
};

} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_HTTP_POST_H_

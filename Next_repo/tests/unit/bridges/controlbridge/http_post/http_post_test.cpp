#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <memory>

#include "http_post.hpp"
#include "websocket_client_async.hpp"

namespace next {
namespace controlbridge {

// TODO: set webserver to test webserver and activate tests again.
//  Implemented by ticket https: // jira-adas.zone2.agileci.conti.de/browse/SIMEN-7785

TEST(HTTP_POST_TEST, DISABLED_SetupWrongHost) {
  next::controlbridge::HttpPost test("notExisting", "/post", "http", std::chrono::seconds(30));
  bool status = test.PostJsonString("testmessage");

  // check received against expected data
  EXPECT_FALSE(status);
}

TEST(HTTP_POST_TEST, DISABLED_SetupWrongScheme) {
  next::controlbridge::HttpPost test("httpbin.org", "/post", "notExisting", std::chrono::seconds(30));
  bool status = test.PostJsonString("testmessage");

  // check received against expected data
  EXPECT_FALSE(status);
}

TEST(HTTP_POST_TEST, DISABLED_SetupWrongUrlPath) {
  next::controlbridge::HttpPost test("httpbin.org", "notExisting", "http", std::chrono::seconds(30));
  bool status = test.PostJsonString("testmessage");

  // check received against expected data
  EXPECT_FALSE(status);
}

TEST(HTTP_POST_TEST, DISABLED_TimeoutExpired) {
  next::controlbridge::HttpPost test("www.httpbin.org", "/post", "http", std::chrono::seconds(0));
  bool status = test.PostJsonString("testmessage");

  // check received against expected data
  EXPECT_FALSE(status);
}

} // namespace controlbridge
} // namespace next


#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <cip_test_support/gtest_reporting.h>

#include <json/json.h>
#include <boost/beast/core/detail/base64.hpp>

// data request server
#include "data_class_manager.h"
// data broadcaster
#include <next/bridgesdk/data_manager.hpp>

#include "websocket_client.hpp"

namespace next {
namespace bridgesdk {
namespace test {

int counter = 0;

void ReceiveMessageFromClient(const std::vector<uint8_t> &data) {

  std::string message(data.begin(), data.end());
  Json::Value root;
  Json::Reader reader;
  bool json_message = reader.parse(message, root);
  if (!json_message) {
    std::cout << "Error parsing the string" << std::endl;
  }

  EXPECT_EQ(root["test_json"].asInt(), counter);
}

TEST(IntegrationTest, TestingSendingMessageToClient) {

  counter = 0;
  // Starting websocket server
  // create a data request server

  // and a class manager for it
  auto class_manager = std::make_shared<next::databridge::DataClassManager>();

  // create the data manager
  std::shared_ptr<next::databridge::data_manager::IDataManager> data_manager =
      next::databridge::data_manager::CreateDataManager();
  data_manager->SetForwardMessageCallback(std::bind(&ReceiveMessageFromClient, std::placeholders::_1));
  data_manager->StartWebServer("127.0.0.1", 3333, 1);

  // prepare JSON builder and needed data structures
  Json::StreamWriterBuilder writer_builder;
  writer_builder["commentStyle"] = "None";
  writer_builder["indentation"] = "";

  using namespace std::chrono_literals;
  std::atomic<bool> cancle_client_thrd{false};

  // The io_context is required for all I/O
  boost::asio::io_context ioc_;
  auto ses_client_datamanager = std::make_shared<WebSocketClient>(ioc_, false);

  std::this_thread::sleep_for(1000ms);
  // Starting websocket client
  std::thread client_thrd_broadcaster([&]() {
    auto const host = "127.0.0.1";
    auto const port = "3333";

    // Launch the asynchronous operation
    ses_client_datamanager->Run(host, port);

    ses_client_datamanager->SetReaderCallback([&](std::vector<uint8_t> data, bool) {
      std::string message(data.begin(), data.end());
      Json::Value root;
      Json::Reader reader;
      bool json_message = reader.parse(message, root);
      if (!json_message) {
        std::cout << "Error parsing the string" << std::endl;
      }

      EXPECT_EQ(root["test_json"].asInt(), counter);
    });

    // Run the I/O service. The call will return when the socket is closed.
    ioc_.run();

    while (!cancle_client_thrd.load()) {
    }
  });

  std::this_thread::sleep_for(1000ms);

  Json::Value data_stream_json;
  for (int i = 0; i < 5; ++i) {

    data_stream_json["test_json"] = counter;
    std::string output = Json::writeString(writer_builder, data_stream_json);
    std::vector<uint8_t> data_to_send(output.length());
    std::copy(output.begin(), output.end(), data_to_send.begin());

    // Submit message with ID to webserver manager to be send
    data_manager->PublishDataToClient(reinterpret_cast<const uint8_t *>(output.c_str()), output.size(), true);
    ses_client_datamanager->DoWrite(data_to_send, false);

    std::this_thread::sleep_for(100ms);
    counter += 1;
  }

  // Close client socket
  std::this_thread::sleep_for(1000ms);
  ses_client_datamanager->DoClose();
  std::this_thread::sleep_for(1000ms);

  cancle_client_thrd.store(true);
  client_thrd_broadcaster.join();
}
} // namespace test
} // namespace bridgesdk
} // namespace next

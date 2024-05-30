
#include <array>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include <next/bridgesdk/web_server.hpp>
#include "websocket_client.hpp"

namespace next {
namespace bridgesdk {
namespace test {

using next::bridgesdk::webserver::IWebServer;

constexpr const char *kLocalhost = "127.0.0.1";

const std::array<const std::string, 5> kTestData5{
    {{"Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod "
      "tempor incidunt ut labore et dolore magna aliqua."},
     {"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat."},
     {"Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur."},
     {"Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."},
     {"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu "
      "feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril "
      "delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, "
      "sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat."}}};
const std::array<const std::string, 20> kTestData20{
    {kTestData5[0], kTestData5[1], kTestData5[2], kTestData5[3], kTestData5[4], kTestData5[0], kTestData5[1],
     kTestData5[2], kTestData5[3], kTestData5[4], kTestData5[0], kTestData5[1], kTestData5[2], kTestData5[3],
     kTestData5[4], kTestData5[0], kTestData5[1], kTestData5[2], kTestData5[3], kTestData5[4]}};

std::unordered_map<size_t, size_t> sent_ids;
std::mutex sent_ids_mtx;
void notification_callback(size_t id) {
  std::lock_guard<std::mutex> lck(sent_ids_mtx);
  sent_ids[id]++;
}

class ComponentBridgesWebServerTest : public ::testing::Test {
public:
  ComponentBridgesWebServerTest() : port_(GetNextPort()) {
    next::sdk::logger::register_to_init_chain();
    initmap_.emplace(next::sdk::logger::getSentinelDisableCfgKey(), true);
    initmap_.emplace(next::sdk::logger::getLogRouterDisableCfgKey(), true);
    sent_ids.reserve(kTestData20.size());
  }

  ~ComponentBridgesWebServerTest() {
    for (auto &client : clients_) {
      if (client.session)
        client.session->DoClose();
    }
    for (auto &client : clients_) {
      if (client.thread.joinable())
        client.thread.join();
      client.session.reset();
    }
    web_server_.reset();

    next::sdk::InitChain::Reset();
    // giving the OS some time to clean up the socket usage
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  virtual void SetUp() override {
    ASSERT_TRUE(next::sdk::InitChain::Run(initmap_)) << "initalization failed";
    ASSERT_TRUE(next::sdk::logger::isInitialized()) << "logger was not initialized";
    sent_ids.clear();
  }

protected:
  void StartServer() {
    web_server_ = webserver::CreateWebServer();
    web_server_->SetIpAddress(host_);
    web_server_->SetPortNumber(port_);
    web_server_->SetNumOfConnections(1);
    web_server_->AddForwardMessageCallback(std::bind(
        [=](const std::vector<uint8_t> &data) {
          std::lock_guard<std::mutex> lck(server_data_mtx_);
          server_data_.push_back(data);
        },
        std::placeholders::_1));
    web_server_->ActivateListingToSocket();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  void StartClient(const bool foxglove) {
    assert(running_clients_ < clients_.size());
    auto &cur_info = clients_[running_clients_];
    cur_info.session = std::make_shared<WebSocketClient>(cur_info.ioc, foxglove);

    cur_info.session->SetReaderCallback([&data_vec = cur_info.received_data, &mtx = cur_info.data_mtx](
                                            const std::vector<uint8_t> &data, const bool binary) {
      std::lock_guard<std::mutex> lck(mtx);
      data_vec.push_back({data, binary});
    });

    cur_info.thread = std::thread([&, &info = clients_[running_clients_], &ioc = clients_[running_clients_].ioc]() {
      info.session->Run(host_.c_str(), std::to_string(port_).c_str()); // Launch the asynchronous operation
      ioc.run(); // Run the I/O service. The call will return when the socket is closed.
    });

    running_clients_++;
  }

  bool AwaitConnectionOnServer(size_t count_connections = 1) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (web_server_->GetNumOfActiveConnections() != count_connections) {
      if (std::chrono::steady_clock::now() > end) {
        return false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return true;
  }

  void SendDataFromServer(bool binary, unsigned int factor = 1) {
    // prepare sending data
    std::list<size_t> ids;
    {
      for (unsigned int i = 0; i < factor; ++i) {
        for (const auto &msg : kTestData20) {
          std::copy(msg.begin(), msg.end(), std::back_inserter(send_data_[send_data_idx_]));
          ids.push_back(send_data_idx_);
          ++send_data_idx_;
        }
      }
    }
    // send the data
    for (auto const idx : ids) {
      if (idx != 0 && idx % kTestData20.size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      web_server_->BroadcastMessageWithNotification(send_data_[idx], idx, {},
                                                    std::bind(&notification_callback, std::placeholders::_1), binary);
    }
  }

  void CheckServerSentEverything(const unsigned int factor = 1) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (true) {
      {
        std::lock_guard<std::mutex> lck(sent_ids_mtx);
        if (kTestData20.size() * factor == sent_ids.size()) {
          break;
        }
      }
      if (std::chrono::steady_clock::now() > end) {
        FAIL() << "not all messages sent within 2 seconds";
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    std::lock_guard<std::mutex> lck(sent_ids_mtx);
    EXPECT_EQ(sent_ids.size(), kTestData20.size() * factor) << "count of sent messages differs";
    for (const auto &e : sent_ids) {
      EXPECT_EQ(e.second, 1) << "message " << e.first << " should be received once";
    }
  }

  void CheckServerData(const unsigned int factor = 1) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (true) {
      {
        std::lock_guard<std::mutex> lck(server_data_mtx_);
        if (kTestData20.size() * factor == server_data_.size()) {
          break;
        }
      }
      if (std::chrono::steady_clock::now() > end) {
        FAIL() << "not all messages received within 2 seconds";
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    std::lock_guard<std::mutex> lck(server_data_mtx_);
    EXPECT_EQ(server_data_.size(), kTestData20.size() * factor) << "count of received messages differs";
    const size_t count = std::min(server_data_.size(), kTestData20.size() * factor);
    for (size_t i = 0; i < count; ++i) {
      const std::string recv(server_data_[i].begin(), server_data_[i].end());
      EXPECT_EQ(recv, kTestData20[i % kTestData20.size()]) << "message " << i << " differs";
    }
  }

  void SendDataFromClient(const size_t id, const unsigned int factor) {
    assert(id < clients_.size());
    assert(clients_[id].session);
    for (unsigned int i = 0; i < factor; ++i) {
      if (i != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      for (const auto &msg : kTestData20) {
        std::vector<uint8_t> data;
        std::copy(msg.begin(), msg.end(), std::back_inserter(data));
        clients_[id].session->DoWrite(data, false);
      }
    }
  }

  void CheckClientData(const size_t id, const bool binary, const unsigned int factor) {
    (void)binary;
    assert(id < clients_.size());
    assert(clients_[id].session);
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (true) {
      {
        std::lock_guard<std::mutex> lck(clients_[id].data_mtx);
        if (kTestData20.size() * factor == clients_[id].received_data.size()) {
          break;
        }
      }
      if (std::chrono::steady_clock::now() > end) {
        FAIL() << "not all messages received within a second";
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    std::lock_guard<std::mutex> lck(clients_[id].data_mtx);
    auto &data = clients_[id].received_data;
    EXPECT_EQ(data.size(), kTestData20.size() * factor) << "count of received messages differs";
    const size_t count = std::min(data.size(), kTestData20.size() * factor);
    for (size_t i = 0; i < count; ++i) {
      const std::string recv(data[i].first.begin(), data[i].first.end());
      EXPECT_EQ(recv, kTestData20[i % kTestData20.size()]) << "message " << i << " differs";
      EXPECT_EQ(data[i].second, binary) << "message " << i << " was not correctly transmitted (binary vs. text)";
    }
  }

  void ShutDown() {
    for (auto &client : clients_) {
      if (client.session)
        client.session->DoClose();
      if (client.thread.joinable())
        client.thread.join();
    }
    running_clients_ = 0u;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(web_server_->GetActiveSessions().size(), 0)
        << "the WebServer should not have any active connections anymore";
    web_server_.reset();
  }

  static uint16_t GetNextPort() {
    std::lock_guard<std::mutex> lck(port_list_mtx_);
    if (port_list_.empty()) {
      warn(__FILE__, "The list of usable ports is already empty, using port 8081");
      return 8081u;
    }
    const uint16_t port = port_list_.front();
    port_list_.pop_front();
    return port;
  }

  const uint16_t port_;
  std::string host_{kLocalhost};
  std::shared_ptr<IWebServer> web_server_;
  next::sdk::InitChain::ConfigMap initmap_;

  struct ClientInfo {
    std::shared_ptr<WebSocketClient> session;
    std::vector<std::pair<std::vector<uint8_t>, bool>> received_data;
    std::mutex data_mtx;
    std::thread thread;
    boost::asio::io_context ioc;
  };
  std::array<ClientInfo, 2> clients_;
  unsigned int running_clients_{0};

  std::vector<std::vector<uint8_t>> server_data_;
  std::mutex server_data_mtx_;

  std::unordered_map<size_t, std::vector<uint8_t>> send_data_;
  size_t send_data_idx_{0u};

  static std::list<uint16_t> port_list_;
  static std::mutex port_list_mtx_;
};

std::list<uint16_t> ComponentBridgesWebServerTest::port_list_ = {5555u, 5556u, 5557u, 5558u, 5559u,
                                                                 5560u, 5561u, 5562u, 5563u};
std::mutex ComponentBridgesWebServerTest::port_list_mtx_;

TEST_F(ComponentBridgesWebServerTest, serverBroadcastMessageWithNotification_nonbinaryData_dataSent) {
  const unsigned int factor = 3;
  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client
  StartClient(false);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  // send the data from the server to the client
  SendDataFromServer(false, factor);

  // check that the server sent all messages
  CheckServerSentEverything(factor);

  // check that the client received the data
  CheckClientData(0u, false, factor);

  EXPECT_EQ(server_data_.size(), 0) << "the server should not have received any data";

  // Close client socket
  ShutDown();
}

TEST_F(ComponentBridgesWebServerTest, serverBroadcastMessageWithNotification_binaryData_dataSent) {
  const unsigned int factor = 3;
  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client
  StartClient(false);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  // send the data from the server to the client
  SendDataFromServer(true, factor);

  // check that the server sent all messages
  CheckServerSentEverything(factor);

  // check that the client received the data
  CheckClientData(0u, true, factor);

  EXPECT_EQ(server_data_.size(), 0) << "the server should not have received any data";

  // Close client socket
  ShutDown();
}

TEST_F(ComponentBridgesWebServerTest, clientDoWrite_textData_serverReceivesData) {
  // in this test we do not care whether the server recognizes that the data was binary or text
  // it simply has to work to transmit data

  const unsigned int factor = 3;

  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client
  StartClient(false);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  // send data from the client to the server
  SendDataFromClient(0u, factor);

  // check that the server received the data
  CheckServerData(factor);

  for (auto &client : clients_) {
    EXPECT_EQ(client.received_data.size(), 0) << "the client should not have received any data";
  }

  // Close client socket
  ShutDown();
}

TEST_F(ComponentBridgesWebServerTest, clientDoWrite_binaryData_serverReceivesData) {
  // in this test we do not care whether the server recognizes that the data was binary or text
  // it simply has to work to transmit data

  const unsigned int factor = 3;

  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client
  StartClient(false);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  // send data from the client to the server
  SendDataFromClient(0u, factor);

  // check that the server received the data
  CheckServerData(factor);

  for (auto &client : clients_) {
    EXPECT_EQ(client.received_data.size(), 0) << "the client should not have received any data";
  }

  // Close client socket
  ShutDown();
}

TEST_F(ComponentBridgesWebServerTest, foxgloveConnection_correctResponse) {
  using namespace std::chrono_literals;

  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client, using foxglove protocol
  StartClient(true);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  auto &client = clients_[0];
  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  {
    while (true) {
      {
        std::lock_guard<std::mutex> lck(client.data_mtx);
        if (1 == client.received_data.size()) {
          break;
        }
      }
      if (std::chrono::steady_clock::now() > end) {
        FAIL() << "not all messages received within a second";
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    std::lock_guard<std::mutex> lck(client.data_mtx);
    auto &data = client.received_data;
    ASSERT_EQ(data.size(), 1u) << "the client should exactly receive one message: the foxglove server info";
    EXPECT_EQ(std::string(data[0].first.begin(), data[0].first.end()),
              std::string("{\"op\":\"serverInfo\",\"name\":\"NEXT simulation "
                          "backend\",\"capabilities\":[\"time\"],\"sessionId\":\"0\"}"));
    EXPECT_EQ(data[0].second, false) << "the answer should be text, not binary";
    data.clear();
  }

  // Close client socket
  ShutDown();
}

TEST_F(ComponentBridgesWebServerTest, serverBroadcastMessageWithNotification_specificSessionIds_noCrash) {
  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting two clients
  StartClient(false);
  StartClient(false);
  // wait until the clients connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer(2u));

  // Send one message per client from the server, with session id 0 and 1 it shall work, the other ones not. The others
  // should not cause a crash.
  std::vector<uint8_t> data;
  std::copy(kTestData5[0].begin(), kTestData5[0].end(), std::back_inserter(data));
  for (size_t id = 0; id < 10; ++id) {
    web_server_->BroadcastMessageWithNotification(data, id, {id},
                                                  std::bind(&notification_callback, std::placeholders::_1));
  }

  // Waiting a moment to give the server time to proceed and make sure that it is not calling the callback function
  // after a short time
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // now check on the two clients that the data was received
  auto end = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  for (size_t i = 0u; clients_.size() > i; ++i) {
    auto &client = clients_[i];
    while (true) {
      {
        std::lock_guard<std::mutex> lck(client.data_mtx);
        if (1 == client.received_data.size()) {
          break;
        }
      }
      if (std::chrono::steady_clock::now() > end) {
        FAIL() << "not all messages received within a second";
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    std::lock_guard<std::mutex> lck(client.data_mtx);
    ASSERT_EQ(client.received_data.size(), 1) << "there should be one message on client " << i;
    const std::string recv(client.received_data[0].first.begin(), client.received_data[0].first.end());
    EXPECT_EQ(recv, kTestData5[0]) << "message on client " << i << " is wrong";
  }

  // for all messages the callbacks shall be called
  end = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  while (true) {
    {
      if (10 == sent_ids.size()) {
        break;
      }
    }
    if (std::chrono::steady_clock::now() > end) {
      FAIL() << "the server did not call all callback functions within the given time";
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }
  ASSERT_EQ(sent_ids.size(), 10);
  for (size_t i = 0; i < 10; ++i) {
    EXPECT_EQ(sent_ids[i], 1) << "for message id " << i << " the count of send events is wrong";
  }

  ShutDown();
}

TEST_F(ComponentBridgesWebServerTest, serverBroadcastMessageWithNotification_doubleSessionId_dataSentTwiceToClient) {
  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client
  StartClient(false);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  // Send one message to the same client twice
  std::vector<uint8_t> data;
  std::copy(kTestData5[0].begin(), kTestData5[0].end(), std::back_inserter(data));
  web_server_->BroadcastMessageWithNotification(data, 5, {0, 0},
                                                std::bind(&notification_callback, std::placeholders::_1));

  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(1);
  auto &client = clients_[0];
  while (true) {
    {
      std::lock_guard<std::mutex> lck(client.data_mtx);
      if (2 == client.received_data.size()) {
        break;
      }
    }
    if (std::chrono::steady_clock::now() > end) {
      FAIL() << "not all messages received within a second";
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }

  // The callback shall be called
  ASSERT_EQ(sent_ids.size(), 1);
  EXPECT_EQ(sent_ids[5], 1);

  // and now check the client data
  std::lock_guard<std::mutex> lck(client.data_mtx);
  ASSERT_EQ(client.received_data.size(), 2) << "there should be two messages";
  const std::string recv1(client.received_data[0].first.begin(), client.received_data[0].first.end());
  EXPECT_EQ(recv1, kTestData5[0]) << "received message is incorrect";
  const std::string recv2(client.received_data[1].first.begin(), client.received_data[1].first.end());
  EXPECT_EQ(recv2, kTestData5[0]) << "received message is incorrect";
}

TEST_F(ComponentBridgesWebServerTest, serverBroadcastMessageWithNotification_invalidSessionId_noDataSentToClient) {
  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // Starting WebSocket client
  StartClient(false);
  // wait until the client connected to the server
  ASSERT_TRUE(AwaitConnectionOnServer());

  // Send one message to non existing session ID, giving it twice but the callback shall be called only once
  std::vector<uint8_t> data;
  std::copy(kTestData5[0].begin(), kTestData5[0].end(), std::back_inserter(data));
  web_server_->BroadcastMessageWithNotification(data, 5, {42, 42},
                                                std::bind(&notification_callback, std::placeholders::_1));

  // Waiting a moment to give the server time to proceed and make sure that it is not calling the callback function
  // after a short time
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // The callback shall be called
  ASSERT_EQ(sent_ids.size(), 1);
  EXPECT_EQ(sent_ids[5], 1);

  EXPECT_TRUE(clients_[0].received_data.empty())
      << "the client should be session ID 0, so it should not receive any data";
}

TEST_F(ComponentBridgesWebServerTest, serverBroadcastMessageWithNotification_GetSessionIds_correctIdsReturned) {
  // Starting WebSocket server
  StartServer();
  ASSERT_EQ(web_server_->GetNumOfActiveConnections(), 0) << "the server should not have any connections so far";

  // starting the first client, normal, and wait for its connection
  StartClient(false);
  ASSERT_TRUE(AwaitConnectionOnServer(1u));

  {
    // check that it is sorted correctly
    const auto ids_normal = web_server_->GetSessionIds(webserver::ConnectionType::normal);
    ASSERT_EQ(ids_normal.size(), 1u);
    EXPECT_EQ(ids_normal[0], 0u);
    const auto ids_foxglove = web_server_->GetSessionIds(webserver::ConnectionType::foxglove);
    EXPECT_TRUE(ids_foxglove.empty());
  }

  // start the second client, this time a foxglove client
  StartClient(true);
  ASSERT_TRUE(AwaitConnectionOnServer(2u));

  {
    // check that both clients are sorted correctly
    const auto ids_normal = web_server_->GetSessionIds(webserver::ConnectionType::normal);
    ASSERT_EQ(ids_normal.size(), 1u);
    EXPECT_EQ(ids_normal[0], 0u);
    const auto ids_foxglove = web_server_->GetSessionIds(webserver::ConnectionType::foxglove);
    ASSERT_EQ(ids_foxglove.size(), 1u);
    EXPECT_EQ(ids_foxglove[0], 1u);
  }

  // now close them one after the other
  clients_[0].session->DoClose();
  ASSERT_TRUE(AwaitConnectionOnServer(1u));

  {
    const auto ids_normal = web_server_->GetSessionIds(webserver::ConnectionType::normal);
    EXPECT_TRUE(ids_normal.empty());
    const auto ids_foxglove = web_server_->GetSessionIds(webserver::ConnectionType::foxglove);
    ASSERT_EQ(ids_foxglove.size(), 1u);
    EXPECT_EQ(ids_foxglove[0], 1u);
  }

  clients_[1].session->DoClose();
  ASSERT_TRUE(AwaitConnectionOnServer(0u));

  {
    const auto ids_normal = web_server_->GetSessionIds(webserver::ConnectionType::normal);
    EXPECT_TRUE(ids_normal.empty());
    const auto ids_foxglove = web_server_->GetSessionIds(webserver::ConnectionType::foxglove);
    EXPECT_TRUE(ids_foxglove.empty());
  }

  ShutDown();
}

} // namespace test
} // namespace bridgesdk
} // namespace next

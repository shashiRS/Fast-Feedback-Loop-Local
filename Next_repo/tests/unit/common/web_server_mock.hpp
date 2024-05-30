/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     web_server_mock.hpp
 * @brief    mock of the Webserver
 *
 **/

#ifndef WEB_SERVER_MOCK_H_
#define WEB_SERVER_MOCK_H_

#include <iostream>
#include <unordered_map>

#include <next/bridgesdk/web_server.hpp>

namespace next {
namespace test {

class WebServerMock : virtual public next::bridgesdk::webserver::IWebServer {
public:
  WebServerMock(){};
  ~WebServerMock(){};
  void BroadcastMessageWithNotification(const std::vector<uint8_t> &data, size_t size, std::vector<size_t>,
                                        std::function<void(size_t)> fCall, [[maybe_unused]] bool binary) override {
#ifdef _DEBUG
    std::cout << "Mock DataManager PublishDataToClient" << std::endl;
#endif

#ifdef _DEBUG
    std::cout << "bytes provided: " << data.size() << std::endl;
#endif

    data_.push_back(data);

    if (fCall) {
      fCall(size);
    }
  }

  void SetIpAddress(const std::string &) override{};
  void SetPortNumber(uint16_t) override{};
  void SetNumOfConnections(uint8_t) override{};
  size_t GetNumOfActiveConnections() override { return GetActiveSessions().size(); };
  void AddForwardMessageCallback([[maybe_unused]] next::bridgesdk::webserver::ForwardMsgCallback callback) override{};
  void ActivateListingToSocket() override{};
  std::unordered_map<size_t, std::shared_ptr<bridgesdk::webserver::Session>> &GetActiveSessions() override {
    return resp_;
  };
  std::vector<size_t> GetSessionIds(const bridgesdk::webserver::ConnectionType) const override {
    std::vector<size_t> res;
    for (const auto &s : resp_) {
      if (s.second)
        res.push_back(s.first);
    }
    return res;
  }

  std::list<std::vector<uint8_t>> data_;

private:
  std::unordered_map<size_t, std::shared_ptr<next::bridgesdk::webserver::Session>> resp_;
};

} // namespace test
} // namespace next

#endif // WEB_SERVER_MOCK_H_

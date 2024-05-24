/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_request_server.cpp
 *  @brief    stub
 */
#include <utility>

#include <next/sdk/logger/logger.hpp>

#include <next/bridgesdk/web_server.hpp>
#include "data_class_manager.h"

#include "data_request_server.h"

namespace next {
namespace databridge {
namespace data_request_server {

void DataRequestServer::StartupWebServer(const std::string &ip_adress_set, uint16_t port_set) {
  auto AdaptDataFromCallback = [this](const std::vector<uint8_t> &char_vector,
                                      const bridgesdk::webserver::ConnectionType type) -> void {
    std::string str(char_vector.begin(), char_vector.end());
    this->AddRequestToQueue(std::move(str), type);
  };

  sp_web_server = bridgesdk::webserver::CreateWebServer();
  sp_web_server->SetPortNumber(port_set);
  sp_web_server->SetIpAddress(ip_adress_set);
  sp_web_server->SetNumOfConnections(1u);
  sp_web_server->AddForwardMessageCallback(
      std::bind(AdaptDataFromCallback, std::placeholders::_1, std::placeholders::_2));
  sp_web_server->ActivateListingToSocket();
}

void DataRequestServer::SetWebServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) {
  auto AdaptDataFromCallback = [this](const std::vector<uint8_t> &char_vector,
                                      const bridgesdk::webserver::ConnectionType type) -> void {
    std::string str(char_vector.begin(), char_vector.end());
    this->AddRequestToQueue(std::move(str), type);
  };
  sp_web_server = ptr;
  sp_web_server->AddForwardMessageCallback(
      std::bind(AdaptDataFromCallback, std::placeholders::_1, std::placeholders::_2));
}

std::shared_ptr<bridgesdk::webserver::IWebServer> DataRequestServer::GetWebServer() { return sp_web_server; }

void DataRequestServer::ShutDownServer() { sp_web_server.reset(); }

void DataRequestServer::AddRequestToQueue(std::string req_str, bridgesdk::webserver::ConnectionType type) {
  Json::Reader reader;
  Json::Value command;

  bool parseSuccess = reader.parse(req_str, command, false);

  if (!parseSuccess) {
    debug(__FILE__, " could not parse request {}", req_str);
    return;
  }

  if (command.isMember("priority") && !command["priority"].isInt()) {
    command["priority"] = 0;
  }
  const std::lock_guard<std::mutex> lock(request_queue_mutex);
  request_queue.push({command.isMember("priority") ? command["priority"].asInt() : 0, std::move(req_str), type});
}

void DataRequestServer::HandleRequest(const WebSocketRequest &req) {
  Json::Reader reader;
  Json::Value command;

  bool parseSuccess = reader.parse(req.request, command, false);

  if (!parseSuccess)
    return;

  if (auto sp_data_class_manager = wp_data_class_manager.lock()) {
    // TODO forward connection type
    handler_.HandleEvent(command, sp_data_class_manager);
  }
}

void DataRequestServer::CheckQueueAndReply() {
  const std::lock_guard<std::mutex> lock(request_queue_mutex);
  while (!request_queue.empty()) {
    const WebSocketRequest &req = request_queue.top();
    HandleRequest(req);
    request_queue.pop();
  }
}

void DataRequestServer::CheckQueueAndReplyInBackgroundMode() {
  auto checkQueueAndReplyInfLoop = [sp = this]() {
    while (sp->b_check_queue) {
      sp->CheckQueueAndReply();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  };

  check_queue_and_reply_fut = std::async(std::launch::async, checkQueueAndReplyInfLoop);
}

const WebSocketRequest &DataRequestServer::GetTopRatedRequest() const { return request_queue.top(); }

size_t DataRequestServer::GetRequestNbr() const { return request_queue.size(); }

void DataRequestServer::SetupDataClassManager(std::shared_ptr<DataClassManager> spDataClassManager) {
  wp_data_class_manager = spDataClassManager;
}

DataRequestServer::~DataRequestServer() {
  // to "kill" the thread corresponding to checkQueueAndReplyFut
  b_check_queue = false;

  ShutDownServer();
}

} // namespace data_request_server
} // namespace databridge
} // namespace next

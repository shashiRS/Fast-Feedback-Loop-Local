/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     version_info_event_distributor.cpp
 * @brief    implementation of small module to handle version info event messages
 **/

#include "version_info_event_distributor.h"

#include <json/json.h>

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace controlbridge {
namespace event_handler {

constexpr int k_wait_for_ecal_register = 2000;

void VersionInfoCommandCompleted(std::vector<uint8_t> *memory) {
  delete memory;
  debug(__FILE__, "delete memory");
}

VersionEventDistributor::VersionEventDistributor(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in)
    : version_event_subscriber_("") {
  eCAL::Util::EnableLoopback(true);
  this->webserver_ = webserver_in;
}

VersionEventDistributor::~VersionEventDistributor() {
  std::lock_guard<std::mutex> lock(version_mutex_);
  if (version_thread_.joinable()) {
    version_thread_.join();
  }
}

void VersionEventDistributor::startVersionThread() {
  version_thread_ = std::thread([&]() { startVersion(); });
}

void VersionEventDistributor::startVersion() {
  std::lock_guard<std::mutex> lock(version_mutex_);
  auto event_hook = std::bind(&VersionEventDistributor::getVersionInfoEventHook, this);
  version_event_subscriber_.subscribe();
  version_event_subscriber_.addEventHook(event_hook);
  version_event_subscriber_.requestEventUpdate();
}

void VersionEventDistributor::getVersionInfoEventHook() {
  next::sdk::events::VersionInfoPackage rec_version_info = version_event_subscriber_.getEventData();
  if (!rec_version_info.component_name.empty()) {
    this->sendJsonToGUI(this->createJson(rec_version_info));
  }
}

std::string VersionEventDistributor::createJson(const next::sdk::events::VersionInfoPackage &version_info) {

  Json::Value root;
  Json::Value payload;

  root["channel"] = "player";
  root["source"] = "ControlBridge";
  root["event"] = "VersionSystemUpdate";

  payload["name"] = version_info.component_name;
  payload["version"] = version_info.component_version;
  payload["documentation"] = version_info.documentation_link;

  root["payload"][version_info.component_name] = payload;

  Json::FastWriter writer;
  std::string response = writer.write(root);
  return response;
}

void VersionEventDistributor::sendJsonToGUI(std::string msg) {
  auto heap_string = new std::vector<uint8_t>();
  heap_string->assign(msg.begin(), msg.end());
  webserver_->BroadcastMessageWithNotification(*heap_string, 0, {},
                                               std::bind(VersionInfoCommandCompleted, heap_string));
}

void VersionEventDistributor::requestVersions() {
  std::lock_guard<std::mutex> lock(version_mutex_);
  version_event_subscriber_.requestEventUpdate();
}

} // namespace event_handler
} // namespace controlbridge
} // namespace next

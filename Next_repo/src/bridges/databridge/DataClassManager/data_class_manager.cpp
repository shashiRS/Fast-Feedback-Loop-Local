/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_class_manager.cpp
 *  @brief    stub
 */

#include "data_class_manager.h"
#include "data_request_server.h"

namespace next {
namespace databridge {

DataClassManager::DataClassManager() {}

void DataClassManager::InitBackgroundMode(const std::string &ip_adress, uint16_t port) {
  // create a data request server
  sp_data_request_server_ = std::make_shared<next::databridge::data_request_server::DataRequestServer>();
  // and a class manager for it
  sp_data_request_server_->StartupWebServer(ip_adress, port);
  sp_data_request_server_->SetupDataClassManager(shared_from_this());

  sp_data_request_server_->CheckQueueAndReplyInBackgroundMode();
}

void DataClassManager::SetWebServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) {
  sp_data_request_server_ = std::make_shared<next::databridge::data_request_server::DataRequestServer>();
  sp_data_request_server_->SetWebServer(ptr);
  sp_data_request_server_->SetupDataClassManager(shared_from_this());
  sp_data_request_server_->CheckQueueAndReplyInBackgroundMode();
}

std::shared_ptr<bridgesdk::webserver::IWebServer> DataClassManager::GetWebServer() {
  return sp_data_request_server_->GetWebServer();
}

void DataClassManager::SetupDataRequestServer(
    std::shared_ptr<next::databridge::data_request_server::DataRequestServer> sp_data_request_server_rhs) {
  sp_data_request_server_ = sp_data_request_server_rhs;
  sp_data_request_server_->SetupDataClassManager(shared_from_this());
}

void DataClassManager::AddUpdateRequestToQueue(DataRequestItem data_request_item,
                                               std::function<void(std::string)> callback) {
  std::string par{"data request added to queue: " + data_request_item.name};
  data_request_queue_.push(data_request_item);
  callback(par);
}

void DataClassManager::GetDataClassRequestBlocking(DataRequestItem &value) { data_request_queue_.wait_and_pop(value); }

bool DataClassManager::GetDataClassRequestNonBlocking(DataRequestItem &value) {
  return data_request_queue_.try_and_pop(value);
}

void DataClassManager::AddConfigRequestToQueue(ConfigDataRequestItem request_item) {
  config_data_request_queue_.push(request_item);
}

bool DataClassManager::GetConfigDataClassRequestNonBlocking(ConfigDataRequestItem &value) {
  return config_data_request_queue_.try_and_pop(value);
}

void DataClassManager::AddBackendRequestToQueue(BackendConfigRequestItem request_item) {
  backend_request_queue_.push(request_item);
}

bool DataClassManager::GetBackendRequestNonBlocking(BackendConfigRequestItem &value) {
  return backend_request_queue_.try_and_pop(value);
}

} // namespace databridge
} // namespace next

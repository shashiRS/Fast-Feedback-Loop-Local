/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */

#include "udex_service.hpp"

#include <next/sdk/sdk_macros.h>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator_player.hpp>
#include <next/sdk/logger/logger.hpp>

UdexService::UdexService() {}

UdexService::~UdexService() {}

bool UdexService::Initialize() {
  LoadUdexLibrary();
  return true;
}

void UdexService::Terminate() {
  if (udex_publisher_) {
    udex_publisher_->Reset();
  }

  return;
}
void UdexService::SetLoadRecordingInfoCallback(next::udex::statusCallbackT callback) {
  load_recording_progress_callback_ = callback;
}

bool UdexService::RegisterDataDescriptions(const std::vector<next::sdk::types::DataDescription> &data_descriptions) {

  if (udex_publisher_.get() == nullptr) {
    return false;
  }

  bool success = true;

  for (const auto &description : data_descriptions) {
    success &= udex_publisher_->RegisterDataDescription(description);
  }
  success &= LoadProcessors();
  return success;
}

bool UdexService::LoadProcessors() {
  bool success = true;
  using config = next::appsupport::ConfigClient;
  using namespace next::appsupport::configkey::player;
  auto search_paths = config::getConfig()->getStringList(getProcessorPathListKey(), {});
  for (const auto &path : search_paths) {
    if (path == "undefined") {
      continue;
    }
    success &= udex_publisher_->LoadProcessors(path);
  }
  if (search_paths.empty()) {
    config::getConfig()->put(getProcessorPathListKey(), "undefined");
  }
  return success;
}

const std::unordered_map<std::string, size_t> &UdexService::GetTopicsAndHashes() {
  if (udex_publisher_) {
    return udex_publisher_->getTopicsAndHashes();
  }
  static std::unordered_map<std::string, size_t> dummy;
  return dummy;
}

std::string UdexService::GetFilterExpressionFromUrls(const std::vector<std::string> &urls) const {
  if (udex_publisher_) {
    return udex_publisher_->GetFilterExpressionFromURLs(urls);
  }
  return "";
}

bool UdexService::PublishPackage(std::unique_ptr<next::sdk::types::IPackage> &package) {
  if (udex_publisher_) {
    return udex_publisher_->publishPackage(package);
  }
  return "";
}

void UdexService::LoadUdexLibrary() {
  try {
    // dll is now loaded by the OS
    // instantiating smart pointer for Udex access
    if (udex_publisher_ == nullptr) {
      udex_publisher_ = std::make_unique<next::udex::publisher::DataPublisher>("Next-Player");
    }
  } catch (std::exception &e) {
    error(__FILE__, "Failed to load shared library: \"{}\"", e.what());
    return;
  }
}

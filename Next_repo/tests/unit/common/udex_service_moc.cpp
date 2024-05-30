
#include "udex_service_moc.hpp"
#include <next/sdk/logger/logger.hpp>

UdexService::UdexService() {
  publish_possible_ = true;
  publish_package_count_ = UINT32_MAX;
  trigger_possible_ = true;
  trigger_package_count_ = UINT32_MAX;
  publish_microsecond_sleep_ = 0;
}

UdexService::~UdexService() {}

bool UdexService::Initialize() { return true; }

void UdexService::Terminate() { return; }

bool UdexService::PublishPackage(std::unique_ptr<next::sdk::types::IPackage> &package) {
  if (publish_microsecond_sleep_) {
    sleep_for(publish_microsecond_sleep_);
  }
  if (publish_package_count_ == 0) {
    publish_possible_ = false;
  } else {
    publish_package_count_--;
    published_packages_size.push_back(package->GetPackageHeader().timestamp);
  }
  return publish_possible_;
}

void UdexService::LoadUdexLibrary() {}
void UdexService::disable_publish() { publish_possible_ = false; }
void UdexService::enable_publish() { publish_possible_ = true; }
void UdexService::set_publish_count(uint32_t count) { publish_package_count_ = count; }
void UdexService::disable_trigger() { trigger_possible_ = false; }
void UdexService::enable_trigger() { trigger_possible_ = true; }
void UdexService::set_trigger_count(uint32_t count) { trigger_package_count_ = count; }
void UdexService::setPublishDuration(uint32_t countMicroseconds) { publish_microsecond_sleep_ = countMicroseconds; }
size_t UdexService::GetPublishedPackageTimeByIndex(size_t idx) {
  if (idx < published_packages_size.size()) {
    return published_packages_size[idx];
  } else {
    return 0;
  }
}
bool UdexService::RegisterDataDescriptions(const std::vector<next::sdk::types::DataDescription> &data_descriptions) {
  if (data_descriptions.size() != 0) {
    return true;
  }
  return false;
}
size_t UdexService::GetPublishedPackageCount() { return published_packages_size.size(); }

std::string UdexService::GetFilterExpressionFromUrls([[maybe_unused]] const std::vector<std::string> &urls) const {
  return "";
}

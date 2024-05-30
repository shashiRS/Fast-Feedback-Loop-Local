
/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     udex_service.hpp
 * @brief    Wrapper for the access to the Udex library for data publishing.
 */

#ifndef UDEX_SERVICE_H_
#define UDEX_SERVICE_H_

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <next/sdk/types/data_description_types.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include <thread>

NEXT_RESTORE_WARNINGS_WINDOWS

class UdexService {
public:
  UdexService();
  ~UdexService();

  bool Initialize();
  void Terminate();
  const std::unordered_map<std::string, size_t> &GetTopicsAndHashes();
  bool PublishPackage(std::unique_ptr<next::sdk::types::IPackage> &package);
  void disable_publish();
  void enable_publish();
  void set_publish_count(uint32_t count);
  void disable_trigger();
  void enable_trigger();
  void set_trigger_count(uint32_t count);
  void setPublishDuration(uint32_t countMicroseconds);
  size_t UdexService::GetPublishedPackageTimeByIndex(size_t idx);
  size_t GetPublishedPackageCount();
  std::string GetFilterExpressionFromUrls(const std::vector<std::string> &urls) const;
  bool RegisterDataDescriptions(const std::vector<next::sdk::types::DataDescription> &data_descriptions);

private:
  void LoadUdexLibrary();

private:
  template <typename T>
  using duration = std::chrono::duration<T>;

  static void sleep_for(double dt) {
    static constexpr duration<double> MinSleepDuration(0);
    std::chrono::steady_clock ::time_point start = std::chrono::steady_clock ::now();
    while (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock ::now() - start)
                      .count()) < dt) {
      std::this_thread::sleep_for(MinSleepDuration);
    }
  }

  std::unique_ptr<next::udex::publisher::DataPublisher> IUDex_Publisher_;
  bool publish_possible_;
  bool trigger_possible_;
  uint32_t publish_package_count_;
  uint32_t trigger_package_count_;
  uint32_t publish_microsecond_sleep_;
  std::condition_variable publish_duration_cond_variable_;
  std::mutex publish_duration_mutex_;
  std::vector<size_t> published_packages_size;
};
#endif

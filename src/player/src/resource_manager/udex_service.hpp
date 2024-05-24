
/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     udex_service.hpp
 * @brief    Wrapper for the access to the Udex library for data publishing.
 */

#ifndef UDEX_SERVICE_H_
#define UDEX_SERVICE_H_

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/runtime/offline/replay_proxy.hpp>
NEXT_RESTORE_WARNINGS

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
#include <next/udex/publisher/data_publisher.hpp>
NEXT_RESTORE_WARNINGS_WINDOWS

#include <next/sdk/types/data_description_types.hpp>
#include <next/sdk/types/package_data_types.hpp>

class UdexService {
public:
  UdexService();
  virtual ~UdexService();

  bool Initialize();
  void Terminate();
  bool RegisterDataDescriptions(const std::vector<next::sdk::types::DataDescription> &data_descriptions);

  // map which contains Topics/signal URLs and their hash values
  const std::unordered_map<std::string, size_t> &GetTopicsAndHashes();

  bool PublishPackage(std::unique_ptr<next::sdk::types::IPackage> &package);

  std::string GetFilterExpressionFromUrls(const std::vector<std::string> &urls) const;
  void SetLoadRecordingInfoCallback(next::udex::statusCallbackT callback);

private:
  void LoadUdexLibrary();
  bool LoadProcessors();

private:
  std::shared_ptr<mts::platform::shared_library> library_;
  std::unique_ptr<next::udex::publisher::DataPublisher> udex_publisher_;
  next::udex::statusCallbackT load_recording_progress_callback_ = NULL;
};
#endif // UDEX_SERVICE_H_

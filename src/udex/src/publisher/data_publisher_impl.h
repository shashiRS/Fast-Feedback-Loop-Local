/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * TODO This is just a first idea - the actual implementation has to be done
 * @file     DataPublisher.h
 * @brief    Class to handle the registration of a new data provider
 *           to enable publishing data via the middleware network
 */

#ifndef NEXT_UDEX_DATA_PUBLISHER_IMPL_H_
#define NEXT_UDEX_DATA_PUBLISHER_IMPL_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <next/sdk/events/update_topic_cache.h>
#include <next/sdk/sdk_macros.h>
#include <next/sdk/types/package_data_types.hpp>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/udex_extractor.h>
#include <mts/extensibility/data_source.hpp>
#include <mts/runtime/data_source_provider.hpp>
NEXT_RESTORE_WARNINGS

#include <boost/concept_check.hpp>
#include <next/udex/data_types.hpp>

#include "../udex_manager.hpp"

#include "../signal_storage/udex_dynamic_publisher.h"
#include "../signal_storage/url_helper.hpp"

#include "core_lib_hash_manager.hpp"

// delay in ms between creation of the publisher and the first publishData
#define PUBLISHER_INIT_TO_SEND_DELAY 2000

class data_package;

namespace next {
namespace udex {
namespace publisher {

enum class UpdateType { ADD_TOPICS = 0, REMOVE_TOPICS };

class DataPublisherImpl {
public:
  DataPublisherImpl(const std::string &data_publisher_name);

  virtual ~DataPublisherImpl();

  bool Reset(void);

  std::string getName();

  bool SetDataSourceInfo(const std::string &data_source_name, uint16_t source_id, uint32_t instance_number,
                         const std::string &format_type);

  bool RegisterDataDescription(const std::string &description_filepath, const bool create_publishers = false,
                               DescriptionType description_type = DESCRIPTION_TYPE_SDL,
                               statusCallbackT callback = NULL);
  bool RegisterDataDescription(const char *file_name, void *binary_description, const size_t content_size = 0,
                               const bool create_publishers = false,
                               DescriptionType description_type = DESCRIPTION_TYPE_SDL,
                               statusCallbackT callback = NULL);
  bool RegisterDataDescription(const std::string &port_name, size_t port_size, const bool create_publishers = false,
                               const std::string &description_type = "NONE", statusCallbackT callback = NULL);

  bool RegisterDataDescription(const next::sdk::types::DataDescription &data_description,
                               statusCallbackT callback = NULL);

  bool LoadProcessors(const std::string &path);

  const std::unordered_map<std::string, size_t> &getTopicsAndHashes();

  bool publishData(const uint64_t vaddr, const void *binary_data, const size_t binary_size,
                   const uint64_t trigger_timestamp);
  bool publishData(const std::string &port_name, const void *binary_data, const size_t binary_size, bool append_device,
                   const uint64_t trigger_timestamp);

  bool publishPackage(std::unique_ptr<sdk::types::IPackage> &package);
  std::string GetFilterExpressionFromURLs(const std::vector<std::string> &urls) const;

private:
  bool publishPackageInternal(const std::unique_ptr<sdk::types::IPackage> &package);
  bool publishProcessorPackages(std::unique_ptr<sdk::types::IPackage> &package);

  void createPublishersFromTopicList(const bool create_publishers, const bool use_type_schema,
                                     statusCallbackT callback = NULL);
  void deletePublishers(void);

  void NotifyUpdateTopicCache(std::vector<std::string> &topic_list, UpdateType op_type);

  void SetProcessorInfoToConfig(const std::string &processor_name,
                                const next::sdk::types::IPackage::PackageMetaInfo &package_infos) const;

  bool SendPackage(const std::unique_ptr<sdk::types::IPackage> &package,
                   const std::shared_ptr<ecal_util::DynamicPublisher> &pub, size_t fallback_time);

  size_t waitForCreation(const size_t creation_time) const;

  void GetPackageNameAndHash(const std::unique_ptr<sdk::types::IPackage> &package, size_t &hash,
                             std::string &package_name);

  bool SendMsg(const void *binary_data, const size_t binary_size, const uint64_t trigger_timestamp,
               const std::shared_ptr<ecal_util::DynamicPublisher> &pub, size_t now_since_epoc_ms) const;

  std::string data_publisher_name_;
  std::unordered_map<size_t, std::shared_ptr<ecal_util::DynamicPublisher>> publishers_;
  std::unordered_map<size_t, long long> publisher_creation_time_;

  std::unordered_map<std::string, size_t> url_hash_map_;

  CoreLibHashManager core_lib;

  next::sdk::events::UpdateTopicCache update_topic_cache_event_{"update topic cache event"};

  next::udex::UdexManager &udex_manager = UdexManager::GetInstance();
};

} // namespace publisher
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_DATA_PUBLISHER_IMPL_H_

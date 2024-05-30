/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     data_publisher.cpp
 * @brief    Class to handle the registration of a new data provider
 *           to enable publishing data via the middleware network
 */

#include <next/udex/publisher/data_publisher.hpp>

#include "data_publisher_impl.h"

namespace next {
namespace udex {
namespace publisher {

DataPublisher::DataPublisher(const std::string &data_publisher_name) {
  impl_ = std::make_unique<DataPublisherImpl>(data_publisher_name);
}

DataPublisher::~DataPublisher(void) { impl_.reset(); }

bool DataPublisher::Reset(void) {
  if (!impl_) {
    return false;
  }
  return impl_->Reset();
}

std::string DataPublisher::getName() {
  if (!impl_) {
    return "Impl not initialized";
  }
  return impl_->getName();
}

bool DataPublisher::SetDataSourceInfo(const std::string &data_source_name, uint16_t source_id, uint32_t instance_number,
                                      const std::string &format_type) {
  if (!impl_) {
    return false;
  }
  return impl_->SetDataSourceInfo(data_source_name, source_id, instance_number, format_type);
}

bool DataPublisher::LoadProcessors(const std::string &path) {
  if (!impl_) {
    return false;
  }
  return impl_->LoadProcessors(path);
}

bool DataPublisher::RegisterDataDescription(const std::string &description_filepath, const bool create_publishers,
                                            DescriptionType description_type, statusCallbackT callback) {
  if (!impl_) {
    return false;
  }
  return impl_->RegisterDataDescription(description_filepath, create_publishers, description_type, callback);
}

bool DataPublisher::RegisterDataDescription(const char *file_name, void *binary_description, const size_t content_size,
                                            const bool create_publishers, DescriptionType description_type,
                                            statusCallbackT callback) {
  if (!impl_) {
    return false;
  }
  return impl_->RegisterDataDescription(file_name, binary_description, content_size, create_publishers,
                                        description_type, callback);
}

bool DataPublisher::RegisterDataDescription(const std::string &port_name, size_t port_size,
                                            const bool create_publishers, const std::string &description_type,
                                            statusCallbackT callback) {
  if (!impl_) {
    return false;
  }
  return impl_->RegisterDataDescription(port_name, port_size, create_publishers, description_type, callback);
}

bool DataPublisher::RegisterDataDescription(const next::sdk::types::DataDescription &data_description,
                                            statusCallbackT callback) {
  if (!impl_) {
    return false;
  }
  return impl_->RegisterDataDescription(data_description, callback);
}

const std::unordered_map<std::string, size_t> &DataPublisher::getTopicsAndHashes() {
  static std::unordered_map<std::string, size_t> dummy;
  if (!impl_) {
    return dummy;
  }
  return impl_->getTopicsAndHashes();
}

bool DataPublisher::publishData(const uint64_t vaddr, const void *binary_data, const size_t binary_size,
                                const uint64_t trigger_timestamp) {
  if (!impl_) {
    return false;
  }
  return impl_->publishData(vaddr, binary_data, binary_size, trigger_timestamp);
}

bool DataPublisher::publishData(const std::string &port_name, const void *binary_data, const size_t binary_size,
                                bool append_device, const uint64_t trigger_timestamp) {
  if (!impl_) {
    return false;
  }
  return impl_->publishData(port_name, binary_data, binary_size, append_device, trigger_timestamp);
}

bool DataPublisher::publishPackage(std::unique_ptr<sdk::types::IPackage> &package) {
  if (!impl_) {
    return false;
  }
  return impl_->publishPackage(package);
}

std::string DataPublisher::GetFilterExpressionFromURLs(const std::vector<std::string> &urls) const {
  if (!impl_) {
    return {""};
  }
  return impl_->GetFilterExpressionFromURLs(urls);
}

} // namespace publisher
} // namespace udex
} // namespace next

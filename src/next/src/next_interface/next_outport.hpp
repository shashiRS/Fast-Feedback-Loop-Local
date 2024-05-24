/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_outport.hpp
 * @brief    provides outport functions and holds respective data
 *
 */
#ifndef NEXT_NEXT_OUTPORT_HPP
#define NEXT_NEXT_OUTPORT_HPP

#include <next/next.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/sdk.hpp>

#include <next/udex/publisher/data_publisher.hpp>

namespace next {
namespace inport {
class NextOutport {
  using config = next::appsupport::ConfigClient;

public:
  NextOutport(const std::string &component_name) : component_name_(component_name) {
    data_publisher_ = std::make_shared<next::udex::publisher::DataPublisher>(component_name_);
  }
  ~NextOutport(){};

  int SetupOutput(const std::vector<std::string> &sdl_filepath,
                  const std::vector<ComponentOutputPortConfig> &output_ports);

  void SetupSavedOutputConfig();

  void TriggerAsyncPush(const std::string &output_port_name, const DataPackage &data, bool append_device,
                        size_t trigger_timestamp);
  void TriggerAsyncPush(const uint64_t virtual_address, const DataPackage &data, size_t trigger_timestamp);

  void Reset();

  std::vector<ComponentOutputPortConfig> &GetOutports() { return configured_output_ports_; }

private:
  std::shared_ptr<next::udex::publisher::DataPublisher> data_publisher_;
  std::string component_name_;
  std::vector<ComponentOutputPortConfig> configured_output_ports_;

  struct SaveComponentOutputPortConfig {
    std::vector<std::string> sdl_filepath;
    std::vector<ComponentOutputPortConfig> output_ports;
    bool initialized_ = false;
  };
  std::vector<SaveComponentOutputPortConfig> saved_output_;

public:
  friend class NextOutportTester;
};

} // namespace inport
} // namespace next

#endif // NEXT_NEXT_OUTPORT_HPP

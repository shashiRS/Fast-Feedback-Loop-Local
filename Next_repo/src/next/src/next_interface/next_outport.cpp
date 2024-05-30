/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_outport.cpp
 * @brief    provides outport functions and holds respective data
 *
 */
#include <next/next.hpp>
#include <next_outport.hpp>

#include <boost/filesystem.hpp>

namespace next {
namespace inport {
int NextOutport::SetupOutput(const std::vector<std::string> &sdl_filepath,
                             const std::vector<ComponentOutputPortConfig> &output_ports) {

  SaveComponentOutputPortConfig saved_output_pair;
  saved_output_pair.sdl_filepath = sdl_filepath;
  saved_output_pair.output_ports = output_ports;

  for (auto &output_port_config : saved_output_pair.output_ports) {
    std::string port_name_safe = output_port_config.port_name;
    // make port safe
    if (port_name_safe.empty() || port_name_safe == " ") {
      port_name_safe = "undefined";
    }

    config::getConfig()->put(next::appsupport::configkey::getOutportSizeKey(component_name_, port_name_safe),
                             (int)output_port_config.port_size);
    /*config::getConfig()->setTag(next::appsupport::configkey::getOutportSizeKey(component_name_, port_name_safe),
                                next::appsupport::configtags::TAG::READONLY);*/
    config::getConfig()->put(next::appsupport::configkey::getOutportVersionKey(component_name_, port_name_safe),
                             (int)output_port_config.port_version_number);
    /*config::getConfig()->setTag(next::appsupport::configkey::getOutportVersionKey(component_name_, port_name_safe),
                                next::appsupport::configtags::TAG::READONLY);*/
  }
  saved_output_.push_back(saved_output_pair);

  return 0;
}

void NextOutport::SetupSavedOutputConfig() {

  if (!data_publisher_) {
    data_publisher_ = std::make_shared<next::udex::publisher::DataPublisher>(component_name_);
  }

  // TODO Get SDL Files from Config also
  // create Data Description for all ports from the SDL files
  for (auto &elem : saved_output_) {

    if (!elem.initialized_) {
      for (auto sdl_file : elem.sdl_filepath) {
        if (!sdl_file.empty() && boost::filesystem::exists(sdl_file)) {
          data_publisher_->RegisterDataDescription(sdl_file, true);
        }
      }

      // Compare created publishers with function arguments and add them if not yet available
      // TODO check which function to use instead of getTopicList
      NEXT_DISABLE_DEPRECATED_WARNING
      auto &sdl_registered_ports = data_publisher_->getTopicsAndHashes();
      NEXT_RESTORE_WARNINGS

      for (auto &output_port_config : elem.output_ports) {
        std::string port_name_safe = output_port_config.port_name;
        // make port safe
        if (port_name_safe.empty() || port_name_safe == " ") {
          port_name_safe = "undefined";
        }

        std::string port_containing_device = "SIM VFB." + port_name_safe;
        if (sdl_registered_ports.find(port_containing_device) != sdl_registered_ports.end()) {
          next::sdk::logger::debug(__FILE__, "{0} is already set up.", port_containing_device);
        } else {
          next::sdk::logger::warn(__FILE__, "{0} not created with sdl files. Will be registered now.",
                                  port_containing_device);
          data_publisher_->RegisterDataDescription(port_containing_device, output_port_config.port_size, true);
        }
        configured_output_ports_.push_back(output_port_config);

        config::getConfig()->put(next::appsupport::configkey::getOutportSizeKey(component_name_, port_name_safe),
                                 (int)output_port_config.port_size);
        // TODO comment in once tag system is implemented in config system
        /*config::getConfig()->setTag(next::appsupport::configkey::getOutportSizeKey(component_name_, port_name_safe),
                                                                                                                        next::appsupport::configtags::TAG::READONLY);*/
        config::getConfig()->put(next::appsupport::configkey::getOutportVersionKey(component_name_, port_name_safe),
                                 (int)output_port_config.port_version_number);
        // TODO comment in once tag system is implemented in config system
        /*config::getConfig()->setTag(next::appsupport::configkey::getOutportVersionKey(component_name_,
           port_name_safe), next::appsupport::configtags::TAG::READONLY);*/
      }
      elem.initialized_ = true;
    }
  }
}

void NextOutport::TriggerAsyncPush(const std::string &output_port_name, const DataPackage &data, bool append_device,
                                   size_t trigger_timestamp) {
  // check if port wit name output_port_name is already available.
  // TODO need to check if publisher shall be created if not available
  if (!data_publisher_) {
    next::sdk::logger::error(__FILE__, "Data Publisher not set up correctly.");
    return;
  }
  data_publisher_->publishData(output_port_name, std::get<void *>(data), std::get<size_t>(data), append_device,
                               trigger_timestamp);
}
void NextOutport::TriggerAsyncPush(const uint64_t virtual_address, const DataPackage &data, size_t trigger_timestamp) {
  // check if port wit name output_port_name is already available.
  // TODO need to check if publisher shall be created if not available
  if (!data_publisher_) {
    next::sdk::logger::error(__FILE__, "Data Publisher not set up correctly.");
    return;
  }
  data_publisher_->publishData(virtual_address, std::get<void *>(data), std::get<size_t>(data), trigger_timestamp);
}

void NextOutport::Reset() {
  // Reset Publisher
  if (data_publisher_) {
    data_publisher_->Reset();
  }
  configured_output_ports_.clear();
  saved_output_.clear();
}

} // namespace inport
} // namespace next

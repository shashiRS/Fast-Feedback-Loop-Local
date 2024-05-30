/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_manager.h
 * @brief    implementation of Configuration Manager
 **/

#ifndef NEXT_BRIDGES_CONFIG_MANAGER_H
#define NEXT_BRIDGES_CONFIG_MANAGER_H

#include <next/appsupport/config/config_server.hpp>
#include <next/bridgesdk/web_server.hpp>

namespace next {
namespace bridges {

class ConfigManager {
  DISABLE_COPY_MOVE(ConfigManager)
public:
  ConfigManager() = default;
  ConfigManager(std::shared_ptr<next::bridgesdk::webserver::IWebServer>,
                std::shared_ptr<next::appsupport::ConfigServer>, const std::string &config_file_with_path = "");
  virtual ~ConfigManager();
  void pushDataToNodes();
  void pushDataToGui();

private:
  void init(const std::string &config_file_with_path);
  void dumpAsAJsonFile(const std::string &file_path);
  void dumpAsAJsonFile(const std::string &file_path, const std::string &component_name);
  void payloadCallback(std::vector<uint8_t> data);
  std::string _config_file_with_path;
  std::shared_ptr<next::bridgesdk::webserver::IWebServer> _webserver;
  std::shared_ptr<next::appsupport::ConfigServer> _config_server;
  static constexpr const char *_c_name = "ControlBridge";
  std::vector<uint8_t> _data_for_gui;
}; // namespace neclassConfigManager

} // namespace bridges
} // namespace next

#endif // NEXT_BRIDGES_CONFIG_MANAGER_H

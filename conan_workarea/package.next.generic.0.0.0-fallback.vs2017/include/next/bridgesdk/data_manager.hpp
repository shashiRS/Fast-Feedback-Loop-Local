/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     data_manager.hpp
 * @brief    Forwading message to socket manager
 */

#ifndef NEXT_DATABRIDGE_DATA_MANAGER_HPP
#define NEXT_DATABRIDGE_DATA_MANAGER_HPP

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/web_server.hpp>

//! forward declaration, see bridgesdk/interface/next/bridgesdk/plugin.h
namespace next::bridgesdk::plugin_addons {
struct RegisteredSensors;
}

namespace next::plugins_utils {
struct SensorConfig;
}

namespace next {
namespace databridge {
namespace data_manager {

class IDataManager {
public:
  IDataManager() = default;
  virtual ~IDataManager() = default;

  IDataManager(IDataManager &&) = delete;
  IDataManager(IDataManager &) = delete;

  virtual void StartWebServer(const std::string &ip_address, uint16_t port_number, std::uint8_t num_connections) = 0;

  virtual void SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) = 0;

  virtual std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebsocketServer() = 0;

  virtual void PublishDataToClient(const uint8_t *payload, const size_t size, const bool force_no_cache = false,
                                   const std::vector<size_t> &session_ids = {},
                                   const std::chrono::milliseconds timeout = std::chrono::milliseconds(500),
                                   bool binary = false) = 0;

  virtual void PublishPluginDescriptionToClient(const std::string &data_class_name, const std::string &plugin_name,
                                                const next::bridgesdk::plugin_config_t &description) = 0;

  virtual void PublishFlatbufferDataToClient(const std::string &path_to_fbs,
                                             const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                             const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                             const std::pair<std::string, std::string> &structure_version_pair,
                                             const bool force_no_cache = false) = 0;

  virtual void SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback callback) = 0;

  //! contains a struct with registered Sensors (UIDs), which can be sent to GUI
  /*!
   * note, this list will be sent by Plugins to GUI, it is not checked if they were registered successfull
   * @return returns a static struct which contains Informations about registered Sensors at GUI
   */
  virtual std::shared_ptr<next::bridgesdk::plugin_addons::RegisteredSensors> GetRegisteredSensors() = 0;

private:
  virtual void
  PublishDataToClientDirectly(const uint8_t *payload, const size_t size, const std::vector<size_t> &session_ids = {},
                              const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) = 0;
};

std::shared_ptr<IDataManager> CreateDataManager();

} // namespace data_manager
} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_DATA_MANAGER_HPP

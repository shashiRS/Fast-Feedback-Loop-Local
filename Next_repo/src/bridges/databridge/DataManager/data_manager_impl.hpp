/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     data_manager_impl.hpp
 * @brief    Forwading message to socket manager
 */

#ifndef NEXT_DATAMANAGER_DATA_MANAGER_IMPL_HPP
#define NEXT_DATAMANAGER_DATA_MANAGER_IMPL_HPP

#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include "data_broadcaster.hpp"
#include "next/bridgesdk/data_manager.hpp"

#include "OutputManager/plugin_data_class_parser.h"
#include "OutputManager/simulation_environment_cache.h"

namespace next {

namespace databridge {

namespace data_manager {

//!  DataBroadcaster Class to handle requests for a memory slot and forwarding to responsible socket manager.
/*!
  The class provide functionality to manage memory pool to be filled with signal data and forwad to socket hander.
*/
class DataManagerImpl : public IDataManager {
public:
  DataManagerImpl();
  virtual ~DataManagerImpl() override;

  DataManagerImpl(DataManagerImpl &&) = delete;
  DataManagerImpl(DataManagerImpl &) = delete;

  void StartWebServer(const std::string &ip_address, uint16_t port_number, std::uint8_t num_connections) override;

  virtual void SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) override;

  virtual std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebsocketServer() override;

  virtual void PublishDataToClient(const uint8_t *payload, const size_t size, const bool force_no_cache = false,
                                   const std::vector<size_t> &session_ids = {},
                                   const std::chrono::milliseconds timeout = std::chrono::milliseconds(500),
                                   bool binary = false) override;

  void PublishFlatbufferDataToClient(const std::string &path_to_fbs,
                                     const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                     const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                     const std::pair<std::string, std::string> &structure_version_pair,
                                     const bool force_no_cache = false) override;

  void PublishPluginDescriptionToClient(const std::string &data_class_name, const std::string &plugin_name,
                                        const next::bridgesdk::plugin_config_t &plugin_description_in) override;

  std::shared_ptr<next::databridge::data_broadcaster::IDataBroadcaster> &GetRefBroadcaster() {
    return data_broadcaster_;
  }
  void SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback callback) override;

  virtual std::shared_ptr<bridgesdk::plugin_addons::RegisteredSensors> GetRegisteredSensors() override;

  //! saves configuration sorted after data classes together with plugin name
  std::multimap<std::string, std::pair<std::string, next::bridgesdk::plugin_config_t>> plugin_descriptions_;

protected:
  virtual void
  PublishDataToClientDirectly(const uint8_t *payload, const size_t size, const std::vector<size_t> &session_ids = {},
                              const std::chrono::milliseconds timeout = std::chrono::milliseconds(500)) override;

private:
  void CheckIncomingMessage(const std::vector<uint8_t> &msg_vector,
                            const bridgesdk::webserver::ConnectionType connection_type);

  std::shared_ptr<next::databridge::data_broadcaster::IDataBroadcaster> data_broadcaster_;
  next::databridge::SimulationEnvironmentCache simulation_environment_;
  outputmanager::PluginDataClassParser data_class_parser_;
  std::mutex foxglove_id_to_channel_mtx_;
  std::unordered_map<unsigned int, unsigned int> foxglove_id_to_channel_;
};

} // namespace data_manager
} // namespace databridge
} // namespace next
#endif // NEXT_DATAMANAGER_DATA_MANAGER_IMPL_HPP

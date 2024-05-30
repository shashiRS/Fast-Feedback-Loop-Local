/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_class_manager.h
 *  @brief    stub
 */

#ifndef NEXT_DATABRIDGE_DATA_CLASS_MANAGER_H_
#define NEXT_DATABRIDGE_DATA_CLASS_MANAGER_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <next/bridgesdk/datatypes.h>
#include <next/sdk/utilities/threadsafe_queue.h>
#include <next/bridgesdk/web_server.hpp>

namespace next {
namespace databridge {
namespace data_request_server {
class DataRequestServer;
} // namespace data_request_server
} // namespace databridge
} // namespace next

namespace next {
namespace databridge {
//! Class to communicate and manage plugin manager in the context of data classes and data bridges
/*!
 * requests available data classes from plugin manager
 * request new data bridge if data class is available
 * activate or deactivate if data class is added or removed
 */
class DataClassManager : public std::enable_shared_from_this<DataClassManager> {
public:
  struct DataRequestItem {
    std::string name;
    std::string version;
    std::string source_name;
    std::vector<std::string> arguments;
  };

  struct ConfigDataRequestItem {
    enum class ITEM_TYPE { GET_DESCRIPTION, SET_CONFIG };

    std::string name;
    std::string version;
    ITEM_TYPE item_type;
    std::string plugin_name;
    std::string source_name;
    std::vector<next::bridgesdk::plugin_config_t> configs;
  };

  struct BackendConfigRequestItem {
    bool synchronization_enabled;
    bool package_drop_enabled;
  };

  //! Constructor
  DataClassManager();
  //! tmp stub
  void AddUpdateRequestToQueue(DataRequestItem data_request_item, std::function<void(std::string)> callback);
  //! Background mode initialization to receive messages
  void InitBackgroundMode(const std::string &ip_adress, uint16_t port);

  void SetWebServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr);

  std::shared_ptr<bridgesdk::webserver::IWebServer> GetWebServer();

  //! tmp stub
  void SetupDataRequestServer(
      std::shared_ptr<next::databridge::data_request_server::DataRequestServer> sp_data_request_server_rhs);

  void GetDataClassRequestBlocking(DataRequestItem &value);
  bool GetDataClassRequestNonBlocking(DataRequestItem &value);

  //! Adding a new request to queue, to be processed by the main loop
  void AddConfigRequestToQueue(ConfigDataRequestItem request_item);

  //! receiving and removing config request from queue (this mehtod is called by the main loop)
  bool GetConfigDataClassRequestNonBlocking(ConfigDataRequestItem &value);

  //! Adding a new request to queue, to be processed by the main loop
  void AddBackendRequestToQueue(BackendConfigRequestItem request_item);

  //! receiving and removing config request from queue (this mehtod is called by the main loop)
  bool GetBackendRequestNonBlocking(BackendConfigRequestItem &value);

  //! Destructor
  virtual ~DataClassManager() = default;

private:
  std::string file_name_;
  next::sdk::threadsafe_queue<DataRequestItem> data_request_queue_;
  next::sdk::threadsafe_queue<ConfigDataRequestItem> config_data_request_queue_;
  next::sdk::threadsafe_queue<BackendConfigRequestItem> backend_request_queue_;
  //! each config contains the plugin name, those names are stored in this unique list, so if the GUI sents an empty
  //! config, only those plugins deletes the configs
  std::shared_ptr<next::databridge::data_request_server::DataRequestServer> sp_data_request_server_;
};

} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_DATA_CLASS_MANAGER_H_

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
#include <next/bridgesdk/plugin.h>
#include <next/sdk/logger/logger.hpp>

#include "../PluginManager/OutputSchema/output_schema_checker.h"
#include "OutputManager/plugin_description_to_json.h"
#include "data_manager_impl.hpp"
#include "flatbuffers_to_json.h"

namespace next {
namespace databridge {
namespace data_manager {

//! DataManager constructure instantiating a broadcaster object.
/*!
 */
DataManagerImpl::DataManagerImpl() {
  data_broadcaster_ = next::databridge::data_broadcaster::CreateDataBroadcaster();
  simulation_environment_.handoverDataBroadcaster(data_broadcaster_);
}

//! DataManager donstructure
/*!
 */
DataManagerImpl::~DataManagerImpl() { data_broadcaster_.reset(); }

//! Starting WebServer to accept incoming connection from client.
/*!
  \param ip_address Ip Address to be used for accpepting connection.
  \param port_number The socket port number.
  \param num_connections Number of possible connection can be established.
*/
void DataManagerImpl::StartWebServer(const std::string &ip_address, uint16_t port_number,
                                     std::uint8_t num_connections) {
  debug(__FILE__, " Starting WebServer From Broadcaster");
  data_broadcaster_->StartWebSocketSever(ip_address, port_number, num_connections);
  data_broadcaster_->SetForwardMessageCallback(
      std::bind(&DataManagerImpl::CheckIncomingMessage, this, std::placeholders::_1, std::placeholders::_2));
}

//! Set the web server.
/*!
  \param ptr The shared pointer of the used web server.
*/
void DataManagerImpl::SetWebsocketServer(std::shared_ptr<bridgesdk::webserver::IWebServer> ptr) {
  data_broadcaster_->SetWebsocketServer(ptr);
  data_broadcaster_->SetForwardMessageCallback(
      std::bind(&DataManagerImpl::CheckIncomingMessage, this, std::placeholders::_1, std::placeholders::_2));
}

//! Get the web server
/*!
  \return Shared pointer of the used web server.
*/
std::shared_ptr<bridgesdk::webserver::IWebServer> DataManagerImpl::GetWebsocketServer() {
  return data_broadcaster_->GetWebsocketServer();
}

//! Sending available payload to client.
/*!
  \param payload The data to be send to client.
  \param size The payload size in number of byte.
  \param session_ids If its not empty the data will be send to given session otherwise to all clients.
  \param timeout Time in ms to wait for the memory of the used data broadcaster.
*/
void DataManagerImpl::PublishDataToClient(const uint8_t *payload, const size_t size, const bool force_no_cache,
                                          const std::vector<size_t> &session_ids /* = {}*/,
                                          const std::chrono::milliseconds timeout /* = std::chrono::milliseconds(500)*/,
                                          bool binary /* = false */) {
  simulation_environment_.addPackage(payload, size, session_ids, force_no_cache, timeout, binary);
}

void DataManagerImpl::PublishDataToClientDirectly(
    const uint8_t *payload, const size_t size, const std::vector<size_t> &session_ids /* = {}*/,
    const std::chrono::milliseconds timeout /*= std::chrono::milliseconds(500)*/) {
  try {
    // request memory
    auto memory_future = data_broadcaster_->RequestNewMemory();

    using namespace std::chrono_literals;

    if (!memory_future.valid() || (memory_future.wait_for(timeout) == std::future_status::timeout)) {
      info(__FILE__, " Request memory failed!!!");
    } else {
      auto memory = memory_future.get();
      memory->data_.resize(size);
      std::copy(payload, payload + size, memory->data_.begin());
      data_broadcaster_->SendDataToSocketManager(memory->id_, memory->data_, session_ids);
    }
  } catch (std::future_error &e) {
    error(__FILE__, "exception in PublishDataToClient: {0}", e.code().message());
  } catch (...) {
    error(__FILE__, " exception in PublishDataToClient");
  }
}

//! Sending available payload to client.
/*!
  \param path_to_fbs The path to flatbuffers schema.
  \param flatbuffer The pointer to flatbuffers memory blob.
  \param sensor_config The sensor configurations to additionally enrich flatbuffers data.
  \param session_ids If its not empty the data will be send to given session otherwise to all clients.
*/
void DataManagerImpl::PublishFlatbufferDataToClient(const std::string &path_to_fbs,
                                                    const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                                    const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                                    const std::pair<std::string, std::string> &structure_version_pair,
                                                    const bool force_no_cache) {
  auto server = GetWebsocketServer();
  if (server) {
    // foxglove
    std::lock_guard<std::mutex> lck(foxglove_id_to_channel_mtx_);
    const auto channel_id = foxglove_id_to_channel_.find(sensor_info.sensor_id);
    if (channel_id != foxglove_id_to_channel_.end()) {
      const unsigned int subscription_id = channel_id->second;
      const auto session_ids = server->GetSessionIds(bridgesdk::webserver::ConnectionType::foxglove);
      if (!session_ids.empty()) {
        std::string response;
        data_class_parser_.parseDataClassToFlatbuffer(response, flatbuffer, subscription_id, sensor_info,
                                                      structure_version_pair);
        PublishDataToClient(reinterpret_cast<const uint8_t *>(response.c_str()), response.size(), true, session_ids,
                            std::chrono::milliseconds(500), true);
      }
    }
    // normal GUI
    const auto session_ids = server->GetSessionIds(bridgesdk::webserver::ConnectionType::normal);
    if (!session_ids.empty()) {
      std::string response;
      if (data_class_parser_.parseDataClass(response, path_to_fbs, flatbuffer, sensor_info, structure_version_pair)) {
        PublishDataToClient(reinterpret_cast<const uint8_t *>(response.c_str()), response.size(), force_no_cache,
                            session_ids, std::chrono::milliseconds(500), false);
      }
    }
  } else {
    debug(__FILE__, "No server set, so data will not sent");
    return;
  }
}

//! transform plugin description to JSON and send to GUI.
/**
 * @param data_class_name name of the plugin
 * @param plugin_description_in reference to the plugin description.
 */
void DataManagerImpl::PublishPluginDescriptionToClient(const std::string &data_class_name,
                                                       const std::string &plugin_name,
                                                       const next::bridgesdk::plugin_config_t &plugin_description_in) {
  // check if description is emppty
  if (plugin_description_in.empty()) {
    debug(__FILE__, "Description is empty nothing to do here");
    return;
  }

  // TODO only insert a description once per plugin
  auto all_matches = plugin_descriptions_.equal_range(data_class_name);
  bool plugin_found = false;
  for (auto it = all_matches.first; it != all_matches.second; it++) {
    if (it->second.first == plugin_name) {
      if (it->second.second == plugin_description_in) {
        plugin_found = true;
      } else {
        plugin_descriptions_.erase(it);
      }
      break;
    }
  }

  if (plugin_found == false) {
    plugin_descriptions_.insert({data_class_name, {plugin_name, plugin_description_in}});
  }

  Json::Value json_full_message;
  json_full_message["channel"] = "dataStreamConfiguration";
  json_full_message["event"] = "AvailableDataClassesOptionsUpdate";
  json_full_message["source"] = "DataSource";

  Json::Value data_classes_full_json;

  std::map<std::string, Json::Value> data_classes_json_map;

  // loop through all descriptions
  for (auto description_temp : plugin_descriptions_) {

    // parse description parameters
    next::databridge::outputmanager::PluginDescriptionToJSON ptj;
    auto single_config = ptj.parameterToJSON(description_temp.second.second);
    if (single_config.empty()) {
      debug(__FILE__, "JSON string is empty nothing to do here");
      continue;
    }

    // add name and plugin name to config
    single_config["plugin"] = description_temp.second.first;
    single_config["name"] = description_temp.first + " " + description_temp.second.first;

    // append it to the current JSON sorted for Data Classes
    Json::Value temp;
    // find correct JSon Array element
    auto it = data_classes_json_map.find(description_temp.first);
    if (it != data_classes_json_map.end()) {
      temp.copy(data_classes_json_map[description_temp.first]);
    }

    temp["name"] = description_temp.first;
    temp["configurations"].append(single_config);
    data_classes_json_map[description_temp.first] = temp;
  }

  for (auto data_class_json_temp : data_classes_json_map) {
    json_full_message["payload"]["dataClasses"].append(data_class_json_temp.second);
  }

  Json::FastWriter fastWriter;
  auto json_string_full = fastWriter.write(json_full_message);

  PublishDataToClientDirectly(reinterpret_cast<const uint8_t *>(json_string_full.data()), json_string_full.size());
}

//! Setting callback to receive message from client.
/*!
  \param callback Callback will be called when client send message.
*/
void DataManagerImpl::SetForwardMessageCallback(bridgesdk::webserver::ForwardMsgCallback callback) {
  data_broadcaster_->SetForwardMessageCallback(callback);
}

std::shared_ptr<bridgesdk::plugin_addons::RegisteredSensors> DataManagerImpl::GetRegisteredSensors() {
  static std::shared_ptr<bridgesdk::plugin_addons::RegisteredSensors> sensor_list =
      std::make_shared<bridgesdk::plugin_addons::RegisteredSensors>();
  return sensor_list;
}

void DataManagerImpl::CheckIncomingMessage(const std::vector<uint8_t> &msg_vector,
                                           const bridgesdk::webserver::ConnectionType connection_type) {
  if (connection_type == bridgesdk::webserver::ConnectionType::foxglove) {
    const std::string message_string((char *)msg_vector.data(), msg_vector.size());
    Json::Reader reader;
    Json::Value message_json;
    bool parseSuccess = reader.parse(message_string, message_json, false);
    if (parseSuccess && message_json.isMember("op")) {
      auto type = message_json["op"].asString();
      if (type == "subscribe") {
        std::lock_guard<std::mutex> lck(foxglove_id_to_channel_mtx_);
        for (auto &element : message_json["subscriptions"]) {
          const unsigned int id = element["id"].asUInt();
          const unsigned int channel_id = element["channelId"].asUInt();
          debug(__FILE__, "Foxglove connection for sensor {0} as channel {1}", id, channel_id);
          foxglove_id_to_channel_[channel_id] = id;
        }
      }
      if (type == "unsubscribe") {
        std::lock_guard<std::mutex> lck(foxglove_id_to_channel_mtx_);
        for (auto &id_json : message_json["subscriptionIds"]) {
          const unsigned int id = id_json.asUInt();
          debug(__FILE__, "Foxglove unsubscribe request for channel {}", id);
          for (auto it = foxglove_id_to_channel_.begin(); it != foxglove_id_to_channel_.end();) {
            if (it->second == id) {
              it = foxglove_id_to_channel_.erase(it);
            } else {
              it++;
            }
          }
        }
      }
    }
  }
}

//! Making an instance of data manager.
std::shared_ptr<IDataManager> CreateDataManager() { return std::make_shared<DataManagerImpl>(); }

} // namespace data_manager
} // namespace databridge
} // namespace next

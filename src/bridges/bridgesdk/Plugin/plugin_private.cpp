/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_private.cpp
 * @brief    Hidden implementation of helper functions of a plugin.
 *
 * See plugin_private.h
 *
 **/

#include "plugin_private.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>

#include <boost/filesystem.hpp>

#include <base64.h>

#include <next/sdk/sdk.hpp>

#include <next/bridgesdk/support_fbs_types.hpp>
#include "view_update.hpp"

namespace next {
namespace bridgesdk {
namespace plugin {

using namespace next::bridgesdk::plugin_addons;

PluginPrivate::PluginPrivate(const char *path) : path_(std::string(path)) {

  data_extractor_ = std::make_shared<next::udex::extractor::DataExtractor>(uint16_t(1));

  json_builder_["indentation"] = "";
  debug(__FILE__, "plugin path: {0}", path);
}

PluginPrivate::~PluginPrivate() {
  // TODO unregister registered sensors
  if (data_subscriber_) {
    for (auto subscription : subscriptions_) {
      data_subscriber_->Unsubscribe(subscription);
    }
    subscriptions_.clear();
  }
}

void PluginPrivate::setSignalExpolrer(std::shared_ptr<next::udex::explorer::SignalExplorer> &signal_explorer) {
  signal_explorer_ = signal_explorer;
}

void PluginPrivate::setDataSubscriber(std::shared_ptr<next::udex::subscriber::DataSubscriber> &data_subscriber) {
  data_subscriber_ = data_subscriber;
}

void PluginPrivate::setDataReceiver(std::shared_ptr<databridge::data_manager::IDataManager> &broadcaster) {
  data_receiver_ = broadcaster;
}

void PluginPrivate::sendResultData([[maybe_unused]] const std::string &channel_name, const void *data,
                                   const size_t size, const std::chrono::milliseconds timeout,
                                   const bool force_no_cache, const std::vector<size_t> &session_ids /* = {} */) const {
  if (data_receiver_) {
    data_receiver_->PublishDataToClient((const uint8_t *)data, size, force_no_cache, session_ids, timeout);
  }
  // TODO: already request memory for the next run
  // but for this we would need a function to give unused memory back
}

void PluginPrivate::sendFlatbufferData(const std::string &path_to_fbs, const FlatbufferPackage &flatbuffer,
                                       const SensorInfoForFbs &sensor_info,
                                       const std::pair<std::string, std::string> &structure_version_pair) const {
  if (data_receiver_) {
    std::vector<size_t> sessionids;

    data_receiver_->PublishFlatbufferDataToClient(path_to_fbs, flatbuffer, sensor_info, structure_version_pair);
  }
  // TODO: already request memory for the next run
  // but for this we would need a function to give unused memory back
}

void PluginPrivate::UdexCallBack(const std::string signal_url, bridgesdk::pluginCallbackT &sub_callback) {

  auto extracted_payload = data_extractor_->GetExtractedData(signal_url);

  bridgesdk::ChannelMessagePointers data;
  data.data = extracted_payload.data;
  data.size = extracted_payload.size;
  data.channel_name = signal_url.c_str();
  data.time = extracted_payload.timestamp;

  sub_callback(signal_url, &data);
}

std::shared_ptr<next::udex::extractor::DataExtractor> PluginPrivate::getExtractorInstance() { return data_extractor_; }

size_t PluginPrivate::subscribeUrl(const std::string &url_name, bridgesdk::pluginCallbackT &sub_callback) {

  std::lock_guard<std::mutex> lck{protect_configs_};
  if (data_subscriber_ && url_name.size() != 0) {
    size_t id = data_subscriber_->Subscribe(url_name, data_extractor_);
    if (id != 0) {
      subscriptions_.push_back(id);

      auto bind_funcion = std::bind(&PluginPrivate::UdexCallBack, this, std::placeholders::_1, sub_callback);
      auto callback_connection_id = data_extractor_->connectCallback(bind_funcion, url_name);

      subscription_id_to_callback_id_map_[id] = callback_connection_id;

      return id;
    }
  }
  return 0;
}

bool PluginPrivate::checkConnectionWait(const std::string &url_name, size_t timeout_ms) {
  size_t wait_time_ms = 50;
  size_t test_counter = timeout_ms / 50;
  bool connection = false;
  for (size_t test_count = 0; test_count < test_counter; test_count++) {
    if (data_subscriber_->IsConnected(url_name)) {
      connection = true;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
  }
  return connection;
}

void PluginPrivate::unsubscribe(const size_t id) {
  std::lock_guard<std::mutex> lck{protect_configs_};
  if (data_subscriber_) {
    data_subscriber_->Unsubscribe(id);
    subscriptions_.remove(id);

    // disconnect callback based on ID
    data_extractor_->disconnectCallback(subscription_id_to_callback_id_map_[id]);
    subscription_id_to_callback_id_map_.erase(id);
  }
}

std::map<size_t, next::udex::subscriber::SubscriptionInfo> PluginPrivate::getSubscriptionsInfo() {
  std::lock_guard<std::mutex> lck{protect_configs_};
  return data_subscriber_->getSubscriptionsInfo();
}

std::unordered_map<std::string, std::vector<std::string>> PluginPrivate::getURLTree(void) const {

  if (signal_explorer_) {
    return signal_explorer_->GetFullUrlTree();
  }
  return std::unordered_map<std::string, std::vector<std::string>>();
}

std::vector<std::string> PluginPrivate::searchSignalTree(const std::string &keyword) const {
  if (signal_explorer_) {
    return signal_explorer_->searchSignalTree(keyword);
  }
  return std::vector<std::string>();
}

std::vector<std::string> PluginPrivate::getTopicByUrl(const std::string &url) const {

  if (signal_explorer_) {
    return signal_explorer_->GetPackageUrl(url);
  }
  return std::vector<std::string>();
}

std::vector<signal_child_info> PluginPrivate::getChildsByUrl(const std::string &url) const {
  std::vector<signal_child_info> vec;
  if (signal_explorer_) {
    const std::vector<udex::ChildInfo> child_infos(signal_explorer_->GetChildByUrl(url));
    for (const auto &info : child_infos) {
      vec.push_back({info.name, info.child_count, info.array_lenght});
    }
  }
  return vec;
}

void PluginPrivate::getDataSourceNames(const std::string &format_id, std::vector<std::string> &data_source_names) {
  if (signal_explorer_) {
    data_source_names = signal_explorer_->getDeviceByFormatType(format_id);
  }
}

udex::SignalDescription PluginPrivate::getPackageDetailsByUrl(const std::string &signal_url) const {
  if (signal_explorer_) {
    bool available;
    return signal_explorer_->getSignalDescription(signal_url, available);
  }
  return {};
}

std::pair<bool, std::string> PluginPrivate::generateSdl(const std::string topic) const {
  if (signal_explorer_) {
    return signal_explorer_->generateSdl(topic);
  } else {
    return std::make_pair(false, "");
  }
}

uid_t PluginPrivate::registerAndPublishSensor(const SensorInfoForGUI &sensorInfo) const {
  if (data_receiver_) {
    std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);
    uid_t uid = getRegisteredPluginByInfo(sensorInfo);
    if (uid == 0) {
      uid = (data_receiver_->GetRegisteredSensors()->uid_counter)++;
      data_receiver_->GetRegisteredSensors()->sensor_list.emplace(uid, sensorInfo);
    }

    ParseSensorInfoForFoxglove(uid, sensorInfo);

    SendSensorListToNextClient(getValueStreams());
    AdvertiseAllSensorsToFoxglove();

    return uid;
  }
  return 0;
}

void PluginPrivate::unregisterAndPublishSensor(const uid_t uid) const {
  if (data_receiver_) {
    std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);

    // old list of value streams before sensor is removed
    std::set<std::string> oldListOfValueStream = getValueStreams();
    auto ret = data_receiver_->GetRegisteredSensors()->sensor_list.erase(uid);
    if (ret) {
      debug(__FILE__, "unregistering UID={0} successfull", uid);
    } else {
      debug(__FILE__, "failed to unregister UID={0}", uid);
    }

    SendSensorListToNextClient(oldListOfValueStream);
    UnadvertiseSensorToFoxglove(uid);
  }
}

uid_t PluginPrivate::getRegisteredPluginByInfo(const SensorInfoForGUI &sensorInfo) const {
  if (data_receiver_) {
    std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);

    //! search through the whole list for the name and stream
    for (const auto &[key, value] : data_receiver_->GetRegisteredSensors()->sensor_list) {
      // get tuple of strings
      // check if name and stream is already there
      if (sensorInfo == value) {
        return key;
      }
    }
  }

  // return default 0 when no plugin is found or pointers are invalid
  return 0;
}

void PluginPrivate::SendSensorListToNextClient(const std::set<std::string> &listOfValueStreams) const {
  std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);
  Json::Value final_message_json;
  std::set<std::string> streamsWOSensors = listOfValueStreams; // checks which streams have no sensors anymore

  for (const auto &[key, value] : data_receiver_->GetRegisteredSensors()->sensor_list) {
    Json::Value single_sensor;
    single_sensor["uid"] = key;
    single_sensor["name"] = value.name;
    single_sensor["color"] = value.color;
    single_sensor["type"] = value.type;
    single_sensor["enabled"] = true;
    single_sensor["referenceCoordinateSystem"] = value.referenceCoordinateSystem;
    single_sensor["coordinateSystemTransform"]["translationX"] = value.coordinateSystem.translationX;
    single_sensor["coordinateSystemTransform"]["translationY"] = value.coordinateSystem.translationY;
    single_sensor["coordinateSystemTransform"]["translationZ"] = value.coordinateSystem.translationZ;
    single_sensor["coordinateSystemTransform"]["rotationX"] = value.coordinateSystem.rotationX;
    single_sensor["coordinateSystemTransform"]["rotationY"] = value.coordinateSystem.rotationY;
    single_sensor["coordinateSystemTransform"]["rotationZ"] = value.coordinateSystem.rotationZ;
    final_message_json["dataStream"][value.stream]["sensors"].append(single_sensor);

    streamsWOSensors.erase(value.stream); // if stream doesn't exist anymore in list nothing happens
  }

  // for all streams without sensors create empty array of sensors
  for (const auto &emptyStreamName : streamsWOSensors) {
    final_message_json["dataStream"][emptyStreamName]["sensors"] = Json::arrayValue;
  }

  if (!final_message_json.isNull()) {
    std::string output_json = Json::writeString(json_builder_, final_message_json);
    sendResultData("dummy", output_json.data(), output_json.size(), std::chrono::milliseconds(100), true, {});
  } else {
    error(__FILE__, "Output string of sendSensorList is empty. No json is sent out.");
  }
}

void PluginPrivate::AdvertiseAllSensorsToFoxglove() const {
  // for how to advertis a sensor see https://github.com/foxglove/ws-protocol/blob/main/docs/spec.md#advertise
  std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);
  Json::Value foxglove_advertise;
  foxglove_advertise["op"] = "advertise";
  for (const auto &[key, value] : data_receiver_->GetRegisteredSensors()->sensor_list) {
    if (!value.flatbuffer_bfbs_base64.empty()) {
      Json::Value foxglove_sensor;
      foxglove_sensor["id"] = key;
      foxglove_sensor["topic"] = value.name;
      foxglove_sensor["encoding"] = "flatbuffer";
      foxglove_sensor["schemaName"] = value.schemaName;
      foxglove_sensor["schema"] = value.flatbuffer_bfbs_base64;
      foxglove_sensor["color"] = value.color;
      foxglove_sensor["type"] = value.type;
      foxglove_sensor["widget_stream"] = value.stream;
      foxglove_advertise["channels"].append(foxglove_sensor);
    }
  }
  if (foxglove_advertise.isMember("channels")) {
    const std::string bfbs_json_ser = Json::writeString(json_builder_, foxglove_advertise);
    std::vector<size_t> session_ids;
    if (auto server = data_receiver_->GetWebsocketServer(); server) {
      session_ids = server->GetSessionIds(webserver::ConnectionType::foxglove);
    }
    if (!session_ids.empty()) {
      sendResultData("dummy", bfbs_json_ser.data(), bfbs_json_ser.size(), std::chrono::milliseconds(500), true,
                     session_ids);
    }
  }
}

void PluginPrivate::UnadvertiseSensorToFoxglove(const plugin_addons::uid_t sensor_id) const {
  // for how to unadvertise a sensor see https://github.com/foxglove/ws-protocol/blob/main/docs/spec.md#unadvertise
  auto server = data_receiver_->GetWebsocketServer();
  std::vector<size_t> session_ids;
  if (server) {
    session_ids = server->GetSessionIds(webserver::ConnectionType::foxglove);
  }
  if (!session_ids.empty()) {
    Json::Value bfbs_json;
    bfbs_json["op"] = "unadvertise";
    bfbs_json["channelIds"].append(sensor_id);
    const std::string bfbs_json_ser = Json::writeString(json_builder_, bfbs_json);
    sendResultData("dummy", bfbs_json_ser.data(), bfbs_json_ser.size(), std::chrono::milliseconds(500), true,
                   session_ids);
  }
}

std::set<std::string> PluginPrivate::getValueStreams() const {
  std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);
  auto &sensorList = data_receiver_->GetRegisteredSensors()->sensor_list;
  std::set<std::string> listOfValueStream;
  for (auto &it : sensorList) {
    listOfValueStream.insert(it.second.stream);
  }
  return listOfValueStream;
}

bool PluginPrivate::setSynchronizationMode(bool enabled) {
  if (!data_extractor_) {
    return false;
  }
  data_extractor_->SetBlockingExtraction(enabled);
  return true;
}
bool PluginPrivate::setPackageDropMode(bool enabled) {
  if (!data_extractor_) {
    return false;
  }
  data_extractor_->SetDropExtraInput(enabled);
  return true;
}

bool PluginPrivate::GetFileContentAsBase64(const std::string &path, std::string &file_content) const {
  std::ifstream bfbs_ifs(path, std::ios::binary);
  if (!bfbs_ifs.is_open()) {
    warn(__FILE__, "{0}: Failed to open {1}", name_, path);
    return false;
  } else {
    debug(__FILE__, "{0}: file {1} opened", name_, path);
    bfbs_ifs.seekg(0, std::ios::end);
    const std::streampos bfbs_size = bfbs_ifs.tellg();
    bfbs_ifs.seekg(0);
    if (0 < bfbs_size) {
      std::string bfbs_binary(static_cast<size_t>(bfbs_size), ' ');
      bfbs_ifs.read(&bfbs_binary[0], bfbs_size);
      file_content = base64_encode(bfbs_binary);
    } else {
      warn(__FILE__, "{0}: Failed to read the content of file {}.", name_, path);
      return false;
    }
    return true;
  }
}

void PluginPrivate::ParseSensorInfoForFoxglove(const uid_t sensor_id, const SensorInfoForGUI &sensor_info) const {
  if (!sensor_info.schemaName.empty() && !sensor_info.flatbuffer_bfbs_path.empty()) {
    std::lock_guard<std::recursive_mutex> lock(data_receiver_->GetRegisteredSensors()->sensor_list_mutex);
    std::string bfbs_base64;

    auto type = SupportedFbsTypes::MapSchemaNameToUiType(sensor_info.schemaName);
    std::string file_content_view;
    bool success;
    if (type != SupportedFbsTypes::supportedFBS::Custom || type != SupportedFbsTypes::supportedFBS::UNIDENTIFIED) {
      bfbs_base64 = base64_encode(bfbs_data, sizeof(bfbs_data));
      data_receiver_->GetRegisteredSensors()->sensor_list[sensor_id].schemaName = "GuiSchema.ViewUpdate";
      success = true;
    } else {
      success = GetFileContentAsBase64(sensor_info.flatbuffer_bfbs_path, bfbs_base64);
    }

    {
      if (success) {
        data_receiver_->GetRegisteredSensors()->sensor_list[sensor_id].flatbuffer_bfbs_base64 = bfbs_base64;
      } else {
        warn(__FILE__, "{}: Since reading the bfbs file failed, this plugin will not be able to send data to foxglove",
             this->name_);
        data_receiver_->GetRegisteredSensors()->sensor_list[sensor_id].flatbuffer_bfbs_path.clear();
        data_receiver_->GetRegisteredSensors()->sensor_list[sensor_id].schemaName.clear();
      }
    }
  }
}

} // namespace plugin
} // namespace bridgesdk
} // namespace next

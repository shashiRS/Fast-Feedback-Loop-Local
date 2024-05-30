/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_private.h
 * @brief    Hidden implementation of helper functions of a plugin.
 *
 * This header is the interface to the hidden implementaiton (PIMPL idiom).
 *
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_PRIVATE_H_
#define NEXT_BRIDGESDK_PLUGIN_PRIVATE_H_

#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <json/json.h>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include <next/bridgesdk/data_manager.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {
class PluginDataPublisherImpl;
class PluginDataSubscriberImpl;
class Plugin3dViewImpl;
class PluginSignalExplorerImpl;
class PluginSignalExtractorImpl;
} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

namespace next {
namespace bridgesdk {
namespace plugin {

//! The hidden implementation of helper functions for using plugins
/*!
 *  This class is the implementatin of the PIMPL technique, see
 *  https://en.cppreference.com/w/cpp/language/pimpl
 *
 *  When an instance of a plugin is created, it will internally also create one
 *  object of this class. On creation the plugin ha sto give the pointer to itself
 *  such that the object of this class can later interact with its plugin object.
 */
class NEXTPLUGINSHARED_EXPORT PluginPrivate {
public:
  friend class plugin_addons::PluginDataPublisherImpl;
  friend class plugin_addons::PluginDataSubscriberImpl;
  friend class plugin_addons::Plugin3dViewImpl;
  friend class plugin_addons::PluginSignalExplorerImpl;
  friend class plugin_addons::PluginSignalExtractorImpl;

  //! default constructor
  /*
   *  Stores path to the plugin and initialize DataExtractor
   *
   */
  explicit PluginPrivate(const char *path);

  PluginPrivate() = delete;
  PluginPrivate(const PluginPrivate &other) = delete;
  PluginPrivate &operator=(const PluginPrivate &other) = delete;
  PluginPrivate(PluginPrivate &&other) = delete;
  PluginPrivate &operator=(PluginPrivate &&other) = delete;

  virtual ~PluginPrivate();

  void setSignalExpolrer(std::shared_ptr<next::udex::explorer::SignalExplorer> &signal_explorer);

  void setDataSubscriber(std::shared_ptr<next::udex::subscriber::DataSubscriber> &data_subscriber);

  //! Sets the data receiver.
  /*!
   *  The given broadcaster will be used as data receiver. All data procuded by the plugin will be given
   *  to this broadcaster. A second call will replace the already set broadcaster.
   */
  void setDataReceiver(std::shared_ptr<databridge::data_manager::IDataManager> &broadcaster);

  //! Distribute data for a specific channel.
  /*!
   *  The function internally iterates over the registered data receivers and calls their data
   *  input functions. This function is called by the plugin to distribute the generated output.
   */
  void sendResultData(const std::string &channel_name, const void *data, const size_t size,
                      const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000),
                      const bool force_no_cache = false, const std::vector<size_t> &session_ids = {}) const;

  //! Sends complete flatbuffer data to all registered recipients
  /*!
   *  Wraps the Flatbuffer message with the matching format and distributes the data
   */
  void sendFlatbufferData(const std::string &path_to_fbs, const FlatbufferPackage &flatbuffer,
                          const SensorInfoForFbs &sensor_info,
                          const std::pair<std::string, std::string> &structure_version_pair) const;

  //! Subscribe to a URL
  /*!
   * Request a new url subscription. The given callback function will be called to receive data for given url.
   *
   * @param url_name the URL to subscribe to
   * @param subscription_type subscription type, see bridgesdk::SubscriptionType for possible values
   * @param sub_callback the function which will be called
   * @return a reference ID of the subscription, 0 on error
   */
  size_t subscribeUrl(const std::string &url_name, pluginCallbackT &sub_callback);

  //! unsubscribe, referenced by the subscription id
  void unsubscribe(size_t id);

  std::map<size_t, next::udex::subscriber::SubscriptionInfo> getSubscriptionsInfo();

  bool checkConnectionWait(const std::string &url_name, size_t timeout_ms);

  std::unordered_map<std::string, std::vector<std::string>> getURLTree(void) const;

  //! see Plugin3dView::registerAndPublishSensor
  plugin_addons::uid_t registerAndPublishSensor(const SensorInfoForGUI &sensorInfo) const;

  //! see Plugin3dView::unregisterAndPublishSensor
  void unregisterAndPublishSensor(const plugin_addons::uid_t uid) const;

  //! see helper function to find an already registered plugin by name
  plugin_addons::uid_t getRegisteredPluginByInfo(const SensorInfoForGUI &sensorInfo) const;

  std::vector<std::string> getTopicByUrl(const std::string &url) const;

  //! searchSignalTree searches for the keyword in the backend and returns the list of urls
  /*!
   * @param keyword search token
   * @return returns the list/vector of urls, after search in backend is successful, otherwise an empty vector/list
   */
  std::vector<std::string> searchSignalTree(const std::string &keyword) const;

  std::vector<plugin_addons::signal_child_info> getChildsByUrl(const std::string &url) const;
  void getDataSourceNames(const std::string &format_id, std::vector<std::string> &data_source_names);

  void UdexCallBack(const std::string signal_url, bridgesdk::pluginCallbackT &sub_callback);

  std::string name_{"(name not set)"};
  std::string version_{"0.0.0-notSet"};
  std::string path_{};

  std::shared_ptr<next::udex::extractor::DataExtractor> getExtractorInstance();

  bool setSynchronizationMode(bool enabled);

  bool setPackageDropMode(bool enabled);

  udex::SignalDescription getPackageDetailsByUrl(const std::string &signal_url) const;

  std::pair<bool, std::string> generateSdl(const std::string topic) const;

private:
  //! Read in the given file and encode its content as Base64.
  /*!
   * The function reads the file (given by its path) as binary file. The file content gets Base64 encoded and returned
   * via the second argument. On error the function loggs an error description and returns false.
   *
   * @param[in] path Path to the file which will be read in.
   * @param[in,out] file_content The BASE64 encoded file content. Unchanged on error.
   * @retrun Returns true on success, false otherwise.
   */
  bool GetFileContentAsBase64(const std::string &path, std::string &file_content) const;

  // need to be handed over from plugin manager -> external
  std::shared_ptr<next::databridge::data_manager::IDataManager> data_receiver_;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_{nullptr};

  // internally created -> but can be overriden for testing purposes with connectPlugin(a,b,c,d);
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_{nullptr};
  std::shared_ptr<next::udex::extractor::DataExtractor> data_extractor_{nullptr};

  std::map<size_t, size_t> subscription_id_to_callback_id_map_;

  //! Send to Next Client which sensors are available and which are removed.
  void SendSensorListToNextClient(const std::set<std::string> &listOfValueStreams) const;

  //! Advertise all currently registered sensors to foxglove.
  void AdvertiseAllSensorsToFoxglove() const;

  //! Send a unadvertise message for a specific sensor to foxglove.
  void UnadvertiseSensorToFoxglove(const plugin_addons::uid_t sensor_id) const;

  std::set<std::string> getValueStreams() const;

  //! Parse sensor info regarding foxglove, e.g. it reads the bfbs file referenced by the sensor info.
  void ParseSensorInfoForFoxglove(const plugin_addons::uid_t sensor_id, const SensorInfoForGUI &sensor_info) const;

  std::list<size_t> subscriptions_;

  //! This pointer holds a reference to the plugin we belong to.
  /*! It is used for registering and unregistering as a data consumer
   */
  Plugin *plugin_{nullptr};
  mutable std::mutex protect_configs_{};

  Json::StreamWriterBuilder json_builder_;
};

} // namespace plugin
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_PRIVATE_H_

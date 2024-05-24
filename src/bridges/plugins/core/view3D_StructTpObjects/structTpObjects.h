/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     structTpObjects.h
 * @brief    Parsing network data of struct with array of object attributes to provide Traffic Participants
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_STRUCT_TPOBJECTS_H_
#define NEXT_PLUGINS_STRUCT_TPOBJECTS_H_

#include <boost/regex.hpp>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_regex_searcher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>
#include <next/plugins_utils/objects/traffic_participant.hpp>

#include <next/plugins_utils/plugins_types.h>

#include "ego_vehicle_struct.hpp"

namespace next {
namespace plugins {
using namespace boost::numeric;

class StructTpObjects : public next::bridgesdk::plugin::Plugin {
  enum class CoordinateSystem : uint8_t { kGlobal = 0U, kEgoVehicleRearAxle = 1U, kEgoVehicleFrontAxle = 2U };
  enum class ECU_type : uint8_t { Parking_MTA5 = 0U, ARS = 1U };
  struct ConfigData {
    std::shared_ptr<TrafficList> traffic_list_{nullptr};
    std::shared_ptr<EgoVehicle> ego_vehicle_{nullptr};
    next::plugins_utils::SensorConfig sensor_config_;
  };

  enum class ObjectValidation : uint8_t { lifetime = 0, dimensions = 1 };

public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;
  void transformPoint(const boost::numeric::ublas::matrix<float> &transform_matrix, float pos_x_in, float pos_y_in,
                      float &pos_x_out, float &pos_y_out);
  boost::numeric::ublas::matrix<float> GetTransformMatrix(float yaw, float x_pos, float y_pos);
  void UpdateTransformMatrix(boost::numeric::ublas::matrix<float> &matrix, float yaw, float x_pos, float y_pos);
  StructTpObjects(const char *);
  virtual ~StructTpObjects();

  //! initialization
  bool init() override;

  //! not in use currently (later for resetting after successfull simulation)
  bool enterReset() override;
  bool exitReset() override;

  /*! provides plugin description to allow for selection in GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;

  /*! adds a selected configuration provided by the GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;

  /*! removes a selected configuration provided by the GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

  /*! Internally used callback to transform incoming data to output
   *
   * @param topic_name
   * @param datablock
   * @param sensor_config
   */
  void ProcessTPObjectsCallback(const std::string topic_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                                const next::plugins_utils::SensorConfig sensor_config,
                                std::shared_ptr<TrafficList> traffic_list, const ObjectValidation object_validation);
  void ProcessDataCallbackEgoVehicle(const std::string topic_name,
                                     const next::bridgesdk::ChannelMessagePointers *datablock,
                                     next::bridgesdk::plugin_config_t config,
                                     next::plugins_utils::SensorConfig sensor_config,
                                     std::shared_ptr<EgoVehicle> ego_vehicle);
  inline size_t numOfActiveConfig() { return active_config_to_sensors_.size(); }

  std::vector<SearchRequest> search_requests_;

private:
  bool GetArraySizeStruct(const std::string &url, size_t &array_size) const;
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);
  std::unordered_map<std::string, ConfigData> active_config_to_sensors_;
  std::mutex traffic_list_protector_;

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginRegexSearcher> plugin_regex_searcher_;

  std::string fbs_path_;
  std::string bfbs_path_;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};
  CoordinateSystem coordinate_system_;
  ECU_type ecu_type_;

  std::vector<std::string> keywords_;
  ublas::matrix<float> trans_rotate_matrix_;
  mutable std::mutex config_regex_protector_;
  mutable std::mutex config_ego_vehicle_protector_;
  std::atomic<float> ego_y_distance_{0.F};
  std::atomic<float> ego_x_distance_{0.F};
  std::atomic<float> ego_yaw_angle_{0.F};
  std::unordered_map<std::string, CoordinateSystem> const coordinate_system_to_enum_ = {
      {"global", CoordinateSystem::kGlobal},
      {"ego_vehicle_rear_axle", CoordinateSystem::kEgoVehicleRearAxle},
      {"ego_vehicle_front_axle", CoordinateSystem::kEgoVehicleFrontAxle}};
  std::unordered_map<std::string, ECU_type> const ECU_type_to_enum_ = {{"parking MTA5", ECU_type::Parking_MTA5},
                                                                       {"ARS", ECU_type::ARS}};

  void InitSearchRequests();
  void SetRegexValuesInConfig();
  void UpdateRegexValuesFromConfig();
  bool GetAvailableTPLists(const std::string &url, std::vector<SearchResult> &search_results);
  bool GetNamingMap(const std::string &url, std::unordered_map<std::string, std::string> &naming_map);
  ObjectValidation stringToObjectValidation(const std::string &object_validation_string);
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // PLUGINS_STRUCT_TPOBJECTS_H

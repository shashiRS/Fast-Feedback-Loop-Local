/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_data_class_parser.h
 * @brief    Transforms Json messages from Plugin to match expected format
 */
#ifndef NEXT_OUTPUTMANAGER_PLUGIN_DATA_CLASS_PARSER_H_
#define NEXT_OUTPUTMANAGER_PLUGIN_DATA_CLASS_PARSER_H_

#include <json/json.h>

#include <next/control/event_types/player_state_event.h>
#include <next/bridgesdk/data_manager.hpp>
#include <next/bridgesdk/support_fbs_types.hpp>
#include "flatbuffers_to_json.h"
#include "next/bridgesdk/schema/3d_view/view_update_generated.h"
#include "next/bridgesdk/schema/core/signal_update_generated.h"

namespace next {
namespace databridge {
namespace outputmanager {

class PluginDataClassParser {
public:
  /*!
   * @brief Construct a new Plugin Data Class Parser object
   *
   */
  PluginDataClassParser();
  /*!
   * @brief Destroy the Plugin Data Class Parser object
   *
   */
  virtual ~PluginDataClassParser() = default;

  /*!
   * @brief this function calls the internal functions to parse and wrap the data with the matching Json sructure
   *
   * @param wrapped_message pointer to the string that should be created
   * @param path_to_fbs path to fbs file that is used
   * @param flatbuffer the flatbuffer package that shall be parsed
   * @param sensor_info additional info from the sensor
   * @param structure_version_pair pair of the flatbuffer structure and version
   * @return true successufl parse and wrap
   * @return false issue during execution
   */
  bool parseDataClass(std::string &wrapped_message, const std::string &path_to_fbs,
                      const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                      const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                      const std::pair<std::string, std::string> &structure_version_pair);

  void parseDataClassToFlatbuffer(std::string &wrapped_message, const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                  const unsigned int subscription_id,
                                  const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                  const std::pair<std::string, std::string> &structure_version_pair) const;

  /*!
   * @brief Maps the used structure to the supported FBS
   *
   * @param structure_version_pair pair of the flatbuffer structure and version
   * @return supportedFBS Enum value of supported FBS
   */
  SupportedFbsTypes::supportedFBS mapFBStoEnum(const std::pair<std::string, std::string> &structure_version_pair) const;

private:
  /*!
   * @brief Transforms FBS structure to JSON Value
   *
   * @param path_to_fbs path to fbs file that is used
   * @param flatbuffer the flatbuffer package that shall be parsed
   * @param flatbuffer the flatbuffer package that shall be parsed
   * @param sensor_info sensor information
   * @param structure_version_pair pair of the flatbuffer structure and version.
   * @param output_message final output message with Fbs content and wrapper for GUI.
   * @return true if conversion is done correctly otherwise false.
   */
  bool FbsToJson(const std::string &path_to_fbs, const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                 const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                 const std::pair<std::string, std::string> &structure_version_pair, std::string &output_message);

  /*!
   * @brief Make string from json format of general information common for all plugin.
   *
   * @param sensor_info sensor information
   * @param structure_version_pair pair of the flatbuffer structure and version.
   * @param output_general_info sensor information as json string.
   * @return true if conversion of json to string is successful otherwise false.
   */
  bool toJsonGeneralInfo(const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                         const std::pair<std::string, std::string> &structure_version_pair,
                         std::string &output_general_info);

  uint64_t GetCurrentTimestampMicroseconds(const bridgesdk::plugin::SensorInfoForFbs &sensor_info);

  /*!
   * @brief Make json object with general informations for fbs types used by 3D view
   *
   * @param sensor_info sensor information
   * @param structure_version_pair pair of the flatbuffer structure and version.
   * @param output_json output as json format for a fbs type used by 3D view.
   */
  void makeJsonFromSignalInfo3dView(const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                    const std::pair<std::string, std::string> &structure_version_pair,
                                    Json::Value &output_json);

  /*!
   * @brief Make json object with general informations for fbs types used by signal provider plugins
   *
   * @param sensor_info sensor information
   * @param structure_version_pair pair of the flatbuffer structure and version.
   * @param output_json output as json format for a fbs type used by signal provider plugins.
   */
  void makeJsonFromSignalList(const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                              const std::pair<std::string, std::string> &structure_version_pair,
                              Json::Value &output_json);

  /*!
   * @brief Make json object with general informations for custom fbs type.
   *
   * @param sensor_info sensor information
   * @param structure_version_pair pair of the flatbuffer structure and version.
   * @param output_json output as json format for a custom fbs type.
   */
  void makeJsonFromCustomType(const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                              const std::pair<std::string, std::string> &structure_version_pair,
                              Json::Value &output_json);

  std::string UpdateJsonSignals(const std::string &input);

  /*!
   * @brief Parses new signal information and stores them in cache.
   *
   * @param content json object containing new_signal_info and signals list
   */
  void AddNewSignalInfoToCache(const Json::Value &content);

  /*!
   * @brief Creates output json with signal list in the format expected by GUI
   *
   * @param content json object containing new_signal_info and signals list
   * @return formatted output json string
   */
  std::string FormatSignalListToJson(const Json::Value &content);

  next::control::events::PlayerStateEvent meta_event_;
  FlatBufferToJSON fbs_to_json_{};
  flatbuffers::Offset<GuiSchema::ViewUpdate>
  AddObjectsToViewUpdate(const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                         flatbuffers::FlatBufferBuilder &builder, SupportedFbsTypes::supportedFBS fbs_type,
                         const flatbuffers::Offset<Core::MetaInformation> &meta_info) const;

  flatbuffers::Offset<Core::SignalUpdate>
  AddObjectsToSignalUpdate(const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                           flatbuffers::FlatBufferBuilder &builder, SupportedFbsTypes::supportedFBS fbs_type,
                           const flatbuffers::Offset<Core::MetaInformation> &meta_info) const;

  flatbuffers::Offset<Core::MetaInformation>
  GetMetaInformation(flatbuffers::FlatBufferBuilder &builder,
                     const bridgesdk::plugin::SensorInfoForFbs &sensor_info) const;

  //       hash  , signal_name
  std::map<size_t, std::string> signals_information_cache_;
};
} // namespace outputmanager
} // namespace databridge
} // namespace next

#endif // NEXT_OUTPUTMANAGER_PLUGIN_DATA_CLASS_PARSER_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_description_to_json.h
 * @brief    create JSON files from Plugin Config / description
 */

#ifndef NEXT_DATAREQUESTSERVER_JSON_TO_PLUGIN_CONFIG_H
#define NEXT_DATAREQUESTSERVER_JSON_TO_PLUGIN_CONFIG_H

#include <json/json.h>

#include <next/bridgesdk/datatypes.h>

namespace next {
namespace databridge {
namespace data_request_server {

using namespace next::bridgesdk;

class JSONToPluginConfig {
public:
  //! transforms plugin description to JSON
  /*!
   * @param  description refreence to a plugin_config_t multimap that contains all param values
   * @return either a JSON string or std::nullopt
   */
  static void fromJson(const Json::Value &in, plugin_config_t &out_config);

private:
};

} // namespace data_request_server
} // namespace databridge
} // namespace next
#endif // NEXT_DATAREQUESTSERVER_JSON_TO_PLUGIN_CONFIG_H

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     json_to_plugin_config.cpp
 *  @brief    see json_to_plugin_config.h
 */

#include "json_to_plugin_config.h"

namespace next {
namespace databridge {
namespace data_request_server {

void JSONToPluginConfig::fromJson(const Json::Value &in, plugin_config_t &out_config) {
  Json::FastWriter fastWriter;

  if (in["parameters"].isArray()) {
    for (Json::Value param : in["parameters"]) {
      auto json_string_full1 = fastWriter.write(param);
      next::bridgesdk::ConfigType config_type = bridgesdk::ConfigType::FIXED;
      if (param.isMember("editable")) {
        if (true == param["editable"].asBool()) {
          config_type = bridgesdk::ConfigType::EDITABLE;
        }
      }
      // TODO fix this if it is array?!
      std::string value_string;
      if (param["value"].isArray()) {
        value_string = param["value"][0].asString();
      } else {
        value_string = param["value"].asString();
      }
      out_config.insert({param["name"].asString(), {config_type, value_string}});
    }
  }
}

} // namespace data_request_server
} // namespace databridge
} // namespace next

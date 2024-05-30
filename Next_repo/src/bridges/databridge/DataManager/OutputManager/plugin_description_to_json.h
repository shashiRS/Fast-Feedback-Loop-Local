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

#ifndef NEXT_OUTPUTMANAGER_PLUGIN_DESCRIPTION_TO_JSON_H
#define NEXT_OUTPUTMANAGER_PLUGIN_DESCRIPTION_TO_JSON_H

#include <json/json.h>

#include <next/bridgesdk/datatypes.h>

namespace next {
namespace databridge {
namespace outputmanager {

using namespace next::bridgesdk;

class PluginDescriptionToJSON {
public:
  //! transforms plugin description to JSON
  /*!
   * @param  description refreence to a plugin_config_t multimap that contains all param values
   * @return either a JSON string or std::nullopt
   */
  Json::Value parameterToJSON(const plugin_config_t &description) const;

private:
  //! helper function: transform one single editable config to JSON
  Json::Value editableToJSON(const plugin_config_t::value_type &item) const;

  //! helper function: transform a list of selectable configs to JSON
  Json::Value
  selectableToJSON(const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const;

  //! helper function: transform one single fixed config to JSON
  Json::Value fixedToJSON(const plugin_config_t::value_type &item) const;

  Json::Value
  selectableFixedToJSON(const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const;
  Json::Value
  multiSelectToJSON(const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const;
  Json::Value multiSelectFixedToJSON(
      const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const;
};

} // namespace outputmanager
} // namespace databridge
} // namespace next
#endif // NEXT_OUTPUTMANAGER_PLUGIN_DESCRIPTION_TO_JSON_H

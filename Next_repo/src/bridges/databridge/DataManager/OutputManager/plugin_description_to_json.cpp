/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_description_to_json.cpp
 *  @brief    see plugin_description_to_json.h
 */

#include "plugin_description_to_json.h"
#include <next/bridgesdk/datatypes.h>

namespace next {
namespace databridge {
namespace outputmanager {

Json::Value PluginDescriptionToJSON::editableToJSON(const next::bridgesdk::plugin_config_t::value_type &item) const {
  Json::Value val;
  val["name"] = item.first;
  val["value"] = item.second.value;
  val["editable"] = true;
  val["selectable"] = false;
  val["multipleSelection"] = false;
  return val;
}

Json::Value PluginDescriptionToJSON::selectableToJSON(
    const std::pair<next::bridgesdk::plugin_config_t::const_iterator, next::bridgesdk::plugin_config_t::const_iterator>
        &range) const {
  Json::Value val;
  val["name"] = range.first->first;
  for (auto i = range.first; i != range.second; i++) {
    val["value"].append(i->second.value);
  }
  val["editable"] = true;
  val["selectable"] = true;
  val["multipleSelection"] = false;
  return val;
}

Json::Value PluginDescriptionToJSON::fixedToJSON(const next::bridgesdk::plugin_config_t::value_type &item) const {
  Json::Value val;
  val["name"] = item.first;
  val["value"] = item.second.value;
  val["editable"] = false;
  val["selectable"] = false;
  val["multipleSelection"] = false;
  return val;
}

Json::Value PluginDescriptionToJSON::selectableFixedToJSON(
    const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const {
  Json::Value val;
  val["name"] = range.first->first;
  for (auto i = range.first; i != range.second; i++) {
    val["value"].append(i->second.value);
  }
  val["editable"] = false;
  val["selectable"] = true;
  val["multipleSelection"] = false;
  return val;
}
Json::Value PluginDescriptionToJSON::multiSelectToJSON(
    const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const {
  Json::Value val;
  val["name"] = range.first->first;
  for (auto i = range.first; i != range.second; i++) {
    val["value"].append(i->second.value);
  }
  val["editable"] = true;
  val["selectable"] = true;
  val["multipleSelection"] = true;
  return val;
}
Json::Value PluginDescriptionToJSON::multiSelectFixedToJSON(
    const std::pair<plugin_config_t::const_iterator, plugin_config_t::const_iterator> &range) const {
  Json::Value val;
  val["name"] = range.first->first;
  for (auto i = range.first; i != range.second; i++) {
    val["value"].append(i->second.value);
  }
  val["editable"] = false;
  val["selectable"] = true;
  val["multipleSelection"] = true;
  return val;
}

Json::Value PluginDescriptionToJSON::parameterToJSON(const next::bridgesdk::plugin_config_t &description) const {
  Json::Value final_message_json;

  //! the idea is to step item by item through the description multimap, transform each element, depending on its
  //! config_type, to JSON
  // except the config_type SELECTABLE. This oone gets a range of all SELECTABLE items with the same name, but different
  // value. this works only because multimap is a search tree and is always ordered by the key!
  for (auto item = description.begin(); item != description.end(); item++) {
    switch (item->second.config_type) {
    case next::bridgesdk::ConfigType::EDITABLE:
      final_message_json["parameters"].append(editableToJSON(*item));
      break;
    case next::bridgesdk::ConfigType::FIXED:
      final_message_json["parameters"].append(fixedToJSON(*item));
      break;
    case next::bridgesdk::ConfigType::SELECTABLE_FIXED: {
      // get a range of all items with the same name but different value
      auto sel_fixed_range = description.equal_range(item->first);
      final_message_json["parameters"].append(selectableFixedToJSON(sel_fixed_range));
      // move the iterator pointer to the last element of the found range and continue from there
      item = --sel_fixed_range.second;
    } break;
    case next::bridgesdk::ConfigType::SELECTABLE: {
      // get a range of all items with the same name but different value
      auto sel_range = description.equal_range(item->first);
      final_message_json["parameters"].append(selectableToJSON(sel_range));
      // move the iterator pointer to the last element of the found range and continue from there
      item = --sel_range.second;
    } break;
    case next::bridgesdk::ConfigType::MULTISELECT: {
      // get a range of all items with the same name but different value
      auto mul_sel_range = description.equal_range(item->first);
      final_message_json["parameters"].append(multiSelectToJSON(mul_sel_range));
      // move the iterator pointer to the last element of the found range and continue from there
      item = --mul_sel_range.second;
      // final_message_json["editable"] = false;
    } break;
    case next::bridgesdk::ConfigType::MULTISELECT_FIXED: {
      // get a range of all items with the same name but different value
      auto mul_sel_fixed_range = description.equal_range(item->first);
      final_message_json["parameters"].append(multiSelectFixedToJSON(mul_sel_fixed_range));
      // move the iterator pointer to the last element of the found range and continue from there
      item = --mul_sel_fixed_range.second;
      // final_message_json["editable"] = false;
    } break;
    }
  }
  return final_message_json;
}

} // namespace outputmanager
} // namespace databridge
} // namespace next

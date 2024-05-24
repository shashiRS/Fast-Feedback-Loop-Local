/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "core_lib_wrap_data_descriptor_parser.hpp"

#ifndef NO_USE_LOGGER_NAMESPACE
#define UNDEF_NO_USE_LOGGER_NAMESPACE
#endif
#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/logger/logger.hpp>
#ifdef UNDEF_NO_USE_LOGGER_NAMESPACE
#undef NO_USE_LOGGER_NAMESPACE
#undef UNDEF_NO_USE_LOGGER_NAMESPACE
#endif

namespace CoreLibWrap {

using namespace mts::introspection;
using namespace next::udex;

bool DataDescriptionParser::GetSignalInfo(const std::string &topic, const std::string &url, const std::string &schema,
                                          next::sdk::types::PackageFormatType format_type, Signal &signal) {
  // check inputs
  if (topic.empty() || url.empty() || schema.empty() ||
      format_type == next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN) {
    return false;
  }

  // check if topic name can be found in the url
  if (url.find(topic) != 0) {
    return false;
  }

  // check if the url info already exist in the cache
  auto it_url_info = url_signal_info_cache_.find(url);
  if (it_url_info != url_signal_info_cache_.end()) {
    signal = it_url_info->second;
    return true;
  }

  if (topic_dynamic_type_.find(topic) == topic_dynamic_type_.end()) {
    auto context = xtypes::parser::parse(schema);
    xtypes::dynamic_type::ptr dynamic_type = get_root_type(context.get_module());
    auto target_dynamic_type = mts::introspection::get_host_platform_type(*dynamic_type, true, true);

    topic_dynamic_type_.insert(std::make_pair(topic, target_dynamic_type));
  }

  // calculate offsets
  signal_list signals_info;
  size_t size = 0;

  // check if signals_info already in cache
  auto it_signal_list = topic_signal_list_cache_.find(topic);
  if (it_signal_list != topic_signal_list_cache_.end()) {
    signals_info = it_signal_list->second.first;
    size = it_signal_list->second.second;

  } else {
    CalculateOffsets(*topic_dynamic_type_[topic], 1, signals_info, size);
    topic_signal_list_cache_.insert(std::make_pair(topic, std::make_pair(signals_info, size)));
  }

  // get the signal name without view and group
  std::string signal_name = url.substr(topic.size());

  if (signal_name.find('.') == 0) {
    signal_name = signal_name.substr(1, signal_name.size());
  }

  size_t index;
  bool is_array = ExtractNameAndIndexFromArraySignal(index, signal_name);

  Signal topic_info;
  for (auto &signal_info : signals_info) {

    if (signal_name.compare(std::get<0>(signal_info)) != 0) {
      continue;
    }

    if (is_array) {
      signal_info.second.start_offset_ += index * signal_info.second.payload_size_;
      signal_info.second.array_length_ = 1;
    }

    topic_info.data_[url] = std::get<1>(signal_info);
    size = signal_info.second.payload_size_;

    break;
  }

  // check if url is in signal list of topic
  bool signal_valid = true;
  if (topic_info.data_.empty()) {
    PayloadType payload_type;

    if (topic == url) {
      payload_type.payload_size_ = size;
      payload_type.sig_type_ = next::udex::SignalType::SIGNAL_TYPE_STRUCT;
      payload_type.array_length_ = 1;
      payload_type.start_offset_ = 0;
    } else {
      // add default signal type to cache if url is not found
      // Hint: Improves runtime of call "SetUrls" from Struct extractor objects with array structs (urls are invalid
      // till call of UpdateUrlInVector)
      payload_type.payload_size_ = 0;
      payload_type.sig_type_ = next::udex::SignalType::SIGNAL_TYPE_STRUCT;
      payload_type.array_length_ = 0;
      payload_type.start_offset_ = 0;

      signal_valid = false;
    }
    topic_info.data_[url] = payload_type;
  }

  url_signal_info_cache_.emplace(url, topic_info);

  signal = topic_info;

  return signal_valid;
}

bool DataDescriptionParser::ExtractNameAndIndexFromArraySignal(size_t &index, std::string &signal_name) {
  // get the index if the signal is an array
  index = 0;
  bool is_array = false;
  auto bracket_pos_end = signal_name.rfind(']');

  if (bracket_pos_end + 1 == signal_name.size()) {
    std::string tmp = signal_name;

    auto bracket_pos_start = tmp.rfind('[');
    if (bracket_pos_start != std::string::npos) {
      // create number from bracket
      tmp = tmp.substr(bracket_pos_start + 1);
      tmp.pop_back();

      try {
        // convert index from string
        index = std::stoul(tmp.c_str());
        signal_name = signal_name.substr(0, bracket_pos_start);
      } catch (std::exception &e) {
        next::sdk::logger::error(__FILE__, "index conversion failure: {0}", e.what());
        return false;
      }

      is_array = true;
    }
  }
  return is_array;
}

void DataDescriptionParser::CalculateOffsets(const xtypes::dynamic_type &dynamic_type_node, size_t depth,
                                             signal_list &signals, size_t &size) {
  dynamic_type_node.for_each([&](const xtypes::dynamic_type::type_node &node) {
    auto member = node.from_member();

    if (member == nullptr || (!node.has_parent()) || node.depth() != depth) {
      return;
    }
    if (node.type().is_primitive_type()) {
      auto kind2 = node.type().kind();
      if (node.type().is_signal_type()) {
        kind2 = kind2 & (~xtypes::type_kind::signal_type);
      }
      PayloadType payload{member->type().memory_size(), SignalType::SIGNAL_TYPE_INT32, 1, member->offset()};
      auto map_it = data_type_mapping_.find(static_cast<int>(kind2));
      if (map_it != data_type_mapping_.end()) {
        payload.sig_type_ = map_it->second;
      }
      signals.emplace_back(node.from_member()->name(), std::move(payload));
    } else if (node.type().is_collection_type()) {
      const auto &collection_type = static_cast<const xtypes::collection_type &>(node.type());
      if (collection_type.content_type().is_primitive_type() && member->type().is_array_type()) {
        const auto &array_type = static_cast<const xtypes::array_type &>(node.type());
        /*calculate offsets for each dimension */
        auto kind2 = collection_type.content_type().kind();
        if (collection_type.content_type().is_signal_type()) {
          kind2 = kind2 & (~xtypes::type_kind::signal_type);
        }
        PayloadType payload{collection_type.content_type().memory_size(), SignalType::SIGNAL_TYPE_INT32, 1,
                            member->offset() + collection_type.content_type().memory_size()};
        auto map_it = data_type_mapping_.find(static_cast<int>(kind2));
        if (map_it != data_type_mapping_.end()) {
          payload.sig_type_ = map_it->second;
        }
        PayloadType payload_array{collection_type.content_type().memory_size(), payload.sig_type_,
                                  array_type.dimension(), member->offset()};
        signals.emplace_back(node.from_member()->name(), std::move(payload_array));

      } else if (collection_type.content_type().is_structure_type() && member->type().is_array_type()) {
        /*Member is of type structure_type*/
        signal_list temp_signals;
        /*calculate the offset for the members of the structure_type*/
        CalculateOffsets(member->type(), 2, temp_signals, size);

        const auto &array_type = static_cast<const xtypes::array_type &>(node.type());

        PayloadType payload{collection_type.content_type().memory_size(), SignalType::SIGNAL_TYPE_STRUCT,
                            array_type.dimension(), member->offset()};
        signals.emplace_back(member->name(), std::move(payload));

        /*calculate offsets for each dimension */
        for (size_t i = 0; i < array_type.dimension(); i++) {
          PayloadType payload_index{collection_type.content_type().memory_size(), SignalType::SIGNAL_TYPE_STRUCT, 1,
                                    member->offset() + collection_type.content_type().memory_size() * i};
          signals.emplace_back(member->name() + "[" + std::to_string(i) + "]", std::move(payload_index));

          /*calculate the offsets of members of structure_type with in parent of structure_type*/
          for (auto &signal : temp_signals) {
            signals.emplace_back(member->name() + "[" + std::to_string(i) + "]." + std::get<0>(signal),
                                 PayloadType{std::get<1>(signal).payload_size_, std::get<1>(signal).sig_type_,
                                             std::get<1>(signal).array_length_,
                                             member->offset() + collection_type.content_type().memory_size() * i +
                                                 std::get<1>(signal).start_offset_});
          }
        }
      }
    } else if (node.type().is_structure_type()) {
      /*Member is of type structure_type*/
      signal_list temp_signals;
      /*calculate the offset for the members of the structure_type*/
      CalculateOffsets(member->type(), 1, temp_signals, size);

      PayloadType payload{member->type().memory_size(), SignalType::SIGNAL_TYPE_STRUCT, 1, member->offset()};
      signals.emplace_back(member->name(), std::move(payload));

      /*calculate the offsets of members of structure_type type with in parent of structure_type*/
      for (auto &signal : temp_signals) {
        std::get<1>(signal).start_offset_ = member->offset() + std::get<1>(signal).start_offset_;
        signals.emplace_back(member->name() + "." + std::get<0>(signal), std::move(std::get<1>(signal)));
      }
    }
  });

  size = dynamic_type_node.memory_size();
}

std::pair<std::string, pt::ptree> DataDescriptionParser::parseDescription(const std::string_view &description) {

  auto context = xtypes::parser::parse(static_cast<std::string>(description));
  xtypes::dynamic_type::ptr dynamic_type = get_root_type(context.get_module());
  auto target_dynamic_type = mts::introspection::get_host_platform_type(*dynamic_type, true, true);

  auto result = fillSignalTree(*target_dynamic_type, 1);
  return std::make_pair(target_dynamic_type->name(), result);
}

pt::ptree DataDescriptionParser::fillSignalTree(const mts::introspection::xtypes::dynamic_type &dynamic_type_node,
                                                const size_t &depth) {
  pt::ptree subnodes;
  std::vector<std::string> signal_names;
  dynamic_type_node.for_each([&](const xtypes::dynamic_type::type_node &node) {
    auto member = node.from_member();
    if (member == nullptr || (!node.has_parent()) || node.depth() != depth) {
      return;
    }
    if (node.type().is_primitive_type()) {
      signal_names.emplace_back(member->name());
    } else if (node.type().is_collection_type()) {
      const auto &collection_type = static_cast<const xtypes::collection_type &>(node.type());
      if (collection_type.content_type().is_primitive_type() && member->type().is_array_type()) {
        const auto &array_type = static_cast<const xtypes::array_type &>(node.type());
        auto child_subnodes = (member->name(), pt::ptree{});
        child_subnodes.put("arrayDimensions", array_type.dimension());
        subnodes.add_child(member->name(), child_subnodes);

      } else if (collection_type.content_type().is_structure_type() && member->type().is_array_type()) {
        const auto &array_type = static_cast<const xtypes::array_type &>(node.type());
        auto child_subnodes = fillSignalTree(member->type(), 2);
        child_subnodes.put("arrayDimensions", array_type.dimension());
        subnodes.add_child(member->name(), child_subnodes);
      }
    } else if (node.type().is_structure_type()) {
      auto child_subnodes = fillSignalTree(member->type(), 1);
      subnodes.add_child(member->name(), child_subnodes);
    }
  });

  for (auto node_name : signal_names) {
    subnodes.add_child(node_name, pt::ptree{});
  }
  return subnodes;
}

} // namespace CoreLibWrap

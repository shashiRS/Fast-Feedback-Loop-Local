/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "signal_tree_module.h"

#include <cctype>
#include <iostream>
#include <regex>

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include <next/sdk/logger/logger.hpp>

// #include

namespace next {
namespace udex {
namespace ecal_util {

SignalTree::SignalTree(const std::string &description, const std::string &topic) : description_(description) {
  if (description.empty()) {
    debug(__FILE__, "Creating SignalTree with empty description for topic: {0}", topic);
  }
  parseDescription(description, topic);
}

void SignalTree::parseDescription(const std::string &description, const std::string &topic) {

  try {
    auto result = core_lib_parser_.parseDescription(description);
    signal_tree_.add_child(result.first, result.second);
    fillSearchSet(topic);
  } catch (std::exception &e) {
    error(__FILE__, " Error parsing the description for the topic: {0}, error: {1}", topic, e.what());
  }
}

void SignalTree::walk_ptree(const boost::property_tree::ptree &tree,
                            std::unordered_set<NodeStruct, NodeHashFunction> &key_set, const std::string &parent_key) {
  if (tree.empty() || (tree.size() == 1 && tree.count("arrayDimensions") == 1)) {
    NodeStruct tmp;
    tmp.url_ = parent_key;
    key_set.insert(tmp);
    return;
  }

  for (const auto &it : tree) {
    if (it.first == "arrayDimensions") {
      continue;
    }
    std::string array_dimmensions = it.second.get<std::string>("arrayDimensions", "");
    if (array_dimmensions != "") {
      int number_elements = std::stoi(array_dimmensions);
      // add a separator between key segments
      std::string key = parent_key;
      std::stringstream ss;
      ss << key << "." << it.first << "[%" << number_elements << "%]";
      walk_ptree(it.second, key_set, ss.str());
    } else {
      // add a separator between key segments
      std::string key = parent_key;
      if (!key.empty()) {
        key += ".";
      }
      key += it.first;
      walk_ptree(it.second, key_set, key);
    }
  }
}

void SignalTree::fillSearchSet(const std::string &topic = "") {
  std::string parent_url = "";

  const auto pos_last_dot = topic.find_last_of(".");
  if (pos_last_dot != std::string::npos) {
    parent_url = topic.substr(0, pos_last_dot);
  }

  walk_ptree(signal_tree_, node_set_, parent_url);
}

void SignalTree::extendArray(const std::string &url, std::vector<std::string> &output) const {
  static boost::regex reg("%(\\d+)%");
  boost::smatch match;
  if (boost::regex_search(url, match, reg)) {
    int array_size = std::stoi(match[1]);
    for (int i = 0; i < array_size; i++) {
      std::string s = match.prefix().str() + std::to_string(i) + match.suffix().str();
      extendArray(s, output);
    }
  } else {
    output.push_back(url);
  }
  return;
}

std::vector<std::string> SignalTree::searchForUrls(const std::string &keyword) const {
  std::vector<std::string> temp_results;
  for (const auto &entry : node_set_) {
    auto search_it =
        std::search(entry.url_.begin(), entry.url_.end(), keyword.begin(), keyword.end(),
                    [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
    if (search_it != entry.url_.end()) {
      temp_results.push_back(entry.url_);
    }
  }
  // we need to expand the arrays from the form array[%i%] to array[0], array[1], ...
  std::vector<std::string> result;
  for (const auto &current_url : temp_results) {
    std::vector<std::string> new_array;
    extendArray(current_url, new_array);
    result.reserve(new_array.size());
    std::move(new_array.begin(), new_array.end(), std::back_inserter(result));
  }
  return result;
}

std::vector<ChildInfo> SignalTree::getChildrenInfo(const std::string &url) {
  std::vector<ChildInfo> children_info;
  boost::optional<pt::ptree &> url_tree = signal_tree_.get_child_optional(url);

  auto get_children = [&children_info](const pt::ptree &node, const std::string &node_name) {
    std::string array_dimmensions = node.get<std::string>("arrayDimensions", "");
    int child_count = static_cast<int>(node.size());
    if (array_dimmensions != "") {
      ChildInfo info;
      info.child_count = child_count - 1;
      info.name = node_name;
      info.array_lenght = std::stoul(array_dimmensions);
      children_info.push_back(info);
    } else {
      ChildInfo info;
      info.name = node_name;
      info.child_count = child_count;
      info.array_lenght = 1; // no array -> single element
      children_info.push_back(info);
    }
  };

  if (url_tree) {
    for (const auto &node : url_tree.get()) {
      if (node.first != "arrayDimensions") {
        get_children(node.second, node.first);
      }
    }
  }
  return children_info;
}

std::vector<std::string> SignalTree::getFullSignalList() {
  if (full_url_list_.size() > 0)
    return full_url_list_;

  for (const auto &entry : node_set_) {
    std::vector<std::string> tmp;
    extendArray(entry.url_, tmp);
    full_url_list_.reserve(tmp.size());
    std::move(tmp.begin(), tmp.end(), std::back_inserter(full_url_list_));
  }

  return full_url_list_;
}

} // namespace ecal_util
} // namespace udex
} // namespace next

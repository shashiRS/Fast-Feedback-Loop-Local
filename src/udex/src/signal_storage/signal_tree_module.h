/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     signal_tree_module.h
 * @brief    parses the topic description and hold a tree like structure for the signals
 */

#ifndef NEXT_UDEX_SIGNAL_TREE_MODULE_H
#define NEXT_UDEX_SIGNAL_TREE_MODULE_H

#include <map>
#include <unordered_set>

#include <boost/json.hpp>
#include <boost/property_tree/ptree.hpp>

#include "core_lib_wrap_data_descriptor_parser.hpp"

namespace next {
namespace udex {
namespace ecal_util {

namespace pt = boost::property_tree;

struct NodeStruct {
  std::string url_{""};
  size_t children_count_{0};
  bool operator==(const NodeStruct &t) const { return (this->url_ == t.url_); }
};

class NodeHashFunction {
public:
  // id is returned as hash function
  size_t operator()(const NodeStruct &t) const { return std::hash<std::string>{}(t.url_); }
};

class SignalTree {
public:
  SignalTree() {}
  //! c-tor
  /*!
   * @param description
   * @param description the signal description json
   * @param topic the topic name of the SiganlTree
   */
  SignalTree(const std::string &description, const std::string &topic);

  //! searches for urls containing the specified keyword
  /*!
   * @param keyword the searched keyword
   * @return vector of urls that contain the searched keyword
   */
  std::vector<std::string> searchForUrls(const std::string &keyword) const;

  //! get the children info of a specified url
  /*!
   * @param url the signal url for which we will get the children info
   * @return vector of ChildInfo for a specific url
   */
  std::vector<ChildInfo> getChildrenInfo(const std::string &url);

  //! get the full url list
  /*!
   * @return the full url list
   */
  std::vector<std::string> getFullSignalList();

private:
  void walk_ptree(const boost::property_tree::ptree &tree, std::unordered_set<NodeStruct, NodeHashFunction> &key_set,
                  const std::string &parent_key);

  void parseDescription(const std::string &description, const std::string &topic);

  //! fills the search cache with data
  void fillSearchSet(const std::string &parent_url);

  //! recursive method for creating a vector of urls  if the current url contains the following syntax
  //! array[%no_elements%] => array[0], array[1], .. array[no_elements-1]
  /*!
   * @param url the urls that will be decoded
   * @param output output where the new urls will be placed
   */
  void extendArray(const std::string &url, std::vector<std::string> &output) const;

  // holds the entire signal tree
  pt::ptree signal_tree_;
  // map containing info about structure description inside the json
  std::unordered_map<std::string, boost::json::value> type_map_;

  // using this set for search functionality
  std::unordered_set<NodeStruct, NodeHashFunction> node_set_;
  std::string description_;
  std::vector<std::string> full_url_list_;
  CoreLibWrap::DataDescriptionParser core_lib_parser_;
};

} // namespace ecal_util
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SIGNAL_TREE_MODULE_H

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_explorer.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_SIGNALEXPLORER_H_
#define NEXT_BRIDGESDK_PLUGIN_SIGNALEXPLORER_H_

#include <next/bridgesdk/plugin.h>
#include <next/sdk/types/package_data_types.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginSignalExplorerImpl;

struct signal_child_info {
  std::string name{""};
  int child_count{0};
  size_t array_size{0};
};

struct PluginSignalDescription {
  std::string data_source_name = "";
  next::sdk::types::PackageFormatType format_type;
  uint16_t source_id = 0;
  uint32_t instance_id = 0;
  uint32_t cycle_id = 0;
  uint64_t vaddr = 0;
};

class DECLSPEC_plugin_config PluginSignalExplorer {
private:
  PluginSignalExplorer() = delete;

  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  std::unique_ptr<PluginSignalExplorerImpl> impl_;
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  PluginSignalExplorer(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginSignalExplorer();

  //! receive information about the current signal list and structure
  /*!
   * List all the children-url of the requested parent-url. Enables information about the current signal structure
   * To enable a full signal tree call this function iteratively
   * @param parent_url requested parent-url for which all children url names will be provided
   * @return list of children urls
   */
  virtual std::vector<signal_child_info> getChildsbyUrl(const std::string &parent_url) const;

  //! Retrives all the data source names of the given format identifier.
  /*!
   * @param format_id format idenetifier of the data source.
   * @param data_source_names list of data source names with given format identifier.
   */
  virtual void getDataSourceNames(const std::string &format_id, std::vector<std::string> &data_source_names) const;

  //! receive information about the whole signal tree
  /*!
   * \todo Please provide detailed explanation here
   * @return a vector of device trees which include all children from a device
   */
  virtual std::unordered_map<std::string, std::vector<std::string>> getUrlTree(void) const;

  //! receive information about the backend data transportation layer to enhance efficiency
  /*!
   * Lists all topics which are required to deliver the signal and all its children.
   * Call this function to receive information to bundle subscription calls.
   *
   * The topic names do not have to be provided later on. This is for pure performance enhancement.
   *
   * @param signal_url requested url
   * @return list of all topics which are used in the backend to supply information about this url and its children
   */
  virtual std::vector<std::string> getTopicsByUrl(const std::string &signal_url) const;

  //! receive information about the backend raw data information
  /*!
   * provides detailed information about a package.
   * Enter any subpackage url.
   * Attention: only delivers package level information. This info is the same for all children of a group
   *
   * @param signal_url requested url
   * @return list of all topics which are used in the backend to supply information about this url and its children
   * emtpy description if no valid signal was found for the url
   */
  virtual PluginSignalDescription getPackageDetailsByUrl(const std::string &signal_url) const;

  //! generate and return sdl for a given signal description
  /*
   * @parma description
   */
  virtual std::pair<bool, std::string> generateSdl(const std::string topic) const;

  //! receives the list of the urls after searching in backend
  /*!
   * @param keyword is the string search token
   * @return list/vector of the urls from the search algorithm in the backend
   */
  virtual std::vector<std::string> searchSignalTree(const std::string &keyword) const;
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_SIGNALEXPLORER_H_

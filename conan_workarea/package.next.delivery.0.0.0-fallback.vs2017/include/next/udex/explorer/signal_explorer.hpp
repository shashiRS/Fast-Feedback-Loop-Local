/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     SignalExplorer.h
 * @brief    Utilities to extract information about the current simulation network
 *           Enables signal search, tree traversal and other simulation context information
 */

#ifndef NEXT_UDEX_SIGNAL_EXPLORER_H
#define NEXT_UDEX_SIGNAL_EXPLORER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <next/sdk/sdk_macros.h>

#include "../data_types.hpp"
#include "../udex_config.h"

namespace next {
namespace udex {
namespace explorer {

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class SignalExplorerImpl;

class UDEX_LIBRARY_EXPORT SignalExplorer {
public:
  SignalExplorer();
  virtual ~SignalExplorer();

  //! extract the children of the requested parent_url
  /*!
   * Extracts the next level of urls and expend the signal list if they are arrays.
   *
   * to navigate through the signal tree call this funciton
   *
   * @param parent_url
   * @return list of all children urls
   */
  virtual std::vector<ChildInfo> GetChildByUrl(std::string parent_url);

  //! Returns the urls which are the parent_url for packages sent over the network
  /*!
   * If the requested signal is available in a single package only a single package_url is given.
   * If multiple packages are needed for the requested signal (for example a device name as a url)
   * multiple urls are returned and the data package has to stiched bakc together manually.
   */
  virtual std::vector<std::string> GetPackageUrl(std::string signal_url);

  //! get a full list of all availble signals expended fully
  /*!
   * warning: takes a lot of effort to compute the full structure
   * not recommended during runtime
   */
  virtual std::unordered_map<std::string, std::vector<std::string>> GetFullUrlTree();

  //! get signal description (basic info and binary info) of a url
  /*!
   * @param url
   * available return true if the description available false if not
   * @return Signal description
   */
  virtual SignalDescription getSignalDescription(const std::string &url, bool &available);

  //! generate and return sdl for a given topic
  /*
   * @param description
   */
  virtual std::pair<bool, std::string> generateSdl(const std::string topic);

  //! search for a specific keyword in the signal tree
  /*!
   * @param keyword to search
   * @return sginals containing the specific keyword, empty if not contianing the specific keyword
   */
  std::vector<std::string> searchSignalTree(std::string keyword);

  //! get a list of all matching devices for a specific format
  /*!
   * take a look at data_types.hpp for all the possible formats (getPackageFormatTypeEnum)
   */
  virtual std::vector<std::string> getDeviceByFormatType(const std::string &format);

  //! enables fast search
  /*!
   * enables fast search in signal watcher for faster initialization
   */
  virtual void EnableFastSearch();

private:
  std::unique_ptr<SignalExplorerImpl> impl_;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace explorer
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SIGNAL_EXPLORER_H

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     package_tree_extractor.hpp
 * @brief    creates a tree structure and contains all signals for a respective package.
 * DOES NOT WORK ON DATA STRUCTURES DEFINED ON MULTIPLE PACKAGES!
 */
#ifndef NEXT_UDEX_PACKAGE_TREE_EXTRACTOR_H
#define NEXT_UDEX_PACKAGE_TREE_EXTRACTOR_H

#include <next/sdk/sdk_macros.h>

#include "container_extractor.hpp"

namespace next {
namespace udex {
namespace ecal_util {
class SignalTree;
}
namespace struct_extractor {

/*! @brief PackageTreeExtractor provides a
 *
 *
 */
NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class UDEX_LIBRARY_EXPORT PackageTreeExtractor {
public:
  //! defines a automatically generated Signal to access all leaf-level signals in the PackageTreeExtractor
  struct Signal {
    std::string name; //! full url of signal
    std::shared_ptr<AnyValue>
        value; //! AnyValue which can be accessed by its original data type. No casting takes place
  };
  std::vector<Signal> signals_;

public:
  //! @brief Create a tree-like structure containing all signals below the parent_url.
  /*! Read out the full tree-like strucutre and create an AnyValue for all leaf-signals (non-parent urls)
   *
   * WATCH OUT!!! Dont use parent_urls which are split over multiple packages!
   * @param parent_url url to the most top-level url which should be extracted. ONLY USE PACKAGE_URLS or LOWER!
   */
  PackageTreeExtractor(const std::string &parent_url);

  /*! Set the associated package Memory. NEVER set the memory explicit to the parent_url but always the PACKAGE_MEMORY
   * (retrieved on the UDex Callback)
   * @param memory pointer to the package-memory block
   * @param size_memory memory block size. Used for simplistic out of bounds checks
   */
  void SetMemory(const char *memory, size_t size_memory);

  /*! Returns the parent url used to set up the TreeExtractor
   *
   * @return name of parent url used on construction
   */
  std::string GetParentUrl();

  /*! Check if setup was successful. This checks if the object is valid after the constructor
   *
   * @return true if successful, false if not useable
   */
  bool SetupSuccessful();

  //! @brief will be removed on NEXT 1.0.0 use SetupSuccessful() instead
  NEXT_DISABLE_DEPRECATED_WARNING
  [[deprecated]] bool init_successful_ = false;
  NEXT_RESTORE_WARNINGS

protected:
  size_t FindPackageUrlPosFromFullUrl(std::string full_url);

  //! reduces the ChildInfo to only container unique structures where arrays are not expended out
  void RemoveDuplicatesAndArrays(std::vector<ChildInfo> &children);

private:
  //! Updates the signal urls to have the full url in them instead of the child only url
  void UpdateUrls();

  //! creates all children based on the url and adds them to the container
  void CreateChildrenByUrl(const std::string &parent_url, std::shared_ptr<ExtractorContainer> container);

  //! Inserts a single child (either AnyValue or ExtractorContainter) to the parent based on the ChildInfo
  void CreateSingleChildByUrl(const std::string &current_url, const ChildInfo &child,
                              std::shared_ptr<ExtractorContainer> parent);

  //! inserts a single container into the parent based on the ChildInfo
  void SetSingleContainer(const ChildInfo &child, const std::shared_ptr<ExtractorContainer> &parent,
                          std::string &full_url);

  //! Sets up an array of Containers into the parent
  void SetArrayContainer(const ChildInfo &child, const std::shared_ptr<ExtractorContainer> &parent,
                         std::string &full_url, size_t array_length);

  //! Sets up a AnyValule for a single leaf url into the parent container
  void SetSingleLeaf(const ChildInfo &child, const std::shared_ptr<ExtractorContainer> &parent,
                     const std::string &full_url);

  //! remove duplicates (used for string comparisons)
  template <typename T>
  void RemoveDuplicates(std::vector<T> &vec);

  //! The SignalTree object can only work with urls which are rooted in the package_url
  //! This functino removes device and view from the url (first two elements of a url)
  std::string CreateSearchStringFromFullUrl(const std::string &parent_url) const;

  //! special treatment of a PackageTreeExtractor being requested for a single leaf signal.
  //! this means the root_url has NO children
  bool HandleLeafAsTreeExtractor(const std::string &root_url);

private:
  std::string parent_url_ = "";

  std::shared_ptr<ecal_util::SignalTree> signal_tree_;
  std::shared_ptr<ExtractorContainer> root_;
  size_t string_offset_root_url_;
};
NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace struct_extractor
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_PACKAGE_TREE_EXTRACTOR_H

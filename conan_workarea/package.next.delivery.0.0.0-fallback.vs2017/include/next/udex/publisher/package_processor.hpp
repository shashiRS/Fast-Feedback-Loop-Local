/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     package_processor.hpp
 * @brief    Class to handle the registration of a new data provider
 *           to enable publishing data via the middleware network
 */
#ifndef NEXT_PACKAGE_PROCESSOR_HPP
#define NEXT_PACKAGE_PROCESSOR_HPP

#include <next/sdk/types/data_description_types.hpp>
#include <next/sdk/types/package_data_types.hpp>

namespace next {
namespace udex {
namespace publisher {

class PackageProcessor {
public:
  PackageProcessor() = default;
  virtual ~PackageProcessor() = default;

  /*! @brief Creates new instance of this reader
   *
   * Called in case another instance of this reader is required (e.g. in case multiple recordings should be read in
   * parallel).
   *
   * @return pointer to new instance of the reader
   */
  virtual std::shared_ptr<PackageProcessor> CreateNewInstance() = 0;

  virtual std::vector<next::sdk::types::DataDescription> provideDataDescription() = 0;
  virtual std::vector<next::sdk::types::IPackage::PackageMetaInfo> provideRequestedPackageInfo() = 0;

  virtual std::unique_ptr<next::sdk::types::IPackage>
  processPackage(const std::unique_ptr<next::sdk::types::IPackage> &package) = 0;
};

} // namespace publisher
} // namespace udex
} // namespace next

inline const char *createProcessorMethodName = "create_processor";
typedef next::udex::publisher::PackageProcessor *processor_creator_t;

#endif // NEXT_PACKAGE_PROCESSOR_HPP

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     dummy_processor.hpp
 * @brief    a dummy implementation for a PackageProcessor.
 *           Can be used with DataPublishers
 *
 **/

#ifndef NEXT_DUMMY_PROCESSOR_HPP
#define NEXT_DUMMY_PROCESSOR_HPP

#include <next/udex/publisher/package_processor.hpp>

class DummyProcessor : public next::udex::publisher::PackageProcessor {
public:
  DummyProcessor();
  ~DummyProcessor();

  virtual std::shared_ptr<PackageProcessor> CreateNewInstance() override;

  virtual std::vector<next::sdk::types::DataDescription> provideDataDescription() override;
  virtual std::vector<next::sdk::types::IPackage::PackageMetaInfo> provideRequestedPackageInfo() override;

  virtual std::unique_ptr<next::sdk::types::IPackage>
  processPackage(const std::unique_ptr<next::sdk::types::IPackage> &package) override;
};

#if defined(_WIN32)
#define NEXTREADERPLUGINSHARED_EXPORT __declspec(dllexport)
#else
#define NEXTREADERPLUGINSHARED_EXPORT
#endif

extern "C" NEXTREADERPLUGINSHARED_EXPORT processor_creator_t create_processor();

#endif // NEXT_DUMMY_PROCESSOR_HPP

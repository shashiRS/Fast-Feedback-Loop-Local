/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     reader_synchronizer.hpp
 * @brief    Manages the concrete reader instance selection to be used by the recording file reader.
 *
 **/

#ifndef READER_SYNCHRONIZER_H_
#define READER_SYNCHRONIZER_H_

#include <next/sdk/types/data_description_types.hpp>

#include "reader_manager.hpp"

namespace next {
namespace player {

class ReaderSynchronizer : public next::player::plugin::ReaderInterface {
public:
  friend class ReaderSynchronizerTester;

  ReaderSynchronizer();
  virtual ~ReaderSynchronizer();

  virtual std::set<std::string> GetSupportedFileTypes() override;
  virtual bool Reset() override;

  virtual bool OpenFile(const std::vector<std::string> &file_paths,
                        const next::player::plugin::LoadingProgressCallback loading_callback) override;
  virtual next::player::plugin::FileStats GetFileStats() override;
  virtual std::vector<next::sdk::types::DataDescription> GetCurrentDataDescriptions() override;
  virtual bool SetCurrentPackageNames(const std::vector<std::string> &package_name_list,
                                      std::vector<std::string> &additional_binary_package_names) override;
  virtual std::vector<std::unique_ptr<next::sdk::types::IPackage>> GetPackages() override;

  virtual uint64_t GetCurrentTimestamp() override;
  virtual bool CheckEndOfFile() override;

  virtual bool JumpToTimestamp(uint64_t target_timestamp) override;

  virtual std::shared_ptr<ReaderInterface> CreateNewInstance() override;

  virtual bool SetFilter(std::string filter_expression) override;
  virtual bool SupportsFilter() override;

  virtual std::vector<CycleInformation> GetCycleDescription() override;

protected:
  std::vector<std::shared_ptr<next::player::plugin::ReaderInterface>> readers;
  bool initWithReaders();

private:
  next::player::plugin::FileStats fileStats;
  bool supportsFilter;
  uint64_t currentTimestamp;
  std::vector<next::sdk::types::DataDescription> data_descriptions{};
  std::vector<next::player::plugin::ReaderInterface::CycleInformation> cycle_descriptions{};

  std::map<const std::shared_ptr<next::player::plugin::ReaderInterface>,
           std::vector<std::unique_ptr<next::sdk::types::IPackage>>>
      packagesPerReader{};
};

} // namespace player
} // namespace next

#endif // READER_SYNCHRONIZER_H_

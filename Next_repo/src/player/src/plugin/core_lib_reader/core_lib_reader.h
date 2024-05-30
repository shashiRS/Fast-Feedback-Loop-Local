/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_PLAYER_CORE_LIB_READER_H
#define NEXT_PLAYER_CORE_LIB_READER_H

#include <chrono>

#include <next/sdk/sdk.hpp>
#include <next/sdk/types/data_description_types.hpp>

#include <next/player/plugin/reader_interface.hpp>

namespace next {
namespace player {
namespace plugin {
namespace corelibreader {
class NextCoreLibReaderImpl;

class NextCoreLibReader : public next::player::plugin::ReaderInterface {
public:
  NextCoreLibReader();
  virtual ~NextCoreLibReader();

  virtual std::set<std::string> GetSupportedFileTypes() override;
  virtual bool Reset() override;

  virtual bool OpenFile(const std::vector<std::string> &file_paths,
                        const LoadingProgressCallback loading_callback) override;
  virtual FileStats GetFileStats() override;
  virtual std::vector<next::sdk::types::DataDescription> GetCurrentDataDescriptions();
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

private:
  std::unique_ptr<NextCoreLibReaderImpl> impl;
};

} // namespace corelibreader
} // namespace plugin
} // namespace player
} // namespace next

#endif // NEXT_PLAYER_CORE_LIB_READER_H

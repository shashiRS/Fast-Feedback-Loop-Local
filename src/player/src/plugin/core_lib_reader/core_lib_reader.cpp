#include "core_lib_reader.h"

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/runtime/offline/replay_proxy.hpp>
#include <mts/runtime/processor/data_processor_proxy.hpp>
#include <mts/runtime/udex/extractor_proxy.hpp>
#include "core_lib_package.h"
NEXT_RESTORE_WARNINGS

#include "next/sdk/types/package_hash.hpp"

#include "core_lib_reader_impl.hpp"

namespace next {
namespace player {
namespace plugin {
namespace corelibreader {

NextCoreLibReader::NextCoreLibReader() { impl = std::make_unique<NextCoreLibReaderImpl>(); }
NextCoreLibReader::~NextCoreLibReader() {}

std::set<std::string> NextCoreLibReader::GetSupportedFileTypes() {
  std::set<std::string> ret = std::set<std::string>();

  ret.insert("rrec");
  ret.insert("rec");
  ret.insert("zrec");
  ret.insert("pcap");

  return ret;
}

bool NextCoreLibReader::Reset() { return true; }

bool NextCoreLibReader::OpenFile(const std::vector<std::string> &file_paths, const LoadingProgressCallback) {
  using namespace std::chrono_literals;

  if (file_paths.size() != 1) {
    return false;
  }
  std::string file_name = file_paths[0];
  auto replay_proxy = mts::runtime::offline::open_recording(file_name, true, {}, 500ms);
  if (replay_proxy == nullptr) {
    next::sdk::logger::error(__FILE__, "Replay not available for file: {}", file_name);
    return false;
  }
  impl->replay_ptr_ = std::move(replay_proxy);
  return true;
}

FileStats NextCoreLibReader::GetFileStats() {
  FileStats file_stats{};
  if (!impl->replay_ptr_) {
    return {};
  }
  try {
    file_stats.start_timestamp = impl->replay_ptr_->get_first_timestamp();
    if (impl->replay_ptr_->supports_extended_navigation()) {
      // Only readers that support extended navigation can provide the last timestamp in the recording
      file_stats.end_timestamp = impl->replay_ptr_->get_last_timestamp();
    }
    if (impl->replay_ptr_->supports_random_access()) {
      // Check if the underlying reader supports random access
      // If random access is not supported then it is not possible to jump to timestamps in the recording
      file_stats.supports_random_access = true;
    }
    file_stats.file_size = 0;
  } catch (std::exception &e) {
    error(__FILE__, "Exception thrown while accessing recording file stats: {}", e.what());
    return {};
  }
  return file_stats;
}

std::vector<next::sdk::types::DataDescription> NextCoreLibReader::GetCurrentDataDescriptions() {

  std::vector<next::sdk::types::DataDescription> data_descriptions{};
  if (!impl->replay_ptr_) {
    return data_descriptions;
  }
  auto data_source_provider = impl->replay_ptr_->get_data_source_provider();
  auto data_sources = data_source_provider->get_data_sources().get();
  // Expected data source count check
  size_t data_source_index = 0;
  for (const auto &data_source : data_sources) {

    if (data_source.get_data_description_count() == 0) {
      next::sdk::types::DataDescription description_temp = impl->CreateEmptyDescription(data_source);
      data_descriptions.push_back(description_temp);
    }

    for (size_t i_description = 0; i_description < data_source.get_data_description_count(); i_description++) {
      mts::runtime::data_description mts_data_description =
          data_source_provider->get_data_description(data_source_index, std::size_t(i_description)).get();
      data_descriptions.push_back(
          impl->CreateDescriptionWithBinary(data_source_provider, data_source, mts_data_description));
    }
    data_source_index++;
  }
  return data_descriptions;
}

std::vector<std::unique_ptr<next::sdk::types::IPackage>> NextCoreLibReader::GetPackages() {
  if (!impl->replay_ptr_) {
    return {};
  }

  auto packages_corelib = impl->replay_ptr_->read();
  std::vector<std::unique_ptr<next::sdk::types::IPackage>> return_packages{};
  for (size_t i_package = 0; i_package < packages_corelib.size(); i_package++) {
    auto package = std::make_unique<CoreLibPackage>(std::move(packages_corelib[i_package]), impl->hash_to_size_map_,
                                                    impl->hash_to_next_vaddr_map_, impl->ethernet_service_info_,
                                                    impl->processor_info_);
    if (package->PackageValid()) {
      return_packages.push_back(std::move(package));
    }
  }
  return return_packages;
}

bool NextCoreLibReader::SetCurrentPackageNames(const std::vector<std::string> &package_name_list,
                                               std::vector<std::string> &additional_binary_package_names) {
  debug(__FILE__, "package name list {0}, return list size {1}", package_name_list.size(),
        additional_binary_package_names.size());
  return true;
}

uint64_t NextCoreLibReader::GetCurrentTimestamp() {
  if (!impl->replay_ptr_) {
    return 0;
  }
  return impl->replay_ptr_->get_current_timestamp();
}

bool NextCoreLibReader::CheckEndOfFile() {
  if (!impl->replay_ptr_) {
    return false;
  }
  return impl->replay_ptr_->eof();
}

bool NextCoreLibReader::JumpToTimestamp(uint64_t target_timestamp) {
  if (!impl->replay_ptr_) {
    return false;
  }
  impl->replay_ptr_->jump_to_timestamp(target_timestamp);
  return true;
}

std::shared_ptr<ReaderInterface> NextCoreLibReader::CreateNewInstance() {
  return std::shared_ptr<ReaderInterface>(new NextCoreLibReader());
}

bool NextCoreLibReader::SetFilter(std::string filter_expression) {
  if (!impl->replay_ptr_) {
    return false;
  } else {
    impl->replay_ptr_->set_filter_expression(filter_expression);
    return true;
  }
}

bool NextCoreLibReader::SupportsFilter() { return true; }

std::vector<ReaderInterface::CycleInformation> NextCoreLibReader::GetCycleDescription() {
  // TODO implement
  return {};
  // when testing, following data can be used, it is for our CEM test case
#if 0
  return {{100, "ADCU_DIAG", "CycleID 100: ADCU_DIAG"},
          {120, "ADCU_R7", "CycleID 120: ADCU_R7"},
          {130, "CEM_data", "CycleID 120: CEM_data"},
          {140, "ADCU_A57", "CycleID 140: ADCU_A547"},
          {205, "MEDIC", "CycleID 205: MEDIC"},
          {207, "AlgoVehCycle", "CycleID 207: AlgoVehCycle"},
          {208, "ACDC", "CycleID 208: ACDC"}};
#endif
}

} // namespace corelibreader
} // namespace plugin
} // namespace player
} // namespace next

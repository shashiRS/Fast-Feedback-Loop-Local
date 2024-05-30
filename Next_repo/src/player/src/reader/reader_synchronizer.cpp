/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     reader_synchronizer.cpp
 * @brief    Manages to load multiple files in parallel to merge and synchronize them.
 *
 **/

#include "reader_synchronizer.hpp"
#include <boost/algorithm/string.hpp>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace player {

ReaderSynchronizer::ReaderSynchronizer() {
  // initialize values
  fileStats.start_timestamp = UINT64_MAX;
  fileStats.end_timestamp = 0x00;
  fileStats.file_size = 0x00;

  supportsFilter = false;
}

ReaderSynchronizer::~ReaderSynchronizer() {}

std::set<std::string> ReaderSynchronizer::GetSupportedFileTypes() {
  std::set<std::string> ret = std::set<std::string>();

  // supports all available file types
  ret = next::player::ReaderManager::GetInstance()->GetSupportedFileTypes();

  return ret;
}

/*!
 * brief: Resets all reader
 */
bool ReaderSynchronizer::Reset() {
  bool ret = true;
  for (std::shared_ptr<next::player::plugin::ReaderInterface> readerPlugin : readers) {
    bool individualRet = readerPlugin->Reset();

    if (!individualRet) {
      next::sdk::logger::error("Reader synchronizer", "Resetting individual reader failed.");
      ret = false;
    }
  }

  // reset filestats
  fileStats.start_timestamp = UINT64_MAX;
  fileStats.end_timestamp = 0U;
  fileStats.file_size = 0U;

  supportsFilter = false;

  currentTimestamp = 0U;

  data_descriptions.clear();

  cycle_descriptions.clear();

  // is .clear() enough? Or should I delete the instances?
  readers.clear();

  // is .clear() enough? Or should I clear individual package lists inside of this map as well?
  packagesPerReader.clear();

  return ret;
}

bool ReaderSynchronizer::initWithReaders() {
  static bool checkForDuplicateDeviceNames = false;

  bool ret = true;

  // collect infos from all the readers
  // deviceNames used to verify the data uniqueness from different recordings
  std::vector<std::string> deviceNames = {};
  for (std::shared_ptr<next::player::plugin::ReaderInterface> individualReader : readers) {
    // file stats
    next::player::plugin::FileStats individualFileStats = individualReader->GetFileStats();
    if (individualFileStats.start_timestamp < fileStats.start_timestamp) {
      fileStats.start_timestamp = individualFileStats.start_timestamp;
    }
    if (individualFileStats.end_timestamp > fileStats.end_timestamp) {
      fileStats.end_timestamp = individualFileStats.end_timestamp;
    }
    fileStats.file_size += individualFileStats.file_size;

    // supportsFilter
    supportsFilter |= individualReader->SupportsFilter();

    std::vector<next::sdk::types::DataDescription> individualDataDescriptions =
        individualReader->GetCurrentDataDescriptions();
    if (checkForDuplicateDeviceNames) {
      // validate device names - there should be no duplicates
      for (next::sdk::types::DataDescription individualDataDescription : individualDataDescriptions) {
        std::vector<std::string>::iterator findResult =
            std::find(deviceNames.begin(), deviceNames.end(), individualDataDescription.device_name);
        if (findResult != deviceNames.end()) {
          // device name already does exist
          next::sdk::logger::error("Reader synchronizer",
                                   "Device name '{}' is already provided by another recording. Using "
                                   "this combination of recording files is not allowed.",
                                   individualDataDescription.device_name);

          // internal reset
          this->Reset();
          return false;
        }
      }
    }

    // add all data descriptions
    data_descriptions.insert(data_descriptions.end(), individualDataDescriptions.begin(),
                             individualDataDescriptions.end());
    for (next::sdk::types::DataDescription individualDataDescription : individualDataDescriptions) {
      std::string deviceName = individualDataDescription.device_name;
      deviceNames.insert(deviceNames.end(), deviceName);
    }

    // ask for cycle descriptions
    std::vector<next::player::plugin::ReaderInterface::CycleInformation> individualCycleDescriptions =
        individualReader->GetCycleDescription();
    if (!individualCycleDescriptions.empty()) {
      cycle_descriptions.insert(cycle_descriptions.end(), individualCycleDescriptions.begin(),
                                individualCycleDescriptions.end());
    }
  }

  // initialize the current timestamp after opening the files
  ret = this->JumpToTimestamp(this->fileStats.start_timestamp);

  return ret;
}

bool ReaderSynchronizer::OpenFile(const std::vector<std::string> &file_paths,
                                  const next::player::plugin::LoadingProgressCallback loadingProgressCallback) {
  using namespace std::chrono_literals;

  if (file_paths.size() == 0) {
    next::sdk::logger::error("Reader synchronizer", "Internal error! OpenFile() called without any file!");
    return false;
  }

  // check validity of the file paths
  for (std::string file_path : file_paths) {
    if (file_path.empty()) {
      next::sdk::logger::error("Reader synchronizer", "Internal error! OpenFile() called with empty path!");
      return false;
    }
  }

  // e.g. with .hdf5 we have a lot of files
  if (file_paths.size() > 20) {
    // it might work with more but never has been tested if it really works
    next::sdk::logger::error("Reader synchronizer",
                             "Opening {0} files at once is not allowed. Too much for now. Please "
                             "create ticket in order to get your use case supported.",
                             file_paths.size());
    return false;
  }

  bool ret = true;

  // go through all files and create FileReaders for each of them
  for (std::string file_path : file_paths) {
    std::shared_ptr<next::player::plugin::ReaderInterface> individual_reader =
        next::player::ReaderManager::GetInstance()->GetReaderByFilePath(file_path);

    if (nullptr == individual_reader) {
      this->Reset();
      return false;
    }

    // and open the file
    std::vector<std::string> individual_file_paths = {file_path};
    ret = individual_reader->OpenFile(individual_file_paths, loadingProgressCallback);

    if (ret == false) {
      // file loading failed - error message should have been already provided
      this->Reset();
      return ret;
    }

    readers.insert(readers.end(), std::move(individual_reader));
  }

  ret = initWithReaders();

  return ret;
}

next::player::plugin::FileStats ReaderSynchronizer::GetFileStats() { return fileStats; }

std::vector<next::sdk::types::DataDescription> ReaderSynchronizer::GetCurrentDataDescriptions() {
  return data_descriptions;
}

std::vector<std::unique_ptr<next::sdk::types::IPackage>> ReaderSynchronizer::GetPackages() {
  // init list of packages per reader (if required)
  if (packagesPerReader.size() == 0) {
    for (std::shared_ptr<next::player::plugin::ReaderInterface> individualReader : readers) {
      // initialize directly with first packages from that reader
      packagesPerReader.insert(std::pair<std::shared_ptr<next::player::plugin::ReaderInterface>,
                                         std::vector<std::unique_ptr<next::sdk::types::IPackage>>>(
          individualReader, individualReader->GetPackages()));
    }
  }

  // read packages from all the readers
  for (std::shared_ptr<next::player::plugin::ReaderInterface> individualReader : readers) {

    // read new packages if current individual package list is empty
    if (packagesPerReader[individualReader].empty() == true) {
      if (individualReader->CheckEndOfFile() == false) {
        bool readerReturnedNullPointerPackage = false;
        std::vector<std::unique_ptr<next::sdk::types::IPackage>> newIndividualPackageList;

        newIndividualPackageList = individualReader->GetPackages();
        if (newIndividualPackageList.empty() == false) {
          while (newIndividualPackageList.size() > 0) {
            std::unique_ptr<next::sdk::types::IPackage> currentPackage = std::move(newIndividualPackageList.at(0));

            // only add in case it is not a nullpointer
            if (nullptr != currentPackage.get()) {
              packagesPerReader[individualReader].push_back(std::move(currentPackage));
            } else {
              next::sdk::logger::error("Reader synchronizer", "Internal error: File reader provided a NullPointer in "
                                                              "list of GetPackages(). Reader should be fixed.");

              readerReturnedNullPointerPackage = true;
            }

            // remove first element from new individual package list
            newIndividualPackageList.erase(newIndividualPackageList.begin());
          }
        }

        // fallback - add second call in case the first call contained nullptr elements
        // we do not fully support all errors (e.g. call GetPackages() again and again until we get valid output)
        // -> reader should be fixed instead!
        if (readerReturnedNullPointerPackage == true) {
          if (individualReader->CheckEndOfFile() == false) {
            newIndividualPackageList = individualReader->GetPackages();
            if (newIndividualPackageList.empty() == false) {
              while (newIndividualPackageList.size() > 0) {
                std::unique_ptr<next::sdk::types::IPackage> currentPackage = std::move(newIndividualPackageList.at(0));

                // only add in case it is not a nullpointer
                if (nullptr != currentPackage.get()) {
                  packagesPerReader[individualReader].push_back(std::move(currentPackage));
                } else {
                  next::sdk::logger::error("Reader synchronizer",
                                           "Internal error: File reader provided a NullPointer in "
                                           "list of GetPackages(). Reader should be fixed.");
                }

                // remove first element from new individual package list
                newIndividualPackageList.erase(newIndividualPackageList.begin());
              }
            }
          }
        }
      }
    }
  }
  // packages from all readers have been read (if available)

  // get package with lowest timestamp
  std::shared_ptr<next::player::plugin::ReaderInterface> readerWithPackageOfLowestTimestamp = nullptr;
  uint64_t lowestTimestamp = UINT64_MAX;
  for (std::shared_ptr<next::player::plugin::ReaderInterface> individualReader : readers) {
    // there is still a package available?
    if (packagesPerReader[individualReader].empty() == false) {
      next::sdk::types::IPackage *currentPackage = packagesPerReader[individualReader].at(0).get();

      uint64_t individualTimestamp = currentPackage->GetPackageHeader().timestamp;
      if (individualTimestamp < lowestTimestamp) {
        lowestTimestamp = individualTimestamp;
        readerWithPackageOfLowestTimestamp = individualReader;
      }
    }
  }
  if (readerWithPackageOfLowestTimestamp != nullptr) {
    std::unique_ptr<next::sdk::types::IPackage> packageWithLowestTimestamp;
    // get first element
    packageWithLowestTimestamp = std::move(packagesPerReader[readerWithPackageOfLowestTimestamp].at(0));

    // remove first element from individual package list
    packagesPerReader[readerWithPackageOfLowestTimestamp].erase(
        packagesPerReader[readerWithPackageOfLowestTimestamp].begin());

    // fill current timestamp according to which was the last package that will be returned
    currentTimestamp = lowestTimestamp;

    // return the package with lowest timestamp
    std::vector<std::unique_ptr<next::sdk::types::IPackage>> ret = {};
    ret.push_back(std::move(packageWithLowestTimestamp));
    return ret;
  } else {
    // we don't have any more package - EOF reached
    return {};
  }
}

bool ReaderSynchronizer::SetCurrentPackageNames(const std::vector<std::string> &package_name_list,
                                                std::vector<std::string> &additional_binary_package_names) {
  // call all readers - maybe they add something
  for (std::shared_ptr<next::player::plugin::ReaderInterface> readerPlugin : readers) {
    readerPlugin->SetCurrentPackageNames(package_name_list, additional_binary_package_names);
  }
  return true;
}

uint64_t ReaderSynchronizer::GetCurrentTimestamp() { return currentTimestamp; }

bool ReaderSynchronizer::CheckEndOfFile() {
  // call all readers - only end of file if all of them ended
  for (std::shared_ptr<next::player::plugin::ReaderInterface> individualReader : readers) {
    // there is still a package available?
    if (packagesPerReader[individualReader].empty() == false) {
      return false;
    }

    // maybe all packages have been sent but new ones do not have been requested yet
    bool individualEndOfFile = individualReader->CheckEndOfFile();
    if (false == individualEndOfFile) {
      return false;
    }
  }

  return true;
}

bool ReaderSynchronizer::JumpToTimestamp(uint64_t target_timestamp) {

  if (this->currentTimestamp == target_timestamp) {
    return true;
  }

  bool ret = true;
  for (std::shared_ptr<next::player::plugin::ReaderInterface> readerPlugin : readers) {
    uint64_t individualTargetTimestamp;
    next::player::plugin::FileStats individualFileStats = readerPlugin->GetFileStats();
    if (individualFileStats.start_timestamp > target_timestamp) {
      individualTargetTimestamp = individualFileStats.start_timestamp;
    } else if (individualFileStats.end_timestamp < target_timestamp) {
      individualTargetTimestamp = individualFileStats.end_timestamp;
    } else {
      individualTargetTimestamp = target_timestamp;
    }

    bool individualRet = readerPlugin->JumpToTimestamp(individualTargetTimestamp);
    // reset internal list of packages that are not yet passed to the NEXT framework (see GetPackages())
    if (packagesPerReader.find(readerPlugin) != packagesPerReader.end()) {
      packagesPerReader[readerPlugin].clear();
    }

    if (!individualRet) {
      next::sdk::logger::error("Reader synchronizer", "Jumping to timestamp in an individual reader failed.");
      ret = false;
    }
  }

  this->currentTimestamp = target_timestamp;

  return ret;
}

std::shared_ptr<next::player::plugin::ReaderInterface> ReaderSynchronizer::CreateNewInstance() {
  // not supported
  return nullptr;
}

bool ReaderSynchronizer::SetFilter(std::string filter_expression) {
  bool ret = true;
  if (supportsFilter) {
    // at least one of the readers supports filter
    for (std::shared_ptr<next::player::plugin::ReaderInterface> readerPlugin : readers) {
      if (readerPlugin->SupportsFilter()) {
        bool individualSetFilterReturn = readerPlugin->SetFilter(filter_expression);

        if (individualSetFilterReturn == false) {
          // No error log. This should already be done by the individual reader.
          ret = false;
        }
      }
    }
  }

  return ret;
}

bool ReaderSynchronizer::SupportsFilter() { return supportsFilter; }

std::vector<next::player::plugin::ReaderInterface::CycleInformation> ReaderSynchronizer::GetCycleDescription() {
  return cycle_descriptions;
}

} // namespace player
} // namespace next

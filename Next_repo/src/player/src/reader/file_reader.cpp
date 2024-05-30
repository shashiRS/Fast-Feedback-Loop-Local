/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */

#include "file_reader.hpp"
#include "reader_manager.hpp"

FileReader::FileReader() {
  // create the one and only instance of the reader synchronizer
  reader_synchronizer_ = std::shared_ptr<next::player::ReaderSynchronizer>(new next::player::ReaderSynchronizer());
}

bool FileReader::OpenFile(std::vector<std::string> filePaths) {
  if (filePaths.empty()) {
    return false;
  }
  if (load_recording_progress_callback_)
    load_recording_progress_callback_("FileReader", "Function [OpenFile] start", false, 0, 0);

  try {
    using namespace std::chrono_literals;
    if (load_recording_progress_callback_)
      load_recording_progress_callback_("FileReader::OpenFile",
                                        " Function [mts::runtime::offline::open_recording] start ", false, 0, 0);

    bool result = reader_synchronizer_->OpenFile(filePaths, [&](uint64_t, float progress_percent) {
      if (load_recording_progress_callback_)
        load_recording_progress_callback_("FileReader::OpenFile",
                                          " Function [mts::runtime::offline::open_recording] start ", false,
                                          (long unsigned int)progress_percent, 100);
    });
    if (result == false) {
      return result;
    }
  } catch (const std::exception &e) {
    error(__FILE__, "Exception thrown while opening file: {}", e.what());
    return false;
  }

  try {
    auto descriptions = reader_synchronizer_->GetCurrentDataDescriptions();
    resource_manager_ptr_->GetUdexService()->RegisterDataDescriptions(descriptions);
  } catch (const std::exception &e) {
    error(__FILE__, "Exception thrown while registering data sources: {}", e.what());
    return false;
  }

  try {
    auto cycles = reader_synchronizer_->GetCycleDescription();
    resource_manager_ptr_->GetCycleManager()->CreateCycleEventPublisher(cycles);
  } catch (const std::exception &e) {
    error(__FILE__, "Exception thrown while creating cycle event publishers: {}", e.what());
    return false;
  }

  reader_has_file_open_ = true;
  try {
    auto file_stats = reader_synchronizer_->GetFileStats();

    uint64_t first_ts = file_stats.start_timestamp;
    uint64_t last_ts = file_stats.end_timestamp; // some recording do not provide the last timestamp

    min_timestamp_ = first_ts;
    max_timestamp_ = last_ts;
    current_timestamp_ = first_ts;
    if (load_recording_progress_callback_)
      load_recording_progress_callback_("FileReader", "Function [OpenFile] end", false, 0, 0);
  } catch (const std::exception &e) {
    error(__FILE__, "Exception thrown while accessing recording file stats: {}", e.what());
    return false;
  }
  return true;
}

void FileReader::GetFileStats(uint64_t &minTimestamp, uint64_t &maxTimestamp, uint64_t &currentTimestamp) {
  minTimestamp = min_timestamp_;
  maxTimestamp = max_timestamp_;
  currentTimestamp = current_timestamp_;
}

void FileReader::StartReadAheadThread() {
  run_read_ahead_ = true;
  resource_manager_ptr_->GetDataBuffer()->startPush();
  // Start read ahead thread
  read_ahead_future_ =
      std::make_shared<std::future<void>>(std::async(std::launch::async, [this]() { ReadAheadThread(); }));
}

void FileReader::StopReadAheadThread() {
  run_read_ahead_ = false;
  if (read_ahead_future_->valid()) {

    try {
      read_ahead_future_->get(); // Block and wait
    } catch (const std::runtime_error &) {
      // Handle timeout here
      // ToDo check if this is ok
      std::cout << "Got timeout...";
    }
  }
}

bool FileReader::ReadAheadThread() {
  if (!reader_synchronizer_)
    return false;
  while (run_read_ahead_ && reader_has_file_open_) {
    if (reader_synchronizer_->CheckEndOfFile()) {
      resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
      break;
    }
    try {
      std::vector<std::unique_ptr<next::sdk::types::IPackage>> packages;
      while (run_read_ahead_ && reader_has_file_open_ && !reader_synchronizer_->CheckEndOfFile() && packages.empty()) {
        auto core_lib_package_list = reader_synchronizer_->GetPackages();
        resource_manager_ptr_->GetReplayStatistics()->package_count += core_lib_package_list.size();
        for (auto &core_lib_package : core_lib_package_list) {
          if (!run_read_ahead_ && !reader_has_file_open_)
            break;
          resource_manager_ptr_->GetDataBuffer()->pushBuffer(std::move(core_lib_package));
        }
      }
    } catch (std::exception &e) {
      if (std::string("Data not available.") != e.what()) {
        error(__FILE__, "Exception thrown while buffering recording packages: {}", e.what());
      }
    }
  }
  return true;
}

void FileReader::SetResourceManager(std::shared_ptr<IResourceManager> resourceManager) {
  resource_manager_ptr_ = resourceManager;
}

void FileReader::ResetRecording() {
  // close read ahead thread
  reader_has_file_open_ = false;
  if (read_ahead_future_->valid()) {
    try {
      read_ahead_future_->get(); // Block and wait
    } catch (const std::runtime_error &) {
      // Handle timeout here
      std::cout << "Got timeout...";
    }
  }
  if (reader_synchronizer_) {
    reader_synchronizer_->Reset();
  }
  resource_manager_ptr_->GetCycleManager()->Reset();
}

bool FileReader::JumpToTimestamp(uint64_t timestamp, std::string tsType) {
  bool retVal = true;
  if (reader_synchronizer_) {
    skip_timestamp_ = timestamp;
    timestamp_type_ = tsType;

    resource_manager_ptr_->GetDataBuffer()->stopPush();
    StopReadAheadThread();
    resource_manager_ptr_->GetDataBuffer()->clearBuffer();
    if (skip_timestamp_ == 0) // handle jump to start
      skip_timestamp_ = min_timestamp_;
    if (skip_timestamp_ == UINT64_MAX) // handle jump to end
      skip_timestamp_ = max_timestamp_;
    reader_synchronizer_->JumpToTimestamp(skip_timestamp_);
    current_timestamp_ = reader_synchronizer_->GetCurrentTimestamp();
    StartReadAheadThread();
  } else {
    retVal = false;
  }
  return retVal;
}

void FileReader::SetLoadRecordingInfoCallback(next::udex::statusCallbackT callback) {
  load_recording_progress_callback_ = callback;
}

bool FileReader::SupportsFilter() {
  if (reader_synchronizer_) {
    return reader_synchronizer_->SupportsFilter();
  }
  return false;
}

bool FileReader::SetFilterExpression(const std::string &filterExpression) {
  if (reader_synchronizer_) {
    try {
      reader_synchronizer_->SetFilter(filterExpression);
    } catch (std::exception &e) {
      error(__FILE__, "Exception thrown while setting filter expresion: {}", e.what());
    }
    return true;
  }
  return false;
}

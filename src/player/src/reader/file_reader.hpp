/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     file_reader.hpp
 * @brief    file reader wrapper
 *
 **/

#ifndef FILE_READER_
#define FILE_READER_

#include <atomic>
#include <memory>
#include <string>

#include <next/sdk/logger/logger.hpp>

#include <next/player/plugin/reader_interface.hpp>
#include "../resource_manager/resource_manager.hpp"
#include "reader_synchronizer.hpp"

class FileReader {
public:
  FileReader();
  bool OpenFile(std::vector<std::string> filePaths);
  void GetFileStats(uint64_t &minTimestamp, uint64_t &maxTimestamp, uint64_t &currentTimestamp);
  void SetResourceManager(std::shared_ptr<IResourceManager> resourceManager);
  void ResetRecording();
  bool JumpToTimestamp(uint64_t timestamp, std::string tsType);

  /*! @brief describes if setting filter expressions are supported.
   *
   * @return true if filters are supported, false otherwise
   */
  bool SupportsFilter();

  /*! @brief set filter expression to filter data URLs from being processed.
   *
   * This can be used to minimize the data packages to be sent.
   * With this the file reader could reduce processing time and
   * increase the overall runtime performance.
   *
   * @param filter_expression
   * @return true if successfull
   */
  bool SetFilterExpression(const std::string &filterExpression);

  void StartReadAheadThread();
  void SetLoadRecordingInfoCallback(next::udex::statusCallbackT callback);

private:
  void StopReadAheadThread();
  bool ReadAheadThread();

  std::shared_ptr<IResourceManager> resource_manager_ptr_;
  std::shared_ptr<next::player::ReaderSynchronizer> reader_synchronizer_;
  std::shared_ptr<std::future<void>> read_ahead_future_;

  uint64_t min_timestamp_ = 0;
  uint64_t max_timestamp_ = 0;
  uint64_t current_timestamp_ = 0;
  std::atomic_bool reader_has_file_open_ = false;
  std::atomic_bool run_read_ahead_ = false;
  uint64_t skip_timestamp_;
  std::string timestamp_type_;
  next::udex::statusCallbackT load_recording_progress_callback_ = nullptr;
};

#endif

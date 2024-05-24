/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     buffer_queue.hpp
 * @brief    Threadsafe buffer class implementataion
 *
 **/

#ifndef BUFFER_QUEUE_
#define BUFFER_QUEUE_

#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator_player.hpp>
#include <next/sdk/types/package_data_types.hpp>

#define MINIMUM_READ_AHEAD_BUFFER_TIME 500000
#define MAXIMUM_READ_AHEAD_BUFFER_TIME 20000000

#define MAX_READ_AHEAD_BUFFER_SIZE_MB 30 // Usually the Players total RAM consumption is ~115 times more

namespace next {
namespace player {

class BufferQueue {
private:
  mutable std::mutex synchronization_mutex_;                           //! Mutex for synchronisation
  std::queue<std::unique_ptr<next::sdk::types::IPackage>> data_queue_; //! Internal data queue
  std::condition_variable cv_pop_;                                     //! Condition variable to be used in pop
  std::condition_variable cvPush_;                                     //! Condition variable to be used in push
  uint64_t max_buffer_data_bytes_ = getDefaultBufferMaxBytes(); //! Track buffer memory usage to limit RAM consumption
  uint64_t buffered_data_byte_size_ = 0; //! Current data size in bytes (accumulated from all stored data_packages)
  bool shutdown_flag_ = false;           //! Will wake up sleeping threads on shutdown
  bool pause_flag_ = false;              //! Will wake up sleeping POP threads on Pause
  bool input_file_eof_ = false;          //! Flag to mark the end of the input file
  bool push_possible_flag_ = false;      //! Flag to control data input

  BufferQueue &operator=(const BufferQueue &) = delete;
  BufferQueue(const BufferQueue &other) = delete;

  //! Returns the time length of the data queue
  uint64_t getBufferTime() {
    if (data_queue_.empty()) {
      return 0;
    }
    uint64_t firstTs = data_queue_.front()->GetPackageHeader().timestamp;
    uint64_t lastTs = data_queue_.back()->GetPackageHeader().timestamp;
    return lastTs - firstTs;
  }

  uint64_t convertMBtoBytes(int megabytes) {
    if (megabytes < 0) {
      return 0;
    }
    constexpr int multiplicator = 1024 * 1024;
    return static_cast<uint64_t>(megabytes) * multiplicator;
  }

  uint64_t getDefaultBufferMaxBytes() { return convertMBtoBytes(MAX_READ_AHEAD_BUFFER_SIZE_MB); }

  size_t sizeofIPackage() {
    static size_t internal_package_size = sizeof(next::sdk::types::IPackage);
    return internal_package_size;
  }

  void UpdateBufferMaxSizeFromConfig() {
    int max_read_ahead = 0;
    auto config_client = next::appsupport::ConfigClient::getConfig();
    if (config_client) {
      max_read_ahead = config_client->get_int(next::appsupport::configkey::player::getBufferMaxReadAheadKey(),
                                              MAX_READ_AHEAD_BUFFER_SIZE_MB);
    }
    if (max_read_ahead > 0) {
      max_buffer_data_bytes_ = convertMBtoBytes(max_read_ahead);
    } else {
      max_buffer_data_bytes_ = getDefaultBufferMaxBytes();
    }
  }

public:
  BufferQueue() = default;

  //! Push value into the Queue
  /*!
   * @param value value to be pushed
   */
  void pushBuffer(std::unique_ptr<sdk::types::IPackage> value) {
    std::unique_lock<std::mutex> lock(synchronization_mutex_);
    cvPush_.wait(lock, [&] {
      bool retVal = false;
      if (shutdown_flag_) {
        retVal = true;
      }
      if (!push_possible_flag_) {
        retVal = true;
      }
      if ((buffered_data_byte_size_ < max_buffer_data_bytes_) && (getBufferTime() < MAXIMUM_READ_AHEAD_BUFFER_TIME)) {
        retVal = true;
      }

      return retVal;
    });
    if (push_possible_flag_ && !shutdown_flag_) {
      buffered_data_byte_size_ += (value->GetSize() + sizeofIPackage()); // approximate memory usage to have a reference
      data_queue_.push(std::move(value));
      cv_pop_.notify_one();
    }
  }

  //! Returns the first queue element and pops it from the queue
  std::unique_ptr<sdk::types::IPackage> popBuffer(bool &eof) {
    std::unique_lock<std::mutex> lock(synchronization_mutex_);
    cv_pop_.wait(lock, [this] {
      bool retVal = false;
      if (shutdown_flag_)
        retVal = true;
      if (!data_queue_.empty())
        retVal = true;
      if (input_file_eof_)
        retVal = true;
      if (pause_flag_)
        retVal = true;
      return retVal;
    });
    if (!data_queue_.empty()) {
      buffered_data_byte_size_ -= (data_queue_.front()->GetSize() + sizeofIPackage());
      auto tmp = std::move(data_queue_.front());
      data_queue_.pop();
      cvPush_.notify_one();
      eof = false;
      return tmp;
    } else {
      eof = input_file_eof_;
      return {};
    }
  }

  //! Removes all queue elements and resets data size
  void clearBuffer() {
    std::unique_lock<std::mutex> lock(synchronization_mutex_);
    while (!data_queue_.empty())
      data_queue_.pop();
    buffered_data_byte_size_ = 0;
    input_file_eof_ = false;
    shutdown_flag_ = false;
  }

  //! Notify that push is possible
  void startPush() {
    UpdateBufferMaxSizeFromConfig();
    push_possible_flag_ = true;
    cvPush_.notify_one();
  }

  //! disables pushing
  void stopPush() {
    push_possible_flag_ = false;
    cvPush_.notify_one();
  }

  //! Check if queue is empty
  bool empty() const {
    std::lock_guard<std::mutex> lock(synchronization_mutex_);
    return data_queue_.empty();
  }

  //! Return the size of the queue
  size_t size() const {
    std::lock_guard<std::mutex> lock(synchronization_mutex_);
    return data_queue_.size();
  }

  //! Interrupt the wait of all sleeping threads.
  void shutdown() {
    std::unique_lock<std::mutex> lck(synchronization_mutex_);
    shutdown_flag_ = true;
    while (!data_queue_.empty())
      data_queue_.pop();
    buffered_data_byte_size_ = 0;
    cvPush_.notify_all();
    cv_pop_.notify_all();
  }

  //! Interrupt the wait of all sleeping threads.
  void setEndOfFileFlag() {
    std::unique_lock<std::mutex> lck(synchronization_mutex_);
    input_file_eof_ = true;
    stopPush();
    cv_pop_.notify_all();
  }

  bool getEndOfFileFlag() {
    std::unique_lock<std::mutex> lck(synchronization_mutex_);
    return input_file_eof_;
  }
  //! Interrupt the wait of POP sleeping thread.
  void SignalPause() {
    std::unique_lock<std::mutex> lck(synchronization_mutex_);
    pause_flag_ = true;
    cv_pop_.notify_all();
  }
  void SignalPlay() {
    std::unique_lock<std::mutex> lck(synchronization_mutex_);
    pause_flag_ = false;
    cv_pop_.notify_all();
  }
};

} // namespace player
} // namespace next

#endif // BUFFER_QUEUE_

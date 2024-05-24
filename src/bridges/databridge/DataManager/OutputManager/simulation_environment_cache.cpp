/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     simulation_environment_cache.cpp
 *  @brief    see simulation_environment_cache.h
 */

#include "simulation_environment_cache.h"
#include <next/sdk/sdk.hpp>

namespace next {
namespace databridge {

SimulationEnvironmentCache::SimulationEnvironmentCache() {
  flush_thread = std::move(std::thread(&SimulationEnvironmentCache::FlushThread, this));
}

SimulationEnvironmentCache::~SimulationEnvironmentCache() {
  shutdown_ = true;
  if (flush_thread.joinable()) {
    flush_thread.join();
  }
}

void SimulationEnvironmentCache::FlushThread() {
  while (!shutdown_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(update_rate_ms_));
    if (start_flush_) {
      flush();
    }
  }
}

bool SimulationEnvironmentCache::setUpdateRateMilliseconds(unsigned int update_time) {
  update_rate_ms_ = update_time;
  return true;
}

void SimulationEnvironmentCache::handoverDataBroadcaster(
    std::shared_ptr<next::databridge::data_broadcaster::IDataBroadcaster> data_broadcaster) {
  data_broadcaster_ = data_broadcaster;
}

bool SimulationEnvironmentCache::addPackage(const void *package_binary, size_t package_size,
                                            const std::vector<size_t> &session_ids, const bool force_no_cache,
                                            const std::chrono::milliseconds timeout, const bool binary) {
  if (force_no_cache) {
    publishData(reinterpret_cast<const uint8_t *>(package_binary), package_size, session_ids, timeout, binary);
  } else {
    // TODO the cache_ variable access was not thread safe, mutex added, improve the solution, see SIMEN-4584
    {
      std::lock_guard lck(cache_mtx_);
      cache_.push_back(CacheData(std::string((char *)package_binary, package_size), session_ids, binary));
    }
    if (cache_.size() >= max_packages_) {
      flush();
    }
  }

  return true;
}

void SimulationEnvironmentCache::AddCharacterToBuffer(char character_to_add, uint8_t *merged_msg,
                                                      size_t &offset) const {
  merged_msg[offset] = (uint8_t)character_to_add;
  offset += sizeof(char);
}

bool SimulationEnvironmentCache::flush(void) {
  std::lock_guard lck(cache_mtx_);

  if (cache_.size() == 0) {
    return true;
  }

  // flush single message
  if (cache_.size() == 1) {
    publishData(reinterpret_cast<const uint8_t *>(cache_.front().data.c_str()), cache_.front().data.size(),
                cache_.front().session_ids, std::chrono::milliseconds(500), cache_.front().binary);
    cache_.clear();
    return true;
  }
  auto cache_it = cache_.begin();
  while (cache_it != cache_.end()) {
    auto front = cache_.front();
    if (front.binary == true) {
      publishData(reinterpret_cast<const uint8_t *>(cache_.front().data.c_str()), cache_.front().data.size(),
                  cache_.front().session_ids, std::chrono::milliseconds(500), cache_.front().binary);
      cache_.pop_front();
    }
    cache_it++;
  }

  bool success = true;
  while (cache_.size() > 0) {
    success &= SelectAndPublishMatchingCacheElements();
  }
  return success;
}

bool SimulationEnvironmentCache::SelectAndPublishMatchingCacheElements() {
  std::list<CacheData> matching_cache;
  //  CacheData reference = cache_.front();
  //  for (auto &msg : cache_) {
  //    if (msg.binary != reference.binary) {
  //      break;
  //    }
  //    if (msg.session_ids != reference.session_ids) {
  //      break;
  //    }
  //    matching_cache.push_back(msg);
  //  }
  //  for (auto &msg : matching_cache) {
  //    cache_.remove(msg);
  //  }

  if (!PublishConsistentCache(cache_)) {
    next::sdk::logger::error(__FILE__, "issue with sending cache. size {0}, first message: {1}", cache_.size(),
                             cache_.front().data);
    return false;
  }
  cache_.clear();
  return true;
}

bool SimulationEnvironmentCache::PublishConsistentCache(std::list<CacheData> &cache_temp) {
  size_t msg_size = ComputeMessageSizes(cache_temp);

  uint8_t *merged_msg = (uint8_t *)malloc(msg_size);
  if (merged_msg == nullptr) {
    return false;
  }
  size_t offset = 0;

  AddCharacterToBuffer('[', merged_msg, offset);
  for (auto &msg : cache_temp) {
    CopyCacheToMessage(msg, ',', merged_msg, offset);
  }
  // now replace the last comma with a closing backet -> reduces offset by 1;
  offset -= 1;
  AddCharacterToBuffer(']', merged_msg, offset);

  if (offset > msg_size) {
    sdk::logger::error(__FILE__, "msg size wrong");
    free(merged_msg);
    return false;
  }

  publishData(merged_msg, msg_size, cache_temp.front().session_ids, std::chrono::milliseconds(500),
              cache_temp.front().binary);

  free(merged_msg);
  cache_temp.clear();
  return true;
}

void SimulationEnvironmentCache::CopyCacheToMessage(const CacheData &msg, char character_to_add, uint8_t *merged_msg,
                                                    size_t &offset) const {
  std::copy_n(msg.data.c_str(), msg.data.size(), &(merged_msg[offset]));
  offset += msg.data.size();
  AddCharacterToBuffer(character_to_add, merged_msg, offset);
}

void SimulationEnvironmentCache::SetMaxPackages(size_t max_packages) { max_packages_ = max_packages; }

size_t SimulationEnvironmentCache::ComputeMessageSizes(std::list<CacheData> &cache_temp) {
  size_t msg_size;
  const size_t k_starting_bracket_size = 1;
  const size_t k_ending_bracket_size = 1;
  const size_t k_separating_comma_size = 1;
  msg_size = k_starting_bracket_size; // starting char
  for (auto &msg : cache_temp) {
    msg_size += msg.data.size() + k_separating_comma_size;
  }
  // replace the last comma with a closing bracket
  msg_size -= k_separating_comma_size;
  msg_size += k_ending_bracket_size;
  return msg_size;
}

void SimulationEnvironmentCache::publishData(const uint8_t *payload, const size_t size,
                                             const std::vector<size_t> &session_ids,
                                             const std::chrono::milliseconds timeout, bool binary) {
  try {
    // request memory
    auto memory_future = data_broadcaster_->RequestNewMemory();

    using namespace std::chrono_literals;

    if (!memory_future.valid() || (memory_future.wait_for(timeout) == std::future_status::timeout)) {
    } else {
      auto memory = memory_future.get();
      memory->data_.resize(size);
      std::copy(payload, payload + size, memory->data_.begin());
      data_broadcaster_->SendDataToSocketManager(memory->id_, memory->data_, session_ids, binary);
    }
  } catch (std::future_error &) {
  } catch (...) {
  }
}

} // namespace databridge
} // namespace next

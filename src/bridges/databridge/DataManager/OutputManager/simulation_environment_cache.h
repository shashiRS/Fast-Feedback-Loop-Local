/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     simulation_environment_cache.h
 * @brief    Collects and publishes multiple data packages to the GUI
 */

#ifndef NEXT_DATAMANAGER_SIMULATION_ENVIRONMENT_CACHE_H
#define NEXT_DATAMANAGER_SIMULATION_ENVIRONMENT_CACHE_H

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "data_broadcaster.hpp"

namespace next {
namespace databridge {

class SimulationEnvironmentCache {
public:
  struct CacheMetaInfo {
    long long timestamp;
    std::string data_type;
    std::string cache_name;
    std::string view_name;
  };

  /*!
   * @brief Construct a new Simulation Environment Cache object
   *
   */
  SimulationEnvironmentCache();
  /*!
   * @brief Destroy the Simulation Environment Cache object
   *
   */
  virtual ~SimulationEnvironmentCache();

  /*!
   * @brief Set the Sending rate
   *
   * @param update_time Timeout after which the cache shall be send (milliseconds)
   * @return true setting succesful
   * @return false issue during setting
   */
  bool setUpdateRateMilliseconds(unsigned int update_time);

  /*!
   * @brief Share the Data Broadcaster to be able to send data to GUI
   *
   * @param data_broadcaster
   */
  void handoverDataBroadcaster(std::shared_ptr<next::databridge::data_broadcaster::IDataBroadcaster> data_broadcaster);

  /*!
   * @brief Adds one package to the internal cache
   *
   * @param package_binary package data that shall be added
   * @param package_size Size of package that shall be added
   * @param meta_info additional information about the package
   * @return true successfully added package to cache
   * @return false issue while adding package to cache
   */
  bool addPackage(const void *package_binary, size_t package_size, const std::vector<size_t> &session_ids,
                  const bool force_no_cache, const std::chrono::milliseconds timeout, const bool binary);

  /*!
   * @brief Sends all packages from the cache. Cleans cache afterwards
   *
   * @return true
   * @return false
   */
  bool flush(void);

  /*!
   * @brief Set the maximum cache elements before a flush is called
   * @param max_packages
   */
  void SetMaxPackages(size_t max_packages);

private:
  /*!
   * @brief publishes the given data via the data-broadcaster
   *
   * @param payload Message that shall be send
   * @param size Size of message that shall be send
   * @param session_ids
   * @param timeout
   */
  virtual void publishData(const uint8_t *payload, const size_t size, const std::vector<size_t> &session_ids,
                           const std::chrono::milliseconds timeout, const bool binary);

protected:
  struct CacheData {
    CacheData(const std::string &d, const std::vector<size_t> &ids, const bool b)
        : data(d), session_ids(ids), binary(b) {}
    bool operator==(const CacheData &a) {
      return a.binary == binary && a.session_ids == session_ids && a.data == a.data;
    }
    const std::string data;
    const std::vector<size_t> session_ids;
    const bool binary;
  };

  unsigned int update_rate_ms_ = 200;
  std::list<CacheData> cache_;
  size_t max_packages_ = 100;
  std::mutex cache_mtx_;

  void StartFlushThread();

  std::thread flush_thread;
  std::atomic<bool> shutdown_ = false;
  std::atomic<bool> start_flush_ = true;

  void FlushThread();

private:
  std::shared_ptr<next::databridge::data_broadcaster::IDataBroadcaster> data_broadcaster_;

  size_t ComputeMessageSizes(std::list<CacheData> &cache_temp);
  void AddCharacterToBuffer(char character_to_add, uint8_t *merged_msg, size_t &offset) const;
  void CopyCacheToMessage(const CacheData &msg, char character_to_add, uint8_t *merged_msg, size_t &offset) const;
  bool PublishConsistentCache(std::list<CacheData> &cache_temp);
  bool SelectAndPublishMatchingCacheElements();
};

} // namespace databridge
} // namespace next

#endif // NEXT_DATAMANAGER_SIMULATION_ENVIRONMENT_CACHE_H

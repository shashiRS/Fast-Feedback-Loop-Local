/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     signal_watcher.h
 * @brief    taps into distributed topic system to extract description data and buffer unregistered urls
 */

#ifndef NEXT_UDEX_SIGNAL_WATCHER_H
#define NEXT_UDEX_SIGNAL_WATCHER_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include <boost/asio/thread_pool.hpp>

#include <ecal/ecal.h>
#include <ecal/ecal_util.h>

#include <next/sdk/events/update_topic_cache.h>
#include <next/sdk/profiler/profile_tags.hpp>

#include <next/udex/data_types.hpp>

#include "signal_description_handler.h"
#include "signal_storage_types.h"
#include "signal_tree_module.h"

#include "url_helper.hpp"

namespace next {
namespace udex {
namespace ecal_util {

struct TopicInfo {
  uint32_t tick = 0;
  std::string type;
  std::string url;
};

struct DataDescription {
  SignalDescriptionHandler desc_handler;
  bool desc_handler_ready = false;
  SignalTree signal_tree;
  bool signal_tree_ready = false;
};

class SignalWatcher {
public:
  enum UrlLevel {
    DEVICE_LEVEL = 0,
    VIEW_LEVEL = 1,
    GROUP_LEVEL = 2,
  };

  typedef std::function<void(std::string topic_name, SignalDescriptionHandler topic_description)> registration_callback;

  //! Disabled SignalWatcher copy constructor
  SignalWatcher(SignalWatcher &other) = delete;
  //! Disabled copy assignment
  void operator=(const SignalWatcher &) = delete;
  //! default destructor
  virtual ~SignalWatcher();

  //! check and report immediate result
  /*!
   *@param url url to be check for
   *@return true if url is available
   */
  bool checkUrlAvailable(const std::string &url);

  //! get the description handler of url from the internal list
  /*!
   * @param url  url of which the description to fetch
   * @param available update whether the url is available or not in the internal list
   * @return return the description if the requested url description is available
   */
  SignalDescriptionHandler getTopicDescriptionHandler(const std::string &url, bool &available);

  //! get the topic name of url from the internal list
  /*!
   * @param url url of which the description to fetch
   * @param available return whether the url is available or not in the internal list
   * @return return the topic name if the requested url topic name is available
   */
  std::string getTopicName(const std::string &url, bool &available);

  //! callback based reporting mechanism for the url from the ecal monitoring
  /*!
   * @param url  url of which the description to fetch
   * @param call_back callback function to be called whenever the url is available
   * @return unique requested id for the callback registered,
   * this id should be use to track and remove the callbacks(All users of SignalWatcher must use this id )
   */
  size_t requestUrl(const std::string &url, const registration_callback call_back);

  //! Remove callbacks functions for urls based on the unique id obtained during the registration time
  /*!
   * @param myRequestedIds  request ids of callbacks to be removed
   * @return true
   */
  bool removeRequests(const std::vector<size_t> &myRequestedIds);

  //! Get all current rooturls in a list from the internal list
  /*!
   * @param url  url of which the description to fetch
   * @return list of root urls and associated topic names
   */
  std::vector<ChildInfo> GetRootUrlList(UrlLevel level, const std::string &parent_url);

  //! Get signal watcher instance , return instance if created already else create new
  static SignalWatcher &GetInstance();

  //! get the signal description of url from the internal list
  /*!
   * @param url  url of which the description to fetch
   * @param available update whether the url is available or not in the internal list
   * @return return the signal description if the requested url description is available
   */
  SignalDescription getSignalDescription(const std::string &url, bool &available);

  //! generate and return sdl for a given signal description
  /*
   * @parma description
   */
  std::pair<bool, std::string> generateSdl(const std::string topic);

  //! serches through all the signals for the keyword
  /*!
   * @param keyword the keyword used in the serch
   * @return return a vector of strings with the found signals that contain the keyword
   */
  std::vector<std::string> searchSignalTree(const std::string &keyword);

  std::vector<std::string> getTopicByUrl(std::string signal_url);
  std::vector<ChildInfo> getChildrenByUrl(std::string signal_url);
  std::unordered_map<std::string, std::vector<std::string>> GetFullUrlTree();
  std::vector<std::string> getDevicesByFormat(std::string format);
  std::vector<std::string> getDevicesByFormat(next::sdk::types::PackageFormatType format_type);

  //! clears all the cache data stops the fill Topic description thread
  void reset();

  //! sets the maximum number allowed ticks until a topic becames stale
  /*!
   * @param tick_count the maximum of allowed ticks that will be set
   */
  void setMaxTickCount(uint32_t tick_count);

  //! get the signal description of url from the internal list
  /*!
   * @param url url of which the signaltree
   * @param available update whether the url is available or not in the internal list
   * @return return the SignalTree if the requested url description is available
   */
  SignalTree getSignalTree(const std::string &url, bool &available);

  //! enable fast search
  /*!
   * will force the creation of SignalTree and Signal Description as soon as topics are available
   */
  void EnableFastSearch();

protected:
  //! Delete the singleton instance
  static void DeleteInstance();

  //! Create SignalWatcher object
  SignalWatcher();

  //! Erase the callbacks based on unique id
  void eraseCallbacks(std::vector<std::pair<registration_callback, size_t>> &callback, size_t id) const;

  //! find and invoke the callbacks for the url if it is already registered, remove the callback once it invoked
  void InvokeCallback(const std::string &url);

  //! list of requested callbacks for each url, along with unique id
  std::unordered_map<std::string, std::vector<std::pair<registration_callback, size_t>>> requested_urls;

private:
  void updateTopicCacheOnEvent(const next::sdk::events::UpdateTopicCacheMessage &msg);

  void removeTopicsFromCache(const std::vector<std::string> &topics_to_erase);

  //! Method called by the monitoring thread to fill cache data inside the SignalWatcher
  void FillTopicCache();

  //! Initialize the internal list of topics
  void InitTopicCacheFromECAL();

  void ParseAndInvokeCallback(const std::vector<std::string> &new_topics,
                              const std::unordered_map<std::string, eCAL::SDataTypeInformation> &topic_info_available);

  //! Fill the internal list of topics by monitoring the ecal (monitoring thread function)
  void FillTopicCacheFromECAL(int filter_resolution_cnt, std::vector<std::string> &new_topics,
                              std::unordered_map<std::string, eCAL::SDataTypeInformation> &topic_info_available);

  void WaitAndFilterGetTopicsFromEcal(const int min_same_result_filter_cnt,
                                      std::unordered_map<std::string, eCAL::SDataTypeInformation> &topic_to_info_map);

  bool InitializeDescriptionHandler(const std::string &url);

  bool InitializeDescriptionSignalTree(const std::string &url);

  //! Create thread to monitor the ecal
  void MonitoringThread();

  SignalTree createSignalTreeElement(const std::string &schema, const std::string &topic);

  //! fill a topic to the caches for matching urls and topics
  void addTopicToCaches(const std::string &signal_url, std::string &topic_from_url);

  //! fill a list of child infos from a url list
  void fillChildListFromUrlList(std::multimap<std::string, std::string> &url_list,
                                std::vector<ChildInfo> &child_list) const;

  //! create a list of urls from a parent_url. Define the level.
  void FillUrlListFromTopics(UrlLevel level, const std::string &parent_url,
                             std::multimap<std::string, std::string> &url_list);

  //! add a signaltree from the futures safely with fallbacks
  void addSignalTreeSafelyFromFuture(std::unordered_map<std::string, std::future<SignalTree>> &futures,
                                     const std::string &topic_name, const SignalDescriptionHandler &handler,
                                     const std::string &url);

  //! finds stale topics and removes them from caches
  void RemoveStaleTopics();

  // unordered_map (root_url, {topic_name, topic_descriptoin})
  // root_url get created with signal_description_hanlder out of the description
  std::unordered_map<std::string, std::pair<std::string, DataDescription>> topic_description_info_;
  std::recursive_mutex topic_info_mutex_;
  std::recursive_mutex callback_info_mutex;
  std::atomic<bool> thread_join = false;
  std::atomic<long> background_update_rate_ = 500000;
  std::thread topic_info_thread;
  std::atomic<bool> shutdown_ = false;
  std::atomic<bool> update_requested_ = false;

  // topic information cache
  std::unordered_map<std::string, TopicInfo> topic_info_cache_;  // cache which holds information for each topic
  std::unordered_map<std::string, std::string> url_topic_cache_; // cache which holds each url and the appropiate topic
  std::unordered_map<std::string, std::vector<std::string>>
      topic_url_list_; // cache which holds for every topic the url list

  std::atomic<bool> first_signal_search_ = true;
  boost::asio::thread_pool pool_;
  static SignalWatcher *signal_watcher_;
  static std::mutex mtx_instance_;

  // conditional variable used to pause the creation thread of the SignalWatcher until the cache is filled
  std::condition_variable fill_initial_cache_;
  std::mutex initial_fill_cahce_mutex_;
  // maximum accepted ticks until topic becomes stale
  uint32_t max_accepted_ticks_ = 40;

  next::sdk::events::UpdateTopicCache update_topic_cache_event_{"update topic cache event"};

  std::condition_variable update_cache_cv_;
  std::mutex update_cache_mtx_;

  std::atomic<int64_t> update_cache_event_time_;
  std::atomic<int> scan_ms_after_update_{3000};
  bool fast_search_enabled_ = false;
};

} // namespace ecal_util
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SIGNAL_WATCHER_H

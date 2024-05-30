/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "signal_watcher.h"
#include "signal_tree_module.h"

#include <cctype>
#include <chrono>
#include <regex>

#include <boost/asio.hpp>

#include <json/json.h>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/init_chain/simple_init_chain.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/sdk/profiler/profiler.hpp>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/introspection/conversion.hpp>
#include <mts/introspection/xtypes/generator/sdl.hpp>
#include <mts/introspection/xtypes/parser/json.hpp>
#include <mts/runtime/offline/reader_proxy.hpp>
NEXT_RESTORE_WARNINGS

using namespace std::chrono;

namespace next {
namespace udex {
namespace ecal_util {

constexpr const int kInitialUpdateLoopCounter = 15;
constexpr const int kUpdateLoopSleepMs = 50;
constexpr const int kUpdatePeriodAfterEvent = 3000;

SignalWatcher *SignalWatcher::signal_watcher_ = nullptr;
std::mutex SignalWatcher::mtx_instance_;

SignalWatcher &SignalWatcher::GetInstance() {
  ProfileUdex_FLL;
  std::lock_guard<std::mutex> lock(mtx_instance_);
  if (signal_watcher_ == nullptr) {
    signal_watcher_ = new SignalWatcher();
  }
  return *signal_watcher_;
}

void SignalWatcher::DeleteInstance() {
  std::lock_guard<std::mutex> lock(mtx_instance_);
  if (signal_watcher_ != nullptr) {
    delete signal_watcher_;
  }
  signal_watcher_ = nullptr;
}

SignalWatcher::SignalWatcher() : pool_() {
  ProfileUdex_FLL;

  static next::sdk::InitChain::El init_el(
      next::sdk::InitPriority::SIGNAL_WATCHER, [](const next::sdk::InitChain::ConfigMap &, int) { return true; },
      [this](const next::sdk::InitChain::ConfigMap &, int) { DeleteInstance(); });

  update_topic_cache_event_.addEventHook(
      [this]() { this->updateTopicCacheOnEvent(update_topic_cache_event_.getEventData()); });
  update_topic_cache_event_.subscribe();

  MonitoringThread();
  std::unique_lock<std::mutex> lock(initial_fill_cahce_mutex_);
  fill_initial_cache_.wait_for(lock, std::chrono::milliseconds(5000));

  auto config = next::appsupport::ConfigClient::getConfig();
  if (config) {
    background_update_rate_ = config->get_int(next::appsupport::configkey::getUdexUpdateRate("GENERIC"), 500000);
    scan_ms_after_update_ = config->get_int(next::appsupport::configkey::getUdexUpdatePeriodMs("GENERIC"), 3000);
  }
}

SignalWatcher::~SignalWatcher() {
  debug(__FILE__, "~SignalWatcher Start");
  ProfileUdex_FLL;
  reset();
  debug(__FILE__, "~SignalWatcher End");
}

void SignalWatcher::FillTopicCache() {
  ProfileUdex_FLL;
  while (eCAL::Ok()) {
    if (first_signal_search_) {
      first_signal_search_ = false;
      InitTopicCacheFromECAL();
      fill_initial_cache_.notify_one();
    }

    if (thread_join) {
      return;
    }

    std::unique_lock lk(update_cache_mtx_);
    if (!update_requested_) {
      update_cache_cv_.wait_for(lk, std::chrono::milliseconds(background_update_rate_));
    }

    update_requested_ = false;
    if (thread_join) {
      return;
    }

    std::vector<std::string> new_topics;
    std::unordered_map<std::string, eCAL::SDataTypeInformation> topic_info_available;
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
    {
      std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
      debug(__FILE__, "FillTopicCache() - Critical Section Start");
      const int filter_resolution_cnt = 25; // the higher the more stable but slower
      while (now < (update_cache_event_time_.load() + kUpdatePeriodAfterEvent)) {
        FillTopicCacheFromECAL(filter_resolution_cnt, new_topics, topic_info_available);
        if (thread_join) {
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateLoopSleepMs));
        now = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
        if (thread_join) {
          return;
        }
      }
      debug(__FILE__, "FillTopicCache() - Critical Section End");
    }
    ParseAndInvokeCallback(new_topics, topic_info_available);

    if (thread_join) {
      return;
    }
  }
}

void SignalWatcher::eraseCallbacks(std::vector<std::pair<registration_callback, size_t>> &callback, size_t id) const {
  ProfileUdex_FLL;
  auto it = callback.begin();
  while (it != callback.end()) {
    if (it->second == id) {
      it = callback.erase(it);
      if (it == callback.end())
        break;
    } else
      ++it;
  }
}

bool SignalWatcher::removeRequests(const std::vector<size_t> &myRequestedIds) {
  ProfileUdex_FLL;

  std::lock_guard<std::recursive_mutex> lock(callback_info_mutex);
  for (auto &callback : requested_urls) {
    for (size_t ids : myRequestedIds) {
      eraseCallbacks(callback.second, ids);
    }
  }
  return true;
}

void SignalWatcher::InitTopicCacheFromECAL() {
  ProfileUdex_FLL;
  if (shutdown_) {
    return;
  }
  std::unordered_set<std::string> all_new_unique_topics;
  std::unordered_map<std::string, eCAL::SDataTypeInformation> all_new_topic_info;

  {
    std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
    debug(__FILE__, "InitTopicCacheFromECAL() - Critical Section Start");
    // we run FillTopicCacheInternal multiple times because when starting the node
    // eCAL::Util::GetTopicNames may not return all the available signals in the first call
    int filter_resolution_cnt = 1; // the higher the more stable but slower
    for (int i = 0; i < kInitialUpdateLoopCounter; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateLoopSleepMs));
      std::vector<std::string> new_topics;
      std::unordered_map<std::string, eCAL::SDataTypeInformation> topic_info_available;
      FillTopicCacheFromECAL(filter_resolution_cnt, new_topics, topic_info_available);

      all_new_unique_topics.insert(new_topics.begin(), new_topics.end());
      all_new_topic_info.insert(topic_info_available.begin(), topic_info_available.end());
    }
    debug(__FILE__, "InitTopicCacheFromECAL() - Critical Section End");
  }

  std::vector<std::string> all_new_topics;
  all_new_topics.reserve(all_new_unique_topics.size());
  all_new_topics.insert(all_new_topics.end(), all_new_unique_topics.begin(), all_new_unique_topics.end());
  ParseAndInvokeCallback(all_new_topics, all_new_topic_info);
}

void SignalWatcher::ParseAndInvokeCallback(
    const std::vector<std::string> &new_topics,
    const std::unordered_map<std::string, eCAL::SDataTypeInformation> &topic_info_available) {
  for (const auto &topic : new_topics) {
    next::udex::ecal_util::SignalDescriptionHandler handler;
    std::string description;
    const auto topic_info_key = topic_info_available.find(topic);
    if (topic_info_key == topic_info_available.end()) {
      continue;
    }
    if (!handler.ParseSignalDescriptionFromString(topic_info_key->second.descriptor)) {
      continue;
    }
    std::string url = handler.GetParentUrl();
    if (url.empty()) {
      continue;
    }
    // call callback function from here if registered
    InvokeCallback(url);
  }
}

/*
 * @startuml
 *   "eCAL::Utils" ->[get all topics from eCAL] "vector<topic_name> topic_name_list"
 *   "vector<topic_name> topic_name_list" -->[check for new entries] "map<topic_name, TopicInfo> topic_info_cache_"
 *   "map<topic_name, TopicInfo> topic_info_cache_" -->[check for topics to delete with tick or if t_type old -> update
 * signal_list ] "new topics" "new topics" -->[parse schema and extract SignalTree] "ThreadPool" "ThreadPool" -->[send
 * SignalTree description] "map<url,description> topic_description_info_" "new topics" -->[add new entries]
 * "map<url,description> topic_description_info_" "map<url,description> topic_description_info_" -> "external API (url
 * based)"
 * @enduml*
 */
void SignalWatcher::FillTopicCacheFromECAL(
    int filter_resolution_cnt, std::vector<std::string> &new_topics,
    std::unordered_map<std::string, eCAL::SDataTypeInformation> &topic_info_available) {
  ProfileUdex_FLL;
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);

  // if clear is done map is empty in some cases
  std::unordered_map<std::string, eCAL::SDataTypeInformation> topic_to_info_map;
  WaitAndFilterGetTopicsFromEcal(filter_resolution_cnt, topic_to_info_map);

  // increase ticks for topics currently in cache
  for (auto &topic_info : topic_info_cache_) {
    topic_info.second.tick++;
  }

  auto IsNextTopic = [](const std::string &topic_name) { return topic_name.rfind("next:") == 0; };

  for (const auto &topic_to_info : topic_to_info_map) {
    const std::string &topic = topic_to_info.first;
    const eCAL::SDataTypeInformation &topic_info = topic_to_info.second;

    // when subscriber is created before the publisher, we will receive empty topic descriptor
    if (topic_info.descriptor.empty()) {
      debug(__FILE__, "Received empty topic descriptor for topic: {0}", topic);
      if (IsNextTopic(topic_info.name)) {
        update_cache_event_time_ =
            std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
      }
      continue;
    }

    topic_info_available.insert(std::make_pair(topic, topic_info));
    // we only check for next publishers
    if (IsNextTopic(topic_info.name) && (topic_info_cache_.find(topic) == topic_info_cache_.end())) {
      // topic not inside the cache so we add it
      TopicInfo info;
      std::string type;
      info.type = topic_info.name;
      topic_info_cache_[topic] = info;
      new_topics.push_back(topic);
    } else if (IsNextTopic(topic_info.name) && topic_info_cache_[topic].type != topic_info.name) {
      // the topic type changed so we delete the old topic entry and use the new one
      std::string cache_type = topic_info_cache_[topic].type;
      new_topics.push_back(topic);

      removeTopicsFromCache({topic});
    }
    if (IsNextTopic(topic_info.name)) {
      // we reset the tick for the current topic
      topic_info_cache_[topic].tick = 0;
    }
  }

  RemoveStaleTopics();

  // add the new topics
  std::unordered_map<std::string, std::future<SignalTree>> futures;
  for (const auto &topic : new_topics) {
    next::udex::ecal_util::SignalDescriptionHandler handler;
    std::string description;
    const auto topic_info_key = topic_info_available.find(topic);
    if (topic_info_key == topic_info_available.end()) {
      continue;
    }
    if (!handler.ParseSignalDescriptionFromString(topic_info_key->second.descriptor)) {
      continue;
    }
    std::string url = handler.GetParentUrl();
    if (url.empty()) {
      continue;
    }

    std::string type;
    topic_info_cache_[topic].url = url;
    topic_info_cache_[topic].type = topic_info_key->second.name;

    if (fast_search_enabled_) {
      DataDescription data_desc;
      data_desc.desc_handler = handler;

      std::vector<char> tmp_desc = handler.GetSignalDescription();
      std::string schema(tmp_desc.begin(), tmp_desc.end());

      topic_description_info_[url] = std::make_pair(topic, data_desc);
      topic_description_info_[url].second.desc_handler = handler;

      futures[topic] = post(pool_, std::packaged_task<SignalTree()>(
                                       std::bind(&SignalWatcher::createSignalTreeElement, this, schema, topic)));
    } else {
      // add empty description
      topic_description_info_[url] = std::make_pair(topic, DataDescription());
    }
  }

  if (fast_search_enabled_) {
    for (const auto &topic : new_topics) {
      next::udex::ecal_util::SignalDescriptionHandler handler;
      const auto topic_info_key = topic_info_available.find(topic);
      if (topic_info_key == topic_info_available.end()) {
        continue;
      }
      if (!handler.ParseSignalDescriptionFromString(topic_info_key->second.descriptor)) {
        continue;
      }
      std::string url = handler.GetParentUrl();

      if (url.empty()) {
        continue;
      }

      addSignalTreeSafelyFromFuture(futures, topic, handler, url);
      topic_description_info_[url].second.signal_tree_ready = true;
    }
  }
  debug(__FILE__, " -FillTopicCacheFromECAL- Topics with data type info available: {0}, Topics in cache: {1}",
        topic_info_available.size(), topic_info_cache_.size());
}

void SignalWatcher::RemoveStaleTopics() {
  // check ticks for stale topics
  std::vector<std::string> topics_to_erase;
  for (const auto &topic_info : topic_info_cache_) {
    if (topic_info.second.tick > max_accepted_ticks_) {
      topics_to_erase.push_back(topic_info.first);
    }
  }

  // delete stale topics
  if (topics_to_erase.size() != 0) {
    debug(__FILE__, " Removing {0} stale topics...", topics_to_erase.size());
    removeTopicsFromCache(topics_to_erase);
  }
}

void SignalWatcher::WaitAndFilterGetTopicsFromEcal(
    const int min_same_result_filter_cnt,
    std::unordered_map<std::string, eCAL::SDataTypeInformation> &topic_to_info_map) {
  if (shutdown_) {
    return;
  }
  topic_to_info_map.clear();
  const int max_attempts = 500;                            // to avoid infinite loop in case of something goes wrong
  const auto wait_time_ms = std::chrono::milliseconds(50); // wait time between samples
  std::unordered_map<std::string, eCAL::SDataTypeInformation> topic_info_map;
  std::unordered_map<std::string, eCAL::SDataTypeInformation> current_sample_topic_info_map;
  size_t last_number_of_topics = 0;
  int successfull_attempts_cnt = 0;
  int search_cnt = 0;

  // min_same_result_filter_cnt represents the same number of topics returned in a row for an acceptable result
  while (successfull_attempts_cnt < min_same_result_filter_cnt) {
    if (search_cnt > max_attempts) {
      swap(topic_to_info_map, topic_info_map);
      debug(__FILE__, " -WaitAndFilterGetTopicsFromEcal- Max allowed searches reached: {0} , found  {1} topics ",
            search_cnt, topic_to_info_map.size());
      return;
    }
    search_cnt++;
    current_sample_topic_info_map.clear();
    std::this_thread::sleep_for(wait_time_ms);

    eCAL::Util::GetTopics(current_sample_topic_info_map);
    if (current_sample_topic_info_map.size() == last_number_of_topics) {
      successfull_attempts_cnt++;
    } else {
      last_number_of_topics = current_sample_topic_info_map.size();
      successfull_attempts_cnt = 0;
    }

    // accumulate topics from each search sample
    std::move(current_sample_topic_info_map.begin(), current_sample_topic_info_map.end(),
              std::inserter(topic_info_map, topic_info_map.end()));
    if (successfull_attempts_cnt >= min_same_result_filter_cnt) {
      // we got a stable result, return
      swap(topic_to_info_map, topic_info_map);
      debug(__FILE__, " -WaitAndFilterGetTopicsFromEcal- Found {0} topics, search was done {1} times.",
            topic_to_info_map.size(), search_cnt);
      return;
    }
  }
}

void SignalWatcher::addSignalTreeSafelyFromFuture(std::unordered_map<std::string, std::future<SignalTree>> &futures,
                                                  const std::string &topic_name,
                                                  const SignalDescriptionHandler &handler, const std::string &url) {
  const auto it_topic_key = futures.find(topic_name);

  if (it_topic_key == futures.end()) {
    std::vector<char> tmp_desc = handler.GetSignalDescription();
    std::string schema(tmp_desc.begin(), tmp_desc.end());
    topic_description_info_[url].second.signal_tree = createSignalTreeElement(schema, topic_name);
  }
  if (!it_topic_key->second.valid() || (it_topic_key->second.wait_for(seconds(1)) == std::future_status::timeout)) {
    std::vector<char> tmp_desc = handler.GetSignalDescription();
    std::string schema(tmp_desc.begin(), tmp_desc.end());
    topic_description_info_[url].second.signal_tree = createSignalTreeElement(schema, topic_name);
  } else {
    topic_description_info_[url].second.signal_tree = it_topic_key->second.get();
  }
}

bool SignalWatcher::InitializeDescriptionHandler(const std::string &url) {
  if (url.empty()) {
    return false;
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  auto it = topic_description_info_.find(url);
  if (it == topic_description_info_.end()) {
    return false; // we don't know about this url yet
  }
  if (it->second.second.desc_handler_ready) {
    return true; // already initialized
  } else {
    next::udex::ecal_util::SignalDescriptionHandler handler;
    eCAL::SDataTypeInformation topic_info;
    if (!eCAL::Util::GetTopicDataTypeInformation(it->second.first, topic_info)) {
      debug(__FILE__, "Did not find topic in eCAL details {0}", it->second.first);
      return false;
    }
    if (!handler.ParseSignalDescriptionFromString(topic_info.descriptor)) {
      debug(__FILE__, "Coud not parse description from {0}", it->second.first);
      return false;
    }
    topic_description_info_[url].second.desc_handler = handler;
    topic_description_info_[url].second.desc_handler_ready = true;
  }
  return true;
}

bool SignalWatcher::InitializeDescriptionSignalTree(const std::string &url) {
  if (url.empty()) {
    return false;
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  auto it = topic_description_info_.find(url);
  if (it == topic_description_info_.end()) {
    return false; // we don't know about this url yet
  }
  if (it->second.second.signal_tree_ready) {
    return true; // already initialized
  } else {
    if (!InitializeDescriptionHandler(url)) {
      return false;
    }
    std::vector<char> tmp_desc = it->second.second.desc_handler.GetSignalDescription();
    std::string schema(tmp_desc.begin(), tmp_desc.end());

    topic_description_info_[url].second.signal_tree = createSignalTreeElement(schema, it->second.first);
    topic_description_info_[url].second.signal_tree_ready = true;
  }
  return true;
}

SignalTree SignalWatcher::createSignalTreeElement(const std::string &schema, const std::string &topic) {
  SignalTree tmp(schema, topic);
  return tmp;
}

void SignalWatcher::MonitoringThread() {
  ProfileUdex_FLL;
  thread_join = false;
  topic_info_thread = std::thread(std::bind(&SignalWatcher::FillTopicCache, this));
}

bool SignalWatcher::checkUrlAvailable(const std::string &url) {
  ProfileUdex_FLL;
  if (shutdown_) {
    return false;
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  auto it = topic_description_info_.find(url);

  return (it != topic_description_info_.end()) ? true : false;
}

SignalDescriptionHandler SignalWatcher::getTopicDescriptionHandler(const std::string &url, bool &available) {
  ProfileUdex_FLL;
  available = false;
  SignalDescriptionHandler description;
  if (shutdown_) {
    return description;
  }

  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  if (InitializeDescriptionHandler(url)) {
    auto it = topic_description_info_.find(url);
    if (it != topic_description_info_.end()) {
      description = it->second.second.desc_handler;
      available = true;
    }
  }
  return description;
}

std::string SignalWatcher::getTopicName(const std::string &url, bool &available) {
  ProfileUdex_FLL;
  std::string topic_name = "";
  if (shutdown_) {
    available = false;
    return topic_name;
  }

  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  auto it = topic_description_info_.find(url);
  if (it == topic_description_info_.end()) {
    available = false;
  } else {
    available = true;
    topic_name = it->second.first;
  }
  return topic_name;
}

size_t SignalWatcher::requestUrl(const std::string &url, const registration_callback call_back) {
  ProfileUdex_FLL;
  if (shutdown_) {
    return 0;
  }
  static size_t request_counter = 0;
  request_counter++;
  std::string url_copy;
  SignalDescriptionHandler desc_handler_copy;
  bool url_available = false;
  {
    std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
    if (InitializeDescriptionHandler(url)) {
      auto it = topic_description_info_.find(url);
      url_available = (it != topic_description_info_.end()) ? true : false;
      if (url_available) {
        url_copy = topic_description_info_[url].first;
        desc_handler_copy = topic_description_info_[url].second.desc_handler;
      }
    }
  }

  if (url_available) {
    call_back(url_copy, desc_handler_copy);
  } else {
    std::lock_guard<std::recursive_mutex> lock_callback(callback_info_mutex);
    requested_urls[url].push_back(std::make_pair(call_back, request_counter));
  }
  return request_counter;
}

SignalDescription SignalWatcher::getSignalDescription(const std::string &url, bool &available) {
  ProfileUdex_DTL;
  SignalDescriptionHandler sign_desc_handler = getTopicDescriptionHandler(url, available);

  SignalDescription sign_desc;
  sign_desc.basic_info = sign_desc_handler.GetBasicInformation();
  sign_desc.binary_data_description = sign_desc_handler.GetSignalDescription();

  return sign_desc;
}

std::pair<bool, std::string> SignalWatcher::generateSdl(const std::string topic) {
  ProfileUdex_DTL;
  using namespace mts::introspection;

  bool exists = false;
  auto descr = getSignalDescription(topic, exists);

  if (!exists || !mts::extensibility::is_sensor(descr.basic_info.source_id)) {
    return std::make_pair(false, "");
  }

  std::string schema = std::string(descr.binary_data_description.begin(), descr.binary_data_description.end());
  auto context = xtypes::parser::parse(schema);
  xtypes::dynamic_type::ptr dynamic_type = get_root_type(context.get_module());
  auto target_dynamic_type = mts::introspection::get_host_platform_type(*dynamic_type, true, true);
  target_dynamic_type->add_annotation(dynamic_type->annotations()[0]);
  auto sdl = xtypes::generator::print_sdl(*target_dynamic_type);
  return std::make_pair(true, sdl);
}

// find and invoke the callbacks for the url if it is already registered
void SignalWatcher::InvokeCallback(const std::string &url) {
  ProfileUdex_DTL;
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> lock_callback(callback_info_mutex);

  auto it = requested_urls.find(url);

  if (it != requested_urls.end()) {
    std::string url_copy;
    SignalDescriptionHandler desc_handler_copy;
    {
      std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
      InitializeDescriptionHandler(url);
      url_copy = topic_description_info_[url].first;
      desc_handler_copy = topic_description_info_[url].second.desc_handler;
    }
    for (auto callback : it->second) {
      // call with copy from topic_description_info_ otherwise deadlock occurs
      callback.first(url_copy, desc_handler_copy);
    }
    requested_urls.erase(url);
  }
}

std::vector<std::string> separateStringByDot_(const std::string &input, const int ignore_sub_str) {
  std::vector<std::string> output;
  std::size_t start = 0;
  int counter = 1;

  while (start < input.size()) {
    auto end = input.find('.', start);
    if (end == std::string::npos) {
      end = input.size();
    }

    if (counter++ > ignore_sub_str) {
      auto bracket_pos = input.find('[', start);
      if (bracket_pos < end) {
        end = bracket_pos;
      }

      auto bracket_close = input.find(']', start);
      if (bracket_close < end) {
        start = bracket_close + 2; // skip over '].'
        continue;
      }

      if (end != start) { // Avoid adding empty strings
        output.emplace_back(input.substr(start, end - start));
      }
    }

    if (end == input.size()) {
      break;
    } else {
      start = end + 1;
    }
  }

  return output;
}

std::string extractGroupFromURL(const std::string &url) {
  size_t pos = 0;
  unsigned int dot_counter = 0;
  const unsigned int url_level = SignalWatcher::UrlLevel::GROUP_LEVEL;

  while (dot_counter < url_level + 1 && pos != std::string::npos) {
    pos = url.find('.', pos + 1);
    if (pos != std::string::npos) {
      dot_counter++;
    } else if (pos == std::string::npos && dot_counter == url_level) {
      // If we don't find enough dots, url is already group level, return the entire string
      return url;
    }
  }

  return pos != std::string::npos ? url.substr(0, pos) : "";
}

// get all current rooturls in a list
std::vector<ChildInfo> SignalWatcher::GetRootUrlList(UrlLevel level, const std::string &parent_url) {
  ProfileUdex_FLL;

  std::multimap<std::string, std::string> url_list;
  FillUrlListFromTopics(level, parent_url, url_list);

  std::vector<ChildInfo> root_url_list;
  fillChildListFromUrlList(url_list, root_url_list);
  return root_url_list;
}

void SignalWatcher::FillUrlListFromTopics(UrlLevel level, const std::string &parent_url,
                                          std::multimap<std::string, std::string> &url_list) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  for (auto &it : topic_description_info_) {
    if (!InitializeDescriptionHandler(it.first)) {
      continue;
    }
    auto basic_info = it.second.second.desc_handler.GetBasicInformation();
    auto device_name = basic_info.data_source_name;
    auto view_name = basic_info.view_name;
    auto topic_name = it.second.first;

    if (device_name.empty()) {
      continue;
    }

    switch (level) {
    case DEVICE_LEVEL: {
      bool found = false;
      for (auto &device : url_list) {
        if (device.first == device_name && device.second == view_name) {
          found = true;
          break;
        }
      }
      if (!found)
        url_list.emplace(std::make_pair(device_name, view_name));
      break;
    }
    case VIEW_LEVEL: {
      if (device_name == parent_url) {
        url_list.insert(std::make_pair(view_name, topic_name));
      }
      break;
    }
    case GROUP_LEVEL: {
      auto device_and_view = device_name + "." + view_name;
      if (device_and_view == parent_url) {
        url_list.insert(std::make_pair(topic_name, topic_name));
      } else if (topic_name == parent_url) {
        url_list.insert(std::make_pair(topic_name, topic_name));
      } else {
        auto pos = parent_url.find(topic_name);
        if (pos != std::string::npos) {
          url_list.insert(std::make_pair(parent_url, topic_name));
        }
      }
      break;
    }
    default:
      break;
    }
  }
}

void SignalWatcher::fillChildListFromUrlList(std::multimap<std::string, std::string> &url_list,
                                             std::vector<ChildInfo> &child_list) const {
  ChildInfo child;
  for (const auto &pair : url_list) {
    // maybe use unorder set than set.extract to vector of childs
    auto it =
        std::find_if(child_list.begin(), child_list.end(),
                     [&name = pair.first](const ChildInfo &child_info) -> bool { return name == child_info.name; });
    if (it == child_list.end()) {
      child.name = pair.first;
      child.child_count = static_cast<int>(url_list.count(pair.first));
      child.array_lenght =
          1; // at this point we assume the group / view is a single array. all other signals are overriden (level2)
      child_list.push_back(child);
    }
  }
}

std::vector<std::string> SignalWatcher::getTopicByUrl(std::string signal_url) {
  ProfileUdex_FLL;
  if (shutdown_) {
    return std::vector<std::string>();
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  std::vector<std::string> ret = {};
  if (url_topic_cache_.find(signal_url) != url_topic_cache_.end()) {
    return {url_topic_cache_[signal_url]};
  } else {
    std::string groupURL = extractGroupFromURL(signal_url);
    if (!groupURL.empty()) {
      auto it = topic_description_info_.find(groupURL);
      if (it != topic_description_info_.end()) {
        std::string topic = it->first;
        ret.push_back(topic);
        addTopicToCaches(signal_url, topic);
        return ret;
      }
    }

    for (auto &topic : topic_description_info_) {
      // disable search and match from start of word!
      if (topic.first.find(signal_url) == 0) {
        // make sure we only take matches based on Urls -> check for point after signal_url

        // same word
        if (topic.first.length() == signal_url.length()) {
          ret.push_back(topic.first);
        }
        if (topic.first.length() > signal_url.length()) {
          if (topic.first[signal_url.length()] == '.') {
            ret.push_back(topic.first);
          }
        }
      }
    }
    if (ret.size() != 0) {
      return ret;
    }

    auto urls{separateStringByDot_(signal_url, 0)};
    std::string url_build_up{};

    for (auto partial_url = urls.begin(); partial_url != urls.end(); ++partial_url) {
      if (url_build_up.empty()) {
        url_build_up.append(*partial_url);
      } else {
        url_build_up.append(".");
        url_build_up.append(*partial_url);
      }

      auto is_available = topic_description_info_.count(url_build_up);
      if (!is_available) {
        continue;
      } else {
        ret.push_back(url_build_up);
        addTopicToCaches(signal_url, url_build_up);
        break;
      }
    }
    if (ret.size() == 0) {
      return std::vector<std::string>();
    } else {
      return ret;
    }
  }
}
void SignalWatcher::addTopicToCaches(const std::string &signal_url, std::string &topic_from_url) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  url_topic_cache_[signal_url] = topic_from_url;
  if (topic_url_list_.find(topic_from_url) != topic_url_list_.end()) {
    topic_url_list_[topic_from_url].push_back(signal_url);
  } else {
    topic_url_list_[topic_from_url] = {signal_url};
  }
}

std::vector<std::string> SignalWatcher::getDevicesByFormat(std::string format) {
  auto format_type = next::sdk::types::getPackageFormatTypeEnum(format);
  return getDevicesByFormat(format_type);
}

std::vector<std::string> SignalWatcher::getDevicesByFormat(next::sdk::types::PackageFormatType format_type) {
  if (shutdown_) {
    return std::vector<std::string>();
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  std::vector<std::string> results = {};
  {
    for (auto &topic_pair : topic_description_info_) {
      if (!InitializeDescriptionHandler(topic_pair.first)) {
        continue;
      }
      auto basic_information = topic_pair.second.second.desc_handler.GetBasicInformation();
      if (basic_information.format_type == format_type) {
        if (std::find(results.begin(), results.end(), basic_information.data_source_name) == results.end()) {
          results.push_back(basic_information.data_source_name);
        }
      }
    }
  }
  return results;
}

std::vector<ChildInfo> SignalWatcher::getChildrenByUrl(std::string signal_url) {
  ProfileUdex_FLL;
  if (signal_url.empty()) {
    return GetRootUrlList(DEVICE_LEVEL, "");
  }

  auto remove_index = [](const auto &signal_url) {
    std::string ret = std::regex_replace(signal_url, std::regex("\\[\\d+\\]"), "$1$2");
    return ret;
  };

  // we remove the index from arrays because in the xtype schema and SignalTree we do not hold the urls with indexes
  signal_url = remove_index(signal_url);

  const auto count = std::count_if(signal_url.begin(), signal_url.end(), [](char c) { return c == '.'; });
  if (count == 0) {
    return GetRootUrlList(VIEW_LEVEL, signal_url);
  }

  std::vector<ChildInfo> temp_topics = GetRootUrlList(GROUP_LEVEL, signal_url);
  std::vector<ChildInfo> children_topic;

  for (auto topic : temp_topics) {

    auto group_name = get_group_name(topic.name);

    bool available = false;
    SignalTree signal_tree = getSignalTree(topic.name, available);
    if (!available) {
      std::vector<std::string> topic_url = getTopicByUrl(signal_url);
      if (topic_url.empty()) {
        continue;
      }

      auto current_topic_url = topic_url.front();
      signal_tree = getSignalTree(current_topic_url, available);
      if (!available) {
        continue;
      }

      group_name = get_group_name(current_topic_url);
      auto pos = signal_url.find(current_topic_url);
      if (pos == std::string::npos) {
        continue;
      }

      group_name.append(signal_url.substr(current_topic_url.size()));
    }

    auto ChildrenInfo = signal_tree.getChildrenInfo(group_name);
    if (signal_url == topic.name) {
      children_topic.insert(children_topic.end(), ChildrenInfo.begin(), ChildrenInfo.end());
    } else {
      ChildInfo tmp;
      tmp.name = group_name;
      tmp.child_count = static_cast<int>(ChildrenInfo.size());
      children_topic.push_back(tmp);
    }
  }

  return children_topic;
}

std::unordered_map<std::string, std::vector<std::string>> SignalWatcher::GetFullUrlTree() {
  ProfileUdex_FLL;
  if (shutdown_) {
    return std::unordered_map<std::string, std::vector<std::string>>();
  }
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  std::map<std::string, std::vector<std::vector<std::string>>> local_view_urls;

  auto devices = GetRootUrlList(DEVICE_LEVEL, "");
  for (const auto &device : devices) {
    auto views = getChildrenByUrl(device.name);

    for (const auto &view : views) {
      auto device_and_view = device.name + "." + view.name;
      auto groups = getChildrenByUrl(device_and_view);

      for (const auto &group : groups) {
        auto device_and_view_and_group = device_and_view + "." + group.name;
        std::vector<std::string> vect;
        if (InitializeDescriptionSignalTree(device_and_view_and_group)) {
          // up to here we have the info in the topic_description_info_
          vect = topic_description_info_[device_and_view_and_group].second.signal_tree.getFullSignalList();
        }

        local_view_urls[device.name].push_back(vect);
      }
    }
  }

  std::unordered_map<std::string, std::vector<std::string>> full_url_tree;
  for (const auto &elem : local_view_urls) {
    std::vector<std::string> append_vector;
    for (const auto &vector_of_view_signals : elem.second) {
      append_vector.insert(append_vector.end(), vector_of_view_signals.begin(), vector_of_view_signals.end());
    }
    full_url_tree.insert(std::make_pair(elem.first, append_vector));
  }

  return full_url_tree;
}

void SignalWatcher::reset() {
  shutdown_ = true;
  thread_join = true;

  fill_initial_cache_.notify_one();
  if (topic_info_thread.joinable()) {
    update_requested_ = true;
    update_cache_cv_.notify_all();
    debug(__FILE__, "SignalWatcher :: reset wait for joinable");
    topic_info_thread.join();
  }
  thread_join = false;
}

SignalTree SignalWatcher::getSignalTree(const std::string &url, bool &available) {
  available = false;
  SignalTree signal_tree;
  if (shutdown_) {
    return signal_tree;
  }

  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  if (InitializeDescriptionSignalTree(url)) {
    auto it = topic_description_info_.find(url);
    if (it != topic_description_info_.end()) {
      signal_tree = it->second.second.signal_tree;
      available = true;
    }
  }
  return signal_tree;
}

std::vector<std::string> SignalWatcher::searchSignalTree(const std::string &keyword) {
  if (shutdown_) {
    return std::vector<std::string>();
  }
  std::vector<std::string> result;
  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);

  for (auto &desc : topic_description_info_) {
    if (!InitializeDescriptionSignalTree(desc.first)) {
      continue;
    }

    std::vector<std::string> tmp;
    auto search_it =
        std::search(desc.first.begin(), desc.first.end(), keyword.begin(), keyword.end(),
                    [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
    if (search_it != desc.first.end()) {
      // keyword in signal list directly no need to search
      tmp = desc.second.second.signal_tree.getFullSignalList();
    } else {
      // search inside the description
      tmp = desc.second.second.signal_tree.searchForUrls(keyword);
    }

    result.insert(result.end(), tmp.begin(), tmp.end());
  }
  return result;
}

void SignalWatcher::setMaxTickCount(uint32_t tick_count) { max_accepted_ticks_ = tick_count; }

void SignalWatcher::updateTopicCacheOnEvent(const next::sdk::events::UpdateTopicCacheMessage &msg) {
  if (msg.topics.size() == 0) {
    debug(__FILE__, "UpdateTopicCacheMessage received, notify monitoring thread...");
    update_cache_event_time_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
    update_requested_ = true;
    update_cache_cv_.notify_all();
  } else {
    removeTopicsFromCache(msg.topics);
  }
}
void SignalWatcher::removeTopicsFromCache(const std::vector<std::string> &topics_to_erase) {
  ProfileUdex_FLL;
  if (shutdown_) {
    return;
  }

  debug(__FILE__, " -removeTopicsFromCache-  Removing {0} topics from cache. Number of topics in cache: {1}",
        topics_to_erase.size(), topic_info_cache_.size());

  std::lock_guard<std::recursive_mutex> lock(topic_info_mutex_);
  for (const auto &topic : topics_to_erase) {
    topic_description_info_.erase(topic_info_cache_[topic].url);
    topic_info_cache_.erase(topic);
    if (topic_url_list_.find(topic) != topic_url_list_.end()) {
      for (auto entry : topic_url_list_[topic]) {
        url_topic_cache_.erase(entry);
      }
      topic_url_list_.erase(topic);
    }
  }
}

void SignalWatcher::EnableFastSearch() { fast_search_enabled_ = true; }

} // namespace ecal_util
} // namespace udex
} // namespace next

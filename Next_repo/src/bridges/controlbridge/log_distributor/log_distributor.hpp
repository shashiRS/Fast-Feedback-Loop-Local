/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_distributor.hpp
 * @brief    sends out log message via websocket
 **/

#ifndef NEXT_CONTROLBRIDGE_LOG_DISTRIBUTOR_H_
#define NEXT_CONTROLBRIDGE_LOG_DISTRIBUTOR_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <next/bridgesdk/web_server.hpp>
#include <next/sdk/events/log_event.hpp>

namespace next {
namespace controlbridge {

class LogDistributor {
public:
  /*!
   * @brief Construct a new Log Distributor object which sends out data via webserver received by the function
   * SendLogEventData
   * Hint: Won't send data till webserver is set.
   * @param flush_level log level which triggers an immediately sending of the log messages skipping the wait_time
   * @param wait_time wait time between sending data via webserver
   */
  LogDistributor(next::sdk::logger::LOGLEVEL flush_level = next::sdk::logger::LOGLEVEL::ERR,
                 std::chrono::milliseconds wait_time = std::chrono::milliseconds(1000));

  /*!
   * @briefConstruct a new Log Distributor object which sends out data via webserver received by the function
   * SendLogEventData
   * @param webserver_in
   * @param flush_level log level which triggers an immediately sending of the log messages skipping the wait_time
   * @param wait_time wait time between sending data via webserver
   */
  LogDistributor(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in,
                 next::sdk::logger::LOGLEVEL flush_level = next::sdk::logger::LOGLEVEL::ERR,
                 std::chrono::milliseconds wait_time = std::chrono::milliseconds(1000));

  /*!
   * @brief Destroy the Log Distributor object
   */
  virtual ~LogDistributor();

  /*!
   * @brief Set webserver for sending log data
   * @param webserver_in
   */
  void SetWebserver(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in) { webserver_ = webserver_in; }

  /*!
   * @brief Set wait time between sending data via webserver
   */
  void SetWaitTime(std::atomic<std::chrono::milliseconds> wait_time) { wait_time_.store(wait_time); }

  /*!
   * @brief Add received log messages to a vector
   * @param log_event_data_list LogEventData received from the LogCollector
   */
  void SendLogEventData(const std::vector<next::sdk::events::LogEventData> &log_event_data_list);

private:
  /*!
   * @brief Convert log event data to a Json object
   * @param log_event_data_list LogEventData received from the LogCollector
   * @param json_payload Output containing the Json object as an uint8 vector.
   */
  void CreateJsonMessagePayload(const std::vector<next::sdk::events::LogEventData> &log_event_data_list,
                                std::vector<uint8_t> &json_payload);

  void NotificationCallback(std::vector<uint8_t> *json_payload);

  /*!
   * @brief Sends log messages as JSON to the GUI
   */
  void SendJsonToGUI();

  std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_{nullptr};

  std::mutex mutex_log_event_data_;

  std::vector<next::sdk::events::LogEventData> log_event_data_;

  std::thread send_json_to_gui_thread_;
  std::atomic_bool send_data_{true};
  std::atomic<std::chrono::milliseconds> wait_time_;
  std::atomic_bool abort_wait_{false};
  std::mutex mutex_wait_send_data_;
  std::condition_variable cond_wait_send_data_;

  next::sdk::logger::LOGLEVEL flush_level_{next::sdk::logger::LOGLEVEL::DEBUG};
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_CONTROLBRIDGE_LOG_DISTRIBUTOR_H_

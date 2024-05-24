/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     log_router_test.cpp
 *  @brief    Testing the log_router, publish logs and receive logs and compare the logs
 */
#include <atomic>
#include <map>
#include <thread>
#include <type_traits>

#include <ecal/ecal.h>

#include "../../sdk/src/log_router.hpp"
#include "next/sdk/sdk.hpp"

using namespace std::chrono_literals;
using next::sdk::InitChain;

class LogRouterTest : public next::sdk::logger::LogRouter {
public:
  LogRouterTest(bool enabled = false) { enabled ? this->publisher_exit_ = true : this->publisher_exit_ = false; }
  void setReadBuffer(bool enabled) { this->read_buffer_ = enabled; }
};

class LogRouterFixture : public next::sdk::testing::TestUsingEcal {
public:
  LogRouterFixture() {
    std::cout << "LogRouterFixture()" << std::endl;
    this->instance_name_ = "LogRouterTest";

    // enable logging
    initmap_[next::sdk::logger::getLogRouterDisableCfgKey()] = true;
    next::sdk::logger::register_to_init_chain();
    next::sdk::testing::TestUsingEcal::SetUp();
    std::this_thread::sleep_for(2000ms);
  }
  void SetUp() override { log_event_subscriber_ = std::make_unique<next::sdk::events::LogEvent>(""); }

  ~LogRouterFixture() {
    std::cout << "~LogRouterFixture()" << std::endl;
    for (auto &[key, log_msg] : expected_data_) {
      (void)key;
      delete[] log_msg->payload.data();
      delete log_msg;
    }
    expected_data_.clear();
    actual_data_.clear();
  }

  void TearDown() override { log_event_subscriber_.reset(nullptr); }

  void checkTestData();
  std::map<std::string, next::sdk::events::LogEventData> keysNotFoundInActualData();

  void createSubscriptions();
  std::unique_ptr<next::sdk::events::LogEvent> log_event_subscriber_;
  size_t magic_number_ = 1000, abort_number_ = 300;
  std::map<std::string, next::sdk::events::LogEventData> actual_data_;
  std::map<std::string, spdlog::details::log_msg *> expected_data_;
  std::mutex protect_actual_data_;
  std::condition_variable cond_;
};

std::map<std::string, next::sdk::events::LogEventData> LogRouterFixture::keysNotFoundInActualData() {

  std::map<std::string, next::sdk::events::LogEventData> difference;

  for (const auto &[key, value] : expected_data_) {
    if (actual_data_.find(key) == actual_data_.end()) {

      next::sdk::events::LogEventData log_event_data;
      log_event_data.log_level = next::sdk::logger::getLogLevelFromSpd(value->level);
      log_event_data.log_message.assign(value->payload.begin(), value->payload.end());
      log_event_data.thread_id = value->thread_id;
      log_event_data.time_stamp = value->time.time_since_epoch().count();
      difference[key] = log_event_data;
    }
  }
  return difference;
}

void LogRouterFixture::checkTestData() {

  auto expected_data_size = expected_data_.size();
  std::unique_lock<std::mutex> lock(protect_actual_data_);
  auto time_out = 12000ms;
  cond_.wait_for(lock, time_out, [&]() { return actual_data_.size() == expected_data_size; });
  lock.unlock();

  auto diff_data = keysNotFoundInActualData();
  if (diff_data.size() > 0) {
    std::cout << "\n##### Keys which are not present in the Actual Data ##########\n";
    for (const auto &[key, value] : diff_data) {
      std::cout << key << ", " << value.time_stamp << ", " << (int)value.log_level << "\n";
    }
  }

  if (actual_data_.size() != expected_data_.size()) {
    EXPECT_TRUE(actual_data_.size() != expected_data_.size()) << " timeout was exceeded\n";
  }

  for (auto &[key, actual_val] : actual_data_) {
    if (expected_data_.find(key) != expected_data_.end()) {
      auto expected_val = expected_data_[key];
      EXPECT_EQ(actual_val.thread_id, expected_val->thread_id);
      EXPECT_EQ(actual_val.log_level, next::sdk::logger::getLogLevelFromSpd(expected_val->level));
      EXPECT_EQ(actual_val.log_message, expected_val->payload.data());
      EXPECT_EQ(actual_val.time_stamp, expected_val->time.time_since_epoch().count());
    } else {
      EXPECT_TRUE(false) << key << " wasn't found inside the expected_data";
    }
  }
}

void LogRouterFixture::createSubscriptions() {
  eCAL::Util::EnableLoopback(true);
  log_event_subscriber_->addEventHook([&]() {
    auto new_data = log_event_subscriber_->getEventData();
    while (!new_data.empty()) {
      protect_actual_data_.lock();
      actual_data_[new_data.back().log_message] = new_data.back();
      new_data.pop_back();
      protect_actual_data_.unlock();
      cond_.notify_all();
    }
  });

  log_event_subscriber_->subscribe(); // subscriber subscribes
}

TEST_F(LogRouterFixture, log_collector_sink_log_function_single_thread_test) {

  LogRouterTest lcs;     // publisher
  createSubscriptions(); // subscriber

  for (size_t i = 0; i < magic_number_; i++) {
    spdlog::details::log_msg *msg = new spdlog::details::log_msg();
    msg->level = spdlog::level::debug;

    auto id = std::this_thread::get_id();
    std::stringstream ss;
    ss << id;
    msg->thread_id = std::stoul(ss.str());

    std::string msg_temp = ss.str() + "_" + std::to_string(i) + "_logging";
    char *str = new char[msg_temp.size() + 1];
    memcpy(str, msg_temp.c_str(), msg_temp.size());
    str[msg_temp.size()] = '\0';
    spdlog::string_view_t fmt(str, msg_temp.size());

    msg->payload = fmt;
    msg->time = std::chrono::system_clock::now();
    lcs.log(*msg);
    expected_data_[msg_temp] = msg;
  }

  checkTestData(); // test the data
}

TEST_F(LogRouterFixture, log_collector_sink_log_function_multiple_threads_test) {

  size_t thresh_hold_threads_for_ci = std::thread::hardware_concurrency();
  std::vector<std::thread> ths(thresh_hold_threads_for_ci);

  LogRouterTest lcs;
  createSubscriptions();

  std::atomic<size_t> magic_number(magic_number_);
  std::mutex mu;

  for (size_t cnt = 0; cnt < thresh_hold_threads_for_ci; cnt++) {
    ths[cnt] = std::thread([&magic_number, &lcs, &mu, this]() {
      for (size_t i = 0; i < magic_number; i++) {
        spdlog::details::log_msg *msg = new spdlog::details::log_msg();
        msg->level = spdlog::level::debug;
        msg->time = std::chrono::system_clock::now();

        auto id = std::this_thread::get_id();
        std::stringstream ss;
        ss << id;
        msg->thread_id = std::stoul(ss.str());

        std::string msg_temp = ss.str() + "_" + std::to_string(i) + "_logging";
        char *str = new char[msg_temp.size() + 1];
        memcpy(str, msg_temp.c_str(), msg_temp.size());
        str[msg_temp.size()] = '\0';
        spdlog::string_view_t fmt(str, msg_temp.size());
        msg->payload = fmt;

        mu.lock();
        lcs.log(*msg); // todo: when locked code works almost in sequence, when out of the lock code execution is stuck
        expected_data_[msg_temp] = msg;
        mu.unlock();
      }
    });
  }

  for (size_t cnt = 0; cnt < thresh_hold_threads_for_ci; cnt++) {
    ths[cnt].join();
  }

  checkTestData(); // test the data
}

TEST_F(LogRouterFixture, log_collector_sink_log_flush_function) {
  LogRouterTest lcs(true);
  createSubscriptions();
  for (size_t i = 0; i < magic_number_; i++) {
    spdlog::details::log_msg *msg = new spdlog::details::log_msg();
    msg->level = spdlog::level::debug;

    auto id = std::this_thread::get_id();
    std::stringstream ss;
    ss << id;
    msg->thread_id = std::stoul(ss.str());

    std::string msg_temp = ss.str() + "_" + std::to_string(i) + "_logging";
    char *str = new char[msg_temp.size() + 1];
    memcpy(str, msg_temp.c_str(), msg_temp.size());
    str[msg_temp.size()] = '\0';
    spdlog::string_view_t fmt(str, msg_temp.size());

    msg->payload = fmt;
    msg->time = std::chrono::system_clock::now();
    lcs.log(*msg);
    expected_data_[msg_temp] = msg;
  }

  lcs.flush();

  checkTestData(); // test the data
}

TEST_F(LogRouterFixture, log_collector_sink_log_abort_publishing_thread) {
  std::unique_ptr<LogRouterTest> u_lcs = std::make_unique<LogRouterTest>();
  u_lcs->setReadBuffer(false);
  createSubscriptions();
  for (size_t i = 0; i < magic_number_; i++) {
    spdlog::details::log_msg *msg = new spdlog::details::log_msg();
    msg->level = spdlog::level::debug;

    auto id = std::this_thread::get_id();
    std::stringstream ss;
    ss << id;
    msg->thread_id = std::stoul(ss.str());

    std::string msg_temp = ss.str() + "_" + std::to_string(i) + "_logging";
    char *str = new char[msg_temp.size() + 1];
    memcpy(str, msg_temp.c_str(), msg_temp.size());
    str[msg_temp.size()] = '\0';
    spdlog::string_view_t fmt(str, msg_temp.size());

    msg->payload = fmt;
    msg->time = std::chrono::system_clock::now();
    u_lcs->log(*msg);
    expected_data_[msg_temp] = msg;
    if (i % abort_number_ == 0) {
      u_lcs.reset(); // abort forecefully the thread
      break;
    }
  }

  checkTestData(); // test the data
}

TEST_F(LogRouterFixture, set_get_logger_level) {
  auto log_level = next::sdk::logger::LOGLEVEL::OFF;
  next::sdk::logger::setLoglevelLogRouter(log_level);
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::OFF, next::sdk::logger::getLogLevelLogRouter());
  log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  next::sdk::logger::setLoglevelLogRouter(log_level);
  EXPECT_EQ(next::sdk::logger::LOGLEVEL::DEBUG, next::sdk::logger::getLogLevelLogRouter());
}

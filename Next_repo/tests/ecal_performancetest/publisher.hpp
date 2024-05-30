/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     publisher.hpp
 * @brief    Class having eCAL::Publisher for sending test data
 *
 **/

#ifndef PERFORMANCETEST_PUBLISHER_H_
#define PERFORMANCETEST_PUBLISHER_H_

#include <chrono>
#include <thread>
#include <vector>

#include <ecal/ecal_publisher.h>

#include <next/sdk/sdk.hpp>

#include "common.hpp"
#include "sleep.hpp"

constexpr const auto inter_msg_pause = std::chrono::milliseconds(0);
constexpr const auto wait_for_connect_max_wait_time = std::chrono::seconds(5);
constexpr const auto inter_wait_for_connect_pause = std::chrono::milliseconds(100);

#define PUBLISHER_SEND_SYNCHRONIZED

class Publisher {
public:
  Publisher(const size_t id) : id_(id), topic_(std::string(topic_prefix) + std::to_string(id)) {
    eCAL::STopicInformation topic_info;
    topic_info.type = topic_type;
    topic_info.descriptor = topic_description;
    publisher_ = eCAL::CPublisher(topic_, topic_info);
    info(__FILE__, "[Publisher_{0}] constructed publisher", id_);
    worker_ = std::thread(std::bind(&Publisher::run, this));
  }
  Publisher(const Publisher &o)
      : id_(o.id_), topic_(o.topic_), payload_(o.payload_), msgs_per_topic_(o.msgs_per_topic_), shutdown_(o.shutdown_) {
    eCAL::STopicInformation topic_info;
    topic_info.type = topic_type;
    topic_info.descriptor = topic_description;
    publisher_ = eCAL::CPublisher(topic_, o.publisher_.GetTopicInformation());
    info(__FILE__, "[Publisher_{0}] constructed by copying", id_);
    worker_ = std::thread(std::bind(&Publisher::run, this));
  }
  ~Publisher() {
    info(__FILE__, "[Publisher_{0}] destroying publisher", id_);
    shutdown_ = true;
    cv_prepared_.notify_one();
    cv_prepared_.notify_all();
    if (worker_.joinable())
      worker_.join();
    publisher_.Destroy();
    info(__FILE__, "[Publisher_{0}] publisher destroyed", id_);
  }

  void prepare_send(const test_payload &payload, const size_t msgs_per_topic) {
    payload_ = payload;
    msgs_per_topic_ = msgs_per_topic;
    is_sending_ = false;
    if (id_ == 0) {
      std::unique_lock<std::mutex> lck(mtx_start_send_);
      sending_requested_ = false;
    }
    {
      std::unique_lock<std::mutex> lck(mtx_prepared_);
      sending_finished_ = false;
      preparation_finished_ = false;
      preparation_requested_ = true;
    }
    cv_prepared_.notify_one();
    while (!preparation_finished_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
  }

  static void start_send() {
    std::unique_lock<std::mutex> lck(mtx_start_send_);
    sending_requested_ = true;
    cv_start_send_.notify_all();
  }

  bool sending_finished() { return sending_finished_; }

  bool subscriber_connected(const bool wait_for_connect = true) {
    const auto max_time = std::chrono::high_resolution_clock::now() + wait_for_connect_max_wait_time;
    do {
      const size_t n_subscribers = publisher_.GetSubscriberCount();
      if (n_subscribers == 1) {
        return true;
      } else if (n_subscribers > 0) {
        warn(__FILE__, "[Publisher_{0}] there are {1} subscriber connected", id_, n_subscribers);
        return true;
      } else if (wait_for_connect) {
        if (std::chrono::high_resolution_clock::now() > max_time) {
          return false;
        }
        debug(__FILE__, "[Publisher_0] waiting for connection", id_);
        std::this_thread::sleep_for(inter_wait_for_connect_pause);
      } else {
        return false;
      }
    } while (!shutdown_);
    return false;
  }

private:
  void run() {
    debug(__FILE__, "[Publisher_{0}] worker thread starting up", id_);
    while (!shutdown_) {
      debug(__FILE__, "[Publisher_{0}] waiting for setup", id_);
      // wait everything got set up
      {
        std::unique_lock<std::mutex> lck(mtx_prepared_);
        cv_prepared_.wait(
            lck, [&shutdown = shutdown_, &prep_req = preparation_requested_]() { return shutdown || prep_req; });
        preparation_requested_ = false;
      }
      if (shutdown_)
        break;
      preparation_finished_ = true;

      debug(__FILE__, "[Publisher_{0}] waiting for GO", id_);
      // wait for go to send
      {
        std::unique_lock<std::mutex> lck(mtx_start_send_);
        cv_start_send_.wait(lck,
                            [&shutdown = shutdown_, &snd_req = sending_requested_]() { return shutdown || snd_req; });
      }
      if (shutdown_)
        break;

      debug(__FILE__, "[Publisher_{0}] worker thread sending messages", id_);
      // send the messages
      is_sending_ = true;
      test_payload payload = payload_;
      auto timestamp = std::chrono::high_resolution_clock::now();
      for (size_t n = 0; msgs_per_topic_ > n; ++n) {
        payload.msg_number = n + 1;
#ifdef PUBLISHER_SEND_SYNCHRONIZED
        const size_t bytes = publisher_.Send((void *)&payload, sizeof(payload), -1, 100);
#else
        const size_t bytes = publisher_.Send((void *)&payload, sizeof(payload));
#endif
        if (sizeof(payload) != bytes) {
          warn(__FILE__, "[Publisher_{0}] sent {1} instead of {2} bytes", id_, bytes, sizeof(payload));
        } else {
          debug(__FILE__, "[Publisher_{0}] sent msg {1} of {2}", id_, n + 1, msgs_per_topic_);
        }
        if (inter_msg_pause != std::chrono::milliseconds::zero()) {
          timestamp += inter_msg_pause;
          std::this_thread::sleep_until(timestamp);
        }
      }
      sending_finished_ = true;
    }
    debug(__FILE__, "[Publisher_{0}] worker thread finishing", id_);
  }

  size_t id_;
  eCAL::CPublisher publisher_;
  const std::string topic_;
  test_payload payload_;
  size_t msgs_per_topic_{0};
  std::thread worker_;
  bool shutdown_{false};
  std::mutex mtx_prepared_;
  bool preparation_requested_{false};
  bool preparation_finished_{false};
  static bool sending_requested_;
  std::atomic<bool> sending_finished_{false};
  std::condition_variable cv_prepared_;
  static std::mutex mtx_start_send_;
  static std::condition_variable cv_start_send_;
  std::atomic<bool> is_sending_{false};
};

bool Publisher::sending_requested_ = false;
std::mutex Publisher::mtx_start_send_ = std::mutex();
std::condition_variable Publisher::cv_start_send_ = std::condition_variable();

#endif // PERFORMANCETEST_PUBLISHER_H_

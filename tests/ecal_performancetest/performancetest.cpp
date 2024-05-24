/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     performancetest.cpp
 * @brief    A server client tool performing some stress test on eCAL.
 *
 * This cpp file is for creating one executable, which can be used for stress testing eCAL.
 * Start the applicaiton without any argument to have a server running. Start it with any argument to start the
 * client. The client and server are communicating via shared memory.
 *
 * The server is creating a lot of publishers with their own topic. The client subscribes to them. Then the server
 * is sending a lot of messages per publisher. Each publisher is running within its own thread. All are triggered via
 * a condition variable.
 *
 **/

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#include <array>
#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/process.hpp>

namespace bi = boost::interprocess;

#include <ecal/ecal.h>
#include <ecal/ecal_publisher.h>
#include <ecal/ecal_subscriber.h>
#include <ecal/msg/string/publisher.h>

#include <next/sdk/sdk.hpp>

#include "common.hpp"
#include "publisher.hpp"
#include "sleep.hpp"
#include "statswriter.hpp"

constexpr const auto inter_iteration_pause = std::chrono::milliseconds(250);
constexpr const size_t min_msgs_per_topic = 100;
constexpr const size_t max_msgs_per_topic = 300;
constexpr const size_t msgs_per_topic_step = 100;
constexpr const size_t min_topics = 500;
constexpr const size_t max_topics = 6000;
constexpr const size_t topics_step = 500;
constexpr const auto max_receive_time = std::chrono::milliseconds(5000);
constexpr const auto watchdog_delay = std::chrono::milliseconds(1000);
constexpr const auto startup_delay = std::chrono::milliseconds(100);
constexpr const uint16_t watchdog_reset_value{5};
constexpr const char *shm_name = "Performancetest";
constexpr const unsigned int shm_create_max_tries{10};
constexpr const auto shm_create_retry_pause = std::chrono::seconds(1);
static_assert(min_topics > 0);
static_assert(topics_step > 0);
static_assert(min_topics <= max_topics);
static_assert(min_msgs_per_topic > 0);
static_assert(msgs_per_topic_step > 0);
static_assert(min_msgs_per_topic <= max_msgs_per_topic);

enum class run_state {
  uninitialized,    // not started up so far (client and server)
  shm_accessable,   // shared memory is accessable (client only)
  watchdog_started, // watchdog was started (client and server)
  pub_sub_started,  // publisher were started (server), subscriber were started (client)
  msg_sent,         // all messages were sent (server only)
  msg_received,     // message receiving finished (client only)
  // msg_cnt_incorrect, // count of received msgs incorrect (client only) // not used so far
};

struct shm_data {
  std::uint16_t server_watchdog{watchdog_reset_value};
  std::uint16_t client_watchdog{watchdog_reset_value};
  bool server_ready{false};
  bool client_ready{false};
  size_t topics{0};         // count of topics in this iteration (this is the count of subscribers and publishers)
  size_t msgs_per_topic{0}; // count of messages per topic within this iteration
  bi::interprocess_semaphore mutex{1};
  std::atomic<run_state> server_run_state{run_state::uninitialized}; // run state of the server
  std::atomic<run_state> client_run_state{run_state::uninitialized}; // run state of the client
  std::chrono::high_resolution_clock::time_point time_msg_send_start;
};

struct shm_mtx_lock {
  shm_mtx_lock(bi::interprocess_semaphore &mtx) : mtx_(mtx) { mtx_.wait(); }
  ~shm_mtx_lock() { mtx_.post(); }

private:
  bi::interprocess_semaphore &mtx_;
};

template <class C, class R, class P>
inline void time_increase(const std::string &name, std::chrono::time_point<C> &timestamp,
                          const std::chrono::duration<R, P> &increase) {
  const auto now = C::now();
  int iterations = 0;
  for (; now > timestamp; ++iterations) {
    timestamp += increase;
  }
  if (iterations > 1) {
    warn(__FILE__, "[{}] time violation!", name);
  }
}

void watchdog(const std::string name, std::atomic<bool> &shutdown_required, bi::interprocess_semaphore &mtx,
              std::uint16_t &other_watchdog, std::uint16_t &own_watchdog, next::sleep &s, bool &other_alive) {
  debug(__FILE__, "[{}] starting watchdog", name);
  auto timestamp = std::chrono::high_resolution_clock::now();
  while (other_alive && !shutdown_required) {
    {
      shm_mtx_lock lck(mtx);
      if (other_watchdog == 0) {
        warn(__FILE__, "[{}] other side seems to be dead, closing", name);
        other_alive = false;
        shutdown_required = true;
        break;
      }
      --other_watchdog;
      own_watchdog = watchdog_reset_value;
      // debug(__FILE__, "[{0}] cycles left for other side: {1}", name, other_watchdog);
    }
    const auto now = std::chrono::high_resolution_clock::now();
    int iterations = 0;
    for (; now > timestamp; ++iterations) {
      timestamp += watchdog_delay;
    }
    if (iterations > 1) {
      warn(__FILE__, "[{}] time violation!", name);
    }
    s.wait_until(timestamp);
  }
  {
    shm_mtx_lock lck(mtx);
    own_watchdog = 0;
  }
  std::cout << "watchdog finished" << std::endl;
}

struct subscriber_data {
  eCAL::CSubscriber subscriber;
  std::string topic;
  size_t number{0};
  size_t msgs_received{0};
  size_t expected_msg_number{1};
  subscriber_data(eCAL::CSubscriber &&s, size_t n) : subscriber(std::move(s)), number(n) {
    topic = subscriber.GetTopicName();
    debug(__FILE__, " subscriber for topic {} created", topic);
  }
};

int main(int argc, char **argv) {
  next::sdk::EcalSingleton::register_to_init_chain();
  next::sdk::logger::register_to_init_chain();
  next::sdk::InitChain::ConfigMap initmap;
  argc == 1 ? initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "Performance_Server")
            : initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "Performance_Client");
  argc == 1 ? initmap.emplace(next::sdk::logger::getFilenameCfgKey(), "performance_server.log")
            : initmap.emplace(next::sdk::logger::getFilenameCfgKey(), "performance_client.log");
#ifdef _DEBUG
  initmap.emplace(next::sdk::logger::getFileLoglevelCfgKey(), next::sdk::logger::LOGLEVEL::DEBUG);
  initmap.emplace(next::sdk::logger::getConsoleLoglevelCfgKey(), next::sdk::logger::LOGLEVEL::DEBUG);
#else
  initmap.emplace(next::sdk::logger::getFilenameCfgKey(), next::sdk::logger::LOGLEVEL::INFO);
  initmap.emplace(next::sdk::logger::getConsoleLoglevelCfgKey(), next::sdk::logger::LOGLEVEL::INFO);
#endif
  if (!next::sdk::InitChain::Run(initmap)) {
    std::cerr << "Failed to initialize the application." << std::endl;
    return 1;
  }
  next::sleep s;
  std::atomic<bool> shutdown_required = false;
  next::sdk::shutdown_handler::register_shutdown_handler([&s, &shutdown_required]() {
    s.shutdown();
    shutdown_required = true;
  });
  next::sdk::EcalSingleton::get_instance();

  // common initialization of server and client
  bi::shared_memory_object shm;
  shm_data *shmdata{nullptr};
  bi::remove_shared_memory_on_destroy mem_remover(shm_name);
  bi::mapped_region region;

  if (argc == 1) {
    debug(__FILE__, " Hello :)");

    // initializing the shared memory
    bi::shared_memory_object::remove(shm_name); // remove it if it is already there
    try {
      shm = bi::shared_memory_object(bi::create_only, shm_name, bi::read_write);
      shm.truncate(sizeof(shm_data));
      region = bi::mapped_region(shm, bi::read_write);
      void *shmaddr = region.get_address();
      shmdata = new (shmaddr) shm_data;
      shmdata->server_run_state = run_state::shm_accessable;
    } catch (bi::interprocess_exception &e) {
      error(__FILE__, " got exception during shm initialization: {}", e.what());
      return 1;
    }
    try {
      assert(shmdata);
      bool client_alive = true;

      // launch the client
      std::string cmd(argv[0]);
      cmd += " client";
      // boost::process::spawn(cmd.c_str());

      debug(__FILE__, " waiting for the client");
      while (!shutdown_required) {
        std::cout << "." << std::flush;
        {
          shm_mtx_lock lck(shmdata->mutex);
          if (shmdata->client_run_state == run_state::shm_accessable)
            break;
        }
        s.wait_for(startup_delay);
      }
      if (shutdown_required) {
        return 0;
      }
      debug(__FILE__, " got the info that the client can access the shared memory");

      // setting up and starting up the server
      // first the watchdog
      // loop:
      //   1) constructing publishers
      //   2) signalling that we are ready
      //   3) sending data

      // starting the watchdog
      std::thread dog(watchdog, "ServerWatchdog", std::ref(shutdown_required), std::ref(shmdata->mutex),
                      std::ref(shmdata->client_watchdog), std::ref(shmdata->server_watchdog), std::ref(s),
                      std::ref(client_alive));

      // vector for our publishers
      std::vector<Publisher> publisher;
      publisher.reserve(max_topics);

      for (size_t n_publisher = min_topics; max_topics >= n_publisher && !shutdown_required;
           n_publisher += topics_step) {
        while (publisher.size() < n_publisher) {
          publisher.emplace_back(publisher.size());
        }

        test_payload payload;

        for (size_t msgs_per_topic = min_msgs_per_topic; max_msgs_per_topic >= msgs_per_topic && !shutdown_required;
             msgs_per_topic += msgs_per_topic_step) {
          // tell the client how many msgs we want to send
          debug(__FILE__, " telling client that this run there will be {} msgs", msgs_per_topic);
          {
            shm_mtx_lock lck(shmdata->mutex);
            shmdata->topics = publisher.size();
            shmdata->msgs_per_topic = msgs_per_topic;
            shmdata->server_run_state = run_state::pub_sub_started;
          }
          // preparing the payload and handing it over to the publisher
          payload.cycle = msgs_per_topic;
          payload.data[0] = 0xAB;
          payload.data[1] = 0xCD;
          std::memset(&payload.data[2], 0, payload.data.size() - 2);

          // wait until client is ready
          debug(__FILE__, " waiting for client");
          while (!shutdown_required) {
            std::cout << "." << std::flush;
            if (shmdata->client_run_state == run_state::pub_sub_started) {
              std::cout << "\n";
              break;
            }
            s.wait_for(startup_delay);
          }
          debug(__FILE__, " checking that all subscribers connected");
          for (size_t i = 0; publisher.size() > i && !shutdown_required; ++i) {
            if (publisher[i].subscriber_connected()) {
              // nothing to do
            } else {
              warn(__FILE__, " publisher {} has no subscriber", i);
            }
          }
          if (shutdown_required)
            break;
          debug(__FILE__, " client reported that all subscribers are running");
          debug(__FILE__, " configuring publisher");
          for (auto &p : publisher) {
            p.prepare_send(payload, msgs_per_topic);
          }

          debug(__FILE__, " starting sending {} msgs per topic", msgs_per_topic);
          {
            shm_mtx_lock lck(shmdata->mutex);
            shmdata->time_msg_send_start = std::chrono::high_resolution_clock::now();
            Publisher::start_send();
          }

          for (auto &p : publisher) {
            while (!p.sending_finished() && !shutdown_required) {
              s.wait_for(std::chrono::milliseconds(100));
            }
          }
          shmdata->server_run_state = run_state::msg_sent;

          debug(__FILE__, " messages sent, waiting for client to receive msgs");
          while (!shutdown_required) {
            std::cout << "." << std::flush;
            if (shmdata->client_run_state == run_state::msg_received) {
              break;
            }
            s.wait_for(startup_delay);
          }
          std::cout << "\n";
          if (shutdown_required)
            break;
          debug(__FILE__, " client received everything");
          s.wait_for(inter_iteration_pause);
        }
        debug(__FILE__, " next iteration");
      }

      shutdown_required = true;
      debug(__FILE__, " joining watchdog");
      dog.join();
      debug(__FILE__, " watchdog joined");

      debug(__FILE__, " destroying publisher");
      shmdata->server_ready = false;
      publisher.clear();
      s.wait_for(std::chrono::seconds(3));
      debug(__FILE__, " leaving");

    } catch (bi::interprocess_exception &e) {
      error(__FILE__, " got exception: {}", e.what());
    }
  } else {
    debug(__FILE__, " Hello :)");

    // open the shared memory
    {
      bool shm_opened{false};
      debug(__FILE__, " trying to open the shared memory");
      for (unsigned int attempt = 0u; attempt < shm_create_max_tries; ++attempt) {
        s.wait_for(shm_create_retry_pause);
        try {
          shm = bi::shared_memory_object(bi::open_only, shm_name, bi::read_write);
          region = bi::mapped_region(shm, bi::read_write);
          void *shmaddr = region.get_address();
          shmdata = static_cast<shm_data *>(
              shmaddr); // on client side we are not initializing the memory, casting is sufficient
          shm_opened = true;
          break;
        } catch (bi::interprocess_exception &) {
          debug(__FILE__, "   try {} failed", attempt + 1u);
          continue;
        }
      }
      if (shm_opened) {
        assert(shmdata != nullptr);
        debug(__FILE__, " opened the shared memory");
      } else {
        error(__FILE__, " failed to open the shared memory");
        return 1;
      }
    }

    try {
      assert(shmdata);
      bool server_alive{true};
      std::vector<std::string> writer_header;
      writer_header.push_back("n_topics");
      for (size_t entry = min_msgs_per_topic; max_msgs_per_topic >= entry; entry += msgs_per_topic_step) {
        writer_header.push_back(std::to_string(entry));
      }
      Statswriter writer_time("performance_time.csv", writer_header);
      Statswriter writer_loss("performance_loss.csv", writer_header);

      // telling the server that we are there
      debug(__FILE__, " telling the server that we are ready");
      {
        shm_mtx_lock(shmdata->mutex);
        shmdata->client_watchdog = watchdog_reset_value;
        shmdata->client_run_state = run_state::shm_accessable;
      }
      debug(__FILE__, " told the server to be ready");

      std::thread dog(watchdog, "clientwatchdog", std::ref(shutdown_required), std::ref(shmdata->mutex),
                      std::ref(shmdata->server_watchdog), std::ref(shmdata->client_watchdog), std::ref(s),
                      std::ref(server_alive));

      std::vector<subscriber_data> subscriber;
      subscriber.reserve(max_topics);
      while (!shutdown_required) {
        // now wait for the server to start publishers
        debug(__FILE__, " waiting for server to start publisher");
        while (!shutdown_required && shmdata->server_run_state != run_state::pub_sub_started) {
          std::cout << "," << std::flush;
          s.wait_for(startup_delay);
        }
        if (shutdown_required)
          break;
        debug(__FILE__, " server started publisher");

        for (auto &sub : subscriber) {
          sub.msgs_received = 0;
          sub.expected_msg_number = 1;
        }
        if (subscriber.size() < shmdata->topics) {
          writer_loss.push(std::to_string(shmdata->topics));
          writer_time.push(std::to_string(shmdata->topics));
        }
        for (size_t subs = subscriber.size(); shmdata->topics > subs; ++subs) {
          std::string topic_name = "Topic_" + std::to_string(subs);
          eCAL::STopicInformation topic_info;
          topic_info.type = topic_type;
          topic_info.descriptor = topic_description;
          subscriber.emplace_back(eCAL::CSubscriber(std::string("Topic_") + std::to_string(subs), topic_info),
                                  subs + 1);
          auto &cur_sub = subscriber[subs];
          cur_sub.subscriber.AddReceiveCallback(
              [&sub_info = cur_sub](const char *topic, const struct eCAL::SReceiveCallbackData *data) mutable {
                debug(__FILE__, "[ClientReceiver{0}] callback of subscriber {1} called", sub_info.number,
                      sub_info.topic);
                if (sub_info.topic == topic) {
                  if (data->size == static_cast<int64_t>(sizeof(test_payload))) {
                    const test_payload *payload = static_cast<const test_payload *>(data->buf);
                    if (payload->msg_number == sub_info.expected_msg_number) {
                      sub_info.msgs_received++;
                      debug(__FILE__, "[ClientReceiver{0}] msg looks good", sub_info.number);
                    } else if (payload->msg_number > sub_info.msgs_received + 1) {
                      debug(__FILE__, "[ClientReceiver{0}] message(s) missed", sub_info.number);
                    } else {
                      error(__FILE__, "[ClientReceiver{0}] seems to be out of sync: received {1}, expected {2}",
                            sub_info.number, payload->msg_number, sub_info.expected_msg_number);
                    }
                    sub_info.expected_msg_number = payload->msg_number + 1;
                  } else {
                    error(__FILE__, "[ClientReceiver{0}] received {1} bytes, expected {2}", sub_info.number, data->size,
                          sizeof(test_payload));
                  }
                } else {
                  error(__FILE__, "[ClientReceiver{0}] topic name incorrect, got '{1}', expected '{2}'",
                        sub_info.number, topic, sub_info.topic);
                }
              });
          debug(__FILE__, " created subscriber for topic {}", topic_name);
        }
        assert(subscriber.size() == shmdata->topics);
        debug(__FILE__, " created subscriber");
        s.wait_for(std::chrono::milliseconds(500));
        shmdata->client_run_state = run_state::pub_sub_started;

        debug(__FILE__, " waiting for messages");
        while (!shutdown_required && shmdata->server_run_state != run_state::msg_sent) {
          std::cout << "," << std::flush;
          s.wait_for(startup_delay);
        }
        for (const auto max_time = std::chrono::high_resolution_clock::now() + max_receive_time;
             !shutdown_required && std::chrono::high_resolution_clock::now() < max_time;) {
          bool all_msgs_received = true;
          for (const auto &sub : subscriber) {
            if (sub.msgs_received != shmdata->msgs_per_topic) {
              all_msgs_received = false;
              break;
            }
          }
          if (all_msgs_received) {
            debug(__FILE__, " seems that all subscriber received the messages");
            break;
          }
          std::this_thread::yield();
        }
        if (shutdown_required)
          break;
        {
          shm_mtx_lock lck(shmdata->mutex);
          const auto transmit_time = std::chrono::duration_cast<std::chrono::microseconds>(
                                         std::chrono::high_resolution_clock::now() - shmdata->time_msg_send_start)
                                         .count();
          writer_time.push(std::to_string(transmit_time));
        }
        size_t msgs_received = 0;
        for (size_t i = 0; subscriber.size() > i; ++i) {
          const auto &cur_sub = subscriber[i];
          msgs_received += cur_sub.msgs_received;
          if (cur_sub.msgs_received != shmdata->msgs_per_topic) {
            warn(__FILE__, " result subscriber {0}: {1} of {2} messages lost", i,
                 shmdata->msgs_per_topic - subscriber[i].msgs_received, shmdata->msgs_per_topic);
          } else {
            debug(__FILE__, " result subscriber {0}: 0 of {1} messages lost", i, shmdata->msgs_per_topic);
          }
        }
        const size_t msgs_expected = shmdata->msgs_per_topic * shmdata->topics;
        const size_t msgs_lost = msgs_expected - msgs_received;
        if (msgs_expected == msgs_received) {
          debug(__FILE__, " received all {0} messages", msgs_received);
        } else {
          warn(__FILE__, " lost {0} of {1} messages", msgs_lost, msgs_expected);
        }
        writer_loss.push(std::to_string(msgs_lost));
        shmdata->client_run_state = run_state::msg_received;
      }

      shutdown_required = true;
      debug(__FILE__, " joining watchdog");
      dog.join();
      debug(__FILE__, " watchdog joined");

      shmdata->client_ready = false;
    } catch (bi::interprocess_exception &e) {
      error(__FILE__, " got exception: {}", e.what());
    }
    info(__FILE__, " Shutting down");
  }
  next::sdk::InitChain::Reset();
  debug(__FILE__, "Waiting 2 secs before closing");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  return (0);
}

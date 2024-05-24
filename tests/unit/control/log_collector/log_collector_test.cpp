#include <cip_test_support/gtest_reporting.h>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <regex>
#include <thread>

#include <ecal/ecal.h>

#include <spdlog/details/os.h>
#include <boost/filesystem.hpp>

#include <next/control/log_collector/log_collector.hpp>
#include <next/sdk/events/log_event.hpp>
#include <next/sdk/sdk.hpp>

namespace next {
namespace control {

using namespace next::sdk;

// using namespace next::sdk::testing;
class LogCollectorTest : public testing::TestUsingEcal {
public:
  LogCollectorTest() {
    this->instance_name_ = "LogCollectorTest";
    logger::register_to_init_chain();
  }

  std::string GetLogFileContent(std::string path_to_log_file) {
    std::ifstream log_file(path_to_log_file);

    return std::string(std::istreambuf_iterator<char>(log_file), std::istreambuf_iterator<char>());
  }

  std::regex date_expr{R"(\[[0-9][0-9]-[0-9][0-9]-[0-9][0-9]\]\[[0-9][0-9]:[0-9][0-9]:[0-9][0-9]\.[0-9][0-9][0-9]\])"};

  std::string DefaultLogFileContentStart() {
    const char *log_file_content = "[info][CompA] ---------------------------------------------\n"
                                   "[info][CompA] ############# Combined log file #############\n"
                                   "[info][CompA] ############### START/CONTINUE ##############\n"
                                   "[info][CompA] #############################################\n";

    return std::string(log_file_content);
  }

  std::string DefaultLogFileContentSetup() {
    const char *log_file_content = "[info][CompA] [LogCollector] Setup start\n"
                                   "[info][CompA] [LogCollector] Start periodic flushing file sink\n"
                                   "[info][CompA] [LogCollector] Setup finished\n";

    return std::string(log_file_content);
  }

  std::string DefaultLogFileContentShutdown() {
    const char *log_file_content = "[info][CompA] [LogCollector] Shutdown start\n"
                                   "[info][CompA] [LogCollector] Stopp periodic flushing file sink\n"
                                   "[info][CompA] [LogCollector] Shutdown finished\n";

    return std::string(log_file_content);
  }

  std::string DefaultLogFileContentEnd() {
    const char *log_file_content = "[info][CompA] #############################################\n"
                                   "[info][CompA] ############# Combined log file #############\n"
                                   "[info][CompA] #################### END ####################\n"
                                   "[info][CompA] ---------------------------------------------\n";
    return std::string(log_file_content);
  }

  std::string getLogFileContentReduced(std::string log_file_name) {
    std::string log_file_content = GetLogFileContent(log_file_name);

    std::istringstream f(log_file_content);
    std::string line;
    std::string log_file_content_reduced{""};
    while (std::getline(f, line)) {
      log_file_content_reduced.append(extractAndCheckLogInfos(line) + "\n");
    }
    return log_file_content_reduced;
  }

  std::string extractAndCheckDateAndTime(const std::string &line) {
    std::smatch cm_date;
    EXPECT_TRUE(std::regex_search(line, cm_date, date_expr) && 0 == cm_date.position())
        << "Date and time in log file not as expected.";

    return cm_date.suffix().str();
  }

  std::string extractAndCheckLogInfos(const std::string &line) {
    std::string suffix = extractAndCheckDateAndTime(line);

    std::smatch cm_thread;
    std::string thread_id_text = R"(\[thread )" + std::to_string(spdlog::details::os::thread_id()) + R"(\])";
    EXPECT_TRUE(std::regex_search(suffix, cm_thread, std::regex(thread_id_text)) && 0 == cm_thread.position())
        << "Thread id is not as expected.";

    return cm_thread.suffix().str();
  }
};

TEST_F(LogCollectorTest, Constructor_CreateLogFile) {
  std::string log_file_name = "Constructor_CreateLogFile.log";
  boost::filesystem::remove(log_file_name);
  std::string log_file_name1 = "Constructor_CreateLogFile.1.log";
  boost::filesystem::remove(log_file_name1);

  // create scope so that log_collector deconstructor is called afterwards
  {
    next::control::log_collector::LogCollector log_collector(log_file_name, "CompA",
                                                             next::sdk::logger::LOGLEVEL::DEBUG);
  }
  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }

  std::string log_file_content = GetLogFileContent(log_file_name);
  std::string log_file_content_reduced = getLogFileContentReduced(log_file_name);

  std::string expected_log_file_content =
      DefaultLogFileContentStart() + DefaultLogFileContentShutdown() + DefaultLogFileContentEnd();
  EXPECT_EQ(log_file_content_reduced, expected_log_file_content)
      << "Combined log file doesn't contain the exptected content.";

  // check if new log is created
  // create scope so that log_collector deconstructor is called afterwards
  {
    next::control::log_collector::LogCollector log_collector(log_file_name, "CompA",
                                                             next::sdk::logger::LOGLEVEL::DEBUG);
  }

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }
  if (!boost::filesystem::exists(log_file_name1)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name1 + "' wasn't created.";
  }

  std::string log_file_content_new1 = GetLogFileContent(log_file_name1);

  EXPECT_EQ(log_file_content, log_file_content_new1) << "Rotated log file changed.";

  // check content of new created file again
  std::string log_file_content_reduced_new = getLogFileContentReduced(log_file_name);

  EXPECT_EQ(log_file_content_reduced_new, expected_log_file_content)
      << "Newly created combined log file doesn't contain the exptected content.";
}

TEST_F(LogCollectorTest, SetupAndShutdown) {
  std::string log_file_name = "SetupAndShutdown.log";
  boost::filesystem::remove(log_file_name);

  next::control::log_collector::LogCollector log_collector(log_file_name, "CompA", next::sdk::logger::LOGLEVEL::DEBUG);
  EXPECT_TRUE(log_collector.Setup());
  EXPECT_TRUE(log_collector.Shutdown());

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }

  std::string log_file_content_reduced = getLogFileContentReduced(log_file_name);

  std::string expected_log_file_content =
      DefaultLogFileContentStart() + DefaultLogFileContentSetup() + DefaultLogFileContentShutdown();

  EXPECT_EQ(log_file_content_reduced, expected_log_file_content)
      << "Combined log file doesn't contain the exptected content.";

  // restart log_collector
  EXPECT_TRUE(log_collector.Setup());
  EXPECT_FALSE(log_collector.Setup());
  EXPECT_TRUE(log_collector.Shutdown());
  EXPECT_TRUE(log_collector.Shutdown());

  std::string log_file_content_reduced_updated = getLogFileContentReduced(log_file_name);

  expected_log_file_content = expected_log_file_content + DefaultLogFileContentSetup() +
                              "[info][CompA] [LogCollector] ERROR: Setup already running without calling shutdown\n" +
                              DefaultLogFileContentShutdown() + DefaultLogFileContentShutdown();

  EXPECT_EQ(log_file_content_reduced_updated, expected_log_file_content)
      << "Combined log file doesn't contain the exptected appended content.";
}

TEST_F(LogCollectorTest, CollectLogMessageAndWriteLog) {
  eCAL::Util::EnableLoopback(true);

  std::string log_file_name = "CollectLogMessageAndWriteLog.log";
  boost::filesystem::remove(log_file_name);

  next::control::log_collector::LogCollector log_collector(log_file_name, "CompA", next::sdk::logger::LOGLEVEL::DEBUG);
  EXPECT_TRUE(log_collector.Setup());

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }
  boost::uintmax_t startFileSize = boost::filesystem::file_size(log_file_name);

  next::sdk::events::LogEvent log_event("");

  std::vector<next::sdk::events::LogEventData> log_data_list;
  next::sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::ERR;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  log_event.publishLogEvent(log_data_list);

  int max_wait_iterations = 0;
  bool file_size_changed = false;
  while (max_wait_iterations < 100 && !file_size_changed) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    ++max_wait_iterations;

    if (boost::filesystem::file_size(log_file_name) > startFileSize) {
      file_size_changed = true;
    }
  }
  ASSERT_TRUE(file_size_changed) << "LogEvent wasn't added to log file.";

  EXPECT_TRUE(log_collector.Shutdown());

  std::string log_file_content = GetLogFileContent(log_file_name);
  std::istringstream f(log_file_content);
  std::string line;
  std::string log_file_content_reduced{""};
  while (std::getline(f, line)) {
    // remove only additional information from header and footer
    if (line.find("ComponentName") == std::string::npos) {
      log_file_content_reduced.append(extractAndCheckLogInfos(line) + "\n");
    } else {
      log_file_content_reduced.append(extractAndCheckDateAndTime(line) + "\n");
    }
  }

  std::string expected_log_file_content =
      DefaultLogFileContentStart() + DefaultLogFileContentSetup() + "[thread " +
      std::to_string(log_data_list[0].thread_id) + "][" +
      next::sdk::logger::kLogLevelLowerCase.at(log_data_list[0].log_level) + "][" + log_data_list[0].component_name +
      "] " + log_data_list[0].log_message + "\n" + "[thread " + std::to_string(log_data_list[1].thread_id) + "][" +
      next::sdk::logger::kLogLevelLowerCase.at(log_data_list[1].log_level) + "][" + log_data_list[1].component_name +
      "] " + log_data_list[1].log_message + "\n" + "[thread " + std::to_string(log_data_list[2].thread_id) + "][" +
      next::sdk::logger::kLogLevelLowerCase.at(log_data_list[2].log_level) + "][" + log_data_list[2].component_name +
      "] " + log_data_list[2].log_message + "\n" + DefaultLogFileContentShutdown();

  EXPECT_EQ(log_file_content_reduced, expected_log_file_content)
      << "Combined log file doesn't contain the exptected content.";
}

TEST_F(LogCollectorTest, CollectMultipleLogMessageAndWriteLog) {
  eCAL::Util::EnableLoopback(true);

  std::string log_file_name = "CollectMultipleLogMessageAndWriteLog.log";
  boost::filesystem::remove(log_file_name);

  next::control::log_collector::LogCollector log_collector(log_file_name, "CompA", next::sdk::logger::LOGLEVEL::DEBUG);
  EXPECT_TRUE(log_collector.Setup());

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }
  boost::uintmax_t startFileSize = boost::filesystem::file_size(log_file_name);

  std::condition_variable cv;
  std::mutex mutex_cv;
  bool publish = false;

  std::vector<std::thread> event_publisher_thread;
  unsigned int magic_number_threads = 20;

  std::map<std::string, unsigned int> thread_id_list;

  for (unsigned int i = 0; i < magic_number_threads; ++i) {
    thread_id_list[std::to_string(i)] = i;

    event_publisher_thread.push_back(std::thread{[&, i]() {
      next::sdk::events::LogEvent log_event("");

      std::vector<next::sdk::events::LogEventData> log_data_list;
      next::sdk::events::LogEventData log_data;
      log_data.time_stamp = i * 1000000000000000;
      log_data.component_name = "ComponentName";
      log_data.channel_name = "ChannelName";
      log_data.log_level = next::sdk::logger::LOGLEVEL::DEBUG;
      log_data.log_message = "LogMessage 0";
      log_data.thread_id = i;
      log_data_list.push_back(log_data);

      log_data.time_stamp = log_data.time_stamp + 100000000000000;
      log_data.log_message = "LogMessage 1";
      log_data_list.push_back(log_data);

      log_data.time_stamp = log_data.time_stamp + 200000000000000;
      log_data.log_message = "LogMessage 2";
      log_data_list.push_back(log_data);

      // wait till all threads are setup to publish in parallel
      std::unique_lock<std::mutex> lock(mutex_cv);
      cv.wait(lock, [&] { return publish; });
      log_event.publishLogEvent(log_data_list);
    }});
  }

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(2s);

  publish = true;
  cv.notify_all();

  for (auto &event_publisher : event_publisher_thread) {
    event_publisher.join();
  }

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(2s);

  if (boost::filesystem::file_size(log_file_name) <= startFileSize) {
    ASSERT_TRUE(false) << "LogEvent wasn't added to log file.";
  }

  std::string log_file_content = GetLogFileContent(log_file_name);
  std::istringstream f(log_file_content);
  std::string line;
  std::string log_file_content_reduced{""};

  for (int i = 0; i < 7; ++i) {
    std::getline(f, line);
    log_file_content_reduced.append(extractAndCheckLogInfos(line) + "\n");
  }

  EXPECT_EQ(log_file_content_reduced, DefaultLogFileContentStart() + DefaultLogFileContentSetup())
      << "Combined log file doesn't contain the exptected start content.";

  unsigned int counter = 0;
  while (!f.eof()) {
    std::string thread_id = "-1";
    for (int i = 0; i < 3; ++i) {
      std::getline(f, line);
      if (line.empty() && counter == magic_number_threads * 3) {
        break;
      }
      ++counter;

      std::smatch cm_date;
      EXPECT_TRUE(std::regex_search(line, cm_date, date_expr) && 0 == cm_date.position())
          << "Date and time in log file not as expected.";

      std::smatch cm_thread;
      std::string suffix = cm_date.suffix();
      std::regex_search(suffix, cm_thread, std::regex(R"(\[thread [0-9]{1,2}\])"));
      EXPECT_EQ(0, cm_thread.position()) << "thread id not found";

      std::smatch cm_thread_id;
      std::string thread_match = cm_thread[0];
      std::regex_search(thread_match, cm_thread_id, std::regex(R"([0-9]{1,2})"));

      if (0 == i) {
        thread_id = cm_thread_id[0];
        auto it = thread_id_list.find(thread_id);
        ASSERT_NE(it, thread_id_list.end()) << "Thread Id not found";
        thread_id_list.erase(it);
      } else {
        EXPECT_EQ(thread_id, cm_thread_id[0]);
      }

      std::smatch cm_log_level;
      suffix = cm_thread.suffix();
      std::regex_search(suffix, cm_log_level, std::regex(R"(\[debug\])"));
      EXPECT_EQ(0, cm_log_level.position()) << "log level debug not found";

      std::smatch cm_comp;
      suffix = cm_log_level.suffix();
      std::regex_search(suffix, cm_comp, std::regex(R"(\[ComponentName\])"));
      EXPECT_EQ(0, cm_comp.position()) << "component name not found";

      std::smatch cm_log_message;
      suffix = cm_comp.suffix();
      std::regex_search(suffix, cm_log_message, std::regex(R"( LogMessage )"));
      EXPECT_EQ(0, cm_log_message.position()) << "log message not found";

      suffix = cm_log_message.suffix();
      EXPECT_EQ(std::to_string(i), suffix) << "log message for one log event not in correct order";
    }
  }

  EXPECT_EQ(0, thread_id_list.size()) << "Not all threads were added to log file";
}

class logEventDataProcessing {
public:
  logEventDataProcessing(const std::vector<sdk::events::LogEventData> &log_data_list_expected)
      : log_data_list_expected_(log_data_list_expected) {}

  void logEventDataProcessingHook(const std::vector<sdk::events::LogEventData> &log_content) {
    EXPECT_EQ(log_data_list_expected_.size(), log_content.size());

    auto it_log_content = log_content.begin();
    for (const auto &log_event_data : log_data_list_expected_) {
      EXPECT_EQ(log_event_data.channel_name, it_log_content->channel_name)
          << "Log content channel_name doesn't match expected one.";
      EXPECT_EQ(log_event_data.component_name, it_log_content->component_name)
          << "Log content component_name doesn't match expected one.";
      EXPECT_EQ(log_event_data.log_level, it_log_content->log_level)
          << "Log content log_level doesn't match expected one.";
      EXPECT_EQ(log_event_data.log_message, it_log_content->log_message)
          << "Log content log_message doesn't match expected one.";
      EXPECT_EQ(log_event_data.thread_id, it_log_content->thread_id)
          << "Log content thread_id doesn't match expected one.";
      EXPECT_EQ(log_event_data.time_stamp, it_log_content->time_stamp)
          << "Log content time_stamp doesn't match expected one.";

      ++it_log_content;
    }
    finished.store(true);
  }

  std::atomic_bool finished{false};
  std::vector<sdk::events::LogEventData> log_data_list_expected_;
};

TEST_F(LogCollectorTest, CollectLogMessageAndTriggerHook) {
  eCAL::Util::EnableLoopback(true);

  std::string log_file_name = "CollectLogMessageAndTriggerHook.log";
  boost::filesystem::remove(log_file_name);

  next::control::log_collector::LogCollector log_collector(log_file_name, "CompA", next::sdk::logger::LOGLEVEL::DEBUG);
  EXPECT_TRUE(log_collector.Setup());

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }

  next::sdk::events::LogEvent log_event("");

  std::vector<next::sdk::events::LogEventData> log_data_list;
  next::sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::ERR;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  logEventDataProcessing log_event_data_processing(log_data_list);

  auto binded_callback =
      std::bind(&logEventDataProcessing::logEventDataProcessingHook, &log_event_data_processing, std::placeholders::_1);

  log_collector.AddLogEventDataProcessingHook(binded_callback);

  log_event.publishLogEvent(log_data_list);

  int max_wait_iterations = 0;
  bool log_event_data_processing_finished = false;
  while (max_wait_iterations < 100 && !log_event_data_processing_finished) {

    if (log_event_data_processing.finished.load()) {
      log_event_data_processing_finished = true;
      break;
    }
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    ++max_wait_iterations;
  }

  EXPECT_TRUE(log_event_data_processing_finished) << "Log event data processing didn't finished in the expected time.";
}

TEST_F(LogCollectorTest, FlushLogsWhenLogLevelHigherInLogMessagesThanLogCollectorWithoutPeriodicFlushing) {
  eCAL::Util::EnableLoopback(true);

  std::string log_file_name = "CollectLogMessageAndTriggerHook.log";
  boost::filesystem::remove(log_file_name);

  next::control::log_collector::LogCollector log_collector(log_file_name, "CompA", next::sdk::logger::LOGLEVEL::DEBUG,
                                                           std::chrono::seconds::zero());
  EXPECT_TRUE(log_collector.Setup());

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }

  boost::uintmax_t startFileSize = boost::filesystem::file_size(log_file_name);

  next::sdk::events::LogEvent log_event("");

  std::vector<next::sdk::events::LogEventData> log_data_list;
  next::sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::INFO;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::ERR;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  log_event.publishLogEvent(log_data_list);

  int max_wait_iterations = 0;
  bool file_size_changed = false;
  while (max_wait_iterations < 100 && !file_size_changed) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    ++max_wait_iterations;

    if (boost::filesystem::file_size(log_file_name) > startFileSize) {
      file_size_changed = true;
    }
  }
  ASSERT_TRUE(file_size_changed) << "LogEvent was added to log file.";

  EXPECT_TRUE(log_collector.Shutdown());
}

TEST_F(LogCollectorTest, FlushLogsWhenWaitTimerIsExpired) {

  eCAL::Util::EnableLoopback(true);
  std::string log_file_name = "CollectLogMessageAndTriggerHook.log";
  boost::filesystem::remove(log_file_name);
  int waiting_time = 5;

  next::control::log_collector::LogCollector log_collector(log_file_name, "CompA", next::sdk::logger::LOGLEVEL::ERR,
                                                           std::chrono::seconds(waiting_time));
  EXPECT_TRUE(log_collector.Setup());

  if (!boost::filesystem::exists(log_file_name)) {
    ASSERT_TRUE(false) << "LogFile: '" + log_file_name + "' wasn't created.";
  }

  boost::uintmax_t startFileSize = boost::filesystem::file_size(log_file_name);

  next::sdk::events::LogEvent log_event("");

  std::vector<next::sdk::events::LogEventData> log_data_list;
  next::sdk::events::LogEventData log_data;
  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName";
  log_data.channel_name = "ChannelName";
  log_data.log_level = next::sdk::logger::LOGLEVEL::INFO;
  log_data.log_message = "LogMessage";
  log_data.thread_id = 1;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 9999999999999999;
  log_data.component_name = "ComponentName2";
  log_data.channel_name = "ChannelName2";
  log_data.log_level = next::sdk::logger::LOGLEVEL::INFO;
  log_data.log_message = "LogMessage2";
  log_data.thread_id = 2;
  log_data_list.push_back(log_data);

  log_data.time_stamp = 111111111111111;
  log_data.component_name = "ComponentName3";
  log_data.channel_name = "ChannelName3";
  log_data.log_level = next::sdk::logger::LOGLEVEL::WARN;
  log_data.log_message = "LogMessage3";
  log_data.thread_id = 3;
  log_data_list.push_back(log_data);

  log_event.publishLogEvent(log_data_list);

  int max_wait_iterations = 0;
  bool file_size_changed = false;
  while (max_wait_iterations < 100 && !file_size_changed) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    ++max_wait_iterations;

    if ((max_wait_iterations >= waiting_time) && boost::filesystem::file_size(log_file_name) > startFileSize) {
      file_size_changed = true;
    }
  }
  ASSERT_TRUE(file_size_changed) << "LogEvent was added to log file.";

  EXPECT_TRUE(log_collector.Shutdown());
}

} // namespace control
} // namespace next

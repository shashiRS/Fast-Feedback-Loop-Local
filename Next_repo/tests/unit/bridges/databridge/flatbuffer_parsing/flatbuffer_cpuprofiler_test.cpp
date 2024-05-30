#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <flatbuffers/flatbuffers.h>
#include <next/bridgesdk/schema/profiler/basic_info_generated.h>
#include <next/bridgesdk/schema/profiler/cpu_profiler_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(FBS_CPU_PROFILER_TEST, map_supported_string_to_supportedFBS_enum) {
  std::pair<std::string, std::string> test_input{"CPU_Profiler", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::CPU_Profiler);
}

TEST(FBS_CPU_PROFILER_TEST, cpuprofiler_fbs_to_JSON_data) {
  outputmanager::PluginDataClassParser data_class_parser;
  flatbuffers::FlatBufferBuilder builder;

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "profiler";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "dummy";

  std::pair<std::string, std::string> structure_version_pair{"CPU_Profiler", "0.0.1"};
  std::vector<size_t> session_ids = {};

  std::string str_name = "cpu_profiler_test";
  auto name = builder.CreateString(str_name);
  auto status = builder.CreateString("Running");
  auto timestamp = 1000u;
  auto location = builder.CreateString("C:");
  auto basic_information = profiler::CreateBasicInformation(builder, name, status, timestamp, location);

  std::vector<flatbuffers::Offset<GuiSchema::CpuUsage>> cpu_usage_all;
  uint8_t core = 1;
  float usage = 0.5;
  auto cpu_information_single = GuiSchema::CreateCpuUsage(builder, core, usage);
  cpu_usage_all.push_back(cpu_information_single);
  auto cpu_information = builder.CreateVector(cpu_usage_all);
  // Converting the usage argument into a string for verification
  std::stringstream stream;
  stream << std::fixed << std::setprecision(1) << usage;
  std::string rounded_usage = stream.str();
  std::vector<flatbuffers::Offset<GuiSchema::Threads>> threads_vector;

  uint64_t tid = 15;
  uint8_t tcore = 2;
  float tusage = 0.7f;
  auto tmp_thread = GuiSchema::CreateThreads(builder, tid, tcore, tusage);
  threads_vector.push_back(tmp_thread);
  auto thread_information = builder.CreateVector(threads_vector);

  std::vector<flatbuffers::Offset<GuiSchema::CpuProfiler>> cpu_profiler_information;
  uint64_t pid = 123;
  uint8_t thread_count = 3;
  auto cpu_profiler =
      GuiSchema::CreateCpuProfiler(builder, basic_information, pid, cpu_information, thread_information, thread_count);
  cpu_profiler_information.push_back(cpu_profiler);

  auto processes = builder.CreateVector(cpu_profiler_information);
  auto cpu_profiler_list = GuiSchema::CreateCpuProfilerList(builder, processes);
  builder.Finish(cpu_profiler_list);
  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "cpu_profiler.fbs";
  std::string cache_name = "test";

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  EXPECT_EQ(result.substr(result.find("name") + 8, str_name.length()), str_name);
  EXPECT_EQ(result.substr(result.find("usage") + 8, rounded_usage.length()), rounded_usage);
  EXPECT_EQ(result.substr(result.find("tid") + 6, std::to_string(tid).length()), std::to_string(tid));
}

TEST(FBS_CPU_PROFILER_TEST, wrap_JSON_data_on_flatbuffer) {
  outputmanager::PluginDataClassParser data_class_parser;
  flatbuffers::FlatBufferBuilder builder;

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "profiler";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "dummy";

  std::pair<std::string, std::string> structure_version_pair{"CPU_Profiler", "0.0.1"};
  std::vector<size_t> session_ids = {};

  auto name = builder.CreateString("cpu_profiler_test");
  auto status = builder.CreateString("Running");
  auto timestamp = 1000u;
  auto location = builder.CreateString("C:");
  auto basic_information = profiler::CreateBasicInformation(builder, name, status, timestamp, location);

  std::vector<flatbuffers::Offset<GuiSchema::CpuUsage>> cpu_usage_all;
  uint8_t core = 1;
  float usage = 0.5f;
  auto cpu_information_single = GuiSchema::CreateCpuUsage(builder, core, usage);
  cpu_usage_all.push_back(cpu_information_single);
  auto cpu_information = builder.CreateVector(cpu_usage_all);

  std::vector<flatbuffers::Offset<GuiSchema::Threads>> threads_vector;
  uint64_t tid = 15u;
  uint8_t tcore = 2u;
  float tusage = 0.7f;
  auto tmp_thread = GuiSchema::CreateThreads(builder, tid, tcore, tusage);
  threads_vector.push_back(tmp_thread);
  auto thread_information = builder.CreateVector(threads_vector);

  std::vector<flatbuffers::Offset<GuiSchema::CpuProfiler>> cpu_profiler_information;
  uint64_t pid = 123u;
  uint8_t thread_count = 3;
  auto cpu_profiler =
      GuiSchema::CreateCpuProfiler(builder, basic_information, pid, cpu_information, thread_information, thread_count);
  cpu_profiler_information.push_back(cpu_profiler);

  auto processes = builder.CreateVector(cpu_profiler_information);
  auto cpu_profiler_list = GuiSchema::CreateCpuProfilerList(builder, processes);
  builder.Finish(cpu_profiler_list);
  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "cpu_profiler.fbs";
  std::string cache_name = "test";

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  std::string expected_wrapper = "{\"dataStream\":{\"profiler\":{\"cpuProfiler\":{\n  \"processes\": ";
  EXPECT_TRUE(result.find(expected_wrapper) == 0) << " Wrapped Json does not start with expected Wrapper\n"
                                                  << " Expected: " << expected_wrapper << "\n Actual:   " << result;
}

} // namespace databridge
} // namespace next

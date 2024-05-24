#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <flatbuffers/flatbuffers.h>
#include <next/bridgesdk/schema/profiler/basic_info_generated.h>
#include <next/bridgesdk/schema/profiler/cpu_profiler_generated.h>

#include "custompointcloud_generated.h"

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(DATA_MANAGER, map_not_supported_string_to_supportedFBS_enum) {
  std::pair<std::string, std::string> test_input{"", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto unsupported_enum_value = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(unsupported_enum_value, SupportedFbsTypes::supportedFBS::UNIDENTIFIED);
}

TEST(DATA_MANAGER, throw_error_on_invalid_structure) {
  outputmanager::PluginDataClassParser data_class_parser;
  flatbuffers::FlatBufferBuilder builder;

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "profiler";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "dummy";

  std::pair<std::string, std::string> structure_version_pair{"", "0.0.1"};

  auto name = builder.CreateString("cpu_profiler_test");
  auto status = builder.CreateString("Running");
  auto timestamp = 1000u;
  auto location = builder.CreateString("C:");
  auto basic_information = profiler::CreateBasicInformation(builder, name, status, timestamp, location);

  std::vector<flatbuffers::Offset<GuiSchema::CpuUsage>> cpu_usage_all;
  uint8_t core = 1u;
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
  uint8_t thread_count = 3u;
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
  std::string result;
  EXPECT_THROW(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair),
      std::invalid_argument);
}

bridgesdk::plugin::FlatbufferPackage createDummyPointcloudFbs(flatbuffers::FlatBufferBuilder &builder) {
  builder.ForceDefaults(true);

  std::vector<float> points = {15.0f, 2.0f, 5.0f, 3.0f, 5.0f, -1.0f};
  std::vector<float> points_colors = {0.1f, 0.1f, 0.9f, 0.1f, 0.1f, 0.9f};
  auto extra_info = test::CreateExtraInfo(builder, 1, 3);
  std::vector<flatbuffers::Offset<test::ExtraInfo>> extra_info_vec;
  extra_info_vec.push_back(extra_info);
  auto point_cloud = test::CreatePointCloudDirect(builder, &points, &points_colors, "#FF6600", 4, &extra_info_vec);
  std::vector<flatbuffers::Offset<test::PointCloud>> point_cloud_list;
  point_cloud_list.push_back(point_cloud);
  auto fbs_wrapper = test::CreatePointCloudWrapperDirect(builder, &point_cloud_list);
  builder.Finish(fbs_wrapper);

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  return flatbuffer;
}

TEST(DATA_MANAGER, send_custom_fbs_full_meta_data) {
  outputmanager::PluginDataClassParser data_class_parser;

  flatbuffers::FlatBufferBuilder builder;
  auto flatbuffer = createDummyPointcloudFbs(builder);
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "custompointcloud.fbs";

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";
  std::pair<std::string, std::string> structure_version_pair{"pointClouds", "0.0.1"};

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  std::string json_test_string = R"(
  {
    "dataStream":
    {
      "objectData":
      {
        "pointClouds":
        {
          "cache_sensor_1":
          [
            {
              "color" : "#FF6600",
              "points" : [15.0, 2.0, 5.0, 3.0, 5.0, -1.0],
              "pointColors" : [0.1, 0.1, 0.9, 0.1, 0.1, 0.9],
              "sensorUid": 4,
              "pointsExtraInfo":
              [
                {
                  "someData": 1,
                  "someOtherData": 3
                }
              ]
            }
          ]
        }
      }
    }
  })";

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(result, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  Json::Value reference_input;
  parseSuccess = reader.parse(json_test_string, reference_input, false);
  ASSERT_EQ(parseSuccess, true) << "parsing reference_input object failed";
  ASSERT_EQ(reference_input.isMember("dataStream"), true) << "reference_input has no dataStream";
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "plugin has no dataStream";

  ASSERT_EQ(reference_input["dataStream"].isMember("objectData"), true) << "reference_input has no objectData";
  ASSERT_EQ(plugin_output["dataStream"].isMember("objectData"), true) << "plugin has no objectData";

  ASSERT_EQ(reference_input["dataStream"]["objectData"].isMember("pointClouds"), true)
      << "reference_input has no pointClouds";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"].isMember("pointClouds"), true) << "plugin has no pointClouds";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["pointClouds"].isMember("cache_sensor_1"), true)
      << "reference_input has no cache_sensor_1";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"].isMember("cache_sensor_1"), true)
      << "plugin has no cache_sensor_1";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"][0].getMemberNames()) {
    ASSERT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"][0].isMember(member_name), true)
        << "values are not available for " << member_name;
    EXPECT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"][0][member_name],
              reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"][0][member_name])
        << "values are not matching for " << member_name;
  }
}

} // namespace databridge
} // namespace next

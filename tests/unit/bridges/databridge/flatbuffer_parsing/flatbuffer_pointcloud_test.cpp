#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <next/bridgesdk/schema/3d_view/pointcloud_generated.h>
#include <next/bridgesdk/support_fbs_types.hpp>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, map_supported_enum_check_pointcloud) {
  std::pair<std::string, std::string> test_input{"PointCloud", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::PointCloud);
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, pointcloud_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  std::string test = R"(
  {
    "dataStream":
    {
      "objectData":
      {
        "pointClouds":
        {
          "cache_sensor_1":
          {
            "metaData":
            {
              "sensorId": 2,
              "timeStamp": 891.912
            },
            "data":
            [
              {
                "color" : "#FF6600",
                "points" : [15.0, 2.0, 5.0, 3.0, 5.0, -1.0],
                "pointColors" : [0.1, 0.1, 0.9, 0.1, 0.1, 0.9]
              }
            ]
          }
        }
      }
    }
  })";

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);

  std::vector<float> point_color = {0.1f, 0.1f, 0.9f, 0.1f, 0.1f, 0.9f};
  std::vector<float> points = {15.0f, 2.0f, 5.0f, 3.0f, 5.0f, -1.0f};

  auto point_cloud = GuiSchema::CreatePointCloudDirect(builder, &points, &point_color, "#FF6600");

  std::vector<flatbuffers::Offset<GuiSchema::PointCloud>> point_cloud_list;
  point_cloud_list.push_back(point_cloud);

  auto fbs_pointcloud_list = GuiSchema::CreatePointCloudListDirect(builder, &point_cloud_list);
  builder.Finish(fbs_pointcloud_list);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  std::pair<std::string, std::string> structure_version_pair{"PointCloud", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "pointcloud.fbs";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(result, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  Json::Value reference_input;
  parseSuccess = reader.parse(test, reference_input, false);
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

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"].isMember("data"), true)
      << "reference_input has no data";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"].isMember("data"), true)
      << "plugin has no data";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"].isArray(), true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"].size(), 1u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"].isArray(), true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"].size(), 1u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"][0].getMemberNames()) {
    ASSERT_EQ(
        plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"][0].isMember(member_name),
        true)
        << "values are not available for " << member_name;
    EXPECT_EQ(plugin_output["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"][0][member_name],
              reference_input["dataStream"]["objectData"]["pointClouds"]["cache_sensor_1"]["data"][0][member_name])
        << "values are not matching for " << member_name;
  }
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, pointcloud_fbs_to_fbs) {
  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);

  std::vector<float> point_color = {0.1f, 0.1f, 0.9f, 0.1f, 0.1f, 0.9f};
  std::vector<float> points = {15.0f, 2.0f, 5.0f, 3.0f, 5.0f, -1.0f};

  const char *color{"#FF6600"};
  auto point_cloud = GuiSchema::CreatePointCloudDirect(builder, &points, &point_color, color);

  std::vector<flatbuffers::Offset<GuiSchema::PointCloud>> point_cloud_list;
  point_cloud_list.push_back(point_cloud);

  auto fbs_pointcloud_list = GuiSchema::CreatePointCloudListDirect(builder, &point_cloud_list);
  builder.Finish(fbs_pointcloud_list);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result_fbs;
  std::pair<std::string, std::string> structure_version_pair{"PointCloud", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;
  data_class_parser.parseDataClassToFlatbuffer(result_fbs, flatbuffer, 1, sensor_info, structure_version_pair);
  size_t offset = 13;
  auto fbs_binary = (void *)&(result_fbs.data()[offset]);
  auto view = GuiSchema::GetViewUpdate(fbs_binary);
  EXPECT_EQ(view->point_clouds()->pointClouds()->size(), 1u);
  EXPECT_EQ(view->meta()->cache_name()->str(), "cache_sensor_1");

  auto point_clouds = view->point_clouds()->pointClouds()->Get(0)->points();
  for (flatbuffers::uoffset_t i = 0; i < point_clouds->size(); ++i) {
    EXPECT_FLOAT_EQ(points[i], point_clouds->Get(i));
  }

  auto point_clouds_colors = view->point_clouds()->pointClouds()->Get(0)->pointColors();
  for (flatbuffers::uoffset_t i = 0; i < point_clouds_colors->size(); ++i) {
    EXPECT_FLOAT_EQ(point_color[i], point_clouds_colors->Get(i));
  }

  auto point_clouds_color = view->point_clouds()->pointClouds()->Get(0)->color()->data();
  EXPECT_STREQ(color, point_clouds_color);
}

} // namespace databridge
} // namespace next

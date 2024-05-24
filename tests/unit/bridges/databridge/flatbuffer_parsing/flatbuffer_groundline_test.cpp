#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <flatbuffers/flatbuffers.h>
#include <next/bridgesdk/schema/3d_view/groundline_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, map_supported_enum_check_groundlines) {
  std::pair<std::string, std::string> test_input{"GroundLines", "0.0.1"};

  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::GroundLines);
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, groundlines_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  std::vector<float> pos_x = {0.0, 10.0, 20.0};
  std::vector<float> pos_y = {0.0, 0.0, 0.0};
  auto point_x = builder.CreateVector(pos_x.data(), pos_x.size());
  auto point_y = builder.CreateVector(pos_y.data(), pos_y.size());

  auto ground_line = GuiSchema::CreateGroundLine(builder, 1, point_x, point_y, false);

  std::vector<flatbuffers::Offset<GuiSchema::GroundLine>> ground_line_list;
  ground_line_list.push_back(ground_line);
  auto vectortest = builder.CreateVector(ground_line_list);
  auto ground_lines = CreateGroundLineList(builder, vectortest);
  builder.Finish(ground_lines);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  std::pair<std::string, std::string> structure_version_pair{"GroundLines", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "groundline.fbs";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result;
  ASSERT_TRUE(
      data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info, structure_version_pair));

  std::string test = R"(
  {
    "dataStream": {
      "objectData": {
        "groundLines": {
          "cache_sensor_1": {
            "metaData": {
              "sensorId": 1,
              "timeStamp": 9123.1
            },
            "data": [
              {
                "id": 1,
                "dashed": false,
                "pointsX": [0.0,10.0,20.0],
                "pointsY": [0.0,0.0,0.0]
              }
            ]
          }
        }
      }
    }
  })";
  Json::Reader reader;
  Json::Value reference_input;

  bool parseSuccess = reader.parse(test, reference_input, false);
  ASSERT_EQ(parseSuccess, true) << "parsing reference_input object failed";

  Json::Value plugin_output;

  parseSuccess = reader.parse(result, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  auto test12 = reference_input.getMemberNames();
  auto test123 = plugin_output.getMemberNames();
  ASSERT_EQ(reference_input.isMember("dataStream"), true) << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "plugin has no Traffic participants";
  ASSERT_EQ(reference_input["dataStream"].isMember("objectData"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"].isMember("objectData"), true) << "plugin has no Traffic participants";
  ASSERT_EQ(reference_input["dataStream"]["objectData"].isMember("groundLines"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"].isMember("groundLines"), true)
      << "plugin has no Traffic participants";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["groundLines"].isMember("cache_sensor_1"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["groundLines"].isMember("cache_sensor_1"), true)
      << "plugin has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"].isMember("data"), true)
      << "plugin has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"].isMember("data"), true)
      << "plugin has no Traffic participants";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"].isArray(), true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"].size(), 1u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"].isArray(), true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"].size(), 1u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"][0].getMemberNames()) {
    ASSERT_EQ(
        plugin_output["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"][0].isMember(member_name),
        true)
        << "values are not available for " << member_name;
    EXPECT_EQ(plugin_output["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"][0][member_name],
              reference_input["dataStream"]["objectData"]["groundLines"]["cache_sensor_1"]["data"][0][member_name])
        << "values are not matching for " << member_name;
  }
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, groundlines_fbs_to_fbs) {
  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  std::vector<float> pos_x = {0.0, 10.0, 20.0};
  std::vector<float> pos_y = {0.0, 0.0, 0.0};
  auto point_x = builder.CreateVector(pos_x.data(), pos_x.size());
  auto point_y = builder.CreateVector(pos_y.data(), pos_y.size());

  auto ground_line = GuiSchema::CreateGroundLine(builder, 1, point_x, point_y, false);

  std::vector<flatbuffers::Offset<GuiSchema::GroundLine>> ground_line_list;
  ground_line_list.push_back(ground_line);
  auto serialized_vector = builder.CreateVector(ground_line_list);
  auto ground_lines = CreateGroundLineList(builder, serialized_vector);
  builder.Finish(ground_lines);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result_fbs;
  std::pair<std::string, std::string> structure_version_pair{"GroundLines", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;
  data_class_parser.parseDataClassToFlatbuffer(result_fbs, flatbuffer, 1, sensor_info, structure_version_pair);
  size_t offset = 13;
  auto fbs_binary = (void *)&(result_fbs.data()[offset]);
  auto view = GuiSchema::GetViewUpdate(fbs_binary);
  EXPECT_EQ(view->ground_lines()->groundLines()->size(), 1u);
  EXPECT_EQ(view->meta()->cache_name()->str(), "cache_sensor_1");

  auto ground_lines_x = view->ground_lines()->groundLines()->Get(0)->pointsX();
  for (flatbuffers::uoffset_t i = 0; i < ground_lines_x->size(); ++i) {
    EXPECT_FLOAT_EQ(pos_x[i], ground_lines_x->Get(i));
  }

  auto ground_lines_y = view->ground_lines()->groundLines()->Get(0)->pointsY();
  for (flatbuffers::uoffset_t i = 0; i < ground_lines_y->size(); ++i) {
    EXPECT_FLOAT_EQ(pos_y[i], ground_lines_y->Get(i));
  }
}

} // namespace databridge
} // namespace next

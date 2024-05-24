#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <next/bridgesdk/schema/3d_view/primitive_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(PRIMITIVE_FBS_CHECK, map_supported_enum_check_primitive) {
  std::pair<std::string, std::string> test_input{"Primitives", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::Primitive);
}

TEST(PRIMITIVE_FBS_CHECK, line_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  std::string test = R"(
  {
    "dataStream":
    {
      "objectData":
      {
        "primitives":
        {
          "cache_sensor_1": {
            "metaData":
            {
              "sensorId": 1,
              "timeStamp": 891.912
            },
            "data":
            {
              "lines":
              [
                {
                  "basicInfo" :
                  {
                    "type":"line",
                    "color": "123",
                    "id": 1,
                    "opacity": 1.0
                  },
                  "points" :
                  [
                    {
                      "x" : 1.0,
                      "y" : 2.0,
                      "z" : 3.0
                    },
                    {
                      "x" : 2.0,
                      "y" : 4.0,
                      "z" : 6.0
                    },
                    {
                      "x" : 3.0,
                      "y" : 6.0,
                      "z" : 9.0
                    },
                    {
                      "x" : 4.0,
                      "y" : 8.0,
                      "z" : 12.0
                    }
                  ]
                },
                {
                  "basicInfo" :
                  {
                    "type":"line",
                    "color": "123",
                    "id": 1,
                    "opacity": 1.0
                  },
                  "points" :
                  [
                    {
                      "x" : 11.0,
                      "y" : 22.0,
                      "z" : 33.0
                    },
                    {
                      "x" : 12.0,
                      "y" : 24.0,
                      "z" : 36.0
                    },
                    {
                      "x" : 13.0,
                      "y" : 26.0,
                      "z" : 39.0
                    },
                    {
                      "x" : 14.0,
                      "y" : 28.0,
                      "z" : 312.0
                    }
                  ]
                }
              ]              
            }
          }
        }
      }
    }
  })";

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  std::vector<GuiSchema::XYZ> shape_points;
  shape_points.push_back(GuiSchema::XYZ(1.0, 2.0, 3.0));
  shape_points.push_back(GuiSchema::XYZ(2.0, 4.0, 6.0));
  shape_points.push_back(GuiSchema::XYZ(3.0, 6.0, 9.0));
  shape_points.push_back(GuiSchema::XYZ(4.0, 8.0, 12.0));

  auto basic_info = GuiSchema::CreatePrimitiveBasicDirect(builder, GuiSchema::PrimitiveType::line, 1, "123", 1.0);
  auto line = GuiSchema::CreateLineDirect(builder, basic_info, &shape_points);

  std::vector<flatbuffers::Offset<GuiSchema::Line>> line_list;
  line_list.push_back(line);

  shape_points.clear();
  shape_points.push_back(GuiSchema::XYZ(11.0, 22.0, 33.0));
  shape_points.push_back(GuiSchema::XYZ(12.0, 24.0, 36.0));
  shape_points.push_back(GuiSchema::XYZ(13.0, 26.0, 39.0));
  shape_points.push_back(GuiSchema::XYZ(14.0, 28.0, 312.0));
  line = GuiSchema::CreateLineDirect(builder, basic_info, &shape_points);
  line_list.push_back(line);

  auto primitve_list_fbs = GuiSchema::CreatePrimitiveListDirect(builder, nullptr, nullptr, nullptr, nullptr, nullptr,
                                                                &line_list, nullptr, nullptr, nullptr);
  builder.Finish(primitve_list_fbs);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  std::pair<std::string, std::string> structure_version_pair{"Primitives", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "primitive.fbs";

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

  ASSERT_EQ(reference_input["dataStream"]["objectData"].isMember("primitives"), true)
      << "reference_input has no primitives";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"].isMember("primitives"), true) << "plugin has no primitives";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"].isMember("cache_sensor_1"), true)
      << "reference_input has no cache_sensor_1";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"].isMember("cache_sensor_1"), true)
      << "plugin has no cache_sensor_1";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"].isMember("data"), true)
      << "reference_input has no data";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"].isMember("data"), true)
      << "plugin has no data";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"].isMember("lines"), true)
      << "reference_input has no data";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"].isMember("lines"), true)
      << "plugin has no data";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"].isArray(),
            true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"].size(), 2u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"].isArray(), true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"].size(), 2u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][0]
           .getMemberNames()) {
    ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][0].isMember(
                  member_name),
              true)
        << "values are not available for " << member_name;
    EXPECT_EQ(
        plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][0][member_name],
        reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][0][member_name])
        << "values are not matching for " << member_name;
  }
  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][1]
           .getMemberNames()) {
    ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][1].isMember(
                  member_name),
              true)
        << "values are not available for " << member_name;
    EXPECT_EQ(
        plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][1][member_name],
        reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["lines"][1][member_name])
        << "values are not matching for " << member_name;
  }
}

} // namespace databridge
} // namespace next

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <next/bridgesdk/schema/3d_view/primitive_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(PRIMITIVE_FBS_CHECK, circle_fbs_to_json) {
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
            "circles" :
             [
                {
                        "basicInfo" :
                        {
                                "color" : "#00ff00",
                                "id" : 5,
                                "opacity" : 0.90000000000000002,
                                "type" : "sphere"
                        },
                        "centerPoint" :
                        {
                                "x" : 2.0,
                                "y" : 3.0,
                                "z" : 0.0
                        },
                        "rotation" :
                        {
                                "x" : 0.0,
                                "y" : 0.0,
                                "z" : 0.0
                        },
                        "scale" :
                        {
                                "x" : 4.0,
                                "y" : 5.0,
                                "z" : 0.0
                        }
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

  GuiSchema::XYZ center_points = {2.F, 3.F, 0.F};
  GuiSchema::XYZ scales = {4.F, 5.F, 0.F};
  GuiSchema::XYZ rotations = {0.F, 0.F, 0.F};

  std::vector<flatbuffers::Offset<GuiSchema::Circle>> circle_vector{};

  auto circle_basic =
      GuiSchema::CreatePrimitiveBasicDirect(builder, GuiSchema::PrimitiveType::sphere, 5, "#00ff00", 0.9);
  auto cirlce_elm = GuiSchema::CreateCircle(builder, circle_basic, &center_points, &scales, &rotations);
  circle_vector.push_back(cirlce_elm);

  auto primitve_list_fbs = GuiSchema::CreatePrimitiveListDirect(builder, &circle_vector, nullptr, nullptr, nullptr,
                                                                nullptr, nullptr, nullptr, nullptr, nullptr);
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

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"].isMember("circles"),
            true)
      << "reference_input has no data";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"].isMember("circles"), true)
      << "plugin has no data";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"].isArray(),
            true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"].size(), 1u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"].isArray(),
            true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"].size(), 1u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"][0]
           .getMemberNames()) {
    ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"][0].isMember(
                  member_name),
              true)
        << "values are not available for " << member_name;
    EXPECT_EQ(
        plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"][0][member_name],
        reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["circles"][0][member_name])
        << "values are not matching for " << member_name;
  }
}

} // namespace databridge
} // namespace next

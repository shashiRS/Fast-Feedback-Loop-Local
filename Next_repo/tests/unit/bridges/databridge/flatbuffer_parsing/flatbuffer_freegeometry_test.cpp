#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <next/bridgesdk/schema/3d_view/freegeometry_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, map_supported_enum_check_freegeometry) {
  std::pair<std::string, std::string> test_input{"FreeGeometry", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::FreeGeometry);
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, freegeometry_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  std::string test = R"({
  "dataStream": {
    "objectData": {
      "freeGeometry": {
        "target-pose": {
          "data": [
            {
              "color": "#FFFFFF",
              "height": 1.0,
              "id": 0,
              "sensorUid": 1,
              "vertices": [
                {
                  "x": -2.158,
                  "y": 0.916,
                  "z": 0
                },
                {
                  "x": -0.924,
                  "y": 0.9,
                  "z": 0
                },
                {
                  "x": -0.874,
                  "y": 1.05,
                  "z": 0
                },
                {
                  "x": -0.954,
                  "y": 1.05,
                  "z": 0
                }
              ]
            }
          ],
          "metaData": {
            "sensorUid": 1
          }
        }
      }
    },
    "timeStamp": 0
  }
})";

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);

  std::vector<GuiSchema::Vertex> vertices = {
      GuiSchema::Vertex{-2.158f, 0.916f, 0.f},
      GuiSchema::Vertex{-0.924f, 0.9f, 0.f},
      GuiSchema::Vertex{-0.874f, 1.05f, 0.f},
      GuiSchema::Vertex{-0.954f, 1.05f, 0.f},
  };

  auto freegeometry = GuiSchema::CreateFreeGeometryDirect(builder, 0, 1, &vertices, 1, "#FFFFFF");

  builder.Finish(freegeometry);
  std::vector<flatbuffers::Offset<GuiSchema::FreeGeometry>> freegeometry_list;
  freegeometry_list.push_back(freegeometry);

  auto fbs_freegeo_list = GuiSchema::CreateFreeGeometryListDirect(builder, &freegeometry_list);
  builder.Finish(fbs_freegeo_list);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "target-pose";

  std::pair<std::string, std::string> structure_version_pair{"FreeGeometry", "0.0.1"};
  boost::filesystem::path current_path_var(boost::filesystem::current_path());

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result;
  ASSERT_TRUE(data_class_parser.parseDataClass(result, current_path_var.string() + "/freegeometry.fbs", flatbuffer,
                                               sensor_info, structure_version_pair));

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

  ASSERT_EQ(reference_input["dataStream"]["objectData"].isMember("freeGeometry"), true)
      << "reference_input has no freeGeometry";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"].isMember("freeGeometry"), true) << "plugin has no freeGeometry";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["freeGeometry"].isMember("target-pose"), true)
      << "reference_input has no target-pose";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"].isMember("target-pose"), true)
      << "plugin has no target-pose";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"].isMember("data"), true)
      << "reference_input has no data";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"].isMember("data"), true)
      << "plugin has no data";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"].isArray(), true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"].size(), 1u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"].isArray(), true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"].size(), 1u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0].getMemberNames()) {
    ASSERT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0].isMember(member_name),
              true)
        << "values are not available for " << member_name;

    // compare vertices
    if (plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name].isArray()) {
      Json::ArrayIndex vertices_size =
          plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name].size();
      for (Json::ArrayIndex i = 0u; i < vertices_size; i++) {
        // compare x,y,z
        for (const auto &points :
             plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name][i]
                 .getMemberNames()) {
          EXPECT_EQ(reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name]
                                   [i][points],
                    reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name]
                                   [i][points])
              << "values are not matching for " << points;
        }
      }
    } else {
      auto t1 = plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name];
      auto t2 = reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name];
      EXPECT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name],
                reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["data"][0][member_name])
          << "values are not matching for " << member_name;
    }
  }

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"].isMember("metaData"), true)
      << "reference_input has no metaData";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"].isMember("metaData"), true)
      << "plugin has no metaData";

  EXPECT_EQ(plugin_output["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["metaData"]["sensorUid"],
            reference_input["dataStream"]["objectData"]["freeGeometry"]["target-pose"]["metaData"]["sensorUid"])
      << "values are not matching for sensorUid";

  EXPECT_EQ(plugin_output["dataStream"]["objectData"]["timeStamp"],
            reference_input["dataStream"]["objectData"]["timeStamp"])
      << "values are not matching for timeStamp";
}

} // namespace databridge
} // namespace next

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>
#include <next/bridgesdk/schema/3d_view/view_update_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, map_supported_enum_check_trafficparticipant) {
  std::pair<std::string, std::string> test_input{"TrafficParticipant", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::TrafficParticipant);
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, traffic_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  std::string test = R"(
  {
    "dataStream":
    {
      "objectData":
      {
        "trafficParticipants":
        {
          "cache_sensor_1": {
            "metaData":
            {
              "sensorId": 1,
              "timeStamp": 891.912
            },
            "data":
            [
              {
                "color" : "#FF6600",
                "opacity" : 0.98,
                "times" : 0.1,
                "pX" : 27.503,
                "pY" : -7.156,
                "x" : 1.0,
                "y" : 0.0,
                "unifiedRefPoint" : 1,
                "unifiedYaw" : 0.5,
                "length" : 0.2,
                "width" : 1.0,
                "id" : 2,
                "uniqueId" : 17,
                "vX" : -9.875,
                "vY" : 0.0,
                "unifiedClassIf" : "car",
                "unifiedDynProp" : "static",
                "sensor" : "ARS510",
                "braking" : true,
                "showExclamation" : true,
                "indicatingLeft" : true,
                "indicatingRight" : false
              }
            ]
          }
        }
      }
    }
  })";

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  auto color_string = builder.CreateString("#FF6600");
  auto sensor_string = builder.CreateString("ARS510");
  auto traffic_participant = GuiSchema::CreateTrafficParticipant(
      builder, 2, 27.503f, -7.156f, 1.0f, 0.0f, 0.5f, 0.2f, 1.0f, -9.875f, 0.0f, 0.1f, 17, 1,
      GuiSchema::TrafficParticipantClass::car, GuiSchema::TrafficParticipantState::static_, sensor_string, color_string,
      0.98f, true, true, false, true);

  std::vector<flatbuffers::Offset<GuiSchema::TrafficParticipant>> tp_list;
  tp_list.push_back(traffic_participant);
  auto vectortest = builder.CreateVector(tp_list);
  auto traffic_participants = CreateTrafficParticipantList(builder, vectortest);
  builder.Finish(traffic_participants);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  std::pair<std::string, std::string> structure_version_pair{"TrafficParticipant", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "trafficparticipant.fbs";

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

  ASSERT_EQ(reference_input.isMember("dataStream"), true) << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "plugin has no Traffic participants";

  ASSERT_EQ(reference_input["dataStream"].isMember("objectData"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"].isMember("objectData"), true) << "plugin has no Traffic participants";

  ASSERT_EQ(reference_input["dataStream"]["objectData"].isMember("trafficParticipants"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"].isMember("trafficParticipants"), true)
      << "plugin has no Traffic participants";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["trafficParticipants"].isMember("cache_sensor_1"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["trafficParticipants"].isMember("cache_sensor_1"), true)
      << "plugin has no Traffic participants";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"].isMember("data"), true)
      << "reference_input has no Traffic participants";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"].isMember("data"), true)
      << "plugin has no Traffic participants";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"].isArray(),
            true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"].size(), 1u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"].isArray(), true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"].size(), 1u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"][0]
           .getMemberNames()) {
    ASSERT_EQ(plugin_output["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"][0].isMember(
                  member_name),
              true)
        << "values are not available for " << member_name;
    EXPECT_EQ(
        plugin_output["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"][0][member_name],
        reference_input["dataStream"]["objectData"]["trafficParticipants"]["cache_sensor_1"]["data"][0][member_name])
        << "values are not matching for " << member_name;
  }
}

TEST(TRAFFIC_PARTICIPANT_FBS_CHECK, traffic_fbs_to_fbs) {
  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  auto color_string = builder.CreateString("#FF6600");
  auto sensor_string = builder.CreateString("ARS510");
  auto traffic_participant = GuiSchema::CreateTrafficParticipant(
      builder, 2, 27.503f, -7.156f, 1.0f, 0.0f, 0.5f, 0.2f, 1.0f, -9.875f, 0.0f, 0.1f, 17, 1,
      GuiSchema::TrafficParticipantClass::car, GuiSchema::TrafficParticipantState::static_, sensor_string, color_string,
      0.98f, true, true, false, true);

  std::vector<flatbuffers::Offset<GuiSchema::TrafficParticipant>> tp_list;
  tp_list.push_back(traffic_participant);
  auto serialized_vector = builder.CreateVector(tp_list);
  auto traffic_participants = CreateTrafficParticipantList(builder, serialized_vector);
  builder.Finish(traffic_participants);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result_fbs;
  std::pair<std::string, std::string> structure_version_pair{"TrafficParticipant", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;
  data_class_parser.parseDataClassToFlatbuffer(result_fbs, flatbuffer, 1, sensor_info, structure_version_pair);
  size_t offset = 13;
  auto fbs_binary = (void *)&(result_fbs.data()[offset]);
  auto view = GuiSchema::GetViewUpdate(fbs_binary);
  EXPECT_EQ(view->traffic_objects()->trafficParticipants()->size(), 1u);
  EXPECT_EQ(view->meta()->cache_name()->str(), "cache_sensor_1");
}

} // namespace databridge
} // namespace next

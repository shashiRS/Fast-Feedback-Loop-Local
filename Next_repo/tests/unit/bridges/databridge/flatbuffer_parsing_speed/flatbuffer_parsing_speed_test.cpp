#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <chrono>
#include <thread>

#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>
#include <next/sdk/logger/logger.hpp>

#include "OutputManager/plugin_data_class_parser.h"

// should be enough, in debug usually it takes around ~100ms to execute (before the improvement it was ~900ms)
constexpr long long expected_max_time = 450; // aim for at least 2x speed

namespace next {
namespace databridge {

TEST(FLATBUFFER_PARSING_SPEED, check_schema_safed_for_same_file) {
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

  // Speed test for FlatBufferToJSON
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  for (int i = 0; i < 200; i++) {
    result.clear();
    ASSERT_TRUE(data_class_parser.parseDataClass(result, path_to_fbs.string(), flatbuffer, sensor_info,
                                                 structure_version_pair));
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  const long long time_span = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
  next::sdk::logger::info(__FILE__, "Flatbuffer loop time span: '{0}' [ms].", time_span);
  std::ostringstream out_message;
  out_message << "Flatbuffers To Json is slower than expected! Expected max time span: "
              << std::to_string(expected_max_time) << " [ms] but it was " << std::to_string(time_span) << " [ms]";
  EXPECT_TRUE(time_span < expected_max_time) << out_message.str();

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

} // namespace databridge
} // namespace next

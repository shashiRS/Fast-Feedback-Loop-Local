#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <next/bridgesdk/schema/3d_view/egovehicle_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(EGO_VEHICLE_FBS_CHECK, map_supported_enum_check_ego_vehicle) {
  std::pair<std::string, std::string> test_input{"EgoVehicle", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;

  auto supported_enum_number = data_class_parser.mapFBStoEnum(test_input);
  EXPECT_EQ(supported_enum_number, SupportedFbsTypes::supportedFBS::EgoVehicle);
}

TEST(EGO_VEHICLE_FBS_CHECK, ego_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  std::string test = R"(
  {
    "dataStream":
    {
      "objectData":
      {
        "egoVehicles":
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
                "width" : 5.49,
                "length" : 0.98,
                "x" : 0.1,
                "y" : 27.503,
                "yaw" : 1.2,
                "velocity" : -7.156,
                "yawRate" : 1.0,
                "shapePoints" :
                [
                  {"x": 1.0,"y": 2.0,"z":3.0},
                  {"x": 2.0,"y": 4.0,"z":6.0},
                  {"x": 3.0,"y": 6.0,"z":9.0},
                  {"x": 4.0,"y": 8.0,"z":12.0}
                ],
                "height" : 1.01
              }
            ]
          }
        }
      }
    }
  })";

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  std::vector<flatbuffers::Offset<GuiSchema::ShapePoint>> shape_points;

  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 1.0, 2.0, 3.0));
  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 2.0, 4.0, 6.0));
  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 3.0, 6.0, 9.0));
  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 4.0, 8.0, 12.0));

  auto ego_vehicle = GuiSchema::CreateEgoVehicleDirect(builder, 5.49f, 0.98f, 0.1f, 27.503f, 1.2f, -7.156f, 1.0f, 1.01f,
                                                       &shape_points);

  std::vector<flatbuffers::Offset<GuiSchema::EgoVehicle>> ego_list;
  ego_list.push_back(ego_vehicle);
  auto ego_fbs_list = GuiSchema::CreateEgoVehicleListDirect(builder, &ego_list);
  builder.Finish(ego_fbs_list);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  std::pair<std::string, std::string> structure_version_pair{"EgoVehicle", "0.0.1"};
  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "egovehicle.fbs";

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

  ASSERT_EQ(reference_input["dataStream"]["objectData"].isMember("egoVehicles"), true)
      << "reference_input has no egoVehicles";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"].isMember("egoVehicles"), true) << "plugin has no egoVehicles";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["egoVehicles"].isMember("cache_sensor_1"), true)
      << "reference_input has no cache_sensor_1";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["egoVehicles"].isMember("cache_sensor_1"), true)
      << "plugin has no cache_sensor_1";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"].isMember("data"), true)
      << "reference_input has no data";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"].isMember("data"), true)
      << "plugin has no data";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"].isArray(), true)
      << "reference_input output has no valid size";
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"].size(), 1u)
      << "reference_input output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"].isArray(), true)
      << "plugin output has no valid size";
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"].size(), 1u)
      << "plugin output has no valid size";

  for (const auto &member_name :
       reference_input["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"][0].getMemberNames()) {
    ASSERT_EQ(
        plugin_output["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"][0].isMember(member_name),
        true)
        << "values are not available for " << member_name;
    EXPECT_EQ(plugin_output["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"][0][member_name],
              reference_input["dataStream"]["objectData"]["egoVehicles"]["cache_sensor_1"]["data"][0][member_name])
        << "values are not matching for " << member_name;
  }
}

TEST(EGO_VEHICLE_FBS_CHECK, ego_fbs_to_fbs) {

  flatbuffers::FlatBufferBuilder builder(2000);
  builder.ForceDefaults(true);
  std::vector<flatbuffers::Offset<GuiSchema::ShapePoint>> shape_points;

  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 1.0, 2.0, 3.0));
  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 2.0, 4.0, 6.0));
  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 3.0, 6.0, 9.0));
  shape_points.push_back(GuiSchema::CreateShapePoint(builder, 4.0, 8.0, 12.0));

  auto ego_vehicle = GuiSchema::CreateEgoVehicleDirect(builder, 5.49f, 0.98f, 0.1f, 27.503f, 1.2f, -7.156f, 1.0f, 1.01f,
                                                       &shape_points);

  std::vector<flatbuffers::Offset<GuiSchema::EgoVehicle>> ego_list;
  ego_list.push_back(ego_vehicle);
  auto ego_fbs_list = GuiSchema::CreateEgoVehicleListDirect(builder, &ego_list);
  builder.Finish(ego_fbs_list);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result_fbs;
  std::pair<std::string, std::string> structure_version_pair{"EgoVehicle", "0.0.1"};
  outputmanager::PluginDataClassParser data_class_parser;
  data_class_parser.parseDataClassToFlatbuffer(result_fbs, flatbuffer, 1, sensor_info, structure_version_pair);
  size_t offset = 13;
  auto fbs_binary = (void *)&(result_fbs.data()[offset]);
  auto view = GuiSchema::GetViewUpdate(fbs_binary);
  EXPECT_EQ(view->ego_vehicles()->egoVehicles()->size(), 1u);
  EXPECT_EQ(view->meta()->cache_name()->str(), "cache_sensor_1");

  auto shape_points_data = builder.GetBufferPointer();
  auto ego_veh_shape_points_ref = GuiSchema::GetEgoVehicleList(shape_points_data)->egoVehicles()->Get(0);
  auto ego_veh_shape_points = view->ego_vehicles()->egoVehicles()->Get(0)->shapePoints();
  for (flatbuffers::uoffset_t i = 0; i < ego_veh_shape_points->size(); i++) {
    EXPECT_FLOAT_EQ(ego_veh_shape_points_ref->shapePoints()->Get(i)->x(), ego_veh_shape_points->Get(i)->x());
    EXPECT_FLOAT_EQ(ego_veh_shape_points_ref->shapePoints()->Get(i)->y(), ego_veh_shape_points->Get(i)->y());
    EXPECT_FLOAT_EQ(ego_veh_shape_points_ref->shapePoints()->Get(i)->z(), ego_veh_shape_points->Get(i)->z());
  }
}
} // namespace databridge
} // namespace next

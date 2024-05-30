#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <next/bridgesdk/schema/3d_view/primitive_generated.h>

#include "OutputManager/plugin_data_class_parser.h"

namespace next {
namespace databridge {

TEST(PRIMITIVE_FBS_CHECK, arc_fbs_to_json) {
  outputmanager::PluginDataClassParser data_class_parser;

  std::string test = R"(
  {
    "dataStream":
    {
      "timestamp": 0,
      "objectData":
      {
        "primitives":
        {
          "cache_sensor_1": {
            "metaData":
            {
              "sensorUid": 1
            },
            "data":
            {
            "arcs" :
             [
                {
                        "basicInfo" :
                        {
                                "color" : "#00ff00",
                                "id" : 5,
                                "opacity" : 0.90000000000000002,
                                "type" : "arc"
                        },
                        "centerPoint" :
                        {
                                "x" : 2.4,
                                "y" : 3.3,
                                "z" : 0.5
                        },
                        "radius" : 12.5,
                        "startAngle" : 20.1,
                        "endAngle" : 60.25
                },
                {
                        "basicInfo" :
                        {
                                "color" : "#ff0000",
                                "id" : 6,
                                "opacity" : 0.80000000000000002,
                                "type" : "arc"
                        },
                        "centerPoint" :
                        {
                                "x" : -7.3,
                                "y" : -8.9,
                                "z" : -12.5
                        },
                        "radius" : 10.12,
                        "startAngle" : 60.125,
                        "endAngle" : 90.75
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

  GuiSchema::XYZ center_point_1 = {2.4F, 3.3F, 0.5F};
  GuiSchema::XYZ center_point_2 = {-7.3F, -8.9F, -12.5F};

  std::vector<flatbuffers::Offset<GuiSchema::Arc>> arc_vector{};

  auto arc_basic_1 = GuiSchema::CreatePrimitiveBasicDirect(builder, GuiSchema::PrimitiveType::arc, 5, "#00ff00", 0.9);
  auto arc_basic_2 = GuiSchema::CreatePrimitiveBasicDirect(builder, GuiSchema::PrimitiveType::arc, 6, "#ff0000", 0.8);

  auto arc_elm_1 = GuiSchema::CreateArc(builder, arc_basic_1, &center_point_1, 12.5f, 20.1f, 60.25f);
  auto arc_elm_2 = GuiSchema::CreateArc(builder, arc_basic_2, &center_point_2, 10.12F, 60.125F, 90.75F);
  arc_vector.push_back(arc_elm_1);
  arc_vector.push_back(arc_elm_2);

  auto primitve_list_fbs = GuiSchema::CreatePrimitiveListDirect(builder, nullptr, nullptr, nullptr, nullptr, nullptr,
                                                                nullptr, nullptr, nullptr, nullptr, &arc_vector);
  builder.Finish(primitve_list_fbs);

  next::bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.data_view = "objectData";
  sensor_info.sensor_id = 1;
  sensor_info.cache_name = "cache_sensor_1";

  std::pair<std::string, std::string> structure_version_pair{"Primitives", "0.0.1"};
  boost::filesystem::path current_path_var(boost::filesystem::current_path());

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  std::string result;
  ASSERT_TRUE(data_class_parser.parseDataClass(result, current_path_var.string() + "/primitive.fbs", flatbuffer,
                                               sensor_info, structure_version_pair));

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(result, plugin_output, false);
  ASSERT_TRUE(parseSuccess) << "parsing test object failed";

  Json::Value reference_input;
  parseSuccess = reader.parse(test, reference_input, false);
  ASSERT_TRUE(parseSuccess) << "parsing reference_input object failed";

  ASSERT_TRUE(reference_input.isMember("dataStream")) << "reference_input has no dataStream";
  ASSERT_TRUE(plugin_output.isMember("dataStream")) << "plugin has no dataStream";

  ASSERT_TRUE(reference_input["dataStream"].isMember("timestamp")) << "reference_input has no timeStamp";
  ASSERT_TRUE(plugin_output["dataStream"].isMember("timestamp")) << "plugin has no timeStamp";

  ASSERT_TRUE(reference_input["dataStream"].isMember("objectData")) << "reference_input has no objectData";
  ASSERT_TRUE(plugin_output["dataStream"].isMember("objectData")) << "plugin has no objectData";

  ASSERT_TRUE(reference_input["dataStream"]["objectData"].isMember("primitives"))
      << "reference_input has no primitives";
  ASSERT_TRUE(plugin_output["dataStream"]["objectData"].isMember("primitives")) << "plugin has no primitives";

  ASSERT_TRUE(reference_input["dataStream"]["objectData"]["primitives"].isMember("cache_sensor_1"))
      << "reference_input has no cache_sensor_1";
  ASSERT_TRUE(plugin_output["dataStream"]["objectData"]["primitives"].isMember("cache_sensor_1"))
      << "plugin has no cache_sensor_1";

  ASSERT_TRUE(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"].isMember("metaData"))
      << "plugin has no metaData";

  for (const auto &meta_member :
       reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["metaData"].getMemberNames()) {
    ASSERT_TRUE(
        plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["metaData"].isMember(meta_member))
        << "values are not available for " << meta_member;
    EXPECT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["metaData"][meta_member],
              reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["metaData"][meta_member])
        << "values are not matching for " << meta_member;
  }

  ASSERT_TRUE(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"].isMember("data"))
      << "reference_input has no data";
  ASSERT_TRUE(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"].isMember("data"))
      << "plugin has no data";

  ASSERT_TRUE(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"].isMember("arcs"))
      << "reference_input has no arcs";
  ASSERT_TRUE(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"].isMember("arcs"))
      << "plugin has no arcs";

  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"].isArray(),
            true);
  ASSERT_EQ(reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"].size(), 2u);
  ASSERT_TRUE(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"].isArray());
  ASSERT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"].size(), 2u);

  for (Json::ArrayIndex i = 0;
       i < plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"].size(); i++) {
    for (const auto &member_name :
         reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i]
             .getMemberNames()) {
      ASSERT_TRUE(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i].isMember(
          member_name))
          << "values are not available for " << member_name;
      if (reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i][member_name]
              .isObject()) {
        for (const auto &sub_member :
             plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i][member_name]
                 .getMemberNames()) {
          EXPECT_EQ(plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i]
                                 [member_name][sub_member],
                    reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i]
                                   [member_name][sub_member])
              << "values are not matching for " << member_name << "." << sub_member << " in arc number " << i;
        }
      } else {
        EXPECT_EQ(
            plugin_output["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i][member_name],
            reference_input["dataStream"]["objectData"]["primitives"]["cache_sensor_1"]["data"]["arcs"][i][member_name])
            << "values are not matching for " << member_name << " in arc number " << i;
      }
    }
  }
}

} // namespace databridge
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_3D_view_test.cpp
 *  @brief    Testing some mock functionalities
 */

#include <cip_test_support/gtest_reporting.h>

#include <json/json.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>

#include "plugin_mock.hpp"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;

const std::string MOCK_URL("MockUrl");

TYPED_TEST_CASE(PluginTest, next::plugins::MockPlugin);

/*Registering a sensor and check whether is it registered or not*/
TYPED_TEST(PluginTest, registerAndPublishSensor_registering_sendSensorListOneSensor) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::Plugin3dView Viewer(Plugin.get());

  next::bridgesdk::plugin::SensorInfoForGUI test_sensor_info_1;

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";
  test_sensor_info_1.referenceCoordinateSystem = "CS";
  test_sensor_info_1.coordinateSystem.translationX = -1.1f;
  test_sensor_info_1.coordinateSystem.translationY = -1.2f;
  test_sensor_info_1.coordinateSystem.translationZ = -1.3f;
  test_sensor_info_1.coordinateSystem.rotationX = -1.4f;
  test_sensor_info_1.coordinateSystem.rotationY = -1.5f;
  test_sensor_info_1.coordinateSystem.rotationZ = -1.6f;

  Viewer.registerAndPublishSensor(test_sensor_info_1);

  auto sensors_ = this->dm_mock_ptr_->GetRegisteredSensors();
  bool sensor_found = false;
  for (auto const &e : sensors_->sensor_list) {
    if (e.second.name == test_sensor_info_1.name) {
      sensor_found = true;
    }
  }
  ASSERT_EQ(sensor_found, true);

  // check output json structure
  std::vector<uint8_t> data_vector = this->dm_mock_->data_.front();
  std::string teststring((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), true)
      << "output json has no member dataStream.Stream1";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream].isMember("sensors"), true)
      << "output json has no member dataStream.Stream1.sensors";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].isArray(), true)
      << "plugin_output dataStream.Stream1.sensors is not an array";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].size(), 1u);

  auto &plugin_output_sensors = plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"];

  ASSERT_EQ(plugin_output_sensors[0].isMember("uid"), true) << "output json has no member uid ";
  ASSERT_EQ(plugin_output_sensors[0]["uid"], 1);

  ASSERT_EQ(plugin_output_sensors[0].isMember("name"), true) << "output json has no member name";
  ASSERT_EQ(plugin_output_sensors[0]["name"], test_sensor_info_1.name);

  ASSERT_EQ(plugin_output_sensors[0].isMember("color"), true) << "output json has no member color";
  ASSERT_EQ(plugin_output_sensors[0]["color"], test_sensor_info_1.color);

  ASSERT_EQ(plugin_output_sensors[0].isMember("type"), true) << "output json has no member type";
  ASSERT_EQ(plugin_output_sensors[0]["type"], test_sensor_info_1.type);

  ASSERT_EQ(plugin_output_sensors[0].isMember("enabled"), true) << "output json has no member type";
  ASSERT_EQ(plugin_output_sensors[0]["enabled"], true);

  ASSERT_EQ(plugin_output_sensors[0].isMember("referenceCoordinateSystem"), true)
      << "output json has no member referenceCoordinateSystem";
  ASSERT_EQ(plugin_output_sensors[0]["referenceCoordinateSystem"], test_sensor_info_1.referenceCoordinateSystem);

  ASSERT_EQ(plugin_output_sensors[0].isMember("coordinateSystemTransform"), true)
      << "output json has no member coordinateSystemTransform";

  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"].isMember("translationX"), true)
      << "output json has no member coordinateSystemTransform.translationX";
  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"]["translationX"],
            test_sensor_info_1.coordinateSystem.translationX);

  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"].isMember("translationY"), true)
      << "output json has no member coordinateSystemTransform.translationY";
  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"]["translationY"],
            test_sensor_info_1.coordinateSystem.translationY);

  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"].isMember("translationZ"), true)
      << "output json has no member coordinateSystemTransform.translationZ";
  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"]["translationZ"],
            test_sensor_info_1.coordinateSystem.translationZ);

  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"].isMember("rotationX"), true)
      << "output json has no member coordinateSystemTransform.rotationX";
  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"]["rotationX"],
            test_sensor_info_1.coordinateSystem.rotationX);

  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"].isMember("rotationY"), true)
      << "output json has no member coordinateSystemTransform.rotationY";
  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"]["rotationY"],
            test_sensor_info_1.coordinateSystem.rotationY);

  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"].isMember("rotationZ"), true)
      << "output json has no member coordinateSystemTransform.rotationZ";
  ASSERT_EQ(plugin_output_sensors[0]["coordinateSystemTransform"]["rotationZ"],
            test_sensor_info_1.coordinateSystem.rotationZ);
}

/*Registering two different sensors, and their uids should be different*/
TYPED_TEST(PluginTest, registerAndPublishSensor_registeringDiffSensors_sendSensorListTwoSensors) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::Plugin3dView Viewer(Plugin.get());

  uint32_t sensor_1_id{0}, sensor_2_id{0};
  next::bridgesdk::plugin::SensorInfoForGUI test_sensor_info_1, test_sensor_info_2;
  std::map<std::string, std::vector<next::bridgesdk::plugin::SensorInfoForGUI>> sensorList;

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";
  sensorList[test_sensor_info_1.stream].push_back(test_sensor_info_1);

  sensor_1_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1) << "sensor list is not sent out.";

  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_2.name = "Test_Sensor2";
  test_sensor_info_2.color = "ffff01";
  test_sensor_info_2.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_2.stream = test_sensor_info_1.stream;
  test_sensor_info_2.type = "TP";
  sensorList[test_sensor_info_2.stream].push_back(test_sensor_info_2);

  sensor_2_id = Viewer.registerAndPublishSensor(test_sensor_info_2);

  ASSERT_NE(sensor_1_id, sensor_2_id);

  // check json output structure
  std::vector<uint8_t> data_vector = this->dm_mock_->data_.front();
  std::string teststring((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // check streams
  int counter = 1;
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  for (auto &stream : sensorList) {
    ASSERT_EQ(plugin_output["dataStream"].isMember(stream.first), true)
        << "output json has no member dataStream." << stream.first;
    ASSERT_EQ(plugin_output["dataStream"][stream.first].isMember("sensors"), true)
        << "output json has no member dataStream." << stream.first << ".sensors";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].isArray(), true)
        << "output json has no member dataStream." << stream.first << ".sensors is not an array ";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].size(), stream.second.size())
        << "output json has no member dataStream." << stream.first << ".sensors size is unequal "
        << std::to_string(stream.second.size());

    auto &plugin_output_sensors = plugin_output["dataStream"][stream.first]["sensors"];

    // check sensors of streams
    auto &sensors_of_stream = stream.second;
    for (int i = 0; static_cast<size_t>(i) < sensors_of_stream.size(); ++i) {
      const size_t vec_idx = static_cast<size_t>(i);
      ASSERT_EQ(plugin_output_sensors[i].isMember("uid"), true) << "output json has no member uid ";
      ASSERT_EQ(plugin_output_sensors[i]["uid"], counter);
      ++counter;

      ASSERT_EQ(plugin_output_sensors[i].isMember("name"), true) << "output json has no member name";
      EXPECT_EQ(plugin_output_sensors[i]["name"], sensors_of_stream[vec_idx].name);

      ASSERT_EQ(plugin_output_sensors[i].isMember("color"), true) << "output json has no member color";
      EXPECT_EQ(plugin_output_sensors[i]["color"], sensors_of_stream[vec_idx].color);

      ASSERT_EQ(plugin_output_sensors[i].isMember("type"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["type"], sensors_of_stream[vec_idx].type);

      ASSERT_EQ(plugin_output_sensors[i].isMember("enabled"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["enabled"], true);

      ASSERT_EQ(plugin_output_sensors[i].isMember("referenceCoordinateSystem"), true)
          << "output json has no member referenceCoordinateSystem";
      EXPECT_EQ(plugin_output_sensors[i]["referenceCoordinateSystem"],
                sensors_of_stream[vec_idx].referenceCoordinateSystem);
    }
  }
}

/*Registering same sensor twice , so uid should be same */
TYPED_TEST(PluginTest, registerAndPublishSensor_registeringSameSensor_sendSensorListOneSensor) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::Plugin3dView Viewer(Plugin.get());

  uint32_t sensor_1_id{0}, sensor_2_id{0};
  next::bridgesdk::plugin::SensorInfoForGUI test_sensor_info_1;

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";
  sensor_1_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1u) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  sensor_2_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  ASSERT_EQ(sensor_1_id, sensor_2_id);

  // check json output structure
  std::vector<uint8_t> data_vector = this->dm_mock_->data_.front();
  std::string teststring((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), true)
      << "output json has no member dataStream.Stream1";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream].isMember("sensors"), true)
      << "output json has no member dataStream.Stream1.sensors";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].isArray(), true)
      << "plugin_output dataStream.Stream1.sensors is not an array";
  // check if only one sensor is sent out
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].size(), 1u)
      << "plugin_output dataStream.Stream1.sensors size is unequal 1";

  auto &plugin_output_sensors = plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"];

  // check first sensor
  ASSERT_EQ(plugin_output_sensors[0].isMember("uid"), true) << "output json has no member uid ";
  ASSERT_EQ(plugin_output_sensors[0]["uid"], 1);

  ASSERT_EQ(plugin_output_sensors[0].isMember("name"), true) << "output json has no member name";
  ASSERT_EQ(plugin_output_sensors[0]["name"], test_sensor_info_1.name);

  ASSERT_EQ(plugin_output_sensors[0].isMember("color"), true) << "output json has no member color";
  ASSERT_EQ(plugin_output_sensors[0]["color"], test_sensor_info_1.color);

  ASSERT_EQ(plugin_output_sensors[0].isMember("type"), true) << "output json has no member type";
  ASSERT_EQ(plugin_output_sensors[0]["type"], test_sensor_info_1.type);

  ASSERT_EQ(plugin_output_sensors[0].isMember("enabled"), true) << "output json has no member type";
  ASSERT_EQ(plugin_output_sensors[0]["enabled"], true);

  ASSERT_EQ(plugin_output_sensors[0].isMember("referenceCoordinateSystem"), true)
      << "output json has no member referenceCoordinateSystem";
  ASSERT_EQ(plugin_output_sensors[0]["referenceCoordinateSystem"], test_sensor_info_1.referenceCoordinateSystem);
}

/*Try registering same sensor name with different properties.
Should only be registered once because name and stream name are equal, so uid should be same */
TYPED_TEST(PluginTest, registerAndPublishSensor_registeringSameSensorsDiffProperties_sendSensorListTwoSensors) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::Plugin3dView Viewer(Plugin.get());

  uint32_t sensor_1_id{0}, sensor_2_id{0};
  next::bridgesdk::plugin::SensorInfoForGUI test_sensor_info_1, test_sensor_info_2;

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_coord1";
  test_sensor_info_1.coordinateSystem.translationX = 11;
  test_sensor_info_1.coordinateSystem.translationX = 12;
  test_sensor_info_1.coordinateSystem.translationX = 13;
  test_sensor_info_1.coordinateSystem.rotationX = 1;
  test_sensor_info_1.coordinateSystem.rotationY = 2;
  test_sensor_info_1.coordinateSystem.rotationZ = 3;
  sensor_1_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_2.name = "Test_Sensor1";
  test_sensor_info_2.color = "ffff01";
  test_sensor_info_2.referenceCoordinateSystem = "Ref_Cordinate_System2";
  test_sensor_info_2.stream = "Stream1";
  test_sensor_info_2.type = "TP2";
  test_sensor_info_2.referenceCoordinateSystem = "Ref_coord2";
  test_sensor_info_2.coordinateSystem.translationX = -11;
  test_sensor_info_2.coordinateSystem.translationX = -12;
  test_sensor_info_2.coordinateSystem.translationX = -13;
  test_sensor_info_2.coordinateSystem.rotationX = -1;
  test_sensor_info_2.coordinateSystem.rotationY = -2;
  test_sensor_info_2.coordinateSystem.rotationZ = -3;
  sensor_2_id = Viewer.registerAndPublishSensor(test_sensor_info_2);

  ASSERT_NE(sensor_1_id, sensor_2_id);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1) << "sensor list is not sent out.";
}

/*UnRegistering sensor, sensor should not be there anymore
and empty sensor list shall be sent out*/
TYPED_TEST(PluginTest, unregisterAndPublishSensor_unregisteringSensor_sendSensorListTwoSensors) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::Plugin3dView Viewer(Plugin.get());

  uint32_t sensor_1_id{0};
  next::bridgesdk::plugin::SensorInfoForGUI test_sensor_info_1;

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";

  /*Registering sensor*/
  sensor_1_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1u) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  /*Unregistering the same sensor*/
  Viewer.unregisterAndPublishSensor(sensor_1_id);

  auto sensors_ = this->dm_mock_ptr_->GetRegisteredSensors();
  bool sensor_found = false;
  for (auto const &e : sensors_->sensor_list) {
    if (e.second.name == test_sensor_info_1.name) {
      sensor_found = true;
    }
  }
  ASSERT_EQ(sensor_found, false);

  // check json output structure
  std::vector<uint8_t> data_vector = this->dm_mock_->data_.front();
  std::string teststring((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // check if json with empty sensor list is send out
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), true)
      << "output json has no member dataStream.Stream1";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream].isMember("sensors"), true)
      << "output json has no member dataStream.Stream1.sensors";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].isArray(), true)
      << "plugin_output dataStream.Stream1.sensors is not an array";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].size(), 0u)
      << "plugin_output dataStream.Stream1.sensors size is unequal 0";

  // try to unregister not existing sensor again
  // flush sending out cache
  this->dm_mock_->data_.clear();

  /*Unregistering the same sensor again*/
  Viewer.unregisterAndPublishSensor(sensor_1_id);

  ASSERT_EQ(this->dm_mock_->data_.size(), 0u) << "not existing sensor is unregistered again";
}

/*Registering different sensors of different streams, and their uids should be different.
Unregister them afterwards*/
TYPED_TEST(PluginTest, register_and_unregister_sensors_of_diff_streams) {

  /////////////// START REGISTER SENSORS///////////////
  // Register 2 sensors for stream 1
  // Register 3 sensors for stream 2
  //////////////////////////////////////////////////
  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::Plugin3dView Viewer(Plugin.get());

  next::bridgesdk::plugin_addons::uid_t sensor_1_id{0}, sensor_2_id{0}, sensor_3_id{0}, sensor_4_id{0}, sensor_5_id{0};
  std::map<std::string, std::vector<next::bridgesdk::plugin::SensorInfoForGUI>> sensorList;
  next::bridgesdk::plugin::SensorInfoForGUI test_sensor_info_1, test_sensor_info_2, test_sensor_info_3,
      test_sensor_info_4, test_sensor_info_5;

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";
  sensorList[test_sensor_info_1.stream].push_back(test_sensor_info_1);

  sensor_1_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1u) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_2.name = "Test_Sensor2";
  test_sensor_info_2.color = test_sensor_info_1.color;
  test_sensor_info_2.referenceCoordinateSystem = test_sensor_info_1.referenceCoordinateSystem;
  test_sensor_info_2.stream = test_sensor_info_1.stream;
  test_sensor_info_2.type = test_sensor_info_1.type;
  sensorList[test_sensor_info_2.stream].push_back(test_sensor_info_2);

  sensor_2_id = Viewer.registerAndPublishSensor(test_sensor_info_2);
  ASSERT_NE(sensor_1_id, sensor_2_id);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1u) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_3.name = "Test_Sensor1";
  test_sensor_info_3.color = test_sensor_info_1.color;
  test_sensor_info_3.referenceCoordinateSystem = test_sensor_info_1.referenceCoordinateSystem;
  test_sensor_info_3.stream = "Stream2";
  test_sensor_info_3.type = test_sensor_info_1.type;
  sensorList[test_sensor_info_3.stream].push_back(test_sensor_info_3);

  sensor_3_id = Viewer.registerAndPublishSensor(test_sensor_info_3);
  ASSERT_NE(sensor_1_id, sensor_3_id);
  ASSERT_NE(sensor_2_id, sensor_3_id);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1u) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_4.name = "Test_Sensor2";
  test_sensor_info_4.color = test_sensor_info_1.color;
  test_sensor_info_4.referenceCoordinateSystem = test_sensor_info_1.referenceCoordinateSystem;
  test_sensor_info_4.stream = test_sensor_info_3.stream;
  test_sensor_info_4.type = test_sensor_info_1.type;
  sensorList[test_sensor_info_4.stream].push_back(test_sensor_info_4);

  sensor_4_id = Viewer.registerAndPublishSensor(test_sensor_info_4);
  ASSERT_NE(sensor_1_id, sensor_4_id);
  ASSERT_NE(sensor_2_id, sensor_4_id);
  ASSERT_NE(sensor_3_id, sensor_4_id);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_5.name = "Test_Sensor3";
  test_sensor_info_5.color = test_sensor_info_1.color;
  test_sensor_info_5.referenceCoordinateSystem = test_sensor_info_1.referenceCoordinateSystem;
  test_sensor_info_5.stream = test_sensor_info_3.stream;
  test_sensor_info_5.type = test_sensor_info_1.type;
  sensorList[test_sensor_info_5.stream].push_back(test_sensor_info_5);

  sensor_5_id = Viewer.registerAndPublishSensor(test_sensor_info_5);
  ASSERT_NE(sensor_1_id, sensor_5_id);
  ASSERT_NE(sensor_2_id, sensor_5_id);
  ASSERT_NE(sensor_3_id, sensor_5_id);
  ASSERT_NE(sensor_4_id, sensor_5_id);

  // check json output structure
  std::vector<uint8_t> data_vector = this->dm_mock_->data_.front();
  std::string teststring((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // check streams
  int counter = 1;
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  for (auto &stream : sensorList) {
    ASSERT_EQ(plugin_output["dataStream"].isMember(stream.first), true)
        << "output json has no member dataStream." << stream.first;
    ASSERT_EQ(plugin_output["dataStream"][stream.first].isMember("sensors"), true)
        << "output json has no member dataStream." << stream.first << ".sensors";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].isArray(), true)
        << "output json has no member dataStream." << stream.first << ".sensors is not an array ";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].size(), stream.second.size())
        << "output json has no member dataStream." << stream.first << ".sensors size is unequal "
        << std::to_string(stream.second.size());

    auto &plugin_output_sensors = plugin_output["dataStream"][stream.first]["sensors"];

    // check sensors of streams
    auto &sensors_of_stream = stream.second;
    for (int i = 0; static_cast<size_t>(i) < sensors_of_stream.size(); ++i) {
      const size_t vec_idx = static_cast<size_t>(i);
      ASSERT_EQ(plugin_output_sensors[i].isMember("uid"), true) << "output json has no member uid ";
      EXPECT_EQ(plugin_output_sensors[i]["uid"], counter);
      ++counter;

      ASSERT_EQ(plugin_output_sensors[i].isMember("name"), true) << "output json has no member name";
      EXPECT_EQ(plugin_output_sensors[i]["name"], sensors_of_stream[vec_idx].name);

      ASSERT_EQ(plugin_output_sensors[i].isMember("color"), true) << "output json has no member color";
      EXPECT_EQ(plugin_output_sensors[i]["color"], sensors_of_stream[vec_idx].color);

      ASSERT_EQ(plugin_output_sensors[i].isMember("type"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["type"], sensors_of_stream[vec_idx].type);

      ASSERT_EQ(plugin_output_sensors[i].isMember("enabled"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["enabled"], true);

      ASSERT_EQ(plugin_output_sensors[i].isMember("referenceCoordinateSystem"), true)
          << "output json has no member referenceCoordinateSystem";
      ASSERT_EQ(plugin_output_sensors[i]["referenceCoordinateSystem"],
                sensors_of_stream[vec_idx].referenceCoordinateSystem);
    }
  }

  ///////////////////////////////////////////////////
  /////////////// END REGISTER SENSORS///////////////
  ///////////////////////////////////////////////////

  /////////////// START UNREGISTER SENSORS///////////////

  //////////////////////////////////////////////////
  // Unregister first sensor for stream 1
  //////////////////////////////////////////////////

  // flush sending out cache
  this->dm_mock_->data_.clear();

  /*Unregistering the same sensor*/
  Viewer.unregisterAndPublishSensor(sensor_1_id);
  sensorList[test_sensor_info_1.stream].erase(sensorList[test_sensor_info_1.stream].begin());

  data_vector = this->dm_mock_->data_.front();
  teststring = std::string((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // check streams
  counter = 2; // start with index 2 because first sensor is removed
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  for (auto &stream : sensorList) {
    ASSERT_EQ(plugin_output["dataStream"].isMember(stream.first), true)
        << "output json has no member dataStream." << stream.first;
    ASSERT_EQ(plugin_output["dataStream"][stream.first].isMember("sensors"), true)
        << "output json has no member dataStream." << stream.first << ".sensors";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].isArray(), true)
        << "output json has no member dataStream." << stream.first << ".sensors is not an array ";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].size(), stream.second.size())
        << "output json has no member dataStream." << stream.first << ".sensors size is unequal "
        << std::to_string(stream.second.size());

    auto &plugin_output_sensors = plugin_output["dataStream"][stream.first]["sensors"];

    // check sensors of streams
    auto &sensors_of_stream = stream.second;
    for (int i = 0; static_cast<size_t>(i) < sensors_of_stream.size(); ++i) {
      const size_t vec_idx = static_cast<size_t>(i);
      ASSERT_EQ(plugin_output_sensors[i].isMember("uid"), true) << "output json has no member uid ";
      EXPECT_EQ(plugin_output_sensors[i]["uid"], counter);
      ++counter;

      ASSERT_EQ(plugin_output_sensors[i].isMember("name"), true) << "output json has no member name";
      EXPECT_EQ(plugin_output_sensors[i]["name"], sensors_of_stream[vec_idx].name);

      ASSERT_EQ(plugin_output_sensors[i].isMember("color"), true) << "output json has no member color";
      EXPECT_EQ(plugin_output_sensors[i]["color"], sensors_of_stream[vec_idx].color);

      ASSERT_EQ(plugin_output_sensors[i].isMember("type"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["type"], sensors_of_stream[vec_idx].type);

      ASSERT_EQ(plugin_output_sensors[i].isMember("enabled"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["enabled"], true);

      ASSERT_EQ(plugin_output_sensors[i].isMember("referenceCoordinateSystem"), true)
          << "output json has no member referenceCoordinateSystem";
      EXPECT_EQ(plugin_output_sensors[i]["referenceCoordinateSystem"],
                sensors_of_stream[vec_idx].referenceCoordinateSystem);
    }
  }

  // End check unregister first sensor of stream 1

  //////////////////////////////////////////////////
  // Unregister second sensor for stream 1
  // -> empty sensor list shall be send out for stream 1
  //////////////////////////////////////////////////

  // flush sending out cache
  this->dm_mock_->data_.clear();

  /*Unregistering the same sensor*/
  Viewer.unregisterAndPublishSensor(sensor_2_id);
  sensorList[test_sensor_info_2.stream].erase(--sensorList[test_sensor_info_2.stream].end());

  data_vector = this->dm_mock_->data_.front();
  teststring = std::string((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // check streams
  counter = 3; // start with index 3 because third sensor is removed
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  for (auto &stream : sensorList) {
    ASSERT_EQ(plugin_output["dataStream"].isMember(stream.first), true)
        << "output json has no member dataStream." << stream.first;
    ASSERT_EQ(plugin_output["dataStream"][stream.first].isMember("sensors"), true)
        << "output json has no member dataStream." << stream.first << ".sensors";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].isArray(), true)
        << "output json has no member dataStream." << stream.first << ".sensors is not an array ";
    // for stream 1 size should be 0
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].size(), stream.second.size())
        << "output json has no member dataStream." << stream.first << ".sensors size is unequal "
        << std::to_string(stream.second.size());

    auto &plugin_output_sensors = plugin_output["dataStream"][stream.first]["sensors"];

    // check sensors of streams
    auto &sensors_of_stream = stream.second;
    for (int i = 0; static_cast<size_t>(i) < sensors_of_stream.size(); ++i) {
      const size_t vec_idx = static_cast<size_t>(i);
      ASSERT_EQ(plugin_output_sensors[i].isMember("uid"), true) << "output json has no member uid ";
      EXPECT_EQ(plugin_output_sensors[i]["uid"], counter);
      ++counter;

      ASSERT_EQ(plugin_output_sensors[i].isMember("name"), true) << "output json has no member name";
      EXPECT_EQ(plugin_output_sensors[i]["name"], sensors_of_stream[vec_idx].name);

      ASSERT_EQ(plugin_output_sensors[i].isMember("color"), true) << "output json has no member color";
      EXPECT_EQ(plugin_output_sensors[i]["color"], sensors_of_stream[vec_idx].color);

      ASSERT_EQ(plugin_output_sensors[i].isMember("type"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["type"], sensors_of_stream[vec_idx].type);

      ASSERT_EQ(plugin_output_sensors[i].isMember("enabled"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["enabled"], true);

      ASSERT_EQ(plugin_output_sensors[i].isMember("referenceCoordinateSystem"), true)
          << "output json has no member referenceCoordinateSystem";
      EXPECT_EQ(plugin_output_sensors[i]["referenceCoordinateSystem"],
                sensors_of_stream[vec_idx].referenceCoordinateSystem);
    }
  }

  // End check unregister second sensor of stream 1

  //////////////////////////////////////////////////
  // Unregister first sensors of stream 2
  // -> no sensor list shall be sent out anymore for stream 1
  //////////////////////////////////////////////////

  // flush sending out cache
  this->dm_mock_->data_.clear();
  Viewer.unregisterAndPublishSensor(sensor_3_id);
  sensorList[test_sensor_info_3.stream].erase(sensorList[test_sensor_info_3.stream].begin());

  data_vector = this->dm_mock_->data_.front();
  teststring = std::string((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  // first stream shouldn't be in the list of streams anymore
  sensorList.erase(test_sensor_info_1.stream); // remove first stream from list
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), false)
      << "output json has still a member dataStream." << test_sensor_info_1.stream;

  // check streams
  counter = 4; // start with index 4 because first 3 sensors are removed
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  for (auto &stream : sensorList) {
    ASSERT_EQ(plugin_output["dataStream"].isMember(stream.first), true)
        << "output json has no member dataStream." << stream.first;
    ASSERT_EQ(plugin_output["dataStream"][stream.first].isMember("sensors"), true)
        << "output json has no member dataStream." << stream.first << ".sensors";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].isArray(), true)
        << "output json has no member dataStream." << stream.first << ".sensors is not an array ";
    ASSERT_EQ(plugin_output["dataStream"][stream.first]["sensors"].size(), stream.second.size())
        << "output json has no member dataStream." << stream.first << ".sensors size is unequal "
        << std::to_string(stream.second.size());

    auto &plugin_output_sensors = plugin_output["dataStream"][stream.first]["sensors"];

    // check sensors of streams
    auto &sensors_of_stream = stream.second;
    for (int i = 0; static_cast<size_t>(i) < sensors_of_stream.size(); ++i) {
      const size_t vec_idx = static_cast<size_t>(i);
      ASSERT_EQ(plugin_output_sensors[i].isMember("uid"), true) << "output json has no member uid ";
      EXPECT_EQ(plugin_output_sensors[i]["uid"], counter);
      ++counter;

      ASSERT_EQ(plugin_output_sensors[i].isMember("name"), true) << "output json has no member name";
      EXPECT_EQ(plugin_output_sensors[i]["name"], sensors_of_stream[vec_idx].name);

      ASSERT_EQ(plugin_output_sensors[i].isMember("color"), true) << "output json has no member color";
      EXPECT_EQ(plugin_output_sensors[i]["color"], sensors_of_stream[vec_idx].color);

      ASSERT_EQ(plugin_output_sensors[i].isMember("type"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["type"], sensors_of_stream[vec_idx].type);

      ASSERT_EQ(plugin_output_sensors[i].isMember("enabled"), true) << "output json has no member type";
      EXPECT_EQ(plugin_output_sensors[i]["enabled"], true);

      ASSERT_EQ(plugin_output_sensors[i].isMember("referenceCoordinateSystem"), true)
          << "output json has no member referenceCoordinateSystem";
      EXPECT_EQ(plugin_output_sensors[i]["referenceCoordinateSystem"].asString(),
                sensors_of_stream[vec_idx].referenceCoordinateSystem);
    }
  }

  // End check unregister first sensor of stream 2

  // flush sending out cache
  this->dm_mock_->data_.clear();

  //////////////////////////////////////////////////
  // Unregister second and third sensors of stream 2
  // -> empty sensor list shall be send out for stream 2
  // -> no sensor list shall be send out anymore for stream 1
  //////////////////////////////////////////////////

  Viewer.unregisterAndPublishSensor(sensor_4_id);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1) << "sensor list is not sent out.";
  // flush sending out cache
  this->dm_mock_->data_.clear();
  Viewer.unregisterAndPublishSensor(sensor_5_id);

  data_vector = this->dm_mock_->data_.front();
  teststring = std::string((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), false)
      << "output json has still a member dataStream." << test_sensor_info_1.stream;

  // check if json with empty sensor list is send out
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_3.stream), true)
      << "output json has no member dataStream." << test_sensor_info_3.stream;
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_3.stream].isMember("sensors"), true)
      << "output json has no member dataStream." << test_sensor_info_3.stream << ".sensors";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_3.stream]["sensors"].isArray(), true)
      << "plugin_output dataStream.." << test_sensor_info_3.stream << ".sensors is not an array ";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_3.stream]["sensors"].size(), 0u)
      << "plugin_output dataStream.." << test_sensor_info_3.stream << ".sensors size is unequal 0 ";

  ///////////////////////////////////////////////////
  /////////////// END UNREGISTER SENSORS///////////////
  ///////////////////////////////////////////////////

  /////////////// REGISTER SENSORS AGAIN///////////////

  ////////////////////////////////////////////////////
  // Register first sensor of first stream again
  // -> sensor is registered again
  // -> sensor uid is increased
  ///////////////////////////////////////////////////

  // flush sending out cache
  this->dm_mock_->data_.clear();

  test_sensor_info_1.name = "Test_Sensor1";
  test_sensor_info_1.color = "ffff00";
  test_sensor_info_1.referenceCoordinateSystem = "Ref_Cordinate_System1";
  test_sensor_info_1.stream = "Stream1";
  test_sensor_info_1.type = "TP";
  sensorList[test_sensor_info_1.stream].push_back(test_sensor_info_1);

  sensor_1_id = Viewer.registerAndPublishSensor(test_sensor_info_1);

  data_vector = this->dm_mock_->data_.front();
  teststring = std::string((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), true)
      << "output json has no member dataStream.Stream1";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream].isMember("sensors"), true)
      << "output json has no member dataStream.Stream1.sensors";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].isArray(), true)
      << "plugin_output dataStream.Stream1.sensors is not an array";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].size(), 1u);

  auto &plugin_output_sensors = plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"];

  ASSERT_EQ(plugin_output_sensors[0].isMember("uid"), true) << "output json has no member uid ";
  // sensor id should be increased by one to last one
  ASSERT_EQ(plugin_output_sensors[0]["uid"], 6);

  ASSERT_EQ(plugin_output_sensors[0].isMember("name"), true) << "output json has no member name";
  ASSERT_EQ(plugin_output_sensors[0]["name"], test_sensor_info_1.name);

  // End check register sensor again

  ////////////////////////////////////////////////////
  // Unregister first sensor of first stream again
  // -> sensor is unregistered again
  // -> empty sensor list is sent out for first stream
  // -> not sensor list is sent out for second stream
  ///////////////////////////////////////////////////

  // flush sending out cache
  this->dm_mock_->data_.clear();
  Viewer.unregisterAndPublishSensor(sensor_1_id);

  data_vector = this->dm_mock_->data_.front();
  teststring = std::string((char *)data_vector.data(), (this->dm_mock_->data_.front()).size());

  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_3.stream), false)
      << "output json has still a member dataStream." << test_sensor_info_3.stream;

  // check if json with empty sensor list is send out
  ASSERT_EQ(plugin_output.isMember("dataStream"), true) << "output json has no member dataStream";
  ASSERT_EQ(plugin_output["dataStream"].isMember(test_sensor_info_1.stream), true)
      << "output json has no member dataStream." << test_sensor_info_1.stream;
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream].isMember("sensors"), true)
      << "output json has no member dataStream." << test_sensor_info_1.stream << ".sensors";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].isArray(), true)
      << "plugin_output dataStream.." << test_sensor_info_1.stream << ".sensors is not an array ";
  ASSERT_EQ(plugin_output["dataStream"][test_sensor_info_1.stream]["sensors"].size(), 0u)
      << "plugin_output dataStream.." << test_sensor_info_1.stream << ".sensors size is unequal 0 ";
}

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include <thread>

#include <flatbuffers/flatbuffers.h>
#include <next/bridgesdk/schema/profiler/basic_info_generated.h>
#include <next/bridgesdk/schema/profiler/cpu_profiler_generated.h>

#include "data_manager_impl.hpp"

namespace next {
namespace databridge {

class DataManagerTest : public data_manager::DataManagerImpl {
public:
  void PublishDataToClient(const uint8_t *payload, const size_t size,
                           [[maybe_unused]] const bool force_no_cache = false,
                           [[maybe_unused]] const std::vector<size_t> &session_ids = {},
                           [[maybe_unused]] const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000),
                           [[maybe_unused]] bool binary = false) override {
#ifdef _DEBUG
    std::cout << "Mock DataManager PublishDataToClient" << std::endl;
#endif
    if (nullptr == payload) {
      std::cout << "payload is nullptr" << std::endl;
      return;
    }
#ifdef _DEBUG
    std::cout << "bytes provided: " << size << std::endl;
#endif

    std::vector<uint8_t> data(payload, payload + size);
    data_.push_back(data);
  }
  void PublishDataToClientDirectly(
      const uint8_t *payload, const size_t size, [[maybe_unused]] const std::vector<size_t> &session_ids = {},
      [[maybe_unused]] const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) override {
#ifdef _DEBUG
    std::cout << "Mock DataManager PublishDataToClientDirectly" << std::endl;
#endif
    if (nullptr == payload) {
      std::cout << "payload is nullptr" << std::endl;
      return;
    }
#ifdef _DEBUG
    std::cout << "bytes provided: " << size << std::endl;
#endif

    std::vector<uint8_t> data(payload, payload + size);
    data_.push_back(data);
  }
  std::list<std::vector<uint8_t>> data_;
};

TEST(DATA_MANAGER, check_single_decription_output) {
  auto dm_test = DataManagerTest();
  bridgesdk::plugin_config_t test;
  test.insert({"asdf", {bridgesdk::ConfigType::FIXED, "asdf"}});
  test.insert({"asdf1", {bridgesdk::ConfigType::FIXED, "asdf1"}});
  dm_test.PublishPluginDescriptionToClient("test", "plugin", test);
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 1);
  EXPECT_EQ(dm_test.data_.size(), 1);

  ASSERT_EQ(dm_test.data_.size(), 1) << "setting fbs to output failed";
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 1);

  std::vector<uint8_t> data_vector = dm_test.data_.front();
  std::string teststring((char *)data_vector.data(), (dm_test.data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("channel"), true) << "output jason  has no channel ";
  ASSERT_EQ(plugin_output.isMember("event"), true) << "output jason  has no event";
  ASSERT_EQ(plugin_output.isMember("payload"), true) << "output jason  has no payload";
  ASSERT_EQ(plugin_output.isMember("source"), true) << "output jason  has no payload";

  ASSERT_EQ(plugin_output["channel"], "dataStreamConfiguration") << "plugin_output has no valid channel data";
  ASSERT_EQ(plugin_output["event"], "AvailableDataClassesOptionsUpdate") << "plugin_output has no valid event data";
  ASSERT_EQ(plugin_output["source"], "DataSource") << "plugin_output has no valid source data";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"].isArray(), true) << "plugin_output  has no valid size";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"].size(), 1u) << "plugin_output  has no valid size";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("configurations"), true)
      << "plugin_output has no valid data for"
      << "configurations";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("name"), true) << "plugin_output has no valid data for"
                                                                               << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("name"), true)
      << "plugin_output has no valid data for "
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["name"], "test plugin")
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("plugin"), true)
      << "plugin_output has no valid data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["plugin"], "plugin")
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("parameters"), true)
      << "plugin_output has no valid data for"
      << "parameters";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].isArray(), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].size(), 2u)
      << "parameters output has no valid size ";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("editable"), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("name"), true)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("selectable"),
            true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("value"), true)
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], false)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], "asdf")
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["selectable"], false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"], "asdf")
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["editable"], false)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["name"], "asdf1")
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["selectable"], false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["value"], "asdf1")
      << "parameters output has no valid  data for"
      << "value";

  dm_test.PublishPluginDescriptionToClient("test", "plugin", test);
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 1);

  dm_test.PublishPluginDescriptionToClient("test1", "plugin", test);
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 2);
}
TEST(DATA_MANAGER, check_modified_description_ouput) {
  auto dm_test = DataManagerTest();

  std::string plugin = "ReferenceCamera";
  std::string data_class = "videostream";
  std::string source = "source";
  std::string device_name_list[4] = {"DOC_CAM_FRONT", "DOC_CAM_REAR", "AXIS_CAM_FRONT", "AXIS_CAM_REAR"};

  bridgesdk::plugin_config_t refcam_config_descriptions;
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE, device_name_list[0]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE, device_name_list[1]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);

  std::vector<uint8_t> data_vector = dm_test.data_.front();
  std::string teststring((char *)data_vector.data(), (dm_test.data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("channel"), true) << "output jason  has no channel ";
  ASSERT_EQ(plugin_output.isMember("event"), true) << "output jason  has no event";
  ASSERT_EQ(plugin_output.isMember("payload"), true) << "output jason  has no payload";
  ASSERT_EQ(plugin_output.isMember("source"), true) << "output jason  has no payload";

  ASSERT_EQ(plugin_output["channel"], "dataStreamConfiguration") << "plugin_output has no valid channel data";
  ASSERT_EQ(plugin_output["event"], "AvailableDataClassesOptionsUpdate") << "plugin_output has no valid event data";
  ASSERT_EQ(plugin_output["source"], "DataSource") << "plugin_output has no valid source data";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"].isArray(), true) << "plugin_output  has no valid size";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"].size(), 1u) << "plugin_output  has no valid size";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("configurations"), true)
      << "plugin_output has no valid data for"
      << "configurations";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("name"), true) << "plugin_output has no valid data for"
                                                                               << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("name"), true)
      << "plugin_output has no valid data for "
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["name"], (data_class + " " + plugin))
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("plugin"), true)
      << "plugin_output has no valid data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["plugin"], plugin)
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("parameters"), true)
      << "plugin_output has no valid data for"
      << "parameters";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].isArray(), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].size(), 1u)
      << "parameters output has no valid size ";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("editable"), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("name"), true)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("selectable"),
            true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("value"), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].isArray(), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].size(), 2u)
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], true)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"],
            false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[0])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[1])
      << "parameters output has no valid  data for"
      << "value";

  /*Test 2*/
  /*Changed device_name_list */
  dm_test.data_.clear();
  refcam_config_descriptions.clear();
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE, device_name_list[2]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE, device_name_list[3]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);
  data_vector = dm_test.data_.front();
  teststring = std::string((char *)data_vector.data(), (dm_test.data_.front()).size());
  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], true)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"],
            false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[2])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[3])
      << "parameters output has no valid  data for"
      << "value";
}
TEST(DATA_MANAGER, check_editable_decription_output) {

  std::string plugin = "test_plugin";
  std::string data_class = "test_data_class";
  std::string source = "source";

  auto dm_test = DataManagerTest();
  bridgesdk::plugin_config_t test;
  test.insert({source, {bridgesdk::ConfigType::EDITABLE, "TEST_1"}});
  test.insert({source, {bridgesdk::ConfigType::EDITABLE, "TEST_2"}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, test);
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 1);
  EXPECT_EQ(dm_test.data_.size(), 1);

  ASSERT_EQ(dm_test.data_.size(), 1) << "setting fbs to output failed";
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 1);

  std::vector<uint8_t> data_vector = dm_test.data_.front();
  std::string teststring((char *)data_vector.data(), (dm_test.data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("channel"), true) << "output jason  has no channel ";
  ASSERT_EQ(plugin_output.isMember("event"), true) << "output jason  has no event";
  ASSERT_EQ(plugin_output.isMember("payload"), true) << "output jason  has no payload";
  ASSERT_EQ(plugin_output.isMember("source"), true) << "output jason  has no payload";

  ASSERT_EQ(plugin_output["channel"], "dataStreamConfiguration") << "plugin_output has no valid channel data";
  ASSERT_EQ(plugin_output["event"], "AvailableDataClassesOptionsUpdate") << "plugin_output has no valid event data";
  ASSERT_EQ(plugin_output["source"], "DataSource") << "plugin_output has no valid source data";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"].isArray(), true) << "plugin_output  has no valid size";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"].size(), 1u) << "plugin_output  has no valid size";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("configurations"), true)
      << "plugin_output has no valid data for"
      << "configurations";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("name"), true) << "plugin_output has no valid data for"
                                                                               << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("name"), true)
      << "plugin_output has no valid data for "
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["name"], (data_class + " " + plugin))
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("plugin"), true)
      << "plugin_output has no valid data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["plugin"], plugin)
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("parameters"), true)
      << "plugin_output has no valid data for"
      << "parameters";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].isArray(), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].size(), 2u)
      << "parameters output has no valid size ";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("editable"), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("name"), true)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("selectable"),
            true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("value"), true)
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], true)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["selectable"], false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"], "TEST_1")
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["editable"], true)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["selectable"], false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][1]["value"], "TEST_2")
      << "parameters output has no valid  data for"
      << "value";

  dm_test.PublishPluginDescriptionToClient("test", "plugin", test);
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 2);

  dm_test.PublishPluginDescriptionToClient("test1", "plugin", test);
  EXPECT_EQ(dm_test.plugin_descriptions_.size(), 3);
}

TEST(DATA_MANAGER, check_selectable_fixed_description_ouput) {
  auto dm_test = DataManagerTest();

  std::string plugin = "ReferenceCamera";
  std::string data_class = "videostream";
  std::string source = "source";
  std::string device_name_list[4] = {"DOC_CAM_FRONT", "DOC_CAM_REAR", "AXIS_CAM_FRONT", "AXIS_CAM_REAR"};

  bridgesdk::plugin_config_t refcam_config_descriptions;
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE_FIXED, device_name_list[0]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE_FIXED, device_name_list[1]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);

  std::vector<uint8_t> data_vector = dm_test.data_.front();
  std::string teststring((char *)data_vector.data(), (dm_test.data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("channel"), true) << "output jason  has no channel ";
  ASSERT_EQ(plugin_output.isMember("event"), true) << "output jason  has no event";
  ASSERT_EQ(plugin_output.isMember("payload"), true) << "output jason  has no payload";
  ASSERT_EQ(plugin_output.isMember("source"), true) << "output jason  has no payload";

  ASSERT_EQ(plugin_output["channel"], "dataStreamConfiguration") << "plugin_output has no valid channel data";
  ASSERT_EQ(plugin_output["event"], "AvailableDataClassesOptionsUpdate") << "plugin_output has no valid event data";
  ASSERT_EQ(plugin_output["source"], "DataSource") << "plugin_output has no valid source data";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"].isArray(), true) << "plugin_output  has no valid size";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"].size(), 1u) << "plugin_output  has no valid size";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("configurations"), true)
      << "plugin_output has no valid data for"
      << "configurations";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("name"), true) << "plugin_output has no valid data for"
                                                                               << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("name"), true)
      << "plugin_output has no valid data for "
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["name"], (data_class + " " + plugin))
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("plugin"), true)
      << "plugin_output has no valid data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["plugin"], plugin)
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("parameters"), true)
      << "plugin_output has no valid data for"
      << "parameters";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].isArray(), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].size(), 1u)
      << "parameters output has no valid size ";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("editable"), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("name"), true)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("selectable"),
            true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("value"), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].isArray(), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].size(), 2u)
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], false)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"],
            false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[0])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[1])
      << "parameters output has no valid  data for"
      << "value";

  /*Test 2*/
  /*Changed device_name_list */
  dm_test.data_.clear();
  refcam_config_descriptions.clear();
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE_FIXED, device_name_list[2]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::SELECTABLE_FIXED, device_name_list[3]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);
  data_vector = dm_test.data_.front();
  teststring = std::string((char *)data_vector.data(), (dm_test.data_.front()).size());
  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], false)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"],
            false)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[2])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[3])
      << "parameters output has no valid  data for"
      << "value";
}

TEST(DATA_MANAGER, check_multiselect_description_ouput) {
  auto dm_test = DataManagerTest();

  std::string plugin = "ReferenceCamera";
  std::string data_class = "videostream";
  std::string source = "source";
  std::string device_name_list[4] = {"DOC_CAM_FRONT", "DOC_CAM_REAR", "AXIS_CAM_FRONT", "AXIS_CAM_REAR"};

  bridgesdk::plugin_config_t refcam_config_descriptions;
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT, device_name_list[0]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT, device_name_list[1]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);

  std::vector<uint8_t> data_vector = dm_test.data_.front();
  std::string teststring((char *)data_vector.data(), (dm_test.data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("channel"), true) << "output jason  has no channel ";
  ASSERT_EQ(plugin_output.isMember("event"), true) << "output jason  has no event";
  ASSERT_EQ(plugin_output.isMember("payload"), true) << "output jason  has no payload";
  ASSERT_EQ(plugin_output.isMember("source"), true) << "output jason  has no payload";

  ASSERT_EQ(plugin_output["channel"], "dataStreamConfiguration") << "plugin_output has no valid channel data";
  ASSERT_EQ(plugin_output["event"], "AvailableDataClassesOptionsUpdate") << "plugin_output has no valid event data";
  ASSERT_EQ(plugin_output["source"], "DataSource") << "plugin_output has no valid source data";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"].isArray(), true) << "plugin_output  has no valid size";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"].size(), 1u) << "plugin_output  has no valid size";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("configurations"), true)
      << "plugin_output has no valid data for"
      << "configurations";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("name"), true) << "plugin_output has no valid data for"
                                                                               << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("name"), true)
      << "plugin_output has no valid data for "
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["name"], (data_class + " " + plugin))
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("plugin"), true)
      << "plugin_output has no valid data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["plugin"], plugin)
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("parameters"), true)
      << "plugin_output has no valid data for"
      << "parameters";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].isArray(), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].size(), 1u)
      << "parameters output has no valid size ";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("editable"), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("name"), true)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("selectable"),
            true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("value"), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].isArray(), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].size(), 2u)
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], true)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"], true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[0])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[1])
      << "parameters output has no valid  data for"
      << "value";

  /*Test 2*/
  /*Changed device_name_list */
  dm_test.data_.clear();
  refcam_config_descriptions.clear();
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT, device_name_list[2]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT, device_name_list[3]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);
  data_vector = dm_test.data_.front();
  teststring = std::string((char *)data_vector.data(), (dm_test.data_.front()).size());
  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], true)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"], true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[2])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[3])
      << "parameters output has no valid  data for"
      << "value";
}

TEST(DATA_MANAGER, check_multiselect_fixed_description_ouput) {
  auto dm_test = DataManagerTest();

  std::string plugin = "ReferenceCamera";
  std::string data_class = "videostream";
  std::string source = "source";
  std::string device_name_list[4] = {"DOC_CAM_FRONT", "DOC_CAM_REAR", "AXIS_CAM_FRONT", "AXIS_CAM_REAR"};

  bridgesdk::plugin_config_t refcam_config_descriptions;
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT_FIXED, device_name_list[0]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT_FIXED, device_name_list[1]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);

  std::vector<uint8_t> data_vector = dm_test.data_.front();
  std::string teststring((char *)data_vector.data(), (dm_test.data_.front()).size());

  Json::Value plugin_output;
  Json::Reader reader;

  bool parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output.isMember("channel"), true) << "output jason  has no channel ";
  ASSERT_EQ(plugin_output.isMember("event"), true) << "output jason  has no event";
  ASSERT_EQ(plugin_output.isMember("payload"), true) << "output jason  has no payload";
  ASSERT_EQ(plugin_output.isMember("source"), true) << "output jason  has no payload";

  ASSERT_EQ(plugin_output["channel"], "dataStreamConfiguration") << "plugin_output has no valid channel data";
  ASSERT_EQ(plugin_output["event"], "AvailableDataClassesOptionsUpdate") << "plugin_output has no valid event data";
  ASSERT_EQ(plugin_output["source"], "DataSource") << "plugin_output has no valid source data";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"].isArray(), true) << "plugin_output  has no valid size";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"].size(), 1u) << "plugin_output  has no valid size";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("configurations"), true)
      << "plugin_output has no valid data for"
      << "configurations";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0].isMember("name"), true) << "plugin_output has no valid data for"
                                                                               << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("name"), true)
      << "plugin_output has no valid data for "
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["name"], (data_class + " " + plugin))
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("plugin"), true)
      << "plugin_output has no valid data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["plugin"], plugin)
      << "plugin_output has no valid data for"
      << "name";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0].isMember("parameters"), true)
      << "plugin_output has no valid data for"
      << "parameters";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].isArray(), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"].size(), 1u)
      << "parameters output has no valid size ";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("editable"), true)
      << "parameters output has no valid size ";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("name"), true)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("selectable"),
            true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0].isMember("value"), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].isArray(), true)
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"].size(), 2u)
      << "parameters output has no valid  data for"
      << "value";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], false)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"], true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[0])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[1])
      << "parameters output has no valid  data for"
      << "value";

  /*Test 2*/
  /*Changed device_name_list */
  dm_test.data_.clear();
  refcam_config_descriptions.clear();
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT_FIXED, device_name_list[2]}});
  refcam_config_descriptions.insert({source, {bridgesdk::ConfigType::MULTISELECT_FIXED, device_name_list[3]}});

  dm_test.PublishPluginDescriptionToClient(data_class, plugin, refcam_config_descriptions);
  data_vector = dm_test.data_.front();
  teststring = std::string((char *)data_vector.data(), (dm_test.data_.front()).size());
  parseSuccess = reader.parse(teststring, plugin_output, false);
  ASSERT_EQ(parseSuccess, true) << "parsing test object failed";

  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["editable"], false)
      << "parameters output has no valid  data for"
      << "editable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["multipleSelection"], true)
      << "parameters output has no valid  data for"
      << "selectable";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["name"], source)
      << "parameters output has no valid  data for"
      << "name";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][0],
            device_name_list[2])
      << "parameters output has no valid  data for"
      << "value";
  ASSERT_EQ(plugin_output["payload"]["dataClasses"][0]["configurations"][0]["parameters"][0]["value"][1],
            device_name_list[3])
      << "parameters output has no valid  data for"
      << "value";
}

} // namespace databridge
} // namespace next

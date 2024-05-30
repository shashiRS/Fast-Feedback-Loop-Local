
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <ecal/msg/string/publisher.h>

#include <next/sdk/sdk.hpp>

#include <core_lib_hash_manager.hpp>

const char *SSIGHEADER_CONTENT =
    "{\"version\":\"1.0\",\"types\":{\"structs\":[{\"name\":\"sSigHeader\",\"size\":12,\"members\":[{\"name\":"
    "\"uiTimeStamp\",\"offset\":0,\"byteOrder\":\"inconsistentBigEndian\",\"type\":\"uint32\"},{\"name\":"
    "\"uiMeasurementCounter\",\"offset\":4,\"byteOrder\":\"inconsistentBigEndian\",\"type\":\"uint16\"},{\"name\":"
    "\"uiCycleCounter\",\"offset\":6,\"byteOrder\":\"inconsistentBigEndian\",\"type\":\"uint16\"},{\"name\":"
    "\"eSigStatus\",\"offset\":8,\"byteOrder\":\"inconsistentBigEndian\",\"type\":\"uint8\"},{\"name\":\"a_reserve\","
    "\"offset\":9,\"type\":\"uint8\",\"arrayDimensions\":3}]}]}}";

const char *VELOCITY_CONTENT =
    "{\"version\":\"1.0\",\"types\":{\"structs\":[{\"name\":\"Velocity\",\"size\":4,\"members\":[{\"name\":"
    "\"Velocity\",\"offset\":0,\"byteOrder\":\"inconsistentBigEndian\",\"type\":\"float\"}]}]}}";

class HashManagerTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  HashManagerTestFixture() { this->instance_name_ = "HashManagerTestFixture"; }

  virtual void SetUp() { core_lib_.Initialize(); }

  virtual void TearDown() { core_lib_.Terminate(); }

  bool get_schema(const std::string &url, const std::string &schema) {
    std::string schema_out = "";
    if (!core_lib_.GetSchema(url, 0, schema_out)) {
      return false;
    }

    if (schema_out.compare(schema) != 0) {
      // CsZ: Better to have GTest produce a diff
      EXPECT_EQ(schema_out, schema);
      return false;
    }

    return true;
  }

protected:
  next::udex::CoreLibHashManager core_lib_;
};

TEST_F(HashManagerTestFixture, get_topic_hash_sdl_success) {
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(core_lib_.RegisterDataSources(sdl_file, "sdl")) << "RegisterDataSources failed";

  constexpr size_t kExpectedHashPackage = 17840117121602961597u;

  auto handle = core_lib_.GetTopicHash(0x20000000);
  EXPECT_EQ(handle, 0u);
  handle = core_lib_.GetTopicHash(0x20350000);
  EXPECT_EQ(handle, kExpectedHashPackage) << "vaddr not found";
  EXPECT_EQ(core_lib_.GetTopicHash("AlgoVehCycle.VehDyn", false), 0);
  EXPECT_EQ(core_lib_.GetTopicHash("AlgoVehCycle.VehDyn", true), kExpectedHashPackage);
}

TEST_F(HashManagerTestFixture, get_topic_hash_register_port_success) {

  constexpr size_t kExpectedHashPort = 16451109571720810867u;

  ASSERT_TRUE(core_lib_.RegisterDataSources("VehPar", 256));
  EXPECT_EQ(core_lib_.GetTopicHash("VehPar", false), kExpectedHashPort);
  EXPECT_EQ(core_lib_.GetTopicHash("VehPar", true), 0);
}

TEST_F(HashManagerTestFixture, get_schema_test) {
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(core_lib_.RegisterDataSources(sdl_file, "sdl")) << "RegisterDataSources failed";

  bool ret = get_schema("", "");
  EXPECT_FALSE(ret);

  ret = get_schema("SIM VFB", "");
  EXPECT_FALSE(ret);

  ret = get_schema("SIM VFB.AlgoVehCycle.VehDyn.sSigHeader", SSIGHEADER_CONTENT);
  EXPECT_TRUE(ret) << "schema is different for sSigHeader";

  ret = get_schema("SIM VFB.AlgoVehCycle.VehDyn.Longitudinal.Velocity", VELOCITY_CONTENT);
  EXPECT_TRUE(ret) << "schema is different for Velocity";
}

TEST_F(HashManagerTestFixture, getTopicsList_temp_test) {
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(core_lib_.RegisterDataSources(sdl_file, "sdl")) << "RegisterDataSources failed";

  auto &topic_list = core_lib_.GetNewRegisteredTopicsList();
  ASSERT_NE(topic_list.size(), 0u) << "topic list is empty";

  auto topic = topic_list.find("SIM VFB.AlgoVehCycle.VehDyn");
  ASSERT_TRUE(topic != topic_list.end()) << "topic not found";

  auto url = topic->first;
  EXPECT_EQ(url, "SIM VFB.AlgoVehCycle.VehDyn") << "topic name does not match";

  const size_t kExpectedHash = 17840117121602961597u;

  auto hash = topic->second.hash;
  EXPECT_EQ(hash, kExpectedHash) << "hash does not match";

  auto datasource = topic->second.data_source_name;
  EXPECT_EQ(datasource, "SIM VFB") << "data source name does not match";

  auto format = topic->second.format_type_string;
  EXPECT_EQ(format, "mts.mta") << "format does not match";
}

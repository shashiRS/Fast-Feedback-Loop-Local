#include <unordered_map>

#include <cip_test_support/gtest_reporting.h>

#include <next/next.hpp>
#include <sync_manager/syncref_handler.hpp>

#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/sdk.hpp>
#include <next_impl.hpp>

// #include <memory>

namespace next {

const char *SDL_CONTENT = R"(<?xml version="1.0" encoding="UTF-8"?>
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance" xsd:noNamespaceSchemaLocation="sdl2-rc1-before-after.xsd" ByteAlignment="1" Version="2.0">
<!-- SDL Generated From Cobolt Tool Chain  !-->
      <View Name="NextSender_0" CycleID="100">
        <Group Name="providePort_VehDyn_V1" Address="0" ArrayLen="1" Size="24">
            <Signal Name="versionNumber" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
            <SubGroup Name="sSigHeader" Offset="4" ArrayLen="1" Size="12">
                <Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
                <Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
                <Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
                <Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
                <Signal Name="a_reserve_array_t" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
            </SubGroup>
            <Signal Name="dummy_float" Offset="10" ArrayLen="1" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
            <Signal Name="dummy_int" Offset="14" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
        </Group>
      </View>
</SdlFile>)";

struct SignalHeader_t {
  uint32_t uiTimeStamp;
  uint16_t uiMeasurementCounter;
  uint16_t uiCycleCounter;
  uint8_t eSigStatus;
  uint8_t a_reserve_array_t[3];
};

struct VehDyn_t {
  uint32_t versionNumber;
  SignalHeader_t sSigHeader;
  float dummy_float;
  int32_t dummy_int;
};

class SyncRefHandlerFixture : public next::sdk::testing::TestUsingEcal {
public:
  SyncRefHandlerFixture() : TestUsingEcal() {
    this->instance_name_ = "SyncRefHandlerFixture";
    next::sdk::logger::register_to_init_chain();
  }
};

using next::udex::publisher::DataPublisher;

TEST_F(SyncRefHandlerFixture, setSyncConfig) {
  std::string publisher_sync_ref_name = "sync_ref_publisher";
  auto sync_ref_publisher = next::udex::publisher::DataPublisher("component");
  ASSERT_TRUE(sync_ref_publisher.SetDataSourceInfo(publisher_sync_ref_name, 22, 42)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(sync_ref_publisher.RegisterDataDescription((const char *)"SDL_CONTENT", (void *)SDL_CONTENT,
                                                         strlen(SDL_CONTENT), true))
      << "RegisterDataDescription failed";

  std::string publisher_sync_ref_sel_name = "sync_ref_selector_publisher";
  auto sync_ref_sel_publisher = next::udex::publisher::DataPublisher("test");
  ASSERT_TRUE(sync_ref_sel_publisher.SetDataSourceInfo(publisher_sync_ref_sel_name, 23, 43))
      << "SetDataSourceInfo failed";
  ASSERT_TRUE(sync_ref_sel_publisher.RegisterDataDescription((const char *)"SDL_CONTENT", (void *)SDL_CONTENT,
                                                             strlen(SDL_CONTENT), true))
      << "RegisterDataDescription failed";

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::string publish_syncref_url = publisher_sync_ref_name + ".NextSender_0.providePort_VehDyn_V1";
  std::string publish_syncref_sel_url = publisher_sync_ref_sel_name + ".NextSender_0.providePort_VehDyn_V1";
  std::string syncref_selector_url = publish_syncref_sel_url + ".sSigHeader.uiTimeStamp";

  std::string test_port_name = "test_port";
  std::string test_port_name2 = "test_port2";
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_sub;
  data_sub = std::make_shared<next::udex::subscriber::DataSubscriber>("test_component");
  SyncRefHandler handler("test_flow", syncref_selector_url, data_sub);

  std::string sync_ref_member_selector = publish_syncref_url + ".sSigHeader.uiTimeStamp";
  std::string sync_ref_port_selector = "sSigHeader.uiMeasurementCounter";
  handler.setupPortSyncData(test_port_name, sync_ref_member_selector, sync_ref_port_selector);
  handler.setupPortSyncData(test_port_name2, sync_ref_member_selector, sync_ref_port_selector);

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  VehDyn_t syncref_data;
  syncref_data.sSigHeader.uiTimeStamp = 1111111;
  syncref_data.sSigHeader.uiMeasurementCounter = 123;
  sync_ref_publisher.publishData(publish_syncref_url, &syncref_data, sizeof(syncref_data));
  auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                 .count();
  uint64_t ts = static_cast<uint64_t>(now);
  sync_ref_sel_publisher.publishData(publish_syncref_sel_url, &syncref_data, sizeof(syncref_data), false, ts);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  next::udex::SyncValue sync_value = handler.getSyncValue(test_port_name, ts);
  next::udex::SyncValue sync_value2 = handler.getSyncValue(test_port_name2, ts);
  EXPECT_EQ(std::get<uint64_t>(sync_value.signal_value), syncref_data.sSigHeader.uiMeasurementCounter);
  EXPECT_EQ(std::get<uint64_t>(sync_value2.signal_value), syncref_data.sSigHeader.uiMeasurementCounter);
}

} // namespace next

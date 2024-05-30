#include <atomic>
#include <numeric>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <ecal/ecal.h>
#include <boost/endian/conversion.hpp>
#include <next/sdk/sdk.hpp>

#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

using next::udex::extractor::DataExtractor;
using next::udex::publisher::DataPublisher;
using next::udex::subscriber::DataSubscriber;

#include <mts/toolbox/ecu/ecu_types.h>
#include <mts/extensibility/package.hpp>
#include "plugin/core_lib_reader/core_lib_package.h"

#define CALLBACK_NOT_CALLED 0
#define CALLBACK_CALLED_ONCE 1
#define CALLBACK_CALLED_TWICE 2
#define CALLBACK_CALLED_3_TIMES 3
#define CALLBACK_CALLED_4_TIMES 4
#define CALLBACK_CALLED_5_TIMES 5
#define CALLBACK_CALLED_6_TIMES 6
#define CALLBACK_CALLED_7_TIMES 7
#define CALLBACK_CALLED_8_TIMES 8
#define CALLBACK_CALLED_9_TIMES 9
#define CALLBACK_CALLED_10_TIMES 10
#define CALLBACK_CALLED_11_TIMES 11
#define CALLBACK_CALLED_12_TIMES 12
#define CALLBACK_CALLED_13_TIMES 13
#define CALLBACK_CALLED_14_TIMES 14
#define CALLBACK_CALLED_15_TIMES 15
#define CALLBACK_CALLED_16_TIMES 16
#define CALLBACK_CALLED_17_TIMES 17
#define CALLBACK_CALLED_18_TIMES 18

#define RECEIVED_DATA_VECTOR_SIZE_1 1
#define RECEIVED_DATA_VECTOR_SIZE_2 2
#define RECEIVED_DATA_VECTOR_SIZE_3 3

const std::string node_name = "generic_node_extractor";
const std::string url_1 = "url_1_extractor";
const std::string url_2 = "url_2_extractor";
const std::string url_3 = "url_3_extractor";
const std::string url_4 = "url_4_extractor";
const std::string url_5 = "url_5_extractor";
const std::string url_6 = "url_6_extractor";
const std::string url_7 = "url_7_extractor";
const std::string url_8 = "url_8_extractor";
const std::string url_9 = "url_9_extractor";
const std::string url_10 = "url_10_extractor";
const std::string url_11 = "url_11_extractor";
const std::string url_12 = "url_12_extractor";
const std::string url_13 = "url_13_extractor";
const std::string url_14 = "url_14_extractor";
const std::string url_15 = "url_15_extractor";

class DataExtractorTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DataExtractorTestFixture() { this->instance_name_ = "DataExtractorTestFixture"; }
};

const std::vector<std::string> urls = {url_1, url_2,  url_3,  url_4,  url_5,  url_6,  url_7, url_8,
                                       url_9, url_10, url_11, url_12, url_13, url_14, url_15};

constexpr int wait_time_ms = 5000;
constexpr int wait_increment_ms = 100;
constexpr int after_send_delay_ms = 100;

void sendPayload(eCAL::CPublisher &pub, std::string binary_data, std::atomic<bool> &success) {
  std::cout << "sending: '" << binary_data << "', waiting for subscription ";
  int waited_time = 0;
  while (!pub.IsSubscribed()) {
    if (waited_time > wait_time_ms) {
      std::cerr << " there is no subscription\n" << std::flush;
      return;
    }
    waited_time += wait_increment_ms;
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_increment_ms));
  }
  std::cout << " subscriber connected\n" << std::flush;

  const size_t n_bytes = pub.Send(binary_data.c_str(), binary_data.length(), -1, wait_time_ms);

  if (n_bytes != binary_data.length()) {
    std::cerr << "failed to send data!\n" << std::flush;
    return;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(after_send_delay_ms));
  success = true;
}

bool sendPayloadAsync(eCAL::CPublisher &pub, std::string binary_data) {
  std::atomic<bool> success(false);

  auto pub_thread = std::thread([&pub, &binary_data, &success]() { sendPayload(pub, binary_data, success); });

  pub_thread.join();
  return success;
}

void DataWait(std::shared_ptr<DataExtractor> extractor, std::string url) {
  std::cout << "waiting for data in queue ";
  int waited_time = 0;
  while (!extractor->isDataAvailable(url)) {
    if (waited_time > wait_time_ms) {
      std::cerr << "wait_time expired" << std::flush;
      ASSERT_TRUE(false);
    }
    waited_time += wait_increment_ms;
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_increment_ms));
  }

  std::cout << "data extracted\n" << std::flush;
  return;
}

void ensure_database_exists(DataPublisher &publisher, const std::string &sdl_file, const std::string &name,
                            const uint16_t source_id, const uint8_t instance_number) {

  // registering the SDL under the given name and numbers
  ASSERT_TRUE(publisher.SetDataSourceInfo(name, source_id, instance_number)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
}

void CheckSubscriberConnection(DataSubscriber &subscriber, const std::string &url) {
  const std::chrono::milliseconds timeout(20000);
  auto start_time = std::chrono::steady_clock::now();
  while (!subscriber.IsConnected(url)) {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);

    ASSERT_LT(elapsed_time, timeout) << "Timeout reached. Aborting the test.";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

TEST_F(DataExtractorTestFixture, input_queue_blocking_test) {

  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  extractor->SetBlockingExtraction(true);
  extractor->SetBlockingExtractionForUrl(url_1, true);

  EXPECT_NE(subscriber.Subscribe(url_1, extractor), 0u);
  eCAL::CPublisher ecal_publisher(url_1);
  eCAL::Util::EnableLoopback(true);

  extractor->connectCallback([](const std::string) {}, url_1);

  auto callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 1);
  CheckIsSubscribed(ecal_publisher);

  std::string payload = "whatever";
  std::atomic<bool> success = false;
  sendPayload(ecal_publisher, payload, success);
  ASSERT_TRUE(success) << "sending data failed";

  // this will be executed only after the extraction is finished
  // during development this was faked with a delay in the udex_queue::exec()
  EXPECT_TRUE(extractor->isDataAvailable(url_1));
}

TEST_F(DataExtractorTestFixture, get_raw_data_and_signal_info) {
  std::string ref_name = generateUniqueName("get_raw_data_and_signal_info");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "get_raw_data_and_signal_info" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);
  eCAL::CPublisher ecal_publisher(url_1t);

  CheckIsSubscribed(ecal_publisher);

  extractor->connectCallback([](const std::string) {}, url_1t);

  auto callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 1);

  std::string payload = "whatever";
  std::atomic<bool> dummy_bool;
  sendPayload(ecal_publisher, payload, dummy_bool);
  ASSERT_TRUE(dummy_bool.load()) << "sending data failed";

  if (!extractor->isDataAvailable(url_1t)) {
    ASSERT_TRUE(false);
  }
  auto data = extractor->GetRawData(url_1t);
  if (data.Size == 0) {
    std::cout << ecal_publisher.IsSubscribed() << std::endl;
  }
  ASSERT_EQ(data.Size, payload.length());
  EXPECT_EQ(memcmp(data.Data, payload.c_str(), payload.length()), 0);
  EXPECT_EQ(data.info.format_type, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN);
  EXPECT_EQ(data.info.PackageUrl, url_1t);
  EXPECT_NE(data.info.Time, 0);

  auto publisher = DataPublisher(node_name_ref);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("ARS5xx.sdl")) << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::udex::SignalInfo signal_info;

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
  EXPECT_EQ(signal_info.signal_size, 160);
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 0);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT32);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint32_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 0);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
  EXPECT_EQ(signal_info.signal_size, 12);
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 4);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT32);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint32_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 4);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
  EXPECT_EQ(signal_info.array_size, 3);
  EXPECT_EQ(signal_info.offset, 13);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve[0]");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 13);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve[1]");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 14);

  signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve[2]");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 15);
}

TEST_F(DataExtractorTestFixture, check_can_data_and_signal_info) {
  std::string ref_name = generateUniqueName("get_raw_data_and_signal_info");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;
  std::string pub_fake_name = "fake" + ref_name;

  auto publisher1 = DataPublisher(pub_fake_name);

  ASSERT_TRUE(publisher1.SetDataSourceInfo("CAN_DATA", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher1.RegisterDataDescription((std::string) "Vehicle_CAN2_Public.dbc", true, DESCRIPTION_TYPE_DBC))
      << "RegisterDataDescription failed";

  auto extractor = std::make_shared<DataExtractor>();

  std::atomic<bool> hit1 = false;
  extractor->connectCallback([&hit1](const std::string) { hit1 = true; }, url_1t);

  auto callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 1);

  auto publisher = DataPublisher(node_name_ref);

  ASSERT_TRUE(publisher.SetDataSourceInfo("CAN_DATA", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription((std::string) "Vehicle_CAN2_Public.dbc", true, DESCRIPTION_TYPE_DBC))
      << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::udex::SignalInfo signal_info;
  auto &topic = publisher.getTopicsAndHashes();
  EXPECT_EQ(topic.size(), 14);

  signal_info = extractor->GetInfo("CAN_DATA.Vehicle_CAN2_Public.METER_2F9.METER_HOD_0X1B_GWDATA_VALUE");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT32);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint32_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 3);

  signal_info = extractor->GetInfo("CAN_DATA.Vehicle_CAN2_Public.IDASB_1DF.IDAS_ENG_TARGET_TORQUE");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_INT16);
  EXPECT_EQ(signal_info.signal_size, sizeof(int16_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 0);

  signal_info = extractor->GetInfo("CAN_DATA.Vehicle_CAN2_Public.IDASB_1DF.IDAS_TARGET_DECELERATION");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
  EXPECT_EQ(signal_info.signal_size, sizeof(float));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 5);

  signal_info = extractor->GetInfo("CAN_DATA.Vehicle_CAN2_Public.METER_2F9.METER_HOD_0X04_GWDATA_ONOFF");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT16);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint16_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 0);

  signal_info = extractor->GetInfo("CAN_DATA.Vehicle_CAN2_Public.ESB_086.BRK_REQ_PRESSURE_STARTUP");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 2);
}

TEST_F(DataExtractorTestFixture, check_ethernet_data_and_signal_info) {

  std::string ref_name = generateUniqueName("check_ethernet_data_and_signal_info");
  std::string node_name_ref = node_name + ref_name;

  auto publisher = DataPublisher(node_name_ref);
  std::string data_source_name = "check_ethernet_data_and_signal_info" + ref_name;
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_source_name.c_str(), 22, 33)) << "SetDataSourceInfo failed";
  std::string description_file_name = "FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS";
  std::string description_file_path = description_file_name + ".xml";
  ASSERT_TRUE(publisher.RegisterDataDescription(description_file_path.c_str(), true, DESCRIPTION_TYPE_FIBEX))
      << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  auto &topics = publisher.getTopicsAndHashes();
  EXPECT_GE(topics.size(), 152);

  auto extractor = std::make_shared<DataExtractor>();

  std::string url = data_source_name + "." + description_file_name;
  std::string complete_url = "";
  next::udex::SignalInfo signal_info;

  // SIGNAL_TYPE_CHAR
  complete_url = url + ".FRR.BrakeAssistFrontRadar_0xd019_0x1.BrakeAssistFrontRadar.idIbrakeTargetObject";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_CHAR);
  EXPECT_EQ(signal_info.signal_size, sizeof(char));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 45);

  // SIGNAL_TYPE_UINT8
  complete_url = url + ".FRR.ObjectFusionFront_0xd006_0x1.ObjectRecognition.numberOfOtherVehicles";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 27);

  // SIGNAL_TYPE_UINT16
  complete_url = url + ".FRR.ObjectFusionFront_0xd006_0x1.ObjectPosition.counter";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT16);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint16_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 2);

  // SIGNAL_TYPE_UINT32
  complete_url = url + ".FRR.ObjectFusionFront_0xd006_0x1.ObjectMovementStandardDeviation.dataID";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT32);
  EXPECT_EQ(signal_info.signal_size, sizeof(uint32_t));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 4);

  // SIGNAL_TYPE_FLOAT
  complete_url = url + ".FRR.BrakeAssistFrontRadar_0xd019_0x1.BrakeAssistFrontRadar.collisionAvoidanceDeceleration";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
  EXPECT_EQ(signal_info.signal_size, sizeof(float));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 32);

  // SIGNAL_TYPE_DOUBLE
  complete_url = url + ".iCAM2.Controller_0x3001_0x1.LateralDynamics.maximalCurvatureLeft";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
  EXPECT_EQ(signal_info.signal_size, sizeof(float));
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 16);

  // SIGNAL_TYPE_STRUCT
  complete_url = url + ".FRR.ObjectFusionFront_0xd006_0x1.ObjectRecognition";
  signal_info = extractor->GetInfo(complete_url.c_str());
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
  EXPECT_EQ(signal_info.signal_size, 332);
  EXPECT_EQ(signal_info.array_size, 1);
  EXPECT_EQ(signal_info.offset, 0);
}

// the test file (arrays.sdl) contains some arrays:
#pragma pack(push, 1)
struct simple_float_arrays_t {
  // here we do not have any crazy offsets
  float ten_floats[10];
  float ten_more_floats[10];
};
static_assert(sizeof(simple_float_arrays_t) == 80);
struct array_of_structs_t {
  struct first_group_t {
    // the first group is easy (size 12 bytes)
    uint64_t big_int;
    unsigned short unsigned_short;
    unsigned short another_unsigned_short;
  };
  struct second_group_t {
    // the second group itself is also easy (4 bytes)
    unsigned char unsigned_char1;
    unsigned char unsigned_char2;
    unsigned char unsigned_char3;
    unsigned char unsigned_char4;
  };
  first_group_t first_group[5];   // 5*12 bytes = 60 bytes
  second_group_t second_group[7]; // 7*4 bytes = 28 bytes
};
struct array_of_array_t {
  struct array_group_t {
    // simple, 21 bytes in total
    unsigned short unsigned_short_array[10];
    unsigned char unsigned_char;
  };
  struct second_group_t {
    // 18 bytes in total
    unsigned char unsigned_char1[6];
    unsigned char unsigned_char2[5];
    unsigned char unsigned_char3[4];
    unsigned char unsigned_char4[3];
  };
  array_group_t array_group[10];  // 10*21 bytes = 210 bytes
  second_group_t second_group[7]; // 7*18 bytes = 126 bytes
};
#pragma pack(pop)
static_assert(sizeof(simple_float_arrays_t) == 80);
static_assert(sizeof(array_of_structs_t) == 88);
static_assert(sizeof(array_of_array_t) == 336);

TEST_F(DataExtractorTestFixture, extractor_get_info_simple_array_as_array) {
  std::string ref_name = generateUniqueName("extractor_get_info_simple_array_as_array");
  std::string node_name_ref = node_name + ref_name;
  std::string data_soruceinfo_name = "extractor_get_info_simple_array_as_array" + ref_name;

  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 21, 41)) << "Failed to set the data source";
  ASSERT_TRUE(publisher.RegisterDataDescription(std::string("arrays.sdl"), true)) << "Failed to load sdl file";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::udex::SignalInfo signal_info;
  signal_info = extractor->GetInfo(data_soruceinfo_name + ".array_root.simple_float_arrays.ten_floats");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
  EXPECT_EQ(signal_info.signal_size, 4);
  EXPECT_EQ(signal_info.array_size, 10);
  EXPECT_EQ(signal_info.offset, 0);
  signal_info = extractor->GetInfo(data_soruceinfo_name + ".array_root.simple_float_arrays.ten_more_floats");
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
  EXPECT_EQ(signal_info.signal_size, 4);
  EXPECT_EQ(signal_info.array_size, 10);
  EXPECT_EQ(signal_info.offset, 40);
}

TEST_F(DataExtractorTestFixture, extractor_get_info_simple_array_signals) {

  std::string ref_name = generateUniqueName("extractor_get_info_simple_array_signals");
  std::string node_name_ref = node_name + ref_name;
  std::string data_soruceinfo_name = "extractor_get_info_simple_array_signals" + ref_name;

  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 21, 41)) << "Failed to set the data source";
  ASSERT_TRUE(publisher.RegisterDataDescription(std::string("arrays.sdl"), true)) << "Failed to load sdl file";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::udex::SignalInfo signal_info;
  for (unsigned int i = 0; i < 10u; ++i) {
    const std::string url1(data_soruceinfo_name + ".array_root.simple_float_arrays.ten_floats[" + std::to_string(i) +
                           "]");
    signal_info = extractor->GetInfo(std::string(url1));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
    EXPECT_EQ(signal_info.signal_size, 4);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * sizeof(float));
    const std::string url2(data_soruceinfo_name + ".array_root.simple_float_arrays.ten_more_floats[" +
                           std::to_string(i) + "]");
    signal_info = extractor->GetInfo(std::string(url2));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_FLOAT);
    EXPECT_EQ(signal_info.signal_size, 4);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * sizeof(float) + 40);
  }
}

TEST_F(DataExtractorTestFixture, extractor_get_info_array_of_struct_signals) {

  std::string ref_name = generateUniqueName("extractor_get_info_array_of_struct_signals");
  std::string node_name_ref = node_name + ref_name;
  std::string data_soruceinfo_name = "extractor_get_info_array_of_struct_signals" + ref_name;

  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 21, 41)) << "Failed to set the data source";
  ASSERT_TRUE(publisher.RegisterDataDescription(std::string("arrays.sdl"), true)) << "Failed to load sdl file";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::udex::SignalInfo signal_info;

  signal_info = extractor->GetInfo(std::string(data_soruceinfo_name + ".array_root.array_of_structs.first_group"));
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
  EXPECT_EQ(signal_info.signal_size, 12);
  EXPECT_EQ(signal_info.array_size, 5);
  EXPECT_EQ(signal_info.offset, 0);

  for (int i = 0; i < 5; ++i) {
    const int block_size = 12;
    const std::string url_base(data_soruceinfo_name + ".array_root.array_of_structs.first_group[" + std::to_string(i) +
                               "]");

    signal_info = extractor->GetInfo(std::string(url_base));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
    EXPECT_EQ(signal_info.signal_size, 12);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * block_size);

    signal_info = extractor->GetInfo(std::string(url_base + ".big_int"));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT64);
    EXPECT_EQ(signal_info.signal_size, 8);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * block_size);
    signal_info = extractor->GetInfo(std::string(url_base + ".unsigned_short"));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT16);
    EXPECT_EQ(signal_info.signal_size, 2);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * block_size + 8);
    signal_info = extractor->GetInfo(std::string(url_base + ".another_unsigned_short"));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT16);
    EXPECT_EQ(signal_info.signal_size, 2);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * block_size + 10);
  }

  signal_info = extractor->GetInfo(std::string(data_soruceinfo_name + ".array_root.array_of_structs.second_group"));
  EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
  EXPECT_EQ(signal_info.signal_size, 4);
  EXPECT_EQ(signal_info.array_size, 7);
  EXPECT_EQ(signal_info.offset, 60);

  for (int i = 0; i < 7; ++i) {
    const int offset = 60;
    const int block_size = 4;
    const std::string url_base(data_soruceinfo_name + ".array_root.array_of_structs.second_group[" + std::to_string(i) +
                               "]");

    signal_info = extractor->GetInfo(std::string(url_base));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_STRUCT);
    EXPECT_EQ(signal_info.signal_size, 4);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, 60 + i * block_size);

    for (int j = 0; j < 3; ++j) {
      signal_info = extractor->GetInfo(std::string(url_base + ".unsigned_char" + std::to_string(j + 1)));
      EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
      EXPECT_EQ(signal_info.signal_size, 1);
      EXPECT_EQ(signal_info.array_size, 1);
      EXPECT_EQ(signal_info.offset, offset + i * block_size + j);
    }
  }
}

TEST_F(DataExtractorTestFixture, extractor_get_info_array_of_arrays) {

  std::string ref_name = generateUniqueName("extractor_get_info_array_of_arrays");
  std::string node_name_ref = node_name + ref_name;
  std::string data_soruceinfo_name = "extractor_get_info_array_of_arrays" + ref_name;

  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 21, 41)) << "Failed to set the data source";
  ASSERT_TRUE(publisher.RegisterDataDescription(std::string("arrays.sdl"), true)) << "Failed to load sdl file";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  next::udex::SignalInfo signal_info;

  for (int i = 0; i < 10; ++i) {
    const int block_size = 21;
    const std::string url_base(data_soruceinfo_name + ".array_root.array_of_array.array_group[" + std::to_string(i) +
                               "]");
    signal_info = extractor->GetInfo(std::string(url_base + ".unsigned_short_array"));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT16);
    EXPECT_EQ(signal_info.signal_size, 2);
    EXPECT_EQ(signal_info.array_size, 10);
    EXPECT_EQ(signal_info.offset, i * block_size);
    for (int j = 0; j < 10; ++j) {
      signal_info = extractor->GetInfo(std::string(url_base + ".unsigned_short_array[" + std::to_string(j) + "]"));
      EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT16);
      EXPECT_EQ(signal_info.signal_size, 2);
      // EXPECT_EQ(signal_info.array_size, 1); //TODO should be 1, but we get 10
      EXPECT_EQ(signal_info.offset, i * block_size + j * 2);
    }
    signal_info = extractor->GetInfo(std::string(url_base + ".unsigned_char"));
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
    EXPECT_EQ(signal_info.signal_size, 1);
    EXPECT_EQ(signal_info.array_size, 1);
    EXPECT_EQ(signal_info.offset, i * block_size + 20);
  }
}

TEST_F(DataExtractorTestFixture, extractor_get_value_simple_array_signals) {

  std::string ref_name = generateUniqueName("extractor_get_value_simple_array_signals");
  std::string node_name_ref = node_name + ref_name;
  std::string data_soruceinfo_name = "extractor_get_value_simple_array_signals" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(data_soruceinfo_name);
  extractor->SetBlockingExtraction(true);

  std::string url = data_soruceinfo_name + ".array_root.simple_float_arrays";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 21, 41)) << "Failed to set the data source";
  ASSERT_TRUE(publisher.RegisterDataDescription(std::string("arrays.sdl"), true)) << "Failed to load sdl file";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  next::udex::SignalInfo signal_info;

  simple_float_arrays_t simple_floats;
  memset(&simple_floats, 0, sizeof(simple_float_arrays_t));
  std::iota(simple_floats.ten_more_floats, simple_floats.ten_more_floats + 10, -5.f);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  publisher.publishData(url, &simple_floats, sizeof(simple_floats));
  DataWait(extractor, url);
  for (int i = 0; i < 10; ++i) {
    next::udex::dataType return_value;
    EXPECT_TRUE(extractor->GetValue(url + ".ten_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value))
        << "float " << i << " failed";
    EXPECT_EQ(std::get<float>(return_value), 0.f);
    EXPECT_TRUE(extractor->GetValue(url + ".ten_more_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), simple_floats.ten_more_floats[i]);
  }
}

TEST_F(DataExtractorTestFixture, extractor_get_value_simple_array_signals_variable_length) {
  std::string ref_name = generateUniqueName("extractor_get_value_simple_array_signals_variable_length");
  std::string node_name_ref = node_name + ref_name;
  std::string data_source_info_name = "extractor_get_value_simple_array_signals_variable_length" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  extractor->SetBlockingExtraction(true);
  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_source_info_name + ".array_root.simple_float_arrays";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  ASSERT_TRUE(publisher.SetDataSourceInfo(data_source_info_name, 21, 41)) << "Failed to set the data source";
  ASSERT_TRUE(publisher.RegisterDataDescription(std::string("arrays.sdl"), true)) << "Failed to load sdl file";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  simple_float_arrays_t simple_floats;
  memset(&simple_floats, 0, sizeof(simple_float_arrays_t));
  std::iota(simple_floats.ten_more_floats, simple_floats.ten_more_floats + 10, -5.f);

  // 1) send full package
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  publisher.publishData(url, &simple_floats, sizeof(simple_floats));
  DataWait(extractor, url);
  for (int i = 0; i < 10; ++i) {
    next::udex::dataType return_value;
    EXPECT_TRUE(extractor->GetValue(url + ".ten_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), 0.f);
    EXPECT_TRUE(extractor->GetValue(url + ".ten_more_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), simple_floats.ten_more_floats[i]);
  }
  extractor->ClearExtractedQueue(url);

  // only publish a limited number of floats
  publisher.publishData(url, &simple_floats, sizeof(float) * 14);
  DataWait(extractor, url);
  for (int i = 0; i < 10; ++i) {
    next::udex::dataType return_value;
    EXPECT_TRUE(extractor->GetValue(url + ".ten_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), 0.f);
    EXPECT_TRUE(extractor->GetValue(url + ".ten_more_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));

    // check if first values are correct
    if (i < 4) {
      EXPECT_EQ(std::get<float>(return_value), simple_floats.ten_more_floats[i]);
    } else {
      EXPECT_EQ(std::get<float>(return_value), 0.f);
    }
  }
  extractor->ClearExtractedQueue(url);

  // full package again
  publisher.publishData(url, &simple_floats, sizeof(simple_floats));
  DataWait(extractor, url);
  for (int i = 0; i < 10; ++i) {
    next::udex::dataType return_value;
    EXPECT_TRUE(extractor->GetValue(url + ".ten_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), 0.f);
    EXPECT_TRUE(extractor->GetValue(url + ".ten_more_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), simple_floats.ten_more_floats[i]);
  }
  extractor->ClearExtractedQueue(url);

  // check again limited number
  publisher.publishData(url, &simple_floats, sizeof(float) * 14);
  DataWait(extractor, url);
  for (int i = 0; i < 10; ++i) {
    next::udex::dataType return_value;
    EXPECT_TRUE(extractor->GetValue(url + ".ten_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), 0.f);
    EXPECT_TRUE(extractor->GetValue(url + ".ten_more_floats[" + std::to_string(i) + "]",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));

    if (i < 4) {
      EXPECT_EQ(std::get<float>(return_value), simple_floats.ten_more_floats[i]);
    } else {
      EXPECT_EQ(std::get<float>(return_value), 0.f);
    }
  }
}

typedef struct {
  uint32_t uiTimeStamp = 32;
  uint16_t uiMeasurementCounter = 16;
  uint16_t uiCycleCounter = 16;
  uint8_t eSigStatus = 8;
  uint8_t a_reserve[3] = {0, 1, 2};
} SignalHeader_t;

typedef struct {
  uint32_t versionNumber = 32;
  SignalHeader_t sigHeader;
} VehDyn_t;

TEST_F(DataExtractorTestFixture, extractor_get_value) {
  std::string ref_name = generateUniqueName("check_ethernet_data_and_signal_info");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "get_value" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url}, 5000);

  VehDyn_t veh_dyn;

  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));

  DataWait(extractor, url);

  try {
    next::udex::dataType return_value;

    EXPECT_FALSE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                     next::udex::SignalType::SIGNAL_TYPE_UINT8, return_value));
    EXPECT_FALSE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                     next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));

    // this is true but it's turning unsigned -> signed, posible loss of data, not sure what to do here
    extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                        next::udex::SignalType::SIGNAL_TYPE_INT32, return_value);

    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 32u);

    auto extracted_signal_information =
        extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber");
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value,
                                    &extracted_signal_information));
    EXPECT_EQ(std::get<uint32_t>(return_value), 32u);

    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT64, return_value));
    EXPECT_EQ(std::get<uint64_t>(return_value), 32u);

    // same as unsigned -> signed, possible data loss
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
    EXPECT_EQ(std::get<float>(return_value), 32.f);
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_DOUBLE, return_value));
    EXPECT_EQ(std::get<double>(return_value), 32.);

    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 32u);
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiMeasurementCounter",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));
    EXPECT_EQ(std::get<uint16_t>(return_value), 16u);
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));
    EXPECT_EQ(std::get<uint16_t>(return_value), 16u);
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.eSigStatus",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT8, return_value));
    EXPECT_EQ(std::get<uint8_t>(return_value), 8u);

    std::vector<next::udex::dataType> vec_return_value;
    ASSERT_TRUE(extractor->GetVectorValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve",
                                          next::udex::SignalType::SIGNAL_TYPE_UINT32, vec_return_value));
    ASSERT_EQ(vec_return_value.size(), 3u);
    for (unsigned int i = 0; i < 3; i++) {
      EXPECT_EQ(std::get<uint32_t>(vec_return_value[i]), i);
    }

    vec_return_value.clear();

    next::udex::SignalInfo signal_info;
    ASSERT_TRUE(extractor->GetValueAndInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve",
                                           signal_info, vec_return_value));
    for (unsigned int i = 0; i < 3; i++) {
      EXPECT_EQ(std::get<uint8_t>(vec_return_value[i]), i);
    }

    signal_info = extractor->GetInfo(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.a_reserve");
    EXPECT_EQ(signal_info.signal_type, next::udex::SignalType::SIGNAL_TYPE_UINT8);
    EXPECT_EQ(signal_info.signal_size, sizeof(uint8_t));
    EXPECT_EQ(signal_info.array_size, 3);
    EXPECT_EQ(signal_info.offset, 13);

    // TODO this needs to be checked and handeled
    // EXPECT_FALSE(extractor->GetSignalValueAndInfo("ARS5xx.AlgoVehCycle.VehDyn.bad_signal", signal_info,
    // vec_return_value));

    EXPECT_FALSE(extractor->GetValueAndInfo("something.that.does.not.exist", signal_info, vec_return_value));
  } catch (std::bad_variant_access const &ex) {
    std::cout << ex.what() << ": nope ";
  }
}

TEST_F(DataExtractorTestFixture, extractor_get_value_big_endian) {
  using namespace next::udex;
  std::string ref_name = generateUniqueName("extractor_get_value_big_endian");

  auto subscriber = std::make_unique<DataSubscriber>(ref_name);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = std::make_unique<DataPublisher>(ref_name);

  std::string url = ref_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber->Subscribe(url, extractor), 0u);

  ASSERT_TRUE(publisher->SetDataSourceInfo(ref_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx_big_endian.sdl");
  ASSERT_TRUE(publisher->RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  VehDyn_t veh_dyn;
  veh_dyn.sigHeader.uiTimeStamp = 1;
  veh_dyn.sigHeader.uiMeasurementCounter = 1;
  veh_dyn.sigHeader.uiCycleCounter = 1;
  veh_dyn.versionNumber = 1;

  publisher->publishData(url, &veh_dyn, sizeof(veh_dyn));
  DataWait(extractor, url);
  dataType return_value;
  ASSERT_TRUE(extractor->GetValue(url + ".sSigHeader.uiTimeStamp", SignalType::SIGNAL_TYPE_UINT32, return_value));
  EXPECT_EQ(std::get<uint32_t>(return_value), boost::endian::endian_reverse(veh_dyn.sigHeader.uiTimeStamp));

  ASSERT_TRUE(extractor->GetValue(url + ".uiVersionNumber", SignalType::SIGNAL_TYPE_UINT32, return_value));
  EXPECT_EQ(std::get<uint32_t>(return_value), boost::endian::endian_reverse(veh_dyn.versionNumber));

  ASSERT_TRUE(
      extractor->GetValue(url + ".sSigHeader.uiMeasurementCounter", SignalType::SIGNAL_TYPE_UINT16, return_value));
  EXPECT_EQ(std::get<uint16_t>(return_value), boost::endian::endian_reverse(veh_dyn.sigHeader.uiMeasurementCounter));

  ASSERT_TRUE(extractor->GetValue(url + ".sSigHeader.uiCycleCounter", SignalType::SIGNAL_TYPE_UINT16, return_value));
  EXPECT_EQ(std::get<uint16_t>(return_value), boost::endian::endian_reverse(veh_dyn.sigHeader.uiCycleCounter));
}

TEST_F(DataExtractorTestFixture, push_data_extractor_single_element_queue_size) {
  std::string ref_name = generateUniqueName("push_data_extractor_single_element_queue_size");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  unsigned short single_element_queue_size = 1;
  auto extractor = std::make_shared<DataExtractor>(single_element_queue_size);

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);

  eCAL::CPublisher publisher(url_1t);

  for (int i = 0; i < 5; i++) {
    std::string payload = "payload_";
    payload.append(std::to_string(i));
    ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";
  }

  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  ASSERT_EQ(extracted_data.size(), single_element_queue_size);
  auto extracted_data_element = extracted_data.front();
  EXPECT_EQ(memcmp(extracted_data_element->data, "payload_4", extracted_data_element->size), 0);
}

TEST_F(DataExtractorTestFixture, push_data_extractor_limited_queue_size) {

  std::string ref_name = generateUniqueName("push_data_extractor_limited_queue_size");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  unsigned short limited_queue_size = 2;
  auto extractor = std::make_shared<DataExtractor>(limited_queue_size);

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);

  eCAL::CPublisher publisher(url_1t);

  for (int i = 0; i < 5; i++) {
    std::string payload = "payload_";
    payload.append(std::to_string(i));
    ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data of payload '" << payload << "' failed";
  }

  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  ASSERT_EQ(extracted_data.size(), limited_queue_size);
  auto extracted_data_element = extracted_data.front();
  EXPECT_EQ(memcmp(extracted_data_element->data, "payload_3", extracted_data_element->size), 0);

  extracted_data_element = extracted_data.back();
  EXPECT_EQ(memcmp(extracted_data_element->data, "payload_4", extracted_data_element->size), 0);
}

TEST_F(DataExtractorTestFixture, push_data_extractor_queue_size_test) {
  std::string ref_name = generateUniqueName("push_data_extractor_queue_size_test");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);

  eCAL::CPublisher publisher(url_1t);

  int samples_number = 30;
  for (int i = 0; i < samples_number; i++) {
    std::string payload = "payload_";
    payload.append(std::to_string(i));
    ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";
  }

  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), samples_number);
}

TEST_F(DataExtractorTestFixture, remove_elements_from_queue_test) {

  std::string ref_name = generateUniqueName("remove_elements_from_queue_test");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);

  eCAL::CPublisher publisher(url_1t);

  int samples_number = 30;
  for (int i = 0; i < samples_number; i++) {
    std::string payload = "payload_";
    payload.append(std::to_string(i));
    ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";
  }

  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), samples_number);

  extractor->RemoveElementsFromQueue(url_1t, 10);
  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 20);

  extractor->RemoveElementsFromQueue(url_1t, 10);
  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 10);

  extractor->RemoveElementsFromQueue(url_1t, 10);
  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 0);

  extractor->RemoveElementsFromQueue(url_1t, 10);
  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 0);
}

TEST_F(DataExtractorTestFixture, push_data_callback_connect_disconnect) {

  std::string ref_name = generateUniqueName("push_data_callback_connect_disconnect");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;
  std::string url_2t = url_2 + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1t, extractor);
  auto id_2 = subscriber.Subscribe(url_2t, extractor);
  EXPECT_NE(id_1, id_2);

  std::atomic<bool> hit1 = false;
  std::atomic<bool> hit2 = false;
  auto id1 = extractor->connectCallback([&hit1](const std::string) { hit1 = true; }, url_1t);
  auto id2 = extractor->connectCallback([&hit2](const std::string) { hit2 = true; }, url_1t);
  auto id3 = extractor->connectCallback([](const std::string) {}, url_2t);

  EXPECT_EQ(id1, 1);
  EXPECT_EQ(id2, 2);
  EXPECT_EQ(id3, 3);

  auto callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 3);

  eCAL::CPublisher publisher(url_1t);
  CheckIsSubscribed(publisher);

  std::string payload = "payload_";
  ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";

  DataWait(extractor, url_1t);

  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 1);

  EXPECT_TRUE(hit1) << "callback for id1 not hit";
  EXPECT_TRUE(hit2) << "callback for id2 not hit";

  extractor->disconnectCallback(id1);

  callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 2);

  hit1 = false;
  hit2 = false;

  ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";
  DataWait(extractor, url_1t);

  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 2);

  EXPECT_FALSE(hit1) << "callback for id1 hit but whould not be called";
  EXPECT_TRUE(hit2) << "callback for id2 not hit";

  subscriber.Unsubscribe(id_1);

  hit1 = false;
  hit2 = false;

  ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";

  extractor->ClearExtractedQueue(url_1t);
  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 0);

  EXPECT_FALSE(hit1) << "callback for id1 hit but whould not be called";
  EXPECT_FALSE(hit2) << "callback for id2  hit but whould not be called";

  subscriber.Subscribe(url_1t, extractor);

  CheckIsSubscribed(publisher);

  hit1 = false;
  hit2 = false;
  std::atomic<bool> hit3 = false;
  auto id4 = extractor->connectCallback([&hit3](const std::string) { hit3 = true; }, url_1t);
  EXPECT_EQ(id4, 4);

  callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 3);

  ASSERT_TRUE(sendPayloadAsync(publisher, payload)) << "sending data failed";

  EXPECT_FALSE(hit1) << "callback for id1 hit but was disconnected earlier";
  EXPECT_TRUE(hit2) << "callback for id2 not hit";
  EXPECT_TRUE(hit3) << "callback for id4 not hit";
}

TEST_F(DataExtractorTestFixture, push_data_no_extractor_callback) {

  std::string ref_name = generateUniqueName("push_data_no_extractor_callback");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);

  eCAL::CPublisher publisher(url_1t);

  std::string payload1 = "some_payload1";
  ASSERT_TRUE(sendPayloadAsync(publisher, payload1)) << "sending data failed";
  DataWait(extractor, url_1t);

  std::string payload2 = "some_payload2_different_size";
  ASSERT_TRUE(sendPayloadAsync(publisher, payload2)) << "sending data failed";
  DataWait(extractor, url_1t);

  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  ASSERT_EQ(extracted_data.size(), 2);
  auto extracted_data_element = extracted_data.front();
  EXPECT_EQ(extracted_data_element->size, payload1.size());
  EXPECT_EQ(memcmp(extracted_data_element->data, payload1.c_str(), payload1.size()), 0);

  extracted_data_element = extracted_data.back();
  EXPECT_EQ(extracted_data_element->size, payload2.size());
  EXPECT_EQ(memcmp(extracted_data_element->data, payload2.c_str(), payload2.size()), 0);

  extractor->ClearExtractedQueue(url_1t);

  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  EXPECT_EQ(extracted_data.size(), 0);

  std::string payload3 = "some_other_payload3";
  ASSERT_TRUE(sendPayloadAsync(publisher, payload3)) << "sending data failed";
  DataWait(extractor, url_1t);

  extracted_data = extractor->GetExtractedQueueWithTimestamp(url_1t);
  ASSERT_EQ(extracted_data.size(), 1);
  extracted_data_element = extracted_data.front();
  EXPECT_EQ(extracted_data_element->size, payload3.size());
  EXPECT_EQ(memcmp(extracted_data_element->data, payload3.c_str(), payload3.size()), 0);
}

TEST_F(DataExtractorTestFixture, callback_connect_disconnect) {
  std::string ref_name = generateUniqueName("callback_connect_disconnect");
  std::string node_name_ref = node_name + ref_name;
  std::string url_1t = url_1 + ref_name;

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(url_1t, extractor), 0u);

  auto id1 = extractor->connectCallback([](const std::string) {}, url_1t);
  auto callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 1);

  extractor->disconnectCallback(id1);

  callbacks_info = extractor->getCallbacksInfo();
  EXPECT_EQ(callbacks_info.size(), 0);
}

TEST_F(DataExtractorTestFixture, multiple_url_callback_test) {
  std::string ref_name = generateUniqueName("multiple_url_callback_test");
  std::string node_name_ref = node_name + ref_name;

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();

  std::map<std::string, eCAL::CPublisher> publisher_map;

  for (auto url_tmp : urls) {
    std::string url = url_tmp + ref_name;
    subscriber.Subscribe(url, extractor);
    std::atomic<bool> called = false;
    extractor->connectCallback([&called](const std::string) { called = true; }, url);

    publisher_map[url] = eCAL::CPublisher(url);
    eCAL::Util::EnableLoopback(true);

    EXPECT_EQ(extractor->GetExtractedQueueWithTimestamp(url).size(), 0);

    ASSERT_TRUE(sendPayloadAsync(publisher_map[url], "payload_" + url)) << "sending data failed";
    DataWait(extractor, url);
    EXPECT_TRUE(called) << "callback not hit";
    EXPECT_EQ(extractor->GetExtractedQueueWithTimestamp(url).size(), RECEIVED_DATA_VECTOR_SIZE_1);
  }
}

TEST_F(DataExtractorTestFixture, GetSynchExtractedData_GetExactValue_test) {
  std::string ref_name = generateUniqueName("GetSynchExtractedData_GetExactValue_test");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "GetSynchExtractedData_GetExactValue_test" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url});

  VehDyn_t veh_dyn;
  veh_dyn.sigHeader.uiTimeStamp = 100;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 101;
  veh_dyn.sigHeader.uiMeasurementCounter = 1;
  veh_dyn.sigHeader.uiCycleCounter = 1;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 102;
  veh_dyn.sigHeader.uiMeasurementCounter = 2;
  veh_dyn.sigHeader.uiCycleCounter = 2;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 103;
  veh_dyn.sigHeader.uiMeasurementCounter = 3;
  veh_dyn.sigHeader.uiCycleCounter = 3;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  std::string sync_url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp";
  DataWait(extractor, url);

  auto no_sync_extraction = extractor->GetExtractedData(url);
  EXPECT_NE(no_sync_extraction.data, nullptr);
  EXPECT_EQ(no_sync_extraction.size, 160);

  try {
    next::udex::dataType return_value;

    extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value);
    EXPECT_EQ(std::get<uint32_t>(return_value), 103u);

    next::udex::SyncAlgoCfg sync_cfg;
    sync_cfg.algorythm_type = next::udex::getSyncAlgoTypeEnum("FIND_EXACT_VALUE");

    sync_cfg.composition_sync_signal_url = sync_url;

    extractor->SetSyncAlgo(url, sync_cfg);

    next::udex::SyncValue value{next::udex::SignalType::SIGNAL_TYPE_UINT64, (uint64_t)102};
    std::vector<next::udex::SyncValue> sync_values;
    sync_values.push_back(value);

    extractor->GetSyncExtractedData(url, sync_values);

    EXPECT_TRUE(extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 102u);

    sync_values[0].signal_value = (uint64_t)100;

    auto bad_extraction = extractor->GetSyncExtractedData(url, sync_values);
    EXPECT_EQ(bad_extraction.data, nullptr);
    EXPECT_EQ(bad_extraction.size, 0);
    EXPECT_EQ(bad_extraction.timestamp, 0);
  } catch (std::bad_variant_access const &ex) {
    std::cout << ex.what() << ": nope ";
  }

  std::vector<next::udex::SyncValue> sync_values;
  auto bad_extraction = extractor->GetSyncExtractedData("something.that.does.not.exist", sync_values);
  EXPECT_EQ(bad_extraction.data, nullptr);
  EXPECT_EQ(bad_extraction.size, 0);
  EXPECT_EQ(bad_extraction.timestamp, 0);
}

TEST_F(DataExtractorTestFixture, GetSynchExtractedData_GetNearestValue_test) {
  std::string ref_name = generateUniqueName("GetSynchExtractedData_GetNearestValue_test");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "GetSynchExtractedData_GetNearestValue_test" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url});

  VehDyn_t veh_dyn;
  veh_dyn.sigHeader.uiTimeStamp = 100;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 101;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 102;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 103;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 104;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  std::string sync_url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp";
  DataWait(extractor, url);

  auto no_sync_extraction = extractor->GetExtractedData(url);
  EXPECT_NE(no_sync_extraction.data, nullptr);
  EXPECT_EQ(no_sync_extraction.size, 160);

  try {
    next::udex::dataType return_value;

    extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value);
    EXPECT_EQ(std::get<uint32_t>(return_value), 104u);

    next::udex::SyncAlgoCfg sync_cfg;
    sync_cfg.algorythm_type = next::udex::getSyncAlgoTypeEnum("FIND_NEAREST_VALUE");
    sync_cfg.composition_sync_signal_url = sync_url;

    /*Request middle element from que */
    extractor->SetSyncAlgo(url, sync_cfg);

    next::udex::SyncValue value{next::udex::SignalType::SIGNAL_TYPE_UINT64, (uint64_t)102};
    std::vector<next::udex::SyncValue> sync_values;
    sync_values.push_back(value);

    extractor->GetSyncExtractedData(url, sync_values);
    EXPECT_TRUE(extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 101u);

    /*Request Last element from que */
    sync_values[0].signal_value = (uint64_t)104;
    extractor->GetSyncExtractedData(url, sync_values);
    EXPECT_TRUE(extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 103u);
  } catch (std::bad_variant_access const &ex) {
    std::cout << ex.what() << ": nope ";
  }
}

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

// enable in ticket SIMEN-7387
TEST_F(DataExtractorTestFixture, GetSynchExtractedData_ExactHeaderTimestamp_test) {

  eCAL::Util::EnableLoopback(true);

  struct url_info_t {
    std::string url;
    size_t size;
    size_t offset;
    int value;
  };

  std::vector<url_info_t> url_info_vec{{"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_INFOBLOCK", 128, 0, 1},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_ETHERNET", 20, 256, 2},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_HIL", 4, 368, 3},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_HWINFO", 12, 388, 4},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_CHANNELVARIATIONDATA", 108, 528, 5},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_METADATA", 96, 768, 6},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_HOMOCTRYSEL", 20, 2032, 7},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_HOMOMONLIMITS", 84, 2064, 8},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_CALIBRATION", 9372, 2304, 9},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_SELFTEST", 3980, 11776, 10},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_OEM", 32, 16272, 11},
                                       {"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_TRACEINFO", 64, 16304, 12}};

  const uint32_t source_id_test = 6403u;
  const uint16_t instance_number_test = 0u;
  // create nodes
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  auto subscriber = next::udex::subscriber::DataSubscriber("generic_node_publisher");
  auto publisher = next::udex::publisher::DataPublisher("generic_node_publisher");

  ASSERT_TRUE(publisher.SetDataSourceInfo("ARS5xx", source_id_test, instance_number_test))
      << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx_RTMLF.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";

  auto url_info = url_info_vec[0];

  EXPECT_NE(subscriber.Subscribe(url_info.url, extractor), 0u);

  CheckConnection(subscriber, {url_info.url}, 5000);
  // setting synch mode
  next::udex::SyncAlgoCfg sync_cfg;
  sync_cfg.algorythm_type = next::udex::getSyncAlgoTypeEnum("FIND_EXACT_HEADER_TIMESTAMP");
  extractor->SetSyncAlgo(url_info.url, sync_cfg);

  mts_ecu_software_header_t header;
  header.address = 0x8004C000;
  header.control = 0;
  header.control_length.length = 16368;
  header.task_counter = 209;
  header.task_id = 209;

  size_t extended_header_size = sizeof(header) + 4;

  std::vector<uint8_t> payload(header.control_length.length + extended_header_size);

  // reset
  memset(payload.data(), 0, header.control_length.length);

  // copy header
  memcpy(payload.data(), &header, sizeof(header));

  // generate some data
  for (const auto &ui : url_info_vec) {
    auto offset = extended_header_size + ui.offset;
    memset(payload.data() + offset, ui.value, ui.size);
  }

  // create package
  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(),
                                      payload.size() + extended_header_size);
  // auto payload_start = package->data();

  package->source_id = source_id_test;
  package->source_location = 0;
  package->instance_number = instance_number_test;
  // package->format_type
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = 209;
  package->cycle_counter = 7974;
  package->cycle_state = 4;
  package->size = header.control_length.length + extended_header_size;

  auto i_package_temp = CreateNextIPackage(package);
  publisher.publishPackage(i_package_temp);
  DataWait(extractor, url_info.url);

  // constructing second package
  auto memory_allocator_ptr2 = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package2(memory_allocator_ptr2, "mts.mta.sw", payload.data(),
                                       payload.size() + extended_header_size);
  // auto payload_start = package->data();

  package2->source_id = source_id_test;
  package2->source_location = 0;
  package2->instance_number = instance_number_test;
  // package->format_type
  package2->hardware_timestamp.id = 13;
  package2->hardware_timestamp.time = 734487770;
  package2->cycle_id = 209;
  package2->cycle_counter = 7974;
  package2->cycle_state = 4;
  package2->size = header.control_length.length + extended_header_size;
  package2->reference_timestamp = 6709316285;
  // url_info_vec

  auto offset = extended_header_size + url_info.offset;
  // altering payload for checking the synch
  memset(payload.data() + offset, 10, url_info.size);
  auto i_package_temp2 = CreateNextIPackage(package2);
  publisher.publishPackage(i_package_temp2);
  DataWait(extractor, url_info.url);

  // constructing third pacakge
  auto memory_allocator_ptr3 = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package3(memory_allocator_ptr3, "mts.mta.sw", payload.data(),
                                       payload.size() + extended_header_size);
  // auto payload_start = package->data();
  package3->source_id = source_id_test;
  package3->source_location = 0;
  package3->instance_number = instance_number_test;
  // package->format_type
  package3->hardware_timestamp.id = 13;
  package3->hardware_timestamp.time = 734487770;
  package3->cycle_id = 209;
  package3->cycle_counter = 7974;
  package3->cycle_state = 4;
  package3->size = header.control_length.length + extended_header_size;
  package3->reference_timestamp = 6709316290;
  memset(payload.data() + offset, 12, url_info.size);

  auto i_package_temp_3 = CreateNextIPackage(package3);
  EXPECT_TRUE(publisher.publishPackage(i_package_temp_3));

  DataWait(extractor, url_info.url);

  try {
    next::udex::dataType return_value;

    // setting timestamp of package 2 and trying to receive it (value should be 10)
    next::udex::SyncValue value{next::udex::SignalType::SIGNAL_TYPE_UINT64, (uint64_t)6709316285u};
    std::vector<next::udex::SyncValue> sync_values;
    sync_values.push_back(value);

    next::udex::TimestampedDataPacket packet = extractor->GetSyncExtractedData(url_info.url, sync_values);
    EXPECT_EQ(packet.timestamp,
              (uint64_t)6709316285); // checking timestamp which should be equal to the given sync value

  } catch (std::bad_variant_access const &ex) {
    std::cout << ex.what() << ": nope ";
  }
}

TEST_F(DataExtractorTestFixture, GetSynchExtractedData_DefaultAlgoFunc_test) {
  std::string ref_name = generateUniqueName("GetSynchExtractedData_DefaultAlgoFunc_test");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "GetSynchExtractedData_DefaultAlgoFunc_test" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url});

  VehDyn_t veh_dyn;
  veh_dyn.sigHeader.uiTimeStamp = 100;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 101;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 102;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  veh_dyn.sigHeader.uiTimeStamp = 103;
  veh_dyn.sigHeader.uiMeasurementCounter = 0;
  veh_dyn.sigHeader.uiCycleCounter = 0;
  publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  std::string sync_url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp";
  DataWait(extractor, url);

  auto no_sync_extraction = extractor->GetExtractedData(url);
  EXPECT_NE(no_sync_extraction.data, nullptr);
  EXPECT_EQ(no_sync_extraction.size, 160);

  try {
    next::udex::dataType return_value;

    extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value);
    EXPECT_EQ(std::get<uint32_t>(return_value), 103u);

    next::udex::SyncAlgoCfg sync_cfg;
    sync_cfg.algorythm_type = next::udex::getSyncAlgoTypeEnum("GET_LAST_ELEMENT");
    sync_cfg.composition_sync_signal_url = sync_url;

    extractor->SetSyncAlgo(url, sync_cfg);
    next::udex::SyncValue value{next::udex::SignalType::SIGNAL_TYPE_UINT64, (uint64_t)102};
    std::vector<next::udex::SyncValue> sync_values;
    sync_values.push_back(value);

    extractor->GetSyncExtractedData(url, sync_values);
    EXPECT_TRUE(extractor->GetValue(sync_url, next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 103u);
  } catch (std::bad_variant_access const &ex) {
    std::cout << ex.what() << ": nope ";
  }
}

TEST_F(DataExtractorTestFixture, udex_que_sync_test) {
  auto publisher_dummy = DataPublisher("fake");
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", "ARS5xx", 22, 33);

  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();
  auto publisher = DataPublisher(node_name);

  eCAL::Util::EnableLoopback(true);

  std::string url = "ARS5xx.AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo("ARS5xx", 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  for (uint16_t count = 0u; count < 5u; count++) {
    VehDyn_t veh_dyn;
    veh_dyn.sigHeader.uiTimeStamp = count;
    veh_dyn.sigHeader.uiMeasurementCounter = uint16_t(count + uint16_t(10u));
    veh_dyn.sigHeader.uiCycleCounter = uint16_t(count + uint16_t(20u));
    publisher.publishData(url, &veh_dyn, sizeof(veh_dyn));
  }
  EXPECT_EQ(extractor->GetExtractedQueueWithTimestamp(url).size(), 5u);
}

TEST_F(DataExtractorTestFixture, processing_queue_callback_loop_blocking_extraction) {
  std::string ref_name = generateUniqueName("processing_queue_callback_loop");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "processing_queue_callback_loop" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  extractor->SetBlockingExtraction(true);

  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);
  size_t size_100kb = 100000;
  char *my_test_data;
  my_test_data = (char *)malloc(size_100kb);
  ASSERT_TRUE(my_test_data != nullptr);
  VehDyn_t *veh_dyn = (VehDyn_t *)my_test_data;
  veh_dyn->sigHeader.uiCycleCounter = 1;

  std::atomic<bool> hit1 = false;
  auto callback = [&extractor, &url](const std::string) {
    auto data = extractor->GetExtractedData(url);
    ASSERT_GE(data.size, sizeof(VehDyn_t)) << "package does not fit expected size";
    VehDyn_t *veh_dyn_cast = (VehDyn_t *)data.data;
    EXPECT_EQ(veh_dyn_cast->sigHeader.uiCycleCounter, 1);
  };
  extractor->connectCallback(callback, url);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url});

  publisher.publishData(url, my_test_data, size_100kb);
  EXPECT_EQ(extractor->GetExtractedQueueWithTimestamp(url).size(), 1u);
  free(my_test_data);
}

TEST_F(DataExtractorTestFixture, processing_queue_callback_loop_non_blocking_extraction) {
  std::string ref_name = generateUniqueName("processing_queue_callback_loop");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "processing_queue_callback_loop" + ref_name;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  extractor->SetBlockingExtraction(false);

  auto publisher = DataPublisher(node_name_ref);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);
  size_t size_100kb = 100000;
  char *my_test_data;
  my_test_data = (char *)malloc(size_100kb);
  ASSERT_TRUE(my_test_data != nullptr);
  VehDyn_t *veh_dyn = (VehDyn_t *)my_test_data;
  veh_dyn->sigHeader.uiCycleCounter = 1;

  std::atomic<bool> hit1 = false;
  auto callback = [&extractor, &url, &hit1](const std::string) {
    std::cout << "test callback" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto data = extractor->GetExtractedData(url);
    ASSERT_GE(data.size, sizeof(VehDyn_t)) << "package does not fit expected size";
    VehDyn_t *veh_dyn_cast = (VehDyn_t *)data.data;
    EXPECT_EQ(veh_dyn_cast->sigHeader.uiCycleCounter, 1);
    hit1 = true;
  };
  extractor->connectCallback(callback, url);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckSubscriberConnection(subscriber, url);

  publisher.publishData(url, my_test_data, size_100kb);
  std::cout << "test" << std::endl;
  EXPECT_EQ(extractor->GetExtractedQueueWithTimestamp(url).size(), 1u);

  free(my_test_data);
  int counter = 0;
  while (hit1.load() == false) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    counter++;
    ASSERT_LE(counter, 20);
  }
}

TEST_F(DataExtractorTestFixture, convertVersion_validInput_correctOutput) {
  DataExtractor extractor;
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.0.1")), 0x000001u);
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.0.10")), 0x00000Au);
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.0.255")), 0x0000FFu);
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.1.0")), 0x000100u);
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.10.0")), 0x000A00u);
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.255.0")), 0x00FF00u);
  EXPECT_EQ(extractor.ConvertVersion(std::string("1.0.0")), 0x010000u);
  EXPECT_EQ(extractor.ConvertVersion(std::string("10.0.0")), 0x0A0000u);
  EXPECT_EQ(extractor.ConvertVersion(std::string("255.0.0")), 0xFF0000u);

  EXPECT_EQ(extractor.ConvertVersion(std::string("0.0.256")), 0x000000u)
      << "256 = 0x0100, first byte dropped -> 0x00 expected";
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.17.300")), 0x00112Cu)
      << "300 = 0x01C2, first byte dropped -> 0x1C expected (without overflow from patch into minor)";
  EXPECT_EQ(extractor.ConvertVersion(std::string("0.256.0")), 0x000000u)
      << "256 = 0x0100, first byte dropped -> 0x00 expected";
  EXPECT_EQ(extractor.ConvertVersion(std::string("34.900.0")), 0x228400u)
      << "900 = 0x0384, first byte dropped -> 0x84 expected (without overflow from minor into major)";
  EXPECT_EQ(extractor.ConvertVersion(std::string("256.0.0")), 0x000000u)
      << "256 = 0x0100, first byte dropped -> 0x00 expected";
  EXPECT_EQ(extractor.ConvertVersion(std::string("321.0.0")), 0x410000u)
      << "321 = 0x0141, first byte dropped -> 0x41 expected";
}

TEST_F(DataExtractorTestFixture, convertVersion_invalidInput_exceptionThrown) {
  DataExtractor extractor;
  EXPECT_THROW(extractor.ConvertVersion(std::string("asdf")), std::invalid_argument);
  EXPECT_THROW(extractor.ConvertVersion(std::string("1")), std::invalid_argument);
  EXPECT_THROW(extractor.ConvertVersion(std::string("1.2.3.4.5")), std::invalid_argument);
  EXPECT_THROW(extractor.ConvertVersion(std::string("0.1.0.1.2")), std::invalid_argument);
  EXPECT_THROW(extractor.ConvertVersion(std::string("0.a.0")), std::invalid_argument);
}

TEST_F(DataExtractorTestFixture, insert_emptyPackage) {
  DataExtractor extractor;

  extractor.InsertEmptyDataPacket("a", 10000);
  auto result = extractor.GetExtractedQueueWithTimestamp("a");
  EXPECT_EQ(result.size(), 1);
  result = extractor.GetExtractedQueueWithTimestamp("b");
  EXPECT_EQ(result.size(), 0);

  extractor.InsertEmptyDataPacket("b", 10000);
  result = extractor.GetExtractedQueueWithTimestamp("b");
  EXPECT_EQ(result.size(), 1);
  extractor.InsertEmptyDataPacket("b", 100);
  result = extractor.GetExtractedQueueWithTimestamp("b");
  EXPECT_EQ(result.size(), 2);
}

TEST_F(DataExtractorTestFixture, insert_emptyPackage_zeroSize) {
  DataExtractor extractor;

  extractor.InsertEmptyDataPacket("a", 0);
  auto result = extractor.GetExtractedQueueWithTimestamp("a");
  EXPECT_EQ(result.size(), 1);
  result = extractor.GetExtractedQueueWithTimestamp("b");
  EXPECT_EQ(result.size(), 0);

  extractor.InsertEmptyDataPacket("b", 0);
  result = extractor.GetExtractedQueueWithTimestamp("b");
  EXPECT_EQ(result.size(), 1);
  extractor.InsertEmptyDataPacket("b", 0);
  result = extractor.GetExtractedQueueWithTimestamp("b");
  EXPECT_EQ(result.size(), 2);
}

#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>

#include <next/sdk/sdk.hpp>

#include <gia_dll_handler.h>
#include <next/sdk/sdk_macros.h>
#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>
#include "veh_dyn_t.h"

const std::string node_name = "generic_node_gia";
const std::string VehDyn_url = "VDY.VehDyn_t";
const std::string VehDyn_data_type = "VehDyn_t";

constexpr int wait_time_ms = 3000;
constexpr int wait_increment_ms = 100;
constexpr int after_send_delay_ms = 100;

class GiaDllHandlerTest : public GiaDllHandler {

public:
  GiaDllHandlerTest(){};
  ~GiaDllHandlerTest(){};

  std::string RemoveExtensionTest(const std::string &adapter_dll, const std::string &extension) {
    return RemoveExtension(adapter_dll, extension);
  }

  std::string HandleFileNameTest(const std::string &adapter_dll) { return HandleFileName(adapter_dll); }
};

class GiaTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  GiaTestFixture() { this->instance_name_ = "GiaTestFixture"; }
};

bool DataWait(std::shared_ptr<next::udex::extractor::DataExtractor> extractor, std::string url) {
  std::cout << "waiting for data in queue ";
  int waited_time = 0;
  while (!extractor->isDataAvailable(url)) {
    if (waited_time > wait_time_ms) {
      std::cerr << "wait_time expired" << std::flush;
      return false;
    }
    waited_time += wait_increment_ms;
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_increment_ms));
  }

  std::cout << "data extracted\n" << std::flush;
  return true;
}

TEST_F(GiaTestFixture, converter_correctness_test) {
  auto subscriber = next::udex::subscriber::DataSubscriber(node_name);

  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>("gia_adapter");

  auto publisher = next::udex::publisher::DataPublisher(node_name);
  eCAL::Util::EnableLoopback(true);

  EXPECT_NE(subscriber.Subscribe(VehDyn_url, extractor), 0u);
  extractor->SetExtractionVersion(VehDyn_url, sizeof(VehDyn_t_V_2::VehDyn_t), "0.0.2", VehDyn_data_type);

  // struct to be published
  VehDyn_t_V_1::VehDyn_t veh_dyn_V_1;
  memset(&veh_dyn_V_1, 0, sizeof(veh_dyn_V_1));
  veh_dyn_V_1.versionNumber = 1;

  veh_dyn_V_1.sigHeader.uiTimeStamp = 0x11111111;
  veh_dyn_V_1.sigHeader.uiMeasurementCounter = 0x2222;
  veh_dyn_V_1.sigHeader.uiCycleCounter = 0x3333;
  veh_dyn_V_1.sigHeader.eSigStatus = 1;

  veh_dyn_V_1.dummy_float = 3.14f;
  veh_dyn_V_1.dummy_int = 0x9999;

  // struct expected after GIA
  VehDyn_t_V_2::VehDyn_t veh_dyn_V_2;
  memset(&veh_dyn_V_2, 0, sizeof(veh_dyn_V_2));
  veh_dyn_V_2.versionNumber = 2;

  veh_dyn_V_2.sigHeader.uiTimeStamp = 0x11111111;
  veh_dyn_V_2.sigHeader.uiMeasurementCounter = 0x2222;
  veh_dyn_V_2.sigHeader.uiCycleCounter = 0x3333;
  veh_dyn_V_2.sigHeader.eSigStatus = 1;

  veh_dyn_V_2.dummy_float = 3.14f;
  veh_dyn_V_2.dummy_int = 0x9999;
  veh_dyn_V_2.dummy_bool = false;

  publisher.RegisterDataDescription(VehDyn_url, sizeof(veh_dyn_V_1), true);

  // publishing the data
  publisher.publishData(VehDyn_url, &veh_dyn_V_1, sizeof(veh_dyn_V_1));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  extractor->isDataAvailable(VehDyn_url);
  // raw data before extraction and GIA
  auto raw_data = extractor->GetRawData(VehDyn_url);

  EXPECT_EQ(raw_data.Size, sizeof(veh_dyn_V_1));
  EXPECT_EQ(memcmp(raw_data.Data, &veh_dyn_V_1, raw_data.Size), 0);

  // wait for extraction and GIA to process
  DataWait(extractor, VehDyn_url);

  auto extracted_data = extractor->GetExtractedData(VehDyn_url);

  EXPECT_EQ(extracted_data.size, sizeof(veh_dyn_V_2));
  EXPECT_EQ(memcmp(extracted_data.data, &veh_dyn_V_2, extracted_data.size), 0);
}

TEST_F(GiaTestFixture, load_adapter_base_input) {
  GiaDllHandler gia_dll_handler;

  auto adapter = gia_dll_handler.GetAdapter("gia_adapter");

  EXPECT_NE(adapter, nullptr);
}

TEST_F(GiaTestFixture, load_adapter_dll_input) {
  GiaDllHandler gia_dll_handler;

  auto adapter = gia_dll_handler.GetAdapter("gia_adapter.dll");

  EXPECT_NE(adapter, nullptr);
}

TEST_F(GiaTestFixture, load_adapter_so_input) {
  GiaDllHandler gia_dll_handler;

  auto adapter = gia_dll_handler.GetAdapter("libgia_adapter.so");

  EXPECT_NE(adapter, nullptr);
}

TEST_F(GiaTestFixture, load_adapter_unconventional_so_fail) {
  GiaDllHandler gia_dll_handler;

  // missing 'lib' prefix
  auto adapter = gia_dll_handler.GetAdapter("gia_adapter.so");

  EXPECT_EQ(adapter, nullptr);
}

TEST_F(GiaTestFixture, gia_dll_handler_RemoveExtension) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension(".ext");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.RemoveExtensionTest(file_name, extension);
  EXPECT_EQ(result, base_name);
}

TEST_F(GiaTestFixture, gia_dll_handler_RemoveExtension_dll) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension(".dll");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.RemoveExtensionTest(file_name, extension);
  EXPECT_EQ(result, base_name);
}

TEST_F(GiaTestFixture, gia_dll_handler_RemoveExtension_so) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension(".so");
  std::string file_name("lib" + base_name + extension);

  auto result = gia_dll_handler.RemoveExtensionTest(file_name, extension);
  EXPECT_EQ(result, base_name);
}

TEST_F(GiaTestFixture, gia_dll_handler_HandleFileName) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension(".ext");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.HandleFileNameTest(file_name);

  std::string reference = base_name + kSharedLibFileExtension;

#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  reference = kSharedLibFilePrefix + reference;
#endif

  EXPECT_EQ(result, reference);
}

TEST_F(GiaTestFixture, gia_dll_handler_HandleFileName_double_dot_fail) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension("..ext");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.HandleFileNameTest(file_name);

  std::string reference = base_name + kSharedLibFileExtension;

#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  reference = kSharedLibFilePrefix + reference;
#endif

  EXPECT_NE(result, reference);
}

TEST_F(GiaTestFixture, gia_dll_handler_HandleFileName_absolute_path) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("d:/folder/file");
  std::string extension(".ext");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.HandleFileNameTest(file_name);

  std::string reference = base_name + kSharedLibFileExtension;

#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  reference = kSharedLibFilePrefix + reference;
#endif

  EXPECT_EQ(result, reference);
}

TEST_F(GiaTestFixture, gia_dll_handler_HandleFileName_relative_path) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("./../folder/file");
  std::string extension(".ext");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.HandleFileNameTest(file_name);

  std::string reference = base_name + kSharedLibFileExtension;

#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  reference = kSharedLibFilePrefix + reference;
#endif

  EXPECT_EQ(result, reference);
}

TEST_F(GiaTestFixture, gia_dll_handler_HandleFileName_dll) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension(".dll");
  std::string file_name(base_name + extension);

  auto result = gia_dll_handler.HandleFileNameTest(file_name);

  std::string reference = base_name + kSharedLibFileExtension;

#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  reference = kSharedLibFilePrefix + reference;
#endif

  EXPECT_EQ(result, reference);
}

TEST_F(GiaTestFixture, gia_dll_handler_HandleFileName_so) {
  GiaDllHandlerTest gia_dll_handler;

  std::string base_name("file");
  std::string extension(".so");
  std::string file_name("lib" + base_name + extension);

  auto result = gia_dll_handler.HandleFileNameTest(file_name);

  std::string reference = base_name + kSharedLibFileExtension;

#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  reference = kSharedLibFilePrefix + reference;
#endif

  EXPECT_EQ(result, reference);
}

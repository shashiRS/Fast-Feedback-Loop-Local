#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

using next::udex::extractor::DataExtractor;
using next::udex::publisher::DataPublisher;
using next::udex::subscriber::DataSubscriber;

class DataExtractorSyncAsyncTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DataExtractorSyncAsyncTestFixture() { this->instance_name_ = "DataExtractorSyncAsyncTestFixture"; }
};

const std::string node_name = "generic_node_extractor";
const std::string url_1 = "url_1_extractor";

void ensure_database_exists(DataPublisher &publisher, const std::string &sdl_file, const std::string &name,
                            const uint16_t source_id, const uint8_t instance_number) {

  // registering the SDL under the given name and numbers
  ASSERT_TRUE(publisher.SetDataSourceInfo(name, source_id, instance_number)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
}

typedef struct {
  uint32_t uiTimeStamp = 32;
  uint16_t uiMeasurementCounter = 16;
  uint16_t uiCycleCounter = 16;
  uint8_t eSigStatus = 8;
  uint8_t a_reserve[3] = {0, 1, 2};
} SignalHeader_t;

typedef struct {
  uint32_t uiVersionNumber = 32;
  SignalHeader_t sigHeader;
} VehDyn_t;

TEST_F(DataExtractorSyncAsyncTestFixture, check_extractor_sync_data_test) {
  std::string ref_name = generateUniqueName("check_extractor_sync_data_test");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "check_extractor_sync_data_test" + ref_name;
  const bool sync_mode = true;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name);
  auto publisher = DataPublisher(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  extractor->SetBlockingExtraction(sync_mode);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);
  size_t size_100kb = 100000;
  char *my_test_data = new char[size_100kb];
  ASSERT_TRUE(my_test_data) << "Data allocation to the heap failed";
  VehDyn_t *veh_dyn = (VehDyn_t *)my_test_data;
  veh_dyn->sigHeader.uiCycleCounter = 1;
  veh_dyn->uiVersionNumber = 32;
  int callback_cnt = 0;
  next::udex::dataType return_value;

  auto callback = [&](const std::string) {
    callback_cnt++;
    EXPECT_TRUE(extractor->isDataAvailable(url)) << "Extracted data is not available at the callback";
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 32u);

    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));
    EXPECT_EQ(std::get<uint16_t>(return_value), 1u);

    next::udex::TimestampedDataPacket extracted_data = extractor->GetExtractedData(url);
    EXPECT_NE(extracted_data.data, nullptr);
    ASSERT_GE(extracted_data.size, sizeof(VehDyn_t)) << "Extracted package does not fit expected size";
  };
  extractor->connectCallback(callback, url);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url});

  publisher.publishData(url, my_test_data, size_100kb);
  delete[] my_test_data;

  EXPECT_NE(callback_cnt, 0) << "Callback not received after publishing data";
  std::cout << "Callbacks received: " << callback_cnt << std::endl;
  EXPECT_TRUE(extractor->isDataAvailable(url)) << "Extracted data is not available right after publishing";

  EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                  next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
  EXPECT_EQ(std::get<uint32_t>(return_value), 32u);

  EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
                                  next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));
  EXPECT_EQ(std::get<uint16_t>(return_value), 1u);

  next::udex::TimestampedDataPacket extracted_data = extractor->GetExtractedData(url);
  EXPECT_NE(extracted_data.data, nullptr);
  ASSERT_GE(extracted_data.size, sizeof(VehDyn_t)) << "Extracted package does not fit expected size";
}

TEST_F(DataExtractorSyncAsyncTestFixture, check_extractor_async_data_test) {
  std::string ref_name = generateUniqueName("check_extractor_async_data_test");
  std::string node_name_ref = node_name + ref_name;
  std::string pub_fake_name = "fake" + ref_name;
  std::string data_soruceinfo_name = "check_extractor_async_data_test" + ref_name;
  const bool sync_mode = false;

  eCAL::Util::EnableLoopback(true);
  auto publisher_dummy = DataPublisher(pub_fake_name);
  ensure_database_exists(publisher_dummy, "ARS5xx.sdl", data_soruceinfo_name, 22, 33);

  auto subscriber = DataSubscriber(node_name);
  auto publisher = DataPublisher(node_name_ref);
  auto extractor = std::make_shared<DataExtractor>();
  extractor->SetBlockingExtraction(sync_mode);

  std::string url = data_soruceinfo_name + ".AlgoVehCycle.VehDyn";
  EXPECT_NE(subscriber.Subscribe(url, extractor), 0u);
  size_t size_100kb = 100000;
  char *my_test_data = new char[size_100kb];
  ASSERT_TRUE(my_test_data) << "Data allocation to the heap failed";
  VehDyn_t *veh_dyn = (VehDyn_t *)my_test_data;
  veh_dyn->sigHeader.uiCycleCounter = 1;
  veh_dyn->uiVersionNumber = 32;
  int callback_cnt = 0;
  next::udex::dataType return_value;

  auto callback = [&](const std::string) {
    callback_cnt++;
    EXPECT_TRUE(extractor->isDataAvailable(url)) << "Extracted data is not available at the callback";
    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
    EXPECT_EQ(std::get<uint32_t>(return_value), 32u);

    EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
                                    next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));
    EXPECT_EQ(std::get<uint16_t>(return_value), 1u);
  };
  extractor->connectCallback(callback, url);

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo(data_soruceinfo_name, 22, 33)) << "SetDataSourceInfo failed";
  const std::string sdl_file("ARS5xx.sdl");
  ASSERT_TRUE(publisher.RegisterDataDescription(sdl_file, true)) << "RegisterDataDescription failed";
  CheckConnection(subscriber, {url});

  publisher.publishData(url, my_test_data, size_100kb);
  delete[] my_test_data;

  EXPECT_TRUE(extractor->isDataAvailable(url)) << "Extracted data is not available right after publishing";

  EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.uiVersionNumber",
                                  next::udex::SignalType::SIGNAL_TYPE_UINT32, return_value));
  EXPECT_EQ(std::get<uint32_t>(return_value), 32u);

  EXPECT_TRUE(extractor->GetValue(data_soruceinfo_name + ".AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
                                  next::udex::SignalType::SIGNAL_TYPE_UINT16, return_value));
  EXPECT_EQ(std::get<uint16_t>(return_value), 1u);

  next::udex::TimestampedDataPacket extracted_data = extractor->GetExtractedData(url);
  EXPECT_NE(extracted_data.data, nullptr);
  ASSERT_GE(extracted_data.size, sizeof(VehDyn_t)) << "Extracted package does not fit expected size";

  std::cout << "Callbacks received: " << callback_cnt << std::endl;
  EXPECT_NE(callback_cnt, 0) << "Callback not received after processing the data"; // at this point data should be
                                                                                   // extracted and callback executed
}

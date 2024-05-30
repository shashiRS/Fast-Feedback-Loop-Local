
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include "../../../../src/udex/src/signal_storage/core_lib_wrap_data_converter.hpp"

#include "../../../thirdparty/pocketlzma/pocketlzma_wrapper.hpp"

using namespace next::udex;

class hw_data_extractor_test : public next::sdk::testing::TestUsingEcal {
public:
  hw_data_extractor_test() { this->instance_name_ = "hw_data_extractor_test"; }
};

TEST_F(hw_data_extractor_test, get_full_url_tree_for_hw_sdl) {
  std::unique_ptr<next::udex::publisher::DataPublisher> publisher1 =
      std::make_unique<next::udex::publisher::DataPublisher>("get_full_url_tree_for_hw_sdl");
  std::unique_ptr<next::udex::explorer::SignalExplorer> signal_explorer =
      std::make_unique<next::udex::explorer::SignalExplorer>();

  std::string data_source_name = "GetFullUrlTreeForHwData";

  ASSERT_TRUE(publisher1->SetDataSourceInfo("GetFullUrlTreeForHwData", 8002u, 0u, "mts.mta.hw"))
      << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"DUMMY_HW_SDL_CONTENT",
                                                  (void *)CoreLibWrap::DataConverter::kDummyHWSDLContent.data(),
                                                  CoreLibWrap::DataConverter::kDummyHWSDLContent.size(), true))
      << "Failed to register data description";

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::unordered_map<std::string, std::vector<std::string>> url_tree = signal_explorer->GetFullUrlTree();

  std::string hw_image_url = "GetFullUrlTreeForHwData.Image.V7.signalHeader.uiMeasurementCounter";

  for (auto &[key, value] : url_tree) {
    if (key != data_source_name) {
      continue;
    }

    EXPECT_TRUE(std::find(value.begin(), value.end(), hw_image_url) != value.end())
        << "Signal not found: " << hw_image_url;
  }
}

TEST_F(hw_data_extractor_test, check_hw_data_extraction_hardware_image_data) {
  PackageInfo package_info = {"MFC5xx Device", 8002u, 0, 60, next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW, true};

  std::vector<uint8_t> hardware_package_payload_data;
  PocketlzmaHelper::ReadMemoryDumpFromFile(hardware_package_payload_data, "dump_hw_image_payload.lzma");

  memory_pointer extracted_data = std::make_unique<std::vector<uint8_t>>();

  std::string dummy_schema = "";
  CoreLibWrap::DataConverter data_converter;

  size_t timestamp = 0;
  // test invalid content
  data_converter.ConvertData("MFC5xx Device.Image.invalid", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_EQ(extracted_data->size(), 0);

  data_converter.ConvertData("Invalid Device.Image.V7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_EQ(extracted_data->size(), 0);

  // test valid content
  data_converter.ConvertData("MFC5xx Device.Image.V7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_GT(extracted_data->size(), 0);

  data_converter.ConvertData("MFC5xx Device.Image.UnpackedV7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_GT(extracted_data->size(), 0);

  data_converter.ConvertData("MFC5xx Device.Image.V7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_GT(extracted_data->size(), 0);

  data_converter.ConvertData("MFC5xx Device.Image.UnpackedV7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_GT(extracted_data->size(), 0);
}

TEST_F(hw_data_extractor_test, check_hw_data_extraction_no_hardware_image_format_empty_schema) {
  std::vector<uint8_t> hardware_package_payload_data;
  PocketlzmaHelper::ReadMemoryDumpFromFile(hardware_package_payload_data, "dump_hw_image_payload.lzma");

  std::string dummy_schema = "";
  PackageInfo package_info = {"MFC5xx Device", 0, 0, 60, next ::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW};

  CoreLibWrap::DataConverter data_converter;
  size_t timestamp = 0;
  memory_pointer extracted_data = std::make_unique<std::vector<uint8_t>>();
  data_converter.ConvertData("MFC5xx Device.Image.V7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_EQ(extracted_data->size(), hardware_package_payload_data.size());

  package_info = {"MFC5xx Device", 8002u, 0, 59, next ::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW};
  data_converter.ConvertData("MFC5xx Device.Image.V7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_EQ(extracted_data->size(), hardware_package_payload_data.size());

  package_info = {"MFC5xx Device", 8002u, 0, 60, next ::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW};
  data_converter.ConvertData("MFC5xx Device.Image.V7", hardware_package_payload_data.data(),
                             hardware_package_payload_data.size(), timestamp++, dummy_schema, package_info,
                             extracted_data);
  EXPECT_EQ(extracted_data->size(), hardware_package_payload_data.size());
}

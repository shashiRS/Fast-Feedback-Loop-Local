#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <iostream>

#include <next/sdk/sdk.hpp>

#include <next/sdk/types/data_description_types.hpp>
#include "plugin/core_lib_reader/core_lib_reader_impl.hpp"

class CoreLibReaderTest : public next::sdk::testing::TestUsingEcal {
public:
  CoreLibReaderTest() { this->instance_name_ = "PlayerTestFixture"; }
};
namespace next {
namespace player {
namespace plugin {
namespace corelibreader {
class NextCoreLibReaderImplTester : public NextCoreLibReaderImpl {
public:
  void TestSetupSwGroupHandling(const next::sdk::types::DataDescription &description) {
    SetupSpecialPackageHandling(description);
  }
  auto GetProcessor() { return processor_info_; }

  const std::unordered_map<size_t, uint32_t> &GetSizeMap() { return hash_to_size_map_; }
  const std::unordered_map<size_t, uint64_t> &GetVaddr() { return hash_to_next_vaddr_map_; }
  EthernetPackage::serialization_info_map &GetServiceInfo() { return ethernet_service_info_; }
};
} // namespace corelibreader
} // namespace plugin
} // namespace player
} // namespace next

struct url_info_t {
  std::string url;
  uint32_t size;
  size_t offset;
  int value;
};
std::vector<url_info_t> sw_url_info_vec{{"ARS5xx.SW_Every10ms.RTLMF_ROM_PP_INFOBLOCK", 128, 0, 1},
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

TEST_F(CoreLibReaderTest, check_mta_sw_group_info_generation_success) {
  next::sdk::types::DataDescription description;
  constexpr uint16_t cycle_id = 209u;
  constexpr uint16_t k_source_id = 6403u;
  constexpr uint32_t k_instance_number = 0u;
  description.source_id = k_source_id;
  description.instance_number = k_instance_number;
  description.filename = "test.sdl";
  description.device_name = "ARS4xx";
  description.device_format_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW;
  description.description_format_type = next::sdk::types::DESCRIPTION_FORMAT_TYPE_SDL;
  next::player::plugin::corelibreader::NextCoreLibReaderImplTester reader;
  std::ifstream myfile("SwMultigroupInPackageCoreLibTest.sdl");
  std::stringstream buffer;
  buffer << myfile.rdbuf();
  std::string sdl_string = buffer.str();
  description.binary_description.resize(sdl_string.size());
  std::copy_n(reinterpret_cast<const char *>(sdl_string.c_str()), sdl_string.size(),
              description.binary_description.data());

  reader.TestSetupSwGroupHandling(description);

  std::vector<uint64_t> vaddr{0x8004C000, 0x8004C100, 0x8004C170, 0x8004C184, 0x8004C210, 0x8004C300,
                              0x8004C7F0, 0x8004C810, 0x8004C900, 0x8004EE00, 0x8004FF90, 0x8004FFB0};

  static std::unordered_map<size_t, uint32_t> expected_size_map;
  static std::unordered_map<uint64_t, uint64_t> next_vaddr_map;
  next_vaddr_map.clear();
  expected_size_map.clear();
  for (size_t i_url_info = 0; i_url_info < sw_url_info_vec.size(); i_url_info++) {
    next::sdk::types::IPackage::PackageMetaInfo info = {
        k_source_id, k_instance_number, cycle_id, vaddr[i_url_info], 0, 0};
    auto hash = next::sdk::types::PackageHash::hashMetaInfo(info);
    expected_size_map[hash] = sw_url_info_vec[i_url_info].size;
    if (i_url_info + 1 < sw_url_info_vec.size()) {
      next_vaddr_map[hash] = vaddr[i_url_info + 1];
    } else {
      next_vaddr_map[hash] = 0;
    }
  }

  auto result_map = reader.GetSizeMap();
  for (auto hash_size : expected_size_map) {
    ASSERT_FALSE(result_map.find(hash_size.first) == result_map.end()) << "hash not found in size map";
    EXPECT_EQ(hash_size.second, result_map[hash_size.first]) << "size not matching";
  }
}

std::vector<url_info_t> hw_url_info_vec{
    {"MFC5xx.Image.Parameters", 16, 0, 0}, {"MFC5xx.Image.V6", 88, 0, 0}, {"MFC5xx.Image.V7", 80, 0, 0}};

TEST_F(CoreLibReaderTest, checkMFC5xx_processor_creation) {
  next::sdk::types::DataDescription description;
  constexpr uint16_t cycle_id = 60u;
  constexpr uint16_t k_source_id = 8002u;
  constexpr uint32_t k_instance_number = 0u;

  description.source_id = mts_sensor_mfc500;
  description.instance_number = k_instance_number;
  description.filename = "test.sdl";
  description.device_name = "MFC5xx Device";
  description.device_format_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA;
  description.description_format_type = next::sdk::types::DESCRIPTION_FORMAT_TYPE_SDL;
  next::player::plugin::corelibreader::NextCoreLibReaderImplTester reader;
  std::ifstream myfile("HwMultigroupInPackageCoreLibTest.sdl");
  std::stringstream buffer;
  buffer << myfile.rdbuf();
  std::string sdl_string = buffer.str();
  description.binary_description.resize(sdl_string.size());
  std::copy_n(reinterpret_cast<const char *>(sdl_string.c_str()), sdl_string.size(),
              description.binary_description.data());

  reader.TestSetupSwGroupHandling(description);

  auto processor_info = reader.GetProcessor();

  next::sdk::types::IPackage::PackageMetaInfo info;
  info.cycle_id = cycle_id;
  info.source_id = k_source_id;
  info.instance_number = k_instance_number;
  info.virtual_address = 0;
  size_t hash = next::sdk::types::PackageHash::hashMetaInfo(info);
  auto processor_found = processor_info.find(hash);
  ASSERT_NE(processor_info.end(), processor_found);
  EXPECT_EQ(processor_found->second.publish_urls.size(), 3);
}

TEST_F(CoreLibReaderTest, check_mts_eth_service_info_generation_success) {
  next::sdk::types::DataDescription description;
  EthernetPackage::serialization_info_map expected_service_info = {{2, {serialization_technology::autosar, 1, 0}},
                                                                   {0, {serialization_technology::other, 0, 0}}};
  constexpr uint16_t k_source_id = 1551u;
  constexpr uint32_t k_instance_number = 0u;
  description.source_id = k_source_id;
  description.instance_number = k_instance_number;
  description.filename = "EthernetFibexPackageCoreLibTest.xml";
  description.device_name = "Ethernet";
  description.device_format_type = next::sdk::types::PACKAGE_FORMAT_TYPE_ETH;
  description.description_format_type = next::sdk::types::DESCRIPTION_FORMAT_TYPE_FIBEX;
  next::player::plugin::corelibreader::NextCoreLibReaderImplTester reader;
  std::ifstream myfile("EthernetFibexPackageCoreLibTest.xml");
  std::stringstream buffer;
  buffer << myfile.rdbuf();
  std::string xml_string = buffer.str();
  description.binary_description.resize(xml_string.size());
  std::copy_n(reinterpret_cast<const char *>(xml_string.c_str()), xml_string.size(),
              description.binary_description.data());

  reader.TestSetupSwGroupHandling(description);

  auto result_map = reader.GetServiceInfo();
  for (auto service_info : expected_service_info) {
    ASSERT_FALSE(result_map.find(service_info.first) == result_map.end()) << "hash not found in size map";
    EXPECT_EQ(service_info.second.serialization_technology_type,
              result_map[service_info.first].serialization_technology_type)
        << "serialization technology matching";
    EXPECT_EQ(service_info.second.interface_version, result_map[service_info.first].interface_version)
        << "interface version not matching";
    EXPECT_EQ(service_info.second.protocol_version, result_map[service_info.first].protocol_version)
        << "protocol version matching";
  }
}

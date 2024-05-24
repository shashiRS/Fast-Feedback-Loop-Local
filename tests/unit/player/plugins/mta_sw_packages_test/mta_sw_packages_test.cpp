#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/ecu/ecu_types.h>

#include "next/sdk/sdk.hpp"

#include "plugin/core_lib_reader/core_lib_package.h"

constexpr int kWaitTimeMs = 3000;
constexpr int kWaitIncrementMs = 100;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

std::unique_ptr<next::sdk::types::IPackage>
CreatePackage(mts::extensibility::package &package, const std::unordered_map<size_t, uint32_t> &size_map_package,
              const std::unordered_map<uint64_t, uint64_t> &next_vaddr_map_package) {
  return std::make_unique<CoreLibPackage>(std::move(package), size_map_package, next_vaddr_map_package);
}

class MtaSwPackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  MtaSwPackagesTest() { this->instance_name_ = "special_packages_test"; }
};

struct url_info_t {
  std::string url;
  uint32_t size;
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

std::unique_ptr<next::sdk::types::IPackage> getMultiGroupPackage() {
  constexpr uint16_t cycle_id = 209u;
  constexpr uint16_t k_source_id = 6403u;
  constexpr uint32_t k_instance_number = 0u;

  std::vector<uint64_t> vaddr{0x8004C000, 0x8004C100, 0x8004C170, 0x8004C184, 0x8004C210, 0x8004C300,
                              0x8004C7F0, 0x8004C810, 0x8004C900, 0x8004EE00, 0x8004FF90, 0x8004FFB0};
  static std::unordered_map<size_t, uint32_t> size_map;
  static std::unordered_map<uint64_t, uint64_t> next_vaddr_map;
  next_vaddr_map.clear();
  size_map.clear();
  for (size_t i_url_info = 0; i_url_info < url_info_vec.size(); i_url_info++) {
    next::sdk::types::IPackage::PackageMetaInfo info = {
        k_source_id, k_instance_number, cycle_id, vaddr[i_url_info], 0, 0};
    auto hash = next::sdk::types::PackageHash::hashMetaInfo(info);
    size_map[hash] = url_info_vec[i_url_info].size;
    if (i_url_info + 1 < url_info_vec.size()) {
      next_vaddr_map[hash] = vaddr[i_url_info + 1];
    } else {
      next_vaddr_map[hash] = 0;
    }
  }

  mts_ecu_software_header_t header;
  header.address = 0x8004C000;
  header.control = 0;
  header.control_length.length = 16368;
  header.task_counter = cycle_id;
  header.task_id = cycle_id;

  size_t extended_header_size = sizeof(header) + 4;

  std::vector<uint8_t> payload(header.control_length.length + extended_header_size);

  // reset
  memset(payload.data(), 0, header.control_length.length);

  // copy header
  memcpy(payload.data(), &header, sizeof(header));

  // generate some data
  for (const auto &url_info : url_info_vec) {
    auto offset = extended_header_size + url_info.offset;
    memset(payload.data() + offset, url_info.value, url_info.size);
  }

  // create package
  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(),
                                      payload.size() + extended_header_size);
  // auto payload_start = package->data();

  package->source_id = 6403;
  package->source_location = 0;
  package->instance_number = k_instance_number;
  // package->format_type
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = cycle_id;
  package->cycle_counter = 7974;
  package->cycle_state = 4;
  package->size = header.control_length.length + extended_header_size;

  std::unique_ptr<next::sdk::types::IPackage> return_package =
      std::make_unique<CoreLibPackage>(std::move(package), size_map, next_vaddr_map);
  return return_package;
}

TEST_F(MtaSwPackagesTest, get_package_hash_multigroup_success) {
  std::vector<size_t> k_expected_hash = {8498497776615309147u, 8499342201545552475u, 8499254240615309771u,
                                         8499179473824587559u, 8499007950010550955u, 8499905151498672219u,
                                         8497987603220357451u, 8496193200244427947u, 8497090401732549211u,
                                         8468661429090099547u, 8500837537358622507u, 8500872721730725515u};

  const std::vector<unsigned long> k_expected_offset{0, 256, 368, 388, 528, 768, 2032, 2064, 2304, 11776, 16272, 16304};
  const std::vector<uint64_t> k_expected_size{128, 20, 4, 12, 108, 96, 20, 84, 9372, 3980, 32, 64};

  auto ipackage = getMultiGroupPackage();

  size_t index = 0;
  const unsigned char *first_adress = 0x0;
  while (ipackage->PayloadAvailable()) {
    auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
    auto payload = ipackage->GetPayload();
    if (index == 0) {
      first_adress = payload.mem_pointer;
    }
    ASSERT_LT(index, k_expected_hash.size()) << "to many packages";
    EXPECT_EQ(hash, k_expected_hash[index]) << "hash not the same";
    ASSERT_LT(index, k_expected_size.size()) << "to many packages";
    EXPECT_EQ(payload.size, k_expected_size[index]) << "size not the same";
    ASSERT_LT(index, k_expected_offset.size()) << "to many packages";
    auto offset_temp = payload.mem_pointer - first_adress;
    EXPECT_EQ(offset_temp, k_expected_offset[index]) << "size not the same";
    index++;
  }
}

TEST_F(MtaSwPackagesTest, package_contains_only_one_payload_extended_size) {
  mts_ecu_software_header_t header;
  header.address = 0x8004C000;
  header.control = 0;
  header.control_length.length = 4153936;
  header.task_counter = 209;
  header.task_id = 209;
  header.control_length.packet_counter = 0U;
  header.control_length.absolute_ts = 0U;
  header.control_length.checksum = 0U;

  const auto extended_header_size = 4;
  const auto package_size = sizeof(header) + extended_header_size + header.control_length.length;
  std::vector<uint8_t> payload(package_size);
  memcpy(payload.data(), &header, sizeof(header));

  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(), payload.size());
  package->source_id = 6403;
  package->source_location = 0;
  package->instance_number = 0;
  // package->format_type
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = 209;
  package->cycle_counter = 7974;
  package->cycle_state = 4;

  static std::unordered_map<size_t, uint32_t> size_map{{8498497776615309147u, 80}};
  static std::unordered_map<uint64_t, uint64_t> next_vaddr_map{{8498497776615309147u, 1191182336}};

  auto ipackage = CreatePackage(package, size_map, next_vaddr_map);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 8498497776615309147u;
  size_t expected_size = 4153936;

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

TEST_F(MtaSwPackagesTest, package_contains_only_one_payload) {
  mts_ecu_software_header_t header;
  header.address = 0x8004C000;
  header.control = 0;
  header.control_length.length = 128;
  header.task_counter = 209;
  header.task_id = 209;
  header.control_length.packet_counter = 0U;
  header.control_length.absolute_ts = 0U;
  header.control_length.checksum = 0U;

  const auto extended_header_size = 4;
  const auto package_size = sizeof(header) + extended_header_size + header.control_length.length;
  std::vector<uint8_t> payload(package_size);
  memcpy(payload.data(), &header, sizeof(header));

  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(), payload.size());
  package->source_id = 6403;
  package->source_location = 0;
  package->instance_number = 0;
  // package->format_type
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = 209;
  package->cycle_counter = 7974;
  package->cycle_state = 4;

  static std::unordered_map<size_t, uint32_t> size_map{{8498497776615309147u, 128}};
  static std::unordered_map<uint64_t, uint64_t> next_vaddr_map{{8498497776615309147u, 0}};

  auto ipackage = CreatePackage(package, size_map, next_vaddr_map);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 8498497776615309147u;
  size_t expected_size = 128;

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

TEST_F(MtaSwPackagesTest, package_contains_no_payload) {

  mts_ecu_software_header_t header;
  header.address = 0x7004C020;
  header.control = 0;
  header.control_length.length = 0;
  header.task_counter = 209;
  header.task_id = 209;

  auto size = sizeof(header);
  std::vector<uint8_t> payload(size);
  memcpy(payload.data(), &header, size);

  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(), payload.size());
  package->source_id = 6403;
  package->source_location = 0;
  package->instance_number = 5;
  // package->format_type
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = 209;
  package->cycle_counter = 7974;
  package->cycle_state = 4;
  package->size = 0;

  std::vector<std::tuple<size_t, uint64_t, size_t>> vec_tuple = {};
  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 6351894252505877794u;

  size_t expected_size = 0;
  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

std::unique_ptr<next::sdk::types::IPackage> getSimplePackage() {
  mts_ecu_software_header_t header;
  header.address = 0x20350000;
  header.control = 0;
  header.control_length.length = 160;
  header.task_counter = 207;
  header.task_id = 207;

  auto size = sizeof(header);
  std::vector<uint8_t> payload(size);
  memcpy(payload.data(), &header, size);

  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(), payload.size());
  package->source_id = 87;
  package->source_location = 0;
  package->instance_number = 37;
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = 207;
  package->cycle_counter = 7974;
  package->cycle_state = 4;
  package->size = 160;

  return std::make_unique<CoreLibPackage>(std::move(package));
}

TEST_F(MtaSwPackagesTest, simple_mta_sw_package) {
  constexpr size_t kExpectedHashPackage = 17840117121602961597u;
  std::unique_ptr<next::sdk::types::IPackage> ipackage = getSimplePackage();
  while (ipackage->PayloadAvailable()) {
    auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
    EXPECT_EQ(hash, kExpectedHashPackage) << "hash not the same";
    EXPECT_EQ(ipackage->GetSize(), 160) << "size not the same";

    next::sdk::types::IPackage::Payload payload;
    ASSERT_NO_THROW(payload = ipackage->GetPayload());
    EXPECT_EQ(payload.size, 160) << "payload size is wrong";
  }
  EXPECT_EQ(next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo()), kExpectedHashPackage)
      << "MetaInfo not available anymore";
}

TEST_F(MtaSwPackagesTest, mta_sw_package_multi_meta_success) {
  constexpr size_t kExpectedHashPackage = 17840117121602961597u;
  std::unique_ptr<next::sdk::types::IPackage> ipackage = getSimplePackage();
  constexpr int kNumberOfMetaInfoCalls = 5;
  for (int i = 0; i < kNumberOfMetaInfoCalls; i++) {
    EXPECT_EQ(next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo()), kExpectedHashPackage)
        << "MetaInfo not available anymore";
  }

  ASSERT_NO_THROW(next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo()))
      << "get Payload should not crash";
}

std::unique_ptr<next::sdk::types::IPackage> getPackageWithProcessor(size_t &hash) {
  mts_ecu_software_header_t header;
  header.address = 0x8004C000;
  header.control = 0;
  header.control_length.length = 128;
  header.task_counter = 209;
  header.task_id = 209;
  header.control_length.packet_counter = 0U;
  header.control_length.absolute_ts = 0U;
  header.control_length.checksum = 0U;

  const auto extended_header_size = 4;
  const auto package_size = sizeof(header) + extended_header_size + header.control_length.length;
  std::vector<uint8_t> payload(package_size);
  memcpy(payload.data(), &header, sizeof(header));

  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.sw", payload.data(), payload.size());
  package->source_id = mts_sensor_mfc500;
  package->source_location = 0;
  package->instance_number = 0;
  // package->format_type
  package->hardware_timestamp.id = 13;
  package->hardware_timestamp.time = 734487770;
  package->reference_timestamp = 6709316281;
  package->cycle_id = 209;
  package->cycle_counter = 7974;
  package->cycle_state = 4;

  std::map<uint16_t, std::vector<std::string>> kExpectedHashList = {
      {mts_sensor_mfc500,
       {"MFC250_Device_working.Image.Parameter", "MFC250_Device_working.Image.V6", "MFC250_Device_working.Image.V7"}}};

  constexpr uint32_t kCycleIdHardwareImageData = 209u;

  std::unordered_map<size_t, CoreLibProcessor> processor_info;
  CoreLibProcessor info;
  next::sdk::types::IPackage::PackageMetaInfo meta;
  meta.source_id = mts_sensor_mfc500;
  meta.instance_number = 0;
  meta.cycle_id = kCycleIdHardwareImageData;
  meta.virtual_address = 0x8004C000;

  // create hash without any vaddr
  size_t hash_plain_package = next::sdk::types::PackageHash::hashMetaInfo(meta);
  hash = hash_plain_package;

  // add the vaddr packages into processor
  info.package_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW;
  info.publish_urls.push_back("MFC250_Device_working.Image.Parameter");
  info.publish_urls.push_back("MFC250_Device_working.Image.V6");
  info.publish_urls.push_back("MFC250_Device_working.Image.V7");
  processor_info[hash_plain_package] = info;

  return std::make_unique<CoreLibPackage>(std::move(package), dummy_size_map, dummy_vaddr_map,
                                          dummy_ethernet_service_info_map, processor_info);
}

TEST_F(MtaSwPackagesTest, mta_sw_package_processor_package_success) {
  size_t hash;
  std::unique_ptr<next::sdk::types::IPackage> ipackage = getPackageWithProcessor(hash);
  EXPECT_TRUE(ipackage->PayloadAvailable());
  EXPECT_EQ(ipackage->GetMetaType(), next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME);
  EXPECT_EQ(ipackage->GetPackageName(), "MFC250_Device_working.Image.Parameter");
  auto payload = ipackage->GetPayload();
  const unsigned char *current_memory = payload.mem_pointer;
  EXPECT_EQ(payload.mem_pointer, current_memory);

  EXPECT_TRUE(ipackage->PayloadAvailable());
  EXPECT_EQ(ipackage->GetMetaType(), next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME);
  EXPECT_EQ(ipackage->GetPackageName(), "MFC250_Device_working.Image.V6");
  payload = ipackage->GetPayload();
  EXPECT_EQ(payload.mem_pointer, current_memory);

  EXPECT_TRUE(ipackage->PayloadAvailable());
  EXPECT_EQ(ipackage->GetMetaType(), next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME);
  EXPECT_EQ(ipackage->GetPackageName(), "MFC250_Device_working.Image.V7");
  payload = ipackage->GetPayload();
  EXPECT_EQ(payload.mem_pointer, current_memory);

  EXPECT_TRUE(ipackage->PayloadAvailable());
  EXPECT_EQ(ipackage->GetMetaType(), next::sdk::types::IPackage::PACKAGE_META_TYPE_INFO);
  EXPECT_EQ(next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo()), hash);
  payload = ipackage->GetPayload();
  EXPECT_GT(payload.mem_pointer, current_memory) << "sw package is staring at raw data";

  EXPECT_FALSE(ipackage->PayloadAvailable());
  EXPECT_FALSE(ipackage->PayloadAvailable());

  EXPECT_TRUE(ipackage->ResetPayloads());
  EXPECT_TRUE(ipackage->PayloadAvailable());
  EXPECT_EQ(ipackage->GetMetaType(), next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME);
  EXPECT_EQ(ipackage->GetPackageName(), "MFC250_Device_working.Image.Parameter");
  payload = ipackage->GetPayload();
  EXPECT_EQ(payload.mem_pointer, current_memory);
}

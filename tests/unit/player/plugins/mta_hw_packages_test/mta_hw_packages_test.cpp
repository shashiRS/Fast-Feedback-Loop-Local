#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/ecu/ecu.hpp>

#include "next/sdk/sdk.hpp"

#include <next/sdk/types/package_data_types.hpp>

#include "plugin/core_lib_reader/core_lib_package.h"
using namespace mts::extensibility;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

std::unique_ptr<next::sdk::types::IPackage>
CreatePackage(mts::extensibility::package &package, const std::unordered_map<size_t, uint32_t> &size_map_package,
              const std::unordered_map<uint64_t, uint64_t> &next_vaddr_map_package) {
  return std::make_unique<CoreLibPackage>(std::move(package), size_map_package, next_vaddr_map_package);
}

class MtaHwPackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  MtaHwPackagesTest() { this->instance_name_ = "special_packages_test"; }
};

TEST_F(MtaHwPackagesTest, simple_mta_hw_package) {
  using namespace mts::toolbox::ecu;
  std::array<uint8_t, 5> hardware_package_payload_data = {1, 2, 3, 4, 5};
  auto allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(allocator_ptr, sensor_hardware_package_format,
                                      hardware_package_payload_data.data(), hardware_package_payload_data.size());
  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = 6403u;
  package->instance_number = 1U;
  package->cycle_id = 20;
  package->cycle_counter = 0U;
  package->cycle_state = cycle_body;

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 6230069727132100430;
  size_t expected_size = hardware_package_payload_data.size();

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";

  // get empty package
  package_payload = ipackage->GetPayload();
  EXPECT_EQ(package_payload.size, 0) << "size not zero";
  EXPECT_EQ(package_payload.mem_pointer, nullptr) << "mem_pointer not nullptr";
}

TEST_F(MtaHwPackagesTest, package_contains_no_payload) {
  using namespace mts::toolbox::ecu;
  std::array<uint8_t, 5> hardware_package_payload_data = {1, 2, 3, 4, 5};
  auto allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(allocator_ptr, sensor_hardware_package_format,
                                      hardware_package_payload_data.data(), hardware_package_payload_data.size());

  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = 6403u;
  package->instance_number = 1U;
  package->cycle_id = 20;
  package->cycle_counter = 0U;
  package->cycle_state = cycle_body;
  package->size = 0u;

  // create clone because CreatePackage uses move call for "package"
  mts::extensibility::package expected_package_content = package.clone();

  auto ipackage = CreateNextIPackage(package);

  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 6230069727132100430;
  size_t expected_size = 0u;

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

struct url_info_t {
  std::string url;
  uint32_t size;
};
std::vector<url_info_t> url_info_vec{{"MFC5xx.Image.Parameters", 16}, {"MFC5xx.Image.V6", 88}, {"MFC5xx.Image.V7", 80}};

std::unique_ptr<next::sdk::types::IPackage>
getMultiGroupPackage(uint16_t source_id, uint32_t cycle_id,
                     std::unordered_map<size_t, CoreLibProcessor> &processor_info) {
  constexpr uint32_t kInstanceNumber = 0u;

  mts_ecu_software_header_t header;
  header.address = 0x100000;
  header.control = 0;
  header.control_length.length = 1000;
  header.task_counter = (uint16_t)cycle_id;
  header.task_id = (uint16_t)cycle_id;

  size_t extended_header_size = sizeof(header) + 4;

  std::vector<uint8_t> payload(header.control_length.length + extended_header_size);

  // reset
  memset(payload.data(), 0, header.control_length.length);

  // copy header
  memcpy(payload.data(), &header, sizeof(header));

  // generate some data
  for (const auto &url_info : url_info_vec) {
    auto offset = extended_header_size;
    memset(payload.data() + offset, 0, url_info.size);
  }

  // create package
  auto memory_allocator_ptr = mts::runtime::get_default_memory_allocator();
  mts::extensibility::package package(memory_allocator_ptr, "mts.mta.hw", payload.data(),
                                      payload.size() + extended_header_size);

  package->source_id = source_id;
  package->source_location = 0u;
  package->instance_number = kInstanceNumber;
  package->cycle_id = cycle_id;

  package->size = header.control_length.length + extended_header_size;

  std::unique_ptr<next::sdk::types::IPackage> return_package = std::make_unique<CoreLibPackage>(
      std::move(package), dummy_size_map, dummy_vaddr_map, dummy_ethernet_service_info_map, processor_info);
  return return_package;
}

TEST_F(MtaHwPackagesTest, get_hardware_image_package_mfc5xx_successful) {
  std::map<uint16_t, std::vector<std::string>> kExpectedHashList = {
      {mts_sensor_mfc500,
       {"MFC250_Device_working.Image.Parameter", "MFC250_Device_working.Image.V6", "MFC250_Device_working.Image.V7"}}};

  constexpr uint32_t kCycleIdHardwareImageData = 60u;

  const std::vector<unsigned long> kExpectedOffset{0, 0, 0};
  const std::vector<uint64_t> kExpectedSize{1016, 1016, 1016};

  std::unordered_map<size_t, CoreLibProcessor> processor_info;
  CoreLibProcessor info;
  next::sdk::types::IPackage::PackageMetaInfo meta;
  meta.source_id = mts_sensor_mfc500;
  meta.cycle_id = kCycleIdHardwareImageData;

  // create hash without any vaddr
  size_t hash_plain_package = next::sdk::types::PackageHash::hashMetaInfo(meta);

  // add the vaddr packages into processor
  info.package_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW;
  info.publish_urls.push_back("MFC250_Device_working.Image.Parameter");
  info.publish_urls.push_back("MFC250_Device_working.Image.V6");
  info.publish_urls.push_back("MFC250_Device_working.Image.V7");
  processor_info[hash_plain_package] = info;

  for (const auto &[kSourceId, kExpectedHash] : kExpectedHashList) {
    auto ipackage = getMultiGroupPackage(kSourceId, kCycleIdHardwareImageData, processor_info);

    size_t index = 0;
    const unsigned char *first_adress = 0x0;
    while (ipackage->PayloadAvailable()) {
      auto package_name = ipackage->GetPackageName();
      auto payload = ipackage->GetPayload();
      if (index == 0) {
        first_adress = payload.mem_pointer;
      }
      ASSERT_LT(index, kExpectedHash.size()) << "to many packages for index " << std::to_string(index);
      ASSERT_LT(index, kExpectedOffset.size()) << "to many packages for index " << std::to_string(index);
      auto offset_temp = payload.mem_pointer - first_adress;
      EXPECT_EQ(offset_temp, kExpectedOffset[index]) << "size not the same for index " << std::to_string(index);
      EXPECT_EQ(package_name, kExpectedHash[index]) << "wrong package name for index" << std::to_string(index);
      index++;
    }
    EXPECT_EQ(index, kExpectedSize.size()) << "not enough packages";
  }
}

TEST_F(MtaHwPackagesTest, get_hardware_image_package_mfc500_wrong_cycle_id_unsuccessful) {

  constexpr uint32_t kCycleIdHardwareImageData = 0u;
  std::unordered_map<size_t, CoreLibProcessor> processor_info;

  auto ipackage = getMultiGroupPackage(mts_sensor_mfc500, kCycleIdHardwareImageData, processor_info);

  EXPECT_TRUE(ipackage->PayloadAvailable());
  next::sdk::types::IPackage::PackageMetaInfo package_info = ipackage->GetMetaInfo();

  EXPECT_EQ(package_info.instance_number, 0u) << "package instance number is not the same";
  EXPECT_EQ(package_info.cycle_id, kCycleIdHardwareImageData) << "package cycle id is not the same";
  EXPECT_EQ(package_info.source_id, mts_sensor_mfc500) << "package source id is not the same";
  EXPECT_EQ(package_info.virtual_address, 0u) << "package virtual adress is not the same";
  EXPECT_EQ(package_info.service_id, 0u) << "package service id is not the same";
  EXPECT_EQ(package_info.method_id, 0u) << "package method id is not the same";

  auto package_payload = ipackage->GetPayload();
  size_t expected_size = 1016;
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
  EXPECT_NE(package_payload.mem_pointer, nullptr) << "size not the same";

  // only one package available instead of three because of wrong cycle id
  EXPECT_FALSE(ipackage->PayloadAvailable());
}

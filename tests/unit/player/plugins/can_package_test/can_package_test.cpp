
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/can/can.hpp>
#include "next/sdk/sdk.hpp"

#include "plugin/core_lib_reader/core_lib_package.h"
using namespace mts::extensibility;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

class CanPackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  CanPackagesTest() { this->instance_name_ = "special_packages_test"; }
};

TEST_F(CanPackagesTest, simple_can_package) {
  using namespace mts::toolbox::can;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();
  can_frame can_frame_hdr;
  can_frame_hdr.id = 0xAB;
  can_frame_hdr.ide = 0;
  can_frame_hdr.dlc = 8;
  can_frame_hdr.dir = 0;
  can_frame_hdr.srr = 0;
  can_frame_hdr.edl = 0;
  can_frame_hdr.brs = 0;
  can_frame_hdr.esi = 0;

  std::array<uint8_t, 8U> data = {1, 2, 3, 4, 5, 6, 7, 8};
  uint32_t data_length = get_data_length_from_dlc(can_frame_hdr.dlc);
  mts::extensibility::package package(memory_allocator,
                                      mts::extensibility::get_format_identifier(mts::extensibility::vehicle_bus_can),
                                      sizeof(can_frame) + data_length);

  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = mts::extensibility::vehicle_bus_can;
  package->instance_number = 1U;
  package->cycle_id = can_frame_hdr.id;
  package->cycle_counter = 0U;
  package->cycle_state = no_cycle;
  auto *data_ptr = reinterpret_cast<uint8_t *>(const_cast<void *>(package->data()));
  memcpy(data_ptr, &can_frame_hdr, sizeof(can_frame));
  memcpy(data_ptr + sizeof(can_frame), data.data(), data_length);

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 10582767037831818376ull;
  size_t expected_size = data_length;

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

TEST_F(CanPackagesTest, package_contains_no_payload) {

  using namespace mts::toolbox::can;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();
  can_frame can_frame_hdr;
  can_frame_hdr.id = 0xAB;
  can_frame_hdr.ide = 0;
  can_frame_hdr.dlc = 8;
  can_frame_hdr.dir = 0;
  can_frame_hdr.srr = 0;
  can_frame_hdr.edl = 0;
  can_frame_hdr.brs = 0;
  can_frame_hdr.esi = 0;

  std::array<uint8_t, 8U> data = {1, 2, 3, 4, 5, 6, 7, 8};
  uint32_t data_length = get_data_length_from_dlc(can_frame_hdr.dlc);
  mts::extensibility::package package(memory_allocator,
                                      mts::extensibility::get_format_identifier(mts::extensibility::vehicle_bus_can),
                                      sizeof(can_frame) + data_length);

  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = mts::extensibility::vehicle_bus_can;
  package->instance_number = 1U;
  package->cycle_id = can_frame_hdr.id;
  package->cycle_counter = 0U;
  package->cycle_state = no_cycle;
  package->size = 0u;

  auto *data_ptr = reinterpret_cast<uint8_t *>(const_cast<void *>(package->data()));
  memcpy(data_ptr, &can_frame_hdr, sizeof(can_frame));
  memcpy(data_ptr + sizeof(can_frame), data.data(), data_length);

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 10588112588485596600ull;

  size_t expected_size = 0;
  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

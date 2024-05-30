
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/rt_range/rt_range.hpp>
#include "next/sdk/sdk.hpp"

#include "plugin/core_lib_reader/core_lib_package.h"
using namespace mts::extensibility;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

class RtrangePackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  RtrangePackagesTest() { this->instance_name_ = "special_packages_test"; }
};

TEST_F(RtrangePackagesTest, simple_rt_range_package) {
  using namespace mts::toolbox::rt_range;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();
  size_t payload_size = sizeof(rt_range_package_header) + sizeof(ncom_package);
  // Create a package and try to match.
  mts::extensibility::package package(
      memory_allocator, mts::extensibility::get_format_identifier(mts::extensibility::reference_rtrange), payload_size);
  // Time stamp
  package->hardware_timestamp.time = 1000ULL;
  package->hardware_timestamp.id = 1U;
  package->cycle_id = cycle_id_udex;
  package->source_id = mts::extensibility::reference_rtrange;
  package->instance_number = 1U;

  rt_range_package_header package_hdr{};
  package_hdr.address = udex_virtual_address_ncom_hunter; // 0x100000U
  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  // Add package header to payload.
  memcpy(current_payload_position, &package_hdr, sizeof(rt_range_package_header));

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 15238109110515861893ull;
  size_t expected_size = sizeof(ncom_package);

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

TEST_F(RtrangePackagesTest, package_contains_no_payload) {

  using namespace mts::toolbox::rt_range;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();
  size_t payload_size = sizeof(rt_range_package_header) + sizeof(ncom_package);
  // Create a package and try to match.
  mts::extensibility::package package(
      memory_allocator, mts::extensibility::get_format_identifier(mts::extensibility::reference_rtrange), payload_size);
  // Time stamp
  package->hardware_timestamp.time = 1000ULL;
  package->hardware_timestamp.id = 1U;
  package->cycle_id = cycle_id_udex;
  package->source_id = mts::extensibility::reference_rtrange;
  package->instance_number = 1U;
  package->size = 0u;

  rt_range_package_header package_hdr{};
  package_hdr.address = udex_virtual_address_ncom_hunter; // 0x100000U
  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  // Add package header to payload.
  memcpy(current_payload_position, &package_hdr, sizeof(rt_range_package_header));

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 16391030614563817861ull;

  size_t expected_size = 0;
  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

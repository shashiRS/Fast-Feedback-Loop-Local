
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/gps/gps.hpp>
#include "next/sdk/sdk.hpp"

#include "plugin/core_lib_reader/core_lib_package.h"
using namespace mts::extensibility;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

class GpsPackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  GpsPackagesTest() { this->instance_name_ = "special_packages_test"; }
};

TEST_F(GpsPackagesTest, simple_gps_package) {
  using namespace mts::toolbox::gps;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();
  nmea_packet nmea_pkt{};
  nmea_pkt.packet_type = static_cast<mts_gps_nmea_packet_type_t>(nmea_packet_type::nmea_vtg);
  nmea_pkt.service_type = static_cast<mts_gps_service_type_t>(service_type::service_gp);
  nmea_pkt.payload.vtg.magnetic_course_over_ground = 155.295;
  nmea_pkt.payload.vtg.mode = 'D';
  nmea_pkt.payload.vtg.speed_over_ground_kmph = 0.090;
  nmea_pkt.payload.vtg.speed_over_ground_knots = 0.049;
  nmea_pkt.payload.vtg.true_course_over_ground = 172.516;
  // NOLINTEND(cppcoreguidelines-pro-type-union-access)

  size_t payload_size = sizeof(nmea_packet);
  // Create a package and try to match.
  mts::extensibility::package package(memory_allocator, gps_nmea_package_format, payload_size);

  package->source_id = mts::extensibility::reference_gps;
  package->source_location = 0;
  package->instance_number = 0;
  package->hardware_timestamp.id = 1;
  package->hardware_timestamp.time = static_cast<uint64_t>(1000U);
  package->reference_timestamp = static_cast<uint64_t>(1000U);
  package->cycle_id = 1;
  package->cycle_state = mts_package_cycle_start_body_end;

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  // Add NMEA packet to payload.
  memcpy(current_payload_position, &nmea_pkt, sizeof(nmea_packet));

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 17639859897096190833ull;
  size_t expected_size = sizeof(nmea_vtg_packet);
  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

TEST_F(GpsPackagesTest, package_contains_no_payload) {

  using namespace mts::toolbox::gps;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();
  nmea_packet nmea_pkt{};
  nmea_pkt.packet_type = static_cast<mts_gps_nmea_packet_type_t>(nmea_packet_type::nmea_vtg);
  nmea_pkt.service_type = static_cast<mts_gps_service_type_t>(service_type::service_gp);
  nmea_pkt.payload.vtg.magnetic_course_over_ground = 155.295;
  nmea_pkt.payload.vtg.mode = 'D';
  nmea_pkt.payload.vtg.speed_over_ground_kmph = 0.090;
  nmea_pkt.payload.vtg.speed_over_ground_knots = 0.049;
  nmea_pkt.payload.vtg.true_course_over_ground = 172.516;
  // NOLINTEND(cppcoreguidelines-pro-type-union-access)

  size_t payload_size = sizeof(nmea_packet);
  // Create a package and try to match.
  mts::extensibility::package package(memory_allocator, gps_nmea_package_format, payload_size);

  package->source_id = mts::extensibility::reference_gps;
  package->source_location = 0;
  package->instance_number = 0;
  package->hardware_timestamp.id = 1;
  package->hardware_timestamp.time = static_cast<uint64_t>(1000U);
  package->reference_timestamp = static_cast<uint64_t>(1000U);
  package->cycle_id = 1;
  package->cycle_state = mts_package_cycle_start_body_end;
  package->size = 0u;

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  // Add NMEA packet to payload.
  memcpy(current_payload_position, &nmea_pkt, sizeof(nmea_packet));

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 328022929041718129ull;

  size_t expected_size = 0;
  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

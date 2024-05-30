
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/ref_cam/ref_cam.hpp>
#include "next/sdk/sdk.hpp"

#include "plugin/core_lib_reader/core_lib_package.h"
using namespace mts::extensibility;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

class RefcamPackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  RefcamPackagesTest() { this->instance_name_ = "special_packages_test"; }
};

TEST_F(RefcamPackagesTest, simple_ref_cam_package) {
  using namespace mts::toolbox::ref_cam;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();

  // Payload
  std::array<uint8_t, 307200U> img_data{};
  size_t payload_size = sizeof(video_frame_header) + img_data.size();
  // Create a package and try to match.
  mts::extensibility::package package(
      memory_allocator, mts::extensibility::get_format_identifier(mts::extensibility::reference_camera_is),
      payload_size);

  package->source_id = mts::extensibility::reference_camera_is;
  package->source_location = 32768U;
  package->instance_number = mts::extensibility::get_instance_number(32768U);
  package->hardware_timestamp.id = 1U;
  package->hardware_timestamp.time = static_cast<uint64_t>(1000U);
  package->reference_timestamp = static_cast<uint64_t>(1000U);
  package->cycle_id = 0U;
  package->cycle_state = mts_package_cycle_start_body_end;
  package->cycle_counter = 0U;

  video_frame_header ref_cam_video_hdr{};
  ref_cam_video_hdr.bits_per_pixel = 8U;
  ref_cam_video_hdr.format = video_frame_format::video_jpeg;
  ref_cam_video_hdr.frame_number = 1U;
  ref_cam_video_hdr.frame_time = static_cast<uint64_t>(1000U);
  ref_cam_video_hdr.height = 480U;
  ref_cam_video_hdr.width = 640U;
  ref_cam_video_hdr.horizontal_res = 0U;
  ref_cam_video_hdr.vertical_res = 0U;
  ref_cam_video_hdr.original_height = 480U;
  ref_cam_video_hdr.original_width = 640U;
  ref_cam_video_hdr.major_version = 3U;
  ref_cam_video_hdr.minor_version = 1U;
  ref_cam_video_hdr.loss_frames = 0U;
  ref_cam_video_hdr.process_delay_time = ref_cam_video_hdr.frame_time + 1U;
  ref_cam_video_hdr.reserved1 = 0U;
  ref_cam_video_hdr.reserved2 = 0U;
  ref_cam_video_hdr.scale_factor = 1U; // Full Size image
  ref_cam_video_hdr.size = sizeof(video_frame_header);
  ref_cam_video_hdr.data_size = static_cast<uint32_t>(img_data.size()); // Size of payload

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));

  // Adding payload.
  memcpy(current_payload_position, &ref_cam_video_hdr, sizeof(video_frame_header));
  current_payload_position += sizeof(video_frame_header);

  memcpy(current_payload_position, img_data.data(), img_data.size());

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 9512751166416504616ull;

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, payload_size) << "size not the same";
}

TEST_F(RefcamPackagesTest, package_contains_no_payload) {
  using namespace mts::toolbox::ref_cam;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();

  // Payload
  std::array<uint8_t, 307200U> img_data{};
  size_t payload_size = sizeof(video_frame_header) + img_data.size();
  // Create a package and try to match.
  mts::extensibility::package package(
      memory_allocator, mts::extensibility::get_format_identifier(mts::extensibility::reference_camera_is),
      payload_size);

  package->source_id = mts::extensibility::reference_camera_is;
  package->source_location = 32768U;
  package->instance_number = mts::extensibility::get_instance_number(32768U);
  package->hardware_timestamp.id = 1U;
  package->hardware_timestamp.time = static_cast<uint64_t>(1000U);
  package->reference_timestamp = static_cast<uint64_t>(1000U);
  package->cycle_id = 0U;
  package->cycle_state = mts_package_cycle_start_body_end;
  package->cycle_counter = 0U;
  package->size = 0u;

  video_frame_header ref_cam_video_hdr{};
  ref_cam_video_hdr.bits_per_pixel = 8U;
  ref_cam_video_hdr.format = video_frame_format::video_jpeg;
  ref_cam_video_hdr.frame_number = 1U;
  ref_cam_video_hdr.frame_time = static_cast<uint64_t>(1000U);
  ref_cam_video_hdr.height = 480U;
  ref_cam_video_hdr.width = 640U;
  ref_cam_video_hdr.horizontal_res = 0U;
  ref_cam_video_hdr.vertical_res = 0U;
  ref_cam_video_hdr.original_height = 480U;
  ref_cam_video_hdr.original_width = 640U;
  ref_cam_video_hdr.major_version = 3U;
  ref_cam_video_hdr.minor_version = 1U;
  ref_cam_video_hdr.loss_frames = 0U;
  ref_cam_video_hdr.process_delay_time = ref_cam_video_hdr.frame_time + 1U;
  ref_cam_video_hdr.reserved1 = 0U;
  ref_cam_video_hdr.reserved2 = 0U;
  ref_cam_video_hdr.scale_factor = 1U; // Full Size image
  ref_cam_video_hdr.size = sizeof(video_frame_header);
  ref_cam_video_hdr.data_size = static_cast<uint32_t>(img_data.size()); // Size of payload

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));

  // Adding payload.
  memcpy(current_payload_position, &ref_cam_video_hdr, sizeof(video_frame_header));
  current_payload_position += sizeof(video_frame_header);

  memcpy(current_payload_position, img_data.data(), img_data.size());

  auto ipackage = CreateNextIPackage(package);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 9512751166416504616ull;

  size_t expected_size = 0;
  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

auto createRefCamPackage(size_t &payload_size) {
  using namespace mts::toolbox::ref_cam;
  // Create generic package
  auto memory_allocator = mts::runtime::get_default_memory_allocator();

  // Payload
  std::array<uint8_t, 307200U> img_data{};
  payload_size = sizeof(video_frame_header) + img_data.size();
  // Create a package and try to match.
  mts::extensibility::package package(
      memory_allocator, mts::extensibility::get_format_identifier(mts::extensibility::reference_camera_is),
      payload_size);

  package->source_id = mts::extensibility::reference_camera_is;
  package->source_location = 32768U;
  package->instance_number = mts::extensibility::get_instance_number(32768U);
  package->hardware_timestamp.id = 1U;
  package->hardware_timestamp.time = static_cast<uint64_t>(1000U);
  package->reference_timestamp = static_cast<uint64_t>(1000U);
  package->cycle_id = 0U;
  package->cycle_state = mts_package_cycle_start_body_end;
  package->cycle_counter = 0U;

  video_frame_header ref_cam_video_hdr{};
  ref_cam_video_hdr.bits_per_pixel = 8U;
  ref_cam_video_hdr.format = video_frame_format::video_jpeg;
  ref_cam_video_hdr.frame_number = 1U;
  ref_cam_video_hdr.frame_time = static_cast<uint64_t>(1000U);
  ref_cam_video_hdr.height = 480U;
  ref_cam_video_hdr.width = 640U;
  ref_cam_video_hdr.horizontal_res = 0U;
  ref_cam_video_hdr.vertical_res = 0U;
  ref_cam_video_hdr.original_height = 480U;
  ref_cam_video_hdr.original_width = 640U;
  ref_cam_video_hdr.major_version = 3U;
  ref_cam_video_hdr.minor_version = 1U;
  ref_cam_video_hdr.loss_frames = 0U;
  ref_cam_video_hdr.process_delay_time = ref_cam_video_hdr.frame_time + 1U;
  ref_cam_video_hdr.reserved1 = 0U;
  ref_cam_video_hdr.reserved2 = 0U;
  ref_cam_video_hdr.scale_factor = 1U; // Full Size image
  ref_cam_video_hdr.size = sizeof(video_frame_header);
  ref_cam_video_hdr.data_size = static_cast<uint32_t>(img_data.size()); // Size of payload

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));

  // Adding payload.
  memcpy(current_payload_position, &ref_cam_video_hdr, sizeof(video_frame_header));
  current_payload_position += sizeof(video_frame_header);

  memcpy(current_payload_position, img_data.data(), img_data.size());
  return package;
}

TEST_F(RefcamPackagesTest, ref_cam_performance_test) {

  size_t payload_size;
  auto package = createRefCamPackage(payload_size);
  auto ipackage = CreateNextIPackage(package);

  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 9512751166416504616ull;

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, payload_size) << "size not the same";
}

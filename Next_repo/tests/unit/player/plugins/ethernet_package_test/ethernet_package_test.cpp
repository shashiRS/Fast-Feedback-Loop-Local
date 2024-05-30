
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <mts/toolbox/ethernet/ethernet.hpp>
#include "next/sdk/sdk.hpp"

#include "plugin/core_lib_reader/core_lib_package.h"
using namespace mts::extensibility;
using namespace mts::runtime;
using namespace mts::toolbox::ethernet;

std::unique_ptr<next::sdk::types::IPackage> CreateNextIPackage(mts::extensibility::package &package) {
  return std::make_unique<CoreLibPackage>(std::move(package));
}

std::unique_ptr<next::sdk::types::IPackage>
CreateNextIPackage(mts::extensibility::package &package, EthernetPackage::serialization_info_map &serilaization_info) {
  return std::make_unique<CoreLibPackage>(std::move(package), serilaization_info);
}
class EthernetPackagesTest : public next::sdk::testing::TestUsingEcal {
public:
  EthernetPackagesTest() { this->instance_name_ = "special_packages_test"; }
};
struct method_ifo {
  uint16_t service_id;
  uint16_t method_id;
  uint32_t length;
};

std::vector<method_ifo> method_info_vec{{1, 11, 30},   {2, 3, 10},   {3, 12, 20},      {4, 111, 15},
                                        {5, 31, 25},   {6, 121, 32}, {7, 1111, 14},    {8, 131, 12},
                                        {9, 1211, 20}, {101, 12, 8}, {102, 1111, 714}, {103, 131, 60}};

std::unique_ptr<next::sdk::types::IPackage> getMultiGroupPackage() {
  std::array<uint8_t, 4ULL> source_ip = {10U, 21U, 1U, 98U};
  uint16_t udp_payload_size = static_cast<uint16_t>(sizeof(mts_ethernet_some_ip_header_t) * method_info_vec.size());
  static EthernetPackage::serialization_info_map serialization_info;
  for (auto &method_info_val : method_info_vec) {
    udp_payload_size = static_cast<uint16_t>(udp_payload_size + method_info_val.length);
    serialization_info[method_info_val.service_id] = {serialization_technology::some_ip, 1, 1};
  }

  size_t payload_size = sizeof(ethernet_frame) + sizeof(ipv4_header) + sizeof(udp_header) + udp_payload_size;
  // Create a generic package and try to match.
  mts::extensibility::package package(get_default_memory_allocator(), payload_size);
  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = 1551U;
  package->instance_number = 1U;
  package.set_format_type(mts::extensibility::get_format_identifier(mts::extensibility::vehicle_bus_ethernet));
  package->cycle_id = 0U;
  package->cycle_counter = 0U;
  package->cycle_state = no_cycle;

  // Ethernet header.
  ethernet_frame eth_header;
  eth_header.ether_type = endian_reverse<uint16_t>(static_cast<uint16_t>(ethernet_type::ipv4));
  std::array<uint8_t, 6ULL> source_mac = {0U, 80U, 194U, 169U, 192U, 107U};
  std::array<uint8_t, 6ULL> destination_mac = {60U, 206U, 21U, 0U, 0U, 11U};
  memcpy(eth_header.source, source_mac.data(), 6ULL);
  memcpy(eth_header.destination, destination_mac.data(), 6ULL);
  // IPv4 header.
  ipv4_header ipv4_frag = {};
  ipv4_frag.version = 4U;
  ipv4_frag.header_len = 5U;
  ipv4_frag.length = endian_reverse<uint16_t>(static_cast<uint16_t>(payload_size - sizeof(ethernet_frame)));
  ipv4_frag.fragment = 0U;
  ipv4_frag.ident = endian_reverse<uint16_t>(1234U);
  ipv4_frag.protocol = static_cast<uint8_t>(ip_protocol_type::udp); // UDP protocol.
  ipv4_frag.tos = 0U;
  ipv4_frag.ttl = 64U;
  ipv4_frag.ident = 28915U;
  std::array<uint8_t, 4ULL> destination_ip = {10U, 21U, 1U, 112U};
  memcpy(ipv4_frag.source, source_ip.data(), source_ip.size());
  memcpy(ipv4_frag.destination, destination_ip.data(), destination_ip.size());
  // UDP header.
  udp_header udp_hdr = {};
  udp_hdr.destination_port =
      endian_reverse<uint16_t>(static_cast<uint16_t>(mts::toolbox::ethernet::ip_protocol_type::udp));
  udp_hdr.source_port = endian_reverse<uint16_t>(17052U);
  udp_hdr.length = static_cast<uint16_t>(sizeof(udp_header) + udp_payload_size);

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  uint64_t header_size = 0U;

  memcpy(current_payload_position, &eth_header, sizeof(ethernet_frame));
  current_payload_position += sizeof(ethernet_frame);
  header_size += sizeof(ethernet_frame);

  memcpy(current_payload_position, &ipv4_frag, sizeof(ipv4_header));
  current_payload_position += sizeof(ipv4_header);
  header_size += sizeof(ipv4_header);

  memcpy(current_payload_position, &udp_hdr, sizeof(udp_header));
  current_payload_position += sizeof(udp_header);
  header_size += sizeof(udp_header);

  // SOMEIP header and payload.
  for (auto &method_info_val : method_info_vec) {
    some_ip_header some_ip;
    some_ip.service_id = endian_reverse<uint16_t>(method_info_val.service_id);
    some_ip.method_id = endian_reverse<uint16_t>(method_info_val.method_id);
    some_ip.length = endian_reverse<uint32_t>(method_info_val.length + 8u);
    some_ip.interface_version = 1u;
    some_ip.protocol_version = 1u;
    memcpy(current_payload_position, &some_ip, sizeof(some_ip_header));
    current_payload_position += sizeof(some_ip_header) + method_info_val.length;
  }

  std::unique_ptr<next::sdk::types::IPackage> return_package =
      std::make_unique<CoreLibPackage>(std::move(package), serialization_info);
  return return_package;
}

TEST_F(EthernetPackagesTest, get_package_hash_multigroup_success) {
  std::vector<size_t> k_expected_hash = {8106221633623355588u, 8106374190861738207u, 8106172705355926898u,
                                         8088971670572978158u, 8105950054251140945u, 8106331309908201272u,
                                         8104884077728680000u, 8105538012268001537u, 8104768079251235398u,
                                         8103725742185425180u, 8102885440424285650u, 8103718595359783076u};
  const std::vector<unsigned long> k_expected_offset{0, 46, 72, 108, 139, 180, 228, 258, 286, 322, 346, 1076};
  const std::vector<uint64_t> k_expected_size{30, 10, 20, 15, 25, 32, 14, 12, 20, 8, 714, 60};

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
TEST_F(EthernetPackagesTest, package_contains_only_one_payload) {

  std::array<uint8_t, 4ULL> source_ip = {10U, 21U, 1U, 98U};
  std::array<uint8_t, 58ULL> payload = {1U, 2U, 3U, 4U};
  size_t payload_size =
      sizeof(ethernet_frame) + sizeof(ipv4_header) + sizeof(udp_header) + sizeof(autosar_soad_header) + payload.size();
  // Create a generic package and try to match.
  mts::extensibility::package package(get_default_memory_allocator(), payload_size);
  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = 1551U;
  package->instance_number = 1U;
  package.set_format_type(mts::extensibility::get_format_identifier(mts::extensibility::vehicle_bus_ethernet));
  package->cycle_id = 0U;
  package->cycle_counter = 0U;
  package->cycle_state = no_cycle;

  // Ethernet header.
  ethernet_frame eth_header;
  eth_header.ether_type = endian_reverse<uint16_t>(static_cast<uint16_t>(ethernet_type::ipv4));
  std::array<uint8_t, 6ULL> source_mac = {0U, 80U, 194U, 169U, 192U, 107U};
  std::array<uint8_t, 6ULL> destination_mac = {60U, 206U, 21U, 0U, 0U, 11U};
  memcpy(eth_header.source, source_mac.data(), 6ULL);
  memcpy(eth_header.destination, destination_mac.data(), 6ULL);
  // IPv4 header.
  ipv4_header ipv4_frag = {};
  ipv4_frag.version = 4U;
  ipv4_frag.header_len = 5U;
  ipv4_frag.length = endian_reverse<uint16_t>(static_cast<uint16_t>(payload.size() + sizeof(ipv4_header)));
  ipv4_frag.fragment = 0U;
  ipv4_frag.ident = endian_reverse<uint16_t>(1234U);
  ipv4_frag.protocol = static_cast<uint8_t>(ip_protocol_type::udp); // UDP protocol.
  ipv4_frag.tos = 0U;
  ipv4_frag.ttl = 64U;
  ipv4_frag.ident = 28915U;
  std::array<uint8_t, 4ULL> destination_ip = {10U, 21U, 1U, 112U};
  memcpy(ipv4_frag.source, source_ip.data(), source_ip.size());
  memcpy(ipv4_frag.destination, destination_ip.data(), destination_ip.size());
  // UDP header.
  udp_header udp_hdr = {};
  udp_hdr.destination_port =
      endian_reverse<uint16_t>(static_cast<uint16_t>(mts::toolbox::ethernet::ip_protocol_type::udp));
  udp_hdr.source_port = endian_reverse<uint16_t>(17052U);
  udp_hdr.length = static_cast<uint16_t>(sizeof(udp_header) + payload.size());
  // AUTOSAR header.
  autosar_soad_header autosar_header;
  autosar_header.service_id = endian_reverse<uint16_t>(static_cast<uint16_t>(0X39FEU));
  autosar_header.method_id = endian_reverse<uint16_t>(static_cast<uint16_t>(0X805AU));
  autosar_header.length = endian_reverse<uint32_t>(static_cast<uint32_t>(payload.size()));

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  uint64_t header_size = 0U;

  memcpy(current_payload_position, &eth_header, sizeof(ethernet_frame));
  current_payload_position += sizeof(ethernet_frame);
  header_size += sizeof(ethernet_frame);

  memcpy(current_payload_position, &ipv4_frag, sizeof(ipv4_header));
  current_payload_position += sizeof(ipv4_header);
  header_size += sizeof(ipv4_header);

  memcpy(current_payload_position, &udp_hdr, sizeof(udp_header));
  current_payload_position += sizeof(udp_header);
  header_size += sizeof(udp_header);

  memcpy(current_payload_position, &autosar_header, sizeof(autosar_soad_header2));
  header_size += sizeof(autosar_soad_header2);

  EthernetPackage::serialization_info_map serialization_info = {{0x39FE, {serialization_technology::autosar, 0, 0}}};
  auto ipackage = CreateNextIPackage(package, serialization_info);
  ASSERT_TRUE(ipackage->PayloadAvailable()) << "no package available";
  auto hash = next::sdk::types::PackageHash::hashMetaInfo(ipackage->GetMetaInfo());
  auto package_payload = ipackage->GetPayload();

  const size_t kExpectedHash = 995100218567184999;
  size_t expected_size = payload.size();

  EXPECT_EQ(hash, kExpectedHash) << "hash not the same";
  EXPECT_EQ(package_payload.size, expected_size) << "size not the same";
}

TEST_F(EthernetPackagesTest, package_contains_no_payload) {

  std::array<uint8_t, 4ULL> source_ip = {10U, 21U, 1U, 98U};
  std::array<uint8_t, 58ULL> payload = {1U, 2U, 3U, 4U};
  size_t payload_size =
      sizeof(ethernet_frame) + sizeof(ipv4_header) + sizeof(udp_header) + sizeof(autosar_soad_header) + payload.size();
  // Create a generic package and try to match.
  mts::extensibility::package package(get_default_memory_allocator(), payload_size);
  package->hardware_timestamp.time = 1U;
  package->hardware_timestamp.id = 1U;
  package->reference_timestamp = 1U;
  package->source_id = 1551U;
  package->instance_number = 1U;
  package.set_format_type(mts::extensibility::get_format_identifier(mts::extensibility::vehicle_bus_ethernet));
  package->cycle_id = 0U;
  package->cycle_counter = 0U;
  package->cycle_state = no_cycle;
  package->size = 0;

  // Ethernet header.
  ethernet_frame eth_header;
  eth_header.ether_type = endian_reverse<uint16_t>(static_cast<uint16_t>(ethernet_type::ipv4));
  std::array<uint8_t, 6ULL> source_mac = {0U, 80U, 194U, 169U, 192U, 107U};
  std::array<uint8_t, 6ULL> destination_mac = {60U, 206U, 21U, 0U, 0U, 11U};
  memcpy(eth_header.source, source_mac.data(), 6ULL);
  memcpy(eth_header.destination, destination_mac.data(), 6ULL);
  // IPv4 header.
  ipv4_header ipv4_frag = {};
  ipv4_frag.version = 4U;
  ipv4_frag.header_len = 5U;
  ipv4_frag.length = endian_reverse<uint16_t>(static_cast<uint16_t>(payload.size() + sizeof(ipv4_header)));
  ipv4_frag.fragment = 0U;
  ipv4_frag.ident = endian_reverse<uint16_t>(1234U);
  ipv4_frag.protocol = static_cast<uint8_t>(ip_protocol_type::udp); // UDP protocol.
  ipv4_frag.tos = 0U;
  ipv4_frag.ttl = 64U;
  ipv4_frag.ident = 28915U;
  std::array<uint8_t, 4ULL> destination_ip = {10U, 21U, 1U, 112U};
  memcpy(ipv4_frag.source, source_ip.data(), source_ip.size());
  memcpy(ipv4_frag.destination, destination_ip.data(), destination_ip.size());
  // UDP header.
  udp_header udp_hdr = {};
  udp_hdr.destination_port =
      endian_reverse<uint16_t>(static_cast<uint16_t>(mts::toolbox::ethernet::ip_protocol_type::udp));
  udp_hdr.source_port = endian_reverse<uint16_t>(17052U);
  udp_hdr.length = static_cast<uint16_t>(sizeof(udp_header) + payload.size());
  // AUTOSAR header.
  autosar_soad_header autosar_header;
  autosar_header.service_id = endian_reverse<uint16_t>(static_cast<uint16_t>(0X39FEU));
  autosar_header.method_id = endian_reverse<uint16_t>(static_cast<uint16_t>(0X805AU));
  autosar_header.length = endian_reverse<uint32_t>(static_cast<uint32_t>(payload.size()));

  auto *current_payload_position = const_cast<uint8_t *>(static_cast<const uint8_t *>(package->data()));
  uint64_t header_size = 0U;

  memcpy(current_payload_position, &eth_header, sizeof(ethernet_frame));
  current_payload_position += sizeof(ethernet_frame);
  header_size += sizeof(ethernet_frame);

  memcpy(current_payload_position, &ipv4_frag, sizeof(ipv4_header));
  current_payload_position += sizeof(ipv4_header);
  header_size += sizeof(ipv4_header);

  memcpy(current_payload_position, &udp_hdr, sizeof(udp_header));
  current_payload_position += sizeof(udp_header);
  header_size += sizeof(udp_header);

  memcpy(current_payload_position, &autosar_header, sizeof(autosar_soad_header2));
  header_size += sizeof(autosar_soad_header2);

  std::vector<std::tuple<size_t, uint64_t, size_t>> vec_tuple = {};
  auto ipackage = CreateNextIPackage(package);
  EXPECT_FALSE(ipackage->PayloadAvailable()) << "package available";
}

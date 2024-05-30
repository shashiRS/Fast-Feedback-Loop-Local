#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

using next::udex::extractor::DataExtractor;
using next::udex::publisher::DataPublisher;

const char *SDL_CONTENT = R"(<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
<View Name="array_root" CycleID="2">
  <Group Name="simple_float_arrays" Address="90100000" ArrayLen="1" Size="80">
    <Signal Name="ten_floats" Offset="0" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian"
        Size="4"/>
    <Signal Name="ten_more_floats" Offset="28" ArrayLen="10" Type="float" Bitmask="ffffffff"
        ByteOrder="little-endian" Size="4"/>
  </Group>
  <Group Name="array_of_structs" Address="901103E8" ArrayLen="1" Size="88">
    <SubGroup Name="first_group" Offset="0" ArrayLen="5" Size="12">
      <Signal Name="big_int" Offset="0" ArrayLen="1" Type="uint64" Bitmask="ffffffffffffffff"
          ByteOrder="little-endian" Size="8"/>
      <Signal Name="unsigned_short" Offset="8" ArrayLen="1" Type="ushort" Bitmask="ffff"
          ByteOrder="little-endian" Size="2"/>
      <Signal Name="another_unsigned_short" Offset="A" ArrayLen="1" Type="ushort" Bitmask="ffff"
          ByteOrder="little-endian" Size="2"/>
    </SubGroup>
    <SubGroup Name="second_group" Offset="3C" ArrayLen="7" Size="4">
      <Signal Name="unsigned_char1" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
      <Signal Name="unsigned_char2" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
      <Signal Name="unsigned_char3" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
      <Signal Name="unsigned_char4" Offset="3" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
    </SubGroup>
  </Group>
  <Group Name="array_of_array" Address="90110440" ArrayLen="1" Size="336">
    <SubGroup Name="array_group" Offset="0" ArrayLen="10" Size="21">
      <Signal Name="unsigned_short_array" Offset="0" ArrayLen="10" Type="ushort" Bitmask="ffff"
          ByteOrder="little-endian" Size="2"/>
      <Signal Name="unsigned_char" Offset="14" ArrayLen="1" Type="uchar" Bitmask="ff"
          ByteOrder="little-endian" Size="1"/>
    </SubGroup>
    <SubGroup Name="second_group" Offset="D2" ArrayLen="7" Size="18">
      <Signal Name="unsigned_char1" Offset="0" ArrayLen="6" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
      <Signal Name="unsigned_char2" Offset="6" ArrayLen="5" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
      <Signal Name="unsigned_char3" Offset="B" ArrayLen="4" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
      <Signal Name="unsigned_char4" Offset="F" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="big-endian"
          Size="1"/>
    </SubGroup>
  </Group>
</View>
</SdlFile>
)";

class DataPublisherTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DataPublisherTestFixture() { this->instance_name_ = "DataPublisherTestFixture"; }
};

constexpr int wait_time_ms = 3000;
constexpr int wait_increment_ms = 100;
constexpr int after_send_delay_ms = 100;

TEST_F(DataPublisherTestFixture, publisher_register_port_name_publish_data) {
  std::string node_name = "publisher_register_port_name_publish_data";
  std::string url_1 = "test" + node_name;
  auto subscriber = next::udex::subscriber::DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();
  extractor->SetBlockingExtraction(true);
  auto publisher = DataPublisher(node_name);

  EXPECT_NE(subscriber.Subscribe(url_1, extractor), 0u);

  size_t size = 1;
  publisher.RegisterDataDescription(url_1, size, true);

  CheckConnection(subscriber, {url_1}, 5000);

  auto sub_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(sub_info.size(), 1);

  auto topic_list = publisher.getTopicsAndHashes();
  EXPECT_EQ(topic_list.size(), 1);

  std::string payload1("payload1");

  // TODO publish data should return bool and to be checked
  publisher.publishData(url_1, payload1.data(), payload1.size(), false, 1);
  publisher.publishData(url_1, payload1.data(), payload1.size(), false, 2);
  publisher.publishData(url_1, payload1.data(), payload1.size(), false, 3);
  publisher.publishData(url_1, payload1.data(), payload1.size(), false, 4);

  extractor->isDataAvailable(url_1);
  auto raw_data = extractor->GetRawData(url_1);

  EXPECT_EQ(raw_data.Size, payload1.length());
  EXPECT_EQ(memcmp(raw_data.Data, payload1.c_str(), raw_data.Size), 0);

  auto extracted_data = extractor->GetExtractedData(url_1);

  EXPECT_EQ(extracted_data.size, payload1.length());
  EXPECT_EQ(memcmp(extracted_data.data, payload1.c_str(), extracted_data.size), 0);

  EXPECT_EQ(extractor->GetExtractedQueueWithTimestamp(url_1).size(), 4);
}

TEST_F(DataPublisherTestFixture, publisher_register_port_name_getTopicHash) {
  std::string node_name = "publisher_register_port_name_getTopicHash";
  auto publisher = DataPublisher(node_name);

  publisher.RegisterDataDescription((std::string) "SIM VFB.view.group1", (size_t)1);
  publisher.RegisterDataDescription((std::string) "SIM VFB.view.group2", (size_t)1);

  auto topic_list = publisher.getTopicsAndHashes();
  EXPECT_EQ(topic_list.size(), 2);

  auto publisher2 = DataPublisher("pub2");
  publisher2.RegisterDataDescription((std::string) "SIM VFB.view.group3", (size_t)1);
  auto list = publisher2.getTopicsAndHashes();
  EXPECT_EQ(list.size(), 1);
  publisher2.Reset();
  list = publisher2.getTopicsAndHashes();
  EXPECT_EQ(list.size(), 0);

  topic_list = publisher.getTopicsAndHashes();
  EXPECT_EQ(topic_list.size(), 2);
  EXPECT_EQ(topic_list.find("view.group1"), topic_list.end());
  EXPECT_EQ(topic_list.find("SIM_VFB.view.group1"), topic_list.end());
  publisher.Reset();

  topic_list = publisher.getTopicsAndHashes();
  EXPECT_EQ(topic_list.size(), 0);
}

TEST_F(DataPublisherTestFixture, publisher_register_data_from_sdl_file_and_publish) {
  std::string ref_name = "publisher_register_data_from_sdl_file_and_publish";
  eCAL::Util::EnableLoopback(true);

  auto publisher_test1 = DataPublisher(ref_name);
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  auto subscriber = next::udex::subscriber::DataSubscriber(ref_name);

  ASSERT_TRUE(publisher_test1.SetDataSourceInfo(ref_name, 87, 37)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher_test1.RegisterDataDescription((std::string) "arrays.sdl", true))
      << "RegisterDataDescription failed";

  subscriber.Subscribe(ref_name + ".array_root.simple_float_arrays", extractor);

  ASSERT_TRUE(
      DataPublisherTestFixture::CheckConnection(subscriber, {ref_name + ".array_root.simple_float_arrays"}, 5000));

  int dummy = 1;
  auto topic_list_from_file = publisher_test1.getTopicsAndHashes();
  EXPECT_EQ(topic_list_from_file.size(), 3);
  EXPECT_EQ(topic_list_from_file.begin()->first, ref_name + ".array_root.simple_float_arrays");
  EXPECT_TRUE(
      publisher_test1.publishData(ref_name + ".array_root.simple_float_arrays", &dummy, sizeof(dummy), false, 1));
  EXPECT_TRUE(publisher_test1.publishData("array_root.simple_float_arrays", &dummy, sizeof(dummy), true, 2));
  EXPECT_TRUE(publisher_test1.publishData(0x90100000, &dummy, sizeof(dummy), 3));

  auto queue = extractor->GetExtractedQueueWithTimestamp(ref_name + ".array_root.simple_float_arrays");
  EXPECT_EQ(queue.size(), 3);

  int expected_timestamp = 1;
  for (auto element : queue) {
    auto element_timestam = element->timestamp;
    EXPECT_EQ(element_timestam, expected_timestamp);
    expected_timestamp++;
  }
}

TEST_F(DataPublisherTestFixture, publisher_register_data_from_sdl_file) {
  auto publisher_test1 = DataPublisher("publisher_register_data_from_sdl_file");

  ASSERT_TRUE(publisher_test1.RegisterDataDescription((std::string) "arrays.sdl")) << "RegisterDataDescription failed";

  auto topic_list_from_file = publisher_test1.getTopicsAndHashes();
  EXPECT_EQ(topic_list_from_file.size(), 3);
  EXPECT_EQ(topic_list_from_file.begin()->first, "SIM VFB.array_root.simple_float_arrays");
  publisher_test1.Reset();
}

TEST_F(DataPublisherTestFixture, publisher_register_sdl_data_from_memory) {
  std::string node_name = "publisher_register_sdl_data_from_memory";
  auto publisher_test2 = DataPublisher(node_name);

  ASSERT_TRUE(
      publisher_test2.RegisterDataDescription((const char *)"SDL_CONTENT", (void *)SDL_CONTENT, strlen(SDL_CONTENT)))
      << "SetDataSourceInfo failed";

  auto topic_list_from_memory = publisher_test2.getTopicsAndHashes();

  EXPECT_EQ(topic_list_from_memory.size(), 3);
  EXPECT_EQ(topic_list_from_memory.begin()->first, "SIM VFB.array_root.simple_float_arrays");

  publisher_test2.Reset();
}

TEST_F(DataPublisherTestFixture, publisher_register_compare_sdl_data_from_file_and_memory) {
  std::string node_name = "publisher_register_compare_sdl_data_from_file_and_memory";
  auto publisher_test1 = DataPublisher("test1" + node_name);

  ASSERT_TRUE(publisher_test1.RegisterDataDescription((std::string) "arrays.sdl")) << "RegisterDataDescription failed";

  auto topic_list_from_file = publisher_test1.getTopicsAndHashes();
  publisher_test1.Reset();

  auto publisher_test2 = DataPublisher("test2" + node_name);

  ASSERT_TRUE(
      publisher_test2.RegisterDataDescription((const char *)"SDL_CONTENT", (void *)SDL_CONTENT, strlen(SDL_CONTENT)))
      << "RegisterDataDescription failed";

  auto topic_list_from_memory = publisher_test2.getTopicsAndHashes();
  publisher_test2.Reset();

  auto publisher_test3 = DataPublisher("test3" + node_name);
  auto last_topic = publisher_test3.getTopicsAndHashes();
  publisher_test3.Reset();

  ASSERT_TRUE(last_topic.empty());

  EXPECT_EQ(topic_list_from_file, topic_list_from_memory);
}

TEST_F(DataPublisherTestFixture, get_filter_expression_from_url) {
  const char *sdl{R"(
		<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
			<View Name="array_root" CycleID="2">
				<Group Name="simple_float_arrays" Address="90100000" ArrayLen="1" Size="80">
					<Signal Name="ten_floats" Offset="0" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
					<Signal Name="ten_more_floats" Offset="28" ArrayLen="10" Type="float" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
				</Group>
				<Group Name="array_of_structs" Address="901103E8" ArrayLen="1" Size="88">
					<SubGroup Name="first_group" Offset="0" ArrayLen="5" Size="12">
						<Signal Name="big_int" Offset="0" ArrayLen="1" Type="uint64" Bitmask="ffffffffffffffff" ByteOrder="little-endian" Size="8"/>
						<Signal Name="unsigned_short" Offset="8" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
						<Signal Name="another_unsigned_short" Offset="A" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian" Size="2"/>
					</SubGroup>
					<SubGroup Name="second_group" Offset="3C" ArrayLen="7" Size="4">
						<Signal Name="unsigned_char1" Offset="0" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
						<Signal Name="unsigned_char2" Offset="1" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
						<Signal Name="unsigned_char3" Offset="2" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
						<Signal Name="unsigned_char4" Offset="3" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="big-endian" Size="1"/>
				</SubGroup>
			</Group>
			</View>
		</SdlFile>)"};
  std::string node_name = "get_filter_expression_from_url";
  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>(node_name);
  ASSERT_TRUE(publisher->SetDataSourceInfo(node_name, 87, 37)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher->RegisterDataDescription(static_cast<const char *>("sdl"), (void *)sdl, strlen(sdl)))
      << "RegisterDataDescription failed";

  std::vector<std::string> single_url{node_name + ".array_root.simple_float_arrays"};
  std::string filter_expression = publisher->GetFilterExpressionFromURLs(single_url);
  std::string filter_expression_ref{"(SourceId = 87 AND Instance = 37 AND Cycle = 2 AND Address = 0x90100000)"};
  EXPECT_EQ(filter_expression, filter_expression_ref);

  // TODO: fix different order of filters on Linux than on Windows
#ifdef _MSC_VER
  std::vector<std::string> multiple_urls{node_name + ".array_root.simple_float_arrays",
                                         node_name + ".array_root.array_of_structs"};
#else
  std::vector<std::string> multiple_urls{node_name + ".array_root.array_of_structs",
                                         node_name + ".array_root.simple_float_arrays"};
#endif

  filter_expression = publisher->GetFilterExpressionFromURLs(multiple_urls);
  filter_expression_ref = "(SourceId = 87 AND Instance = 37 AND Cycle = 2 AND Address = 0x901103E8) OR "
                          "(SourceId = 87 AND Instance = 37 AND Cycle = 2 AND Address = 0x90100000)";
  EXPECT_EQ(filter_expression, filter_expression_ref);

  std::vector<std::string> invalid_urls{node_name + ".array_root.simple_float_arrays.ten_floats",
                                        node_name + ".array_root.array_of_structs.first_group", "Invalid_URL"};
  filter_expression = publisher->GetFilterExpressionFromURLs(invalid_urls);
  filter_expression_ref = "";
  EXPECT_EQ(filter_expression, filter_expression_ref);
}

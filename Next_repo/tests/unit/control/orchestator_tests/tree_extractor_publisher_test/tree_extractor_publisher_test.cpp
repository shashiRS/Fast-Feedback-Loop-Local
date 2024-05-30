#include <gtest/gtest.h>

#include <list>

#include <next/sdk/sdk.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include "next/udex/subscriber/data_subscriber.hpp"

#include "base_ctrl_data_generator/base_ctrl_data_pool.hpp"
#include "base_ctrl_data_generator/tree_extractor_publisher.h"

namespace next {
namespace control {
namespace orchestrator {

class TreeExtractorPublisherTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  TreeExtractorPublisherTestFixture() { instance_name_ = "TreeExtractorPublisherTestFixture"; }
};

class TreeExtractorPublisherTester : public TreeExtractorPublisher {
public:
  TreeExtractorPublisherTester(const std::string &flow_name, const std::string &client_name)
      : TreeExtractorPublisher(flow_name, client_name) {}
  bool ParseSdlToFlowTest(std::string sdl_filename) { return ParseSdlToFlow(sdl_filename); }
  const std::string &getSdlContent() { return sdl_string_; }
  std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor> &getTreeExtractor() { return tree_extractor; }
  const char *getPackageMemory() { return tree_package_ptr_; }
  size_t getPackageSize() { return tree_package_size_; }
};

std::string sdl_transform_ref =
    R"(<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema" ByteAlignment="1" Version="2.0">
  <View Name="flow_client" CycleID="207">
    <Group Name="SigHeader" Address="20350000" ArrayLen="1" Size="12">
      <Signal Name="uiTimeStamp" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian"
              Size="4"/>
      <Signal Name="uiMeasurementCounter" Offset="4" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian"
              Size="2"/>
      <Signal Name="uiCycleCounter" Offset="6" ArrayLen="1" Type="ushort" Bitmask="ffff" ByteOrder="little-endian"
              Size="2"/>
      <Signal Name="eSigStatus" Offset="8" ArrayLen="1" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
      <Signal Name="a_reserve" Offset="9" ArrayLen="3" Type="uchar" Bitmask="ff" ByteOrder="little-endian" Size="1"/>
    </Group>
  </View>
</SdlFile>
)";

TEST_F(TreeExtractorPublisherTestFixture, checkSdlReplacement) {
  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.ParseSdlToFlowTest("sig_header.sdl"));
  EXPECT_EQ(tester.getSdlContent(), sdl_transform_ref);
}

TEST_F(TreeExtractorPublisherTestFixture, CheckPublisherCreation) {
  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));

  auto explorer = next::udex::explorer::SignalExplorer();
  auto test = explorer.GetChildByUrl("Orchestrator.flow_client.SigHeader");

  auto element = std::find_if(test.begin(), test.end(),
                              [](const next::udex::ChildInfo &child_info) { return child_info.name == "uiTimeStamp"; });
  EXPECT_TRUE(element != test.end());
  element = std::find_if(test.begin(), test.end(), [](const next::udex::ChildInfo &child_info) {
    return child_info.name == "uiMeasurementCounter";
  });
  EXPECT_TRUE(element != test.end());
  element = std::find_if(test.begin(), test.end(),
                         [](const next::udex::ChildInfo &child_info) { return child_info.name == "uiCycleCounter"; });
  EXPECT_TRUE(element != test.end());
  element = std::find_if(test.begin(), test.end(),
                         [](const next::udex::ChildInfo &child_info) { return child_info.name == "eSigStatus"; });
  EXPECT_TRUE(element != test.end());
}

TEST_F(TreeExtractorPublisherTestFixture, CheckTreeExtractor) {
  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));
  EXPECT_EQ(tester.getTreeExtractor()->signals_.size(), 5) << "TreeExtractor has not correct signals found";
}

void TestMemorySetToZero(TreeExtractorPublisherTester &tester) {
  auto memblock = tester.getPackageMemory();
  ASSERT_TRUE(tester.getPackageSize() != 0) << "memory for tree extractor was not allocated";
  ASSERT_TRUE(memblock != nullptr) << "memory for tree extractor was not allocated";
  for (size_t i_byte = 0; i_byte < tester.getPackageSize(); i_byte++) {
    EXPECT_EQ(memblock[i_byte], 0);
  }
}

TEST_F(TreeExtractorPublisherTestFixture, CheckCleanMemoryAfterSetup) {
  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));
  TestMemorySetToZero(tester);
}

template <class setValue>
void SetAndCheckValueType(TreeExtractorPublisherTester &tester, setValue value, const char *memblock,
                          unsigned int test_u_int, unsigned short shorttest_u_short, unsigned char test_u_char) {
  EXPECT_TRUE(tester.SetValue(value, "SigHeader.uiTimeStamp"));
  EXPECT_TRUE(tester.SetValue(value, "SigHeader.uiMeasurementCounter"));
  EXPECT_TRUE(tester.SetValue(value, "SigHeader.eSigStatus"));
  unsigned int test_value = ((unsigned int *)&(memblock[0]))[0];
  EXPECT_EQ(test_value, test_u_int);
  unsigned short test_value_short = ((unsigned short *)&(memblock[4]))[0];
  EXPECT_EQ(test_value_short, shorttest_u_short);
  unsigned char test_value_char = ((unsigned char *)&(memblock[8]))[0];
  EXPECT_EQ(test_value_char, test_u_char);
}

void SetAndCheckValueTypeFromValueUnion(TreeExtractorPublisherTester &tester, next::udex::ValueUnion value,
                                        next::udex::SignalType type, const char *memblock, unsigned int test_u_int,
                                        unsigned short shorttest_u_short, unsigned char test_u_char) {
  EXPECT_TRUE(tester.SetValueFromValueUnion(value, type, "SigHeader.uiTimeStamp"));
  EXPECT_TRUE(tester.SetValueFromValueUnion(value, type, "SigHeader.uiMeasurementCounter"));
  EXPECT_TRUE(tester.SetValueFromValueUnion(value, type, "SigHeader.eSigStatus"));
  unsigned int test_value = ((unsigned int *)&(memblock[0]))[0];
  EXPECT_EQ(test_value, test_u_int);
  unsigned short test_value_short = ((unsigned short *)&(memblock[4]))[0];
  EXPECT_EQ(test_value_short, shorttest_u_short);
  unsigned char test_value_char = ((unsigned char *)&(memblock[8]))[0];
  EXPECT_EQ(test_value_char, test_u_char);
}

TEST_F(TreeExtractorPublisherTestFixture, CheckSetMemory) {
  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));
  auto memblock = tester.getPackageMemory();
  TestMemorySetToZero(tester);

  float value_float;
  value_float = 1.571f;
  SetAndCheckValueType(tester, value_float, memblock, 1u, 1u, 1u);

  double value_double;
  value_double = 25.571f;
  SetAndCheckValueType(tester, value_double, memblock, 25u, 25u, 25u);

  uint64_t value_uint64t;
  value_uint64t = 113u;
  SetAndCheckValueType(tester, value_uint64t, memblock, 113u, 113u, 113u);
}

TEST_F(TreeExtractorPublisherTestFixture, CheckSetMemoryNoCrashOnNoSetup) {
  auto tester = TreeExtractorPublisherTester("flow", "client");

  tester.SetValueFromValueUnion({}, {}, "asdf");
  tester.SetValue(1, "asdfasdf");
  tester.Publish(1);
}

TEST_F(TreeExtractorPublisherTestFixture, CheckPublisherNotBlockingIfSetupWrong) {

  auto tester = TreeExtractorPublisherTester("flow", "client");

  EXPECT_FALSE(tester.SetValueFromValueUnion({}, {}, "asdf"));
  EXPECT_FALSE(tester.SetValue(1, "asdfasdf"));
  EXPECT_FALSE(tester.Publish(1));
  EXPECT_FALSE(tester.IsReady());

  EXPECT_FALSE(tester.Setup(""));
  EXPECT_FALSE(tester.IsReady());
  auto starttime =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();

  EXPECT_FALSE(tester.Publish(1));
  EXPECT_FALSE(tester.SetValue(1, "asdfasdf"));
  EXPECT_FALSE(tester.SetValueFromValueUnion({}, {}, "asdf"));
  auto endtime =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();

  ASSERT_LE(endtime - starttime, 100);
}

TEST_F(TreeExtractorPublisherTestFixture, CheckSetMemoryWithValueUnion) {
  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));
  auto memblock = tester.getPackageMemory();
  TestMemorySetToZero(tester);

  next::udex::ValueUnion value_float;
  value_float.asFloat = 1.571f;
  SetAndCheckValueTypeFromValueUnion(tester, value_float, udex::SignalType::SIGNAL_TYPE_FLOAT, memblock, 1u, 1u, 1u);

  next::udex::ValueUnion value_double;
  value_double.asDouble = 25.571f;
  SetAndCheckValueTypeFromValueUnion(tester, value_double, udex::SignalType::SIGNAL_TYPE_DOUBLE, memblock, 25u, 25u,
                                     25u);

  next::udex::ValueUnion value_uint64t;
  value_uint64t.asUint64 = 113u;
  SetAndCheckValueTypeFromValueUnion(tester, value_uint64t, udex::SignalType::SIGNAL_TYPE_UINT64, memblock, 113u, 113u,
                                     113u);
}

TEST_F(TreeExtractorPublisherTestFixture, CheckSetMemoryAfterPublish) {
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  extractor->SetBlockingExtraction(true);
  next::udex::subscriber::DataSubscriber subscriber("ConnectDataPoolToPublisher_success");
  subscriber.Subscribe("Orchestrator.flow_client.SigHeader", extractor);

  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_FALSE(tester.IsReady());
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));
  EXPECT_TRUE(tester.IsReady());

  for (int i_request = 0; i_request < 10; i_request++) {
    if (subscriber.IsConnected("Orchestrator.flow_client.SigHeader")) {
      continue;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  EXPECT_TRUE(subscriber.IsConnected("Orchestrator.flow_client.SigHeader"));

  auto memblock = tester.getPackageMemory();
  TestMemorySetToZero(tester);

  int value_int;
  value_int = 113;
  SetAndCheckValueType(tester, value_int, memblock, 113u, 113u, 113u);
  tester.Publish(1u);

  next::udex::dataType return_value;
  extractor->GetValue("Orchestrator.flow_client.SigHeader.uiTimeStamp", next::udex::SignalType::SIGNAL_TYPE_UINT64,
                      return_value);
  uint64_t test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 113u);

  TestMemorySetToZero(tester);
}

TEST_F(TreeExtractorPublisherTestFixture, ConnectDataPoolToPublisher_success) {
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  extractor->SetBlockingExtraction(true);
  next::udex::subscriber::DataSubscriber subscriber("ConnectDataPoolToPublisher_success");
  subscriber.Subscribe("Orchestrator.flow_client.SigHeader", extractor);

  auto tester = TreeExtractorPublisherTester("flow", "client");
  EXPECT_TRUE(tester.Setup("sig_header.sdl"));

  for (int i_request = 0; i_request < 10; i_request++) {
    if (subscriber.IsConnected("Orchestrator.flow_client.SigHeader")) {
      continue;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  EXPECT_TRUE(subscriber.IsConnected("Orchestrator.flow_client.SigHeader"));

  TestMemorySetToZero(tester);

  // auto testdata = test.GetNewData(component_name_dummy, execution_name_dummy, success);

  DataPool dataPool;
  dataPool.fixed_signal["SigHeader.uiTimeStamp"] = 113.f;

  udex::ValueUnion val_uni;
  val_uni.asUint16 = 321u;

  dataPool.connected_signal["SigHeader.uiCycleCounter"] = {val_uni, udex::SignalType::SIGNAL_TYPE_UINT16};

  for (auto signal : dataPool.fixed_signal) {
    tester.SetValue(signal.second, signal.first);
  }

  for (auto signal : dataPool.connected_signal) {
    tester.SetValueFromValueUnion(signal.second.value, signal.second.signal_type, signal.first);
  }

  tester.Publish(1u);

  next::udex::dataType return_value;
  extractor->GetValue("Orchestrator.flow_client.SigHeader.uiTimeStamp", next::udex::SignalType::SIGNAL_TYPE_UINT64,
                      return_value);
  uint64_t test = std::get<uint64_t>(return_value);
  EXPECT_EQ(test, 113u);
  next::udex::dataType return_value_connected;
  extractor->GetValue("Orchestrator.flow_client.SigHeader.uiCycleCounter", next::udex::SignalType::SIGNAL_TYPE_UINT64,
                      return_value_connected);
  uint64_t test_connected = std::get<uint64_t>(return_value_connected);
  EXPECT_EQ(test_connected, 321u);
  TestMemorySetToZero(tester);
}

} // namespace orchestrator
} // namespace control
} // namespace next

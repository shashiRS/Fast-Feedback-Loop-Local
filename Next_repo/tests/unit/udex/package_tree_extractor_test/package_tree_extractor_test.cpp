#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <ecal/ecal_util.h>

#include <next/sdk/sdk.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>

#include <next/udex/struct_extractor/container_extractor.hpp>
#include <next/udex/struct_extractor/package_tree_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>

#include "struct_info_provider_test.h"

using namespace next::udex::extractor;

class ExtractionTreeTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  ExtractionTreeTestFixture() { this->instance_name_ = "ExtractionTreeTestFixture"; }
  void SetUp() override {
    next::sdk::testing::TestUsingEcal::SetUp();
    memory = (char *)malloc(memory_size);
    ASSERT_NE(memory, nullptr) << "memory allocation failed";
  }
  void TearDown() override {
    next::sdk::testing::TestUsingEcal::TearDown();
    free(memory);
  }
  char *memory = nullptr;
  size_t memory_size = 336;
};

namespace next {
namespace udex {
namespace extractor {

class StructExtractorArrayFactoryTest : public StructExtractorArrayFactory {
public:
  StructExtractorArrayFactoryTest(){};

  void UpdateUrlAsArrayTest(std::shared_ptr<StructExtractorBasic> obj, int index) { UpdateUrlAsArray(obj, index); }

  bool CopyOffsetAndTypeTest(std::shared_ptr<StructExtractorBasic> obj_from,
                             std::shared_ptr<StructExtractorBasic> obj_to) {
    return copyOffsetsAndType(obj_from, obj_to);
  }

  void UpdateOffsetTest(std::shared_ptr<StructExtractorBasic> obj) { UpdateOffset(obj); }
  void ShiftOffsetTest(std::shared_ptr<StructExtractorBasic> obj, size_t shift) { shiftOffset(obj, shift); }
};

class StructExtractorBasicTest {
public:
  StructExtractorBasicTest(){};

  size_t GetOffset(std::shared_ptr<StructExtractorBasic> obj) { return obj->offset_; }
  size_t GetOffset(StructExtractorBasic *obj) { return obj->offset_; }
  size_t GetOffset(StructExtractorBasic &obj) { return obj.offset_; }
  SignalType GetType(std::shared_ptr<StructExtractorBasic> obj) { return obj->type_; }
  SignalType GetType(StructExtractorBasic *obj) { return obj->type_; }
  std::string GetUrl(std::shared_ptr<StructExtractorBasic> obj) { return obj->url_; }
};

} // namespace extractor
} // namespace udex
} // namespace next

namespace next {
namespace udex {
namespace struct_extractor_tree_test {

class TestingRootContainer : public StructExtractor<TestingRootContainer> {
public:
  TestingRootContainer(std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(emptyRequestBasic, signal_info_provider_in) {}
  virtual TestingRootContainer Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override { return StructExtractorBasic::SetUrls(url); }
};

TEST_F(ExtractionTreeTestFixture, testAnyValueSingle) {
  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto SignalInjectorContainer = new TestingRootContainer(parser);
  SignalInjectorContainer->SetUrls("");

  AnyValue testStack(SignalInjectorContainer);
  auto testHeap = new AnyValue(SignalInjectorContainer);
  auto testShared = std::make_shared<AnyValue>(SignalInjectorContainer);

  testStack.SetUrls("sur.tp[0].pos");
  testHeap->SetUrls("sur.tp[0].pos");
  testShared->SetUrls("sur.tp[0].pos");

  StructExtractorBasicTest tester;
  EXPECT_EQ(tester.GetOffset(testStack), 44) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(testHeap), 44) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(testShared), 44) << "update the url failed";
  delete testHeap;
  delete SignalInjectorContainer;
}

TEST_F(ExtractionTreeTestFixture, testAnyValueSingleNoMemory_nocrash) {
  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto SignalInjectorContainer = new TestingRootContainer(parser);
  SignalInjectorContainer->SetUrls("");

  AnyValue testStack(SignalInjectorContainer);
  auto testHeap = new AnyValue(SignalInjectorContainer);
  auto testShared = std::make_shared<AnyValue>(SignalInjectorContainer);

  testStack.SetUrls("sur.tp[0].pos");
  testHeap->SetUrls("sur.tp[0].pos");
  testShared->SetUrls("sur.tp[0].pos");
  EXPECT_NO_THROW(testStack.Get());
  EXPECT_NO_THROW(testHeap->Get());
  EXPECT_NO_THROW(testShared->Get());

  delete testHeap;
  delete SignalInjectorContainer;
}

std::shared_ptr<TestingRootContainer> CreateSignalInfoInjector() {
  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto SignalInjectorContainer = std::make_shared<TestingRootContainer>(parser);
  SignalInjectorContainer->SetUrls("");
  return SignalInjectorContainer;
}

void SetDummyValues(const char *memory_block) {
  // testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1
  // Offset D2
  size_t offset = 210;
  auto valuePointer = (unsigned char *)(memory_block + offset);
  valuePointer[0] = 1;
  valuePointer[1] = 2;
  valuePointer[2] = 3;
  valuePointer[3] = 4;
  valuePointer[4] = 5;
  valuePointer[5] = 6;

  // testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2
  // offset D2 + sizeof(secondgroup) + sizof(usigned_char1)
  offset = 210 + 18 + 6;
  valuePointer = (unsigned char *)(memory_block + offset);
  valuePointer[0] = 10;
  valuePointer[1] = 20;
  valuePointer[2] = 30;
  valuePointer[3] = 40;
  valuePointer[4] = 50;

  // testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4
  // offset D2 + (sizeof(secondgroup)*6 + offset F
  offset = 210 + 18 * 6 + 15;
  valuePointer = (unsigned char *)(memory_block + offset);
  valuePointer[0] = 10;
  valuePointer[1] = 20;
  valuePointer[2] = 30;
}

void CheckDummyValues(const char *memory_block) {
  // testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1
  // Offset D2
  size_t offset = 210;
  auto valuePointer = (unsigned char *)(memory_block + offset);
  EXPECT_EQ(valuePointer[0], 10) << "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1[0]";
  EXPECT_EQ(valuePointer[1], 20) << "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1[1]";
  EXPECT_EQ(valuePointer[2], 30) << "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1[2]";
  EXPECT_EQ(valuePointer[3], 40) << "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1[3]";
  EXPECT_EQ(valuePointer[4], 50) << "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1[4]";
  EXPECT_EQ(valuePointer[5], 60) << "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1[5]";

  // testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2
  // offset D2 + sizeof(secondgroup) + sizof(usigned_char1)
  offset = 210 + 18 + 6;
  valuePointer = (unsigned char *)(memory_block + offset);
  EXPECT_EQ(valuePointer[0], 1) << "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2[0]";
  EXPECT_EQ(valuePointer[1], 2) << "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2[1]";
  EXPECT_EQ(valuePointer[2], 3) << "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2[2]";
  EXPECT_EQ(valuePointer[3], 4) << "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2[3]";
  EXPECT_EQ(valuePointer[4], 5) << "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2[4]";

  // testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4
  // offset D2 + (sizeof(secondgroup)*6 + offset F
  offset = 210 + 18 * 6 + 15;
  valuePointer = (unsigned char *)(memory_block + offset);
  EXPECT_EQ(valuePointer[0], 1) << "testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4[0]";
  EXPECT_EQ(valuePointer[1], 2) << "testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4[1]";
  EXPECT_EQ(valuePointer[2], 3) << "testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4[2]";
}

TEST_F(ExtractionTreeTestFixture, testAnyValueWithSubContainers) {
  auto groupParent = std::make_shared<ExtractorContainer>(CreateSignalInfoInjector());
  groupParent->SetUrls("sur");

  std::vector<std::shared_ptr<ExtractorContainer>> tp_container;
  std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
  for (unsigned int i = 0; i < 4u; i++) {
    tp_container.push_back(std::make_shared<ExtractorContainer>(groupParent));
    tp_container[i]->SetUrls(".tp");
    tp_container[i]->SetChild({true, ".vel", {}, std::make_shared<AnyValue>(tp_container[i])});
    tp_container[i]->SetChild({true, ".pos", {}, std::make_shared<AnyValue>(tp_container[i])});
    cast_vector.push_back(tp_container[i]);
  }
  next::udex::extractor::StructExtractorArrayFactoryTest factory =
      next::udex::extractor::StructExtractorArrayFactoryTest();
  factory.UpdateUrlInVector(cast_vector);

  StructExtractorBasicTest tester;
  ASSERT_EQ(tp_container.size(), 4) << "not enough tp objects created";
  ASSERT_EQ(tp_container[0]->GetChildren().size(), 2) << "not enough tp objects created";
  ASSERT_EQ(tp_container[1]->GetChildren().size(), 2) << "not enough tp objects created";
  ASSERT_EQ(tp_container[2]->GetChildren().size(), 2) << "not enough tp objects created";
  ASSERT_EQ(tp_container[3]->GetChildren().size(), 2) << "not enough tp objects created";
  EXPECT_EQ(tester.GetOffset(tp_container[0]), 40) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[0]->GetChildren()[0].leaf), 40) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[0]->GetChildren()[1].leaf), 44) << "update the url failed";
  EXPECT_EQ(tp_container[0]->GetChildren()[1].leaf->GetTypeOfAnyValue(), SignalType::SIGNAL_TYPE_FLOAT)
      << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[1]), 48) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[1]->GetChildren()[0].leaf), 48) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[1]->GetChildren()[1].leaf), 52) << "update the url failed";
  EXPECT_EQ(tp_container[1]->GetChildren()[0].leaf->GetTypeOfAnyValue(), SignalType::SIGNAL_TYPE_FLOAT)
      << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[2]), 56) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[2]->GetChildren()[0].leaf), 56) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[2]->GetChildren()[1].leaf), 60) << "update the url failed";
  EXPECT_EQ(tp_container[2]->GetChildren()[1].leaf->GetTypeOfAnyValue(), SignalType::SIGNAL_TYPE_FLOAT)
      << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[3]), 64) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[3]->GetChildren()[0].leaf), 64) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_container[3]->GetChildren()[1].leaf), 68) << "update the url failed";
  EXPECT_EQ(tp_container[3]->GetChildren()[0].leaf->GetTypeOfAnyValue(), SignalType::SIGNAL_TYPE_FLOAT)
      << "update the url failed";
}

TEST_F(ExtractionTreeTestFixture, testTreeExtractorWithUdex) {

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";

  SetDummyValues(memory);

  struct_extractor::PackageTreeExtractor extractor("testAnyValueWithUdex.array_root.array_of_array");
  extractor.SetMemory(memory, memory_size);
  int signal_found = 0;
  constexpr int k_signalsToCheck = 3;
  for (auto &signal : extractor.signals_) {
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1") {
      EXPECT_EQ(signal.value->Get(0).asUint8, 1u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(1).asUint8, 2u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(2).asUint8, 3u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(3).asUint8, 4u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(4).asUint8, 5u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(5).asUint8, 6u) << "signals not correct";
      signal_found++;
    }
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2") {
      EXPECT_EQ(signal.value->Get(0).asUint8, 10u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(1).asUint8, 20u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(2).asUint8, 30u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(3).asUint8, 40u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(4).asUint8, 50u) << "signals not correct";
      signal_found++;
    }
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4") {
      EXPECT_EQ(signal.value->Get(0).asUint8, 10u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(1).asUint8, 20u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(2).asUint8, 30u) << "signals not correct";
      signal_found++;
    }
  }
  EXPECT_EQ(signal_found, k_signalsToCheck) << "not all signals found";
}

TEST_F(ExtractionTreeTestFixture, testAnyValueWithUdex) {

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";

  auto value = std::make_shared<AnyValue>(emptyRequestBasic);
  value->SetUrls("testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2");
  EXPECT_EQ(SignalType::SIGNAL_TYPE_UINT8, value->GetTypeOfAnyValue());
}

TEST_F(ExtractionTreeTestFixture, testStructWithArrayOfStruct_success) {

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";
  struct_extractor::PackageTreeExtractor extractor("testAnyValueWithUdex.array_root.UsDrvDetectionList");
  int signal_found = 0;
  constexpr int k_signalsToCheck = 2;
  for (auto &signal : extractor.signals_) {
    if (signal.name == "testAnyValueWithUdex.array_root.UsDrvDetectionList.detections[0].sensorId") {
      signal_found++;
    }
    if (signal.name == "testAnyValueWithUdex.array_root.UsDrvDetectionList.detections[1].amplitude") {
      signal_found++;
    }
  }
  EXPECT_EQ(signal_found, k_signalsToCheck) << "not all signals found";
}

TEST_F(ExtractionTreeTestFixture, testSingleArrayObject_success) {
  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";
  SetDummyValues(memory);

  struct_extractor::PackageTreeExtractor extractor("testAnyValueWithUdex.array_root.array_of_array.second_group[1]");
  extractor.SetMemory(memory, memory_size);
  int signal_found = 0;
  constexpr int k_signalsToCheck = 1;
  for (auto &signal : extractor.signals_) {
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2") {
      EXPECT_EQ(signal.value->Get(0).asUint8, 10u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(1).asUint8, 20u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(2).asUint8, 30u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(3).asUint8, 40u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(4).asUint8, 50u) << "signals not correct";
      signal_found++;
    }
  }
  EXPECT_EQ(signal_found, k_signalsToCheck) << "not all signals found";

  signal_found = 0;
  for (auto &signal : extractor.signals_) {
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2") {
      size_t max;
      size_t array_size = signal.value->ArraySize();
      size_t data_size = next::udex::any_value_size_map.at(signal.value->GetTypeOfAnyValue()) * array_size;

      unsigned char test[5];
      void *ptr = signal.value->GetAddress(max);
      memcpy(&test[0], ptr, data_size);

      EXPECT_EQ(data_size <= max, true) << "sdl not correct";
      EXPECT_EQ(max, 336 /*group size*/ - (21 * 10 /*second subgroup offset*/ + 18 /*second index of subgroup array*/ +
                                           6 /*offset to signal*/))
          << "sdl not correct";
      EXPECT_EQ(test[0], 10u) << "signals not correct";
      EXPECT_EQ(test[1], 20u) << "signals not correct";
      EXPECT_EQ(test[2], 30u) << "signals not correct";
      EXPECT_EQ(test[3], 40u) << "signals not correct";
      EXPECT_EQ(test[4], 50u) << "signals not correct";
      signal_found++;
    }
  }
  EXPECT_EQ(signal_found, k_signalsToCheck) << "not all signals found";
}

TEST_F(ExtractionTreeTestFixture, testSingleLeafUrl_success) {

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";
  SetDummyValues(memory);
  struct_extractor::PackageTreeExtractor extractor(
      "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2");
  extractor.SetMemory(memory, memory_size);
  int signal_found = 0;
  constexpr int k_signalsToCheck = 1;
  for (auto &signal : extractor.signals_) {
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2") {
      EXPECT_EQ(signal.value->Get(0).asUint8, 10u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(1).asUint8, 20u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(2).asUint8, 30u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(3).asUint8, 40u) << "signals not correct";
      EXPECT_EQ(signal.value->Get(4).asUint8, 50u) << "signals not correct";
      signal_found++;
    }
  }
  EXPECT_EQ(signal_found, k_signalsToCheck) << "not all signals found";
}

TEST_F(ExtractionTreeTestFixture, testSetAnyValueUint8ArrayWithTreeExtractorUdex) {

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";

  SetDummyValues(memory);

  struct_extractor::PackageTreeExtractor extractor("testAnyValueWithUdex.array_root.array_of_array");
  extractor.SetMemory(memory, memory_size);
  ValueUnion myUnion;

  int signal_found = 0;
  constexpr int k_signalsToCheck = 3;
  for (auto &signal : extractor.signals_) {
    // set unsigned char array test
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[0].unsigned_char1") {
      myUnion.asUint8 = 10u;
      EXPECT_TRUE(signal.value->Set(myUnion, 0));

      myUnion.asUint8 = 20u;
      EXPECT_TRUE(signal.value->Set(myUnion, 1));

      myUnion.asUint8 = 30u;
      EXPECT_TRUE(signal.value->Set(myUnion, 2));

      myUnion.asUint8 = 40u;
      EXPECT_TRUE(signal.value->Set(myUnion, 3));

      myUnion.asUint8 = 50u;
      EXPECT_TRUE(signal.value->Set(myUnion, 4));

      myUnion.asUint8 = 60u;
      EXPECT_TRUE(signal.value->Set(myUnion, 5));

      signal_found++;
    }
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[1].unsigned_char2") {
      myUnion.asUint8 = 1u;
      EXPECT_TRUE(signal.value->Set(myUnion, 0));

      myUnion.asUint8 = 2u;
      EXPECT_TRUE(signal.value->Set(myUnion, 1));

      myUnion.asUint8 = 3u;
      EXPECT_TRUE(signal.value->Set(myUnion, 2));

      myUnion.asUint8 = 4u;
      EXPECT_TRUE(signal.value->Set(myUnion, 3));

      myUnion.asUint8 = 5u;
      EXPECT_TRUE(signal.value->Set(myUnion, 4));
      signal_found++;
    }
    // test set for last signals in group
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_array.second_group[6].unsigned_char4") {
      myUnion.asUint8 = 1u;
      EXPECT_TRUE(signal.value->Set(myUnion, 0));

      myUnion.asUint8 = 2u;
      EXPECT_TRUE(signal.value->Set(myUnion, 1));

      myUnion.asUint8 = 3u;
      EXPECT_TRUE(signal.value->Set(myUnion, 2));
      signal_found++;
    }
  }

  CheckDummyValues(memory);

  EXPECT_EQ(signal_found, k_signalsToCheck) << "not all signals found";
}

TEST_F(ExtractionTreeTestFixture, testSetAnyValueFloatArrayWithTreeExtractorUdex) {

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";

  size_t memory_size_local = 80;
  char *simple_float_arrays = (char *)malloc(memory_size_local);
  ASSERT_NE(simple_float_arrays, nullptr) << "memory allocation failed";
  memset(simple_float_arrays, 0, memory_size_local);

  struct_extractor::PackageTreeExtractor extractor("testAnyValueWithUdex.array_root.simple_float_arrays");
  extractor.SetMemory(simple_float_arrays, memory_size_local);
  ValueUnion myUnion;

  for (auto &signal : extractor.signals_) {
    // set first and last floats in first array
    if (signal.name == "testAnyValueWithUdex.array_root.simple_float_arrays.ten_floats") {
      myUnion.asFloat = 1.5f;
      EXPECT_TRUE(signal.value->Set(myUnion, 0));

      myUnion.asFloat = 5.1f;
      EXPECT_TRUE(signal.value->Set(myUnion, 9));
    }
    // set first and last floats in second array
    if (signal.name == "testAnyValueWithUdex.array_root.simple_float_arrays.ten_more_floats") {
      myUnion.asFloat = 2.5f;
      EXPECT_TRUE(signal.value->Set(myUnion, 0));

      myUnion.asFloat = 5.2f;
      EXPECT_TRUE(signal.value->Set(myUnion, 9));
    }
    // attempt to set data outside memory block, expect false
    if (signal.name == "testAnyValueWithUdex.array_root.simple_float_arrays.ten_more_floats") {
      myUnion.asFloat = 3.14f;
      EXPECT_FALSE(signal.value->Set(myUnion, 10));
    }
  }

  EXPECT_EQ(((float *)simple_float_arrays)[0], 1.5f);
  EXPECT_EQ(((float *)simple_float_arrays)[9], 5.1f);
  EXPECT_EQ(((float *)simple_float_arrays)[10], 2.5f);
  EXPECT_EQ(((float *)simple_float_arrays)[19], 5.2f);
  free(simple_float_arrays);
}

TEST_F(ExtractionTreeTestFixture, testSetAnyValueSingleWithUdex) {
  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");
  ASSERT_TRUE(publisher.SetDataSourceInfo("testAnyValueWithUdex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("arrays_tree.sdl", true)) << "RegisterDataDescription failed";

  size_t memory_size_local = 12;
  char *test_memory = (char *)malloc(memory_size_local);
  ASSERT_NE(test_memory, nullptr) << "memory allocation failed";
  memset(test_memory, 0, memory_size_local);

  struct_extractor::PackageTreeExtractor extractor("testAnyValueWithUdex.array_root.array_of_structs.first_group[0]");
  extractor.SetMemory(test_memory, memory_size_local);
  ValueUnion myUnion;

  for (auto &signal : extractor.signals_) {
    // set uint64 signal value
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_structs.first_group[0].big_int") {
      myUnion.asUint64 = 42;
      EXPECT_TRUE(signal.value->Set(myUnion));
    }
    // set unsigned short signal value
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_structs.first_group[0].unsigned_short") {
      myUnion.asUint16 = 12;
      EXPECT_TRUE(signal.value->Set(myUnion));
    }
    // set unsigned short signal value at border
    if (signal.name == "testAnyValueWithUdex.array_root.array_of_structs.first_group[0].another_unsigned_short") {
      myUnion.asUint16 = 5;
      EXPECT_TRUE(signal.value->Set(myUnion));
    }
  }

  EXPECT_EQ(((uint64_t *)test_memory)[0], 42) << "signals not correct";
  EXPECT_EQ(((uint16_t *)test_memory)[4], 12) << "signals not correct";
  EXPECT_EQ(((uint16_t *)test_memory)[5], 5) << "signals not correct";

  free(test_memory);
}

class TreeExtractorTest : public struct_extractor::PackageTreeExtractor {
public:
  TreeExtractorTest(std::string url) : struct_extractor::PackageTreeExtractor(url){};
  size_t TestFindString(std::string url) { return FindPackageUrlPosFromFullUrl(url); };
  void RemoveDuplicatesTest(std::vector<ChildInfo> &test) { RemoveDuplicatesAndArrays(test); }
};

TEST_F(ExtractionTreeTestFixture, removeDuplicatesChildInfo_success) {

  auto value = std::make_shared<TreeExtractorTest>("");

  std::vector<ChildInfo> test_childs;
  test_childs.push_back({"name1[0]", 1, 1});
  test_childs.push_back({"name1[1]", 1, 1});
  test_childs.push_back({"name2[0]", 1, 1});
  test_childs.push_back({"name2[1]", 1, 1});
  test_childs.push_back({"name1[1]", 1, 1});
  value->RemoveDuplicatesTest(test_childs);
  ASSERT_EQ(test_childs.size(), 2) << "children not sorted and removed duplicates correctly";
  EXPECT_TRUE(test_childs[0].name != test_childs[1].name) << "children not different. Most probably wrong sorting";
}

TEST_F(ExtractionTreeTestFixture, stringSplitParentUrl_success) {
  TreeExtractorTest extractor("");
  std::string teststring = "device.view.packageurl.asdf.testing";
  size_t start = extractor.TestFindString(teststring);
  size_t expected_start = 12;
  ASSERT_EQ("packageurl.asdf.testing", teststring.substr(expected_start)) << "expected test value wrong";
  EXPECT_EQ(expected_start, start) << "wrong offset computed";
  EXPECT_EQ("packageurl.asdf.testing", teststring.substr(start)) << "wrong offset computed";
}

TEST_F(ExtractionTreeTestFixture, stringSplitReturnNoSplitForShortUrl_success) {
  TreeExtractorTest extractor("");
  std::string teststring = "device.view";
  size_t start = extractor.TestFindString(teststring);
  size_t expected_start = 0;
  ASSERT_EQ("device.view", teststring.substr(expected_start)) << "expected test value wrong";
  EXPECT_EQ(expected_start, start) << "wrong offset computed";
  EXPECT_EQ("device.view", teststring.substr(start)) << "wrong offset computed";
}

} // namespace struct_extractor_tree_test
} // namespace udex
} // namespace next
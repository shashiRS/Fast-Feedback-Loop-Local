#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

#include "struct_info_provider_test.h"

using namespace next::udex::extractor;

class ExtractionStructArrayFactoryTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  ExtractionStructArrayFactoryTestFixture() { this->instance_name_ = "ExtractionStructArrayFactoryTestFixture"; }
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
  SignalType GetType(std::shared_ptr<StructExtractorBasic> obj) { return obj->type_; }
  SignalType GetType(StructExtractorBasic *obj) { return obj->type_; }
  std::string GetUrl(std::shared_ptr<StructExtractorBasic> obj) { return obj->url_; }
};

} // namespace extractor
} // namespace udex
} // namespace next

namespace next {
namespace udex {
namespace struct_extractor_array_factory_test {

class TpObjectArray : public StructExtractor<TpObjectArray> {
public:
  TpObjectArray(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(parent, signal_info_provider_in) {
    vel = new floatValue(this);
    pos = new floatValue(this);
  }
  TpObjectArray(StructExtractorBasic *parent) : StructExtractor(parent) {
    vel = new floatValue(this);
    pos = new floatValue(this);
  }
  ~TpObjectArray() {
    if (vel)
      delete vel;
    if (pos)
      delete pos;
  }
  floatValue *vel = nullptr;
  floatValue *pos = nullptr;

  virtual TpObjectArray Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    bool error = StructExtractorBasic::SetUrls(url);
    error &= vel->SetUrls(".vel");
    error &= pos->SetUrls(".pos");
    return error;
  }
};
class Container : public StructExtractor<Container> {
public:
  Container(std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(emptyRequestBasic, signal_info_provider_in) {}
  virtual Container Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override { return StructExtractorBasic::SetUrls(url); }
};

TEST_F(ExtractionStructArrayFactoryTestFixture, testUpdateUrlAsArray) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto container = new Container(parser);
  container->SetUrls("sur");
  auto tpobject = std::make_shared<TpObjectArray>(container);
  tpobject->SetUrls(".tp");

  next::udex::extractor::StructExtractorArrayFactoryTest factory =
      next::udex::extractor::StructExtractorArrayFactoryTest();
  factory.UpdateUrlAsArrayTest(tpobject, 0);

  StructExtractorBasicTest tester;
  EXPECT_EQ(tester.GetUrl(tpobject), "sur.tp[0]") << "update the url failed";
}

TEST_F(ExtractionStructArrayFactoryTestFixture, testUpdateOffset) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto container = new Container(parser);
  container->SetUrls("sur");
  auto tpobject = std::make_shared<TpObjectArray>(container);
  tpobject->SetUrls(".tp");

  next::udex::extractor::StructExtractorArrayFactoryTest factory =
      next::udex::extractor::StructExtractorArrayFactoryTest();
  factory.UpdateUrlAsArrayTest(tpobject, 0);

  StructExtractorBasicTest tester;
  EXPECT_EQ(tester.GetUrl(tpobject), "sur.tp[0]") << "update the url failed";

  factory.UpdateOffsetTest(tpobject);
  EXPECT_EQ(tester.GetOffset(tpobject), 40) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject), next::udex::SignalType::SIGNAL_TYPE_STRUCT) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->vel), 40) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject->vel), next::udex::SignalType::SIGNAL_TYPE_FLOAT) << "update the offset failed";
}

TEST_F(ExtractionStructArrayFactoryTestFixture, testShiftOffset) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto container = new Container(parser);
  container->SetUrls("sur");
  auto tpobject = std::make_shared<TpObjectArray>(container);
  tpobject->SetUrls(".tp");

  next::udex::extractor::StructExtractorArrayFactoryTest factory =
      next::udex::extractor::StructExtractorArrayFactoryTest();
  factory.UpdateUrlAsArrayTest(tpobject, 1);

  StructExtractorBasicTest tester;
  EXPECT_EQ(tester.GetUrl(tpobject), "sur.tp[1]") << "update the url failed";

  factory.UpdateOffsetTest(tpobject);
  EXPECT_EQ(tester.GetOffset(tpobject), 48) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject), next::udex::SignalType::SIGNAL_TYPE_STRUCT) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->vel), 48) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject->vel), next::udex::SignalType::SIGNAL_TYPE_FLOAT) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->pos), 52) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject->pos), next::udex::SignalType::SIGNAL_TYPE_FLOAT) << "update the offset failed";

  factory.ShiftOffsetTest(tpobject, 10);
  EXPECT_EQ(tester.GetOffset(tpobject), 58) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->vel), 58) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject->vel), next::udex::SignalType::SIGNAL_TYPE_FLOAT) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->pos), 62) << "update the offset failed";
  EXPECT_EQ(tester.GetType(tpobject->pos), next::udex::SignalType::SIGNAL_TYPE_FLOAT) << "update the offset failed";
}

TEST_F(ExtractionStructArrayFactoryTestFixture, testCopyOffsets) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto container = new Container(parser);
  container->SetUrls("sur");

  auto tpobject = std::make_shared<TpObjectArray>(container);
  tpobject->SetUrls(".tp");

  auto tpobject_test = std::make_shared<TpObjectArray>(container);
  tpobject_test->SetUrls(".tp");

  next::udex::extractor::StructExtractorArrayFactoryTest factory =
      next::udex::extractor::StructExtractorArrayFactoryTest();
  factory.UpdateUrlAsArrayTest(tpobject, 1);
  factory.UpdateOffsetTest(tpobject);

  EXPECT_TRUE(factory.CopyOffsetAndTypeTest(tpobject, tpobject_test));

  StructExtractorBasicTest tester;
  EXPECT_EQ(tester.GetOffset(tpobject), tester.GetOffset(tpobject_test)) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->vel), tester.GetOffset(tpobject_test->vel)) << "update the offset failed";
  EXPECT_EQ(tester.GetOffset(tpobject->pos), tester.GetOffset(tpobject_test->pos)) << "update the offset failed";
}

TEST_F(ExtractionStructArrayFactoryTestFixture, testArrayofStruct) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  auto container = new Container(parser);
  container->SetUrls("sur");

  std::vector<std::shared_ptr<TpObjectArray>> tp_objects;
  std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
  for (unsigned int i = 0; i < 4u; i++) {
    tp_objects.push_back(std::make_shared<TpObjectArray>(container));
    tp_objects[i]->SetUrls(".tp");
    cast_vector.push_back(tp_objects[i]);
  }
  next::udex::extractor::StructExtractorArrayFactoryTest factory =
      next::udex::extractor::StructExtractorArrayFactoryTest();
  factory.UpdateUrlInVector(cast_vector);

  StructExtractorBasicTest tester;
  EXPECT_EQ(tester.GetOffset(tp_objects[1]), 48) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_objects[1]->vel), 48) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_objects[1]->pos), 52) << "update the url failed";

  EXPECT_EQ(tester.GetOffset(tp_objects[2]), 56) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_objects[2]->vel), 56) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_objects[2]->pos), 60) << "update the url failed";

  EXPECT_EQ(tester.GetOffset(tp_objects[3]), 64) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_objects[3]->vel), 64) << "update the url failed";
  EXPECT_EQ(tester.GetOffset(tp_objects[3]->pos), 68) << "update the url failed";
}

} // namespace struct_extractor_array_factory_test
} // namespace udex
} // namespace next

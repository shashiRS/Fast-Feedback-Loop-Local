#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <atomic>
#include <fstream>
#include <next/sdk/sdk.hpp>
#include <next/udex/struct_extractor/struct_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>
#include <vector>

#include "struct_info_provider_test.h"

using namespace next::udex::extractor;

class ExtractionStructTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  ExtractionStructTestFixture() { this->instance_name_ = "ExtractionStructTestFixture"; }
};

constexpr float testmemory[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

namespace next {
namespace udex {
namespace struct_extractor_test {

class FailStruct : public StructExtractor<FailStruct> {
public:
  FailStruct(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(parent, signal_info_provider_in) {
    dummy = new floatValue(this);
  }
  ~FailStruct() { delete dummy; }
  floatValue *dummy;

  virtual FailStruct Get() { return *this; }
  virtual bool SetUrls(const std::string &) override {
    if (dummy->SetUrls(".vel") != true) {
      return false;
    }
    return true;
  }
};

class TpObject : public StructExtractor<TpObject> {
public:
  TpObject(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(parent, signal_info_provider_in) {
    vel = std::make_shared<floatValue>(this);
    pos = std::make_shared<floatValue>(this);
  }
  TpObject(StructExtractorBasic *parent) : StructExtractor(parent) {
    vel = std::make_shared<floatValue>(this);
    pos = std::make_shared<floatValue>(this);
  }
  std::shared_ptr<floatValue> vel;
  std::shared_ptr<floatValue> pos;

  virtual TpObject Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    bool error = StructExtractorBasic::SetUrls(url);
    error &= vel->SetUrls(".vel");
    error &= pos->SetUrls(".pos");
    return error;
  }
};

class Surrounding : public StructExtractor<Surrounding> {
public:
  // Construct as a child
  Surrounding(StructExtractorBasic *parent) : StructExtractor(parent) {
    tp1 = std::make_shared<TpObject>(this);
    tp2 = std::make_shared<TpObject>(this);
    number_objects = std::make_shared<floatValue>(this);
  }

  // construct as a root
  Surrounding(std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(emptyRequestBasic, signal_info_provider_in) {
    tp1 = std::make_shared<TpObject>(this);
    tp2 = std::make_shared<TpObject>(this);
    number_objects = std::make_shared<floatValue>(this);
  }
  virtual Surrounding Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    bool error = StructExtractorBasic::SetUrls(url);
    error &= tp2->SetUrls(".tp2");
    error &= tp1->SetUrls(".tp1");
    error &= number_objects->SetUrls(".numobj");
    if (!error) {
      return error;
    }
    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
    for (unsigned int i = 0; i < 4u; i++) {
      tp_objects.push_back(std::make_shared<TpObject>(this));
      error &= tp_objects[i]->SetUrls(".tp");
      if (!error) {
        return error;
      }
      cast_vector.push_back(tp_objects[i]);
    }

    StructExtractorArrayFactory factory = StructExtractorArrayFactory();
    error &= factory.UpdateUrlInVector(cast_vector);
    return error;
  }
  std::shared_ptr<TpObject> tp1;
  std::shared_ptr<TpObject> tp2;
  std::vector<std::shared_ptr<TpObject>> tp_objects;
  std::shared_ptr<floatValue> number_objects;
};

class ContainerOfArrayStruct : public StructExtractor<ContainerOfArrayStruct> {
public:
  ContainerOfArrayStruct(std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(emptyRequestBasic, signal_info_provider_in) {}
  virtual ContainerOfArrayStruct Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    StructExtractorBasic::SetUrls(url);
    bool success = true;

    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
    for (unsigned int i = 0; i < 2u; i++) {
      surroundings.push_back(std::make_shared<Surrounding>(this));
      success &= surroundings[i]->SetUrls(".sur");
      if (!success) {
        return success;
      }
      cast_vector.push_back(surroundings[i]);
    }
    StructExtractorArrayFactory factory = StructExtractorArrayFactory();
    success &= factory.UpdateUrlInVector(cast_vector);
    if (!success) {
      return success;
    }
    return success;
  }

  std::vector<std::shared_ptr<Surrounding>> surroundings;
};

TEST_F(ExtractionStructTestFixture, testBasicSingleStruct) {
  auto parser = std::make_shared<SignalInfoProviderTest>();

  TpObject tp(emptyRequestBasic, parser);
  tp.SetUrls("tp1");
  tp.SetAddress((char *)testmemory, sizeof(testmemory));

  auto test_pos_tp_single = tp.pos->Get();
  EXPECT_EQ(test_pos_tp_single, 0.0f) << "pos failed for single tp";

  auto test_vel_tp_single = tp.vel->Get();
  EXPECT_EQ(test_vel_tp_single, 1.0f) << "velocity failed for single tp";
}

TEST_F(ExtractionStructTestFixture, testBasicSingleStructWithAddedParser) {
  auto parser = std::make_shared<SignalInfoProviderTest>();

  parser->offsetdummy.clear();
  TpObject tp(emptyRequestBasic, parser);
  tp.SetUrls("tp1");
  tp.SetAddress((char *)testmemory, sizeof(testmemory));

  auto test_pos_tp_single = tp.pos->Get();
  EXPECT_EQ(test_pos_tp_single, 0.0f) << "pos failed for single tp";

  auto test_vel_tp_single = tp.vel->Get();
  EXPECT_EQ(test_vel_tp_single, 0.0f) << "velocity failed for single tp";
}
TEST_F(ExtractionStructTestFixture, testFailSetUrls) {
  auto parser = std::make_shared<SignalInfoProviderTest>();

  FailStruct failStruct(emptyRequestBasic, parser);
  EXPECT_FALSE(failStruct.SetUrls("tp1"));
}

TEST_F(ExtractionStructTestFixture, testStructWithDifferentOffset) {
  auto parser = std::make_shared<SignalInfoProviderTest>();
  parser->offsetdummy["tp1.vel"] = 4 * sizeof(float);
  parser->offsetdummy["tp1.pos"] = 5 * sizeof(float);
  TpObject tp(emptyRequestBasic, parser);
  tp.SetUrls("tp1");
  tp.SetAddress((char *)testmemory, sizeof(testmemory));

  auto test_pos_tp_single = tp.pos->Get();
  EXPECT_EQ(test_pos_tp_single, 5.0f) << "pos failed for single tp";

  auto test_vel_tp_single = tp.vel->Get();
  EXPECT_EQ(test_vel_tp_single, 4.0f) << "velocity failed for single tp";
}

TEST_F(ExtractionStructTestFixture, testStructOfStruct) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  Surrounding sur(parser);
  sur.SetAddress((char *)testmemory, sizeof(testmemory));

  sur.SetUrls("sur");

  auto test_pos_tp1_in_sur = sur.tp1->pos->Get();
  EXPECT_EQ(test_pos_tp1_in_sur, 6.0f);
  auto test_vel_tp1_in_sur = sur.tp1->vel->Get();
  EXPECT_EQ(test_vel_tp1_in_sur, 7.0f);

  auto test_vel_tp2_in_sur = sur.tp2->vel->Get();
  EXPECT_EQ(test_vel_tp2_in_sur, 9.0f);
  auto test_pos_tp2_in_sur = sur.tp2->pos->Get();
  EXPECT_EQ(test_pos_tp2_in_sur, 8.0f);

  auto test_num_in_sur = sur.number_objects->Get();
  EXPECT_EQ(test_num_in_sur, 11.0f);

  ASSERT_EQ(sur.tp_objects.size(), 4) << "tp list not set up properly";
  EXPECT_EQ(sur.tp_objects[0]->vel->Get(), 10.0f);
  EXPECT_EQ(sur.tp_objects[2]->vel->Get(), 14.0f);
}

TEST_F(ExtractionStructTestFixture, testArrayOfArrayStruct) {

  auto parser = std::make_shared<SignalInfoProviderTest>();
  ContainerOfArrayStruct sur(parser);
  sur.SetAddress((char *)testmemory, sizeof(testmemory));

  sur.SetUrls("device");

  ASSERT_EQ(sur.surroundings.size(), 2);
  ASSERT_EQ(sur.surroundings[0]->tp_objects.size(), 4);
  ASSERT_EQ(sur.surroundings[1]->tp_objects.size(), 4);

  EXPECT_EQ(sur.surroundings[1]->tp_objects[1]->vel->Get(), 12);
  EXPECT_EQ(sur.surroundings[1]->tp_objects[2]->vel->Get(), 14);
  EXPECT_EQ(sur.surroundings[1]->tp_objects[3]->pos->Get(), 17);
  EXPECT_EQ(sur.surroundings[1]->tp_objects[0]->vel->Get(), 10);
  EXPECT_EQ(sur.surroundings[0]->tp_objects[1]->vel->Get(), 2);
  EXPECT_EQ(sur.surroundings[0]->tp_objects[1]->pos->Get(), 3);
  EXPECT_EQ(sur.surroundings[0]->tp_objects[2]->pos->Get(), 5);
}

} // namespace struct_extractor_test
} // namespace udex
} // namespace next

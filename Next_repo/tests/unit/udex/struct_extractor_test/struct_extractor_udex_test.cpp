#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/struct_extractor/struct_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

using namespace next::udex::extractor;

class ExtractionStructUdexTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  ExtractionStructUdexTestFixture() { this->instance_name_ = "ExtractionStructUdexTestFixture"; }
};

namespace next {
namespace udex {

class VehDynSigHeaderTest : public StructExtractor<VehDynSigHeaderTest> {
public:
  VehDynSigHeaderTest(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(parent, signal_info_provider_in) {
    uiTimeStamp = new uint32Value(this);
    uiMeasurementCounter = new uint16Value(this);
    uiCycleCounter = new uint16Value(this);
    eSigStatus = new uCharValue(this);
  }
  VehDynSigHeaderTest(StructExtractorBasic *parent) : StructExtractor(parent) {
    uiTimeStamp = new uint32Value(this);
    uiMeasurementCounter = new uint16Value(this);
    uiCycleCounter = new uint16Value(this);
    eSigStatus = new uCharValue(this);
  }

  ~VehDynSigHeaderTest() {
    delete uiTimeStamp;
    delete uiMeasurementCounter;
    delete uiCycleCounter;
    delete eSigStatus;
  }

  uint32Value *uiTimeStamp;
  uint16Value *uiMeasurementCounter;
  uint16Value *uiCycleCounter;
  uCharValue *eSigStatus;
  std::vector<std::shared_ptr<uCharValue>> a_reserve;

  virtual VehDynSigHeaderTest Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    bool error = StructExtractorBasic::SetUrls(url);
    error &= uiTimeStamp->SetUrls(".uiTimeStamp");
    error &= uiMeasurementCounter->SetUrls(".uiMeasurementCounter");
    error &= uiCycleCounter->SetUrls(".uiCycleCounter");
    error &= eSigStatus->SetUrls(".eSigStatus");
    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
    for (unsigned int i = 0; i < 3u; i++) {
      a_reserve.push_back(std::make_shared<uCharValue>(this));
      error &= a_reserve[i]->SetUrls(".a_reserve");
      if (!error) {
        return error;
      }
      cast_vector.push_back(a_reserve[i]);
    }

    StructExtractorArrayFactory factory = StructExtractorArrayFactory();
    error &= factory.UpdateUrlInVector(cast_vector);
    return error;
  }
};

class canTp : public StructExtractor<canTp> {
public:
  canTp(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(parent, signal_info_provider_in) {
    eRefPointPos_OBJ_ = new int32Value(this);
    fDistX_OBJ_ = new floatValue(this);
    fDistY_OBJ_ = new floatValue(this);
    fOrientationStd_OBJ_ = new floatValue(this);
    fWidth_OBJ_ = new floatValue(this);
    flength_OBJ_ = new floatValue(this);
    eClassification_OBJ_ = new int32Value(this);
    fVrelX_OBJ_ = new floatValue(this);
    fVrelY_OBJ_ = new floatValue(this);
    uiID_OBJ_ = new floatValue(this);
  }
  canTp(StructExtractorBasic *parent) : StructExtractor(parent) {
    eRefPointPos_OBJ_ = new int32Value(this);
    fDistX_OBJ_ = new floatValue(this);
    fDistY_OBJ_ = new floatValue(this);
    fOrientationStd_OBJ_ = new floatValue(this);
    fWidth_OBJ_ = new floatValue(this);
    flength_OBJ_ = new floatValue(this);
    eClassification_OBJ_ = new int32Value(this);
    fVrelX_OBJ_ = new floatValue(this);
    fVrelY_OBJ_ = new floatValue(this);
    uiID_OBJ_ = new floatValue(this);
  }
  ~canTp() {
    delete eRefPointPos_OBJ_;
    delete fDistX_OBJ_;
    delete fDistY_OBJ_;
    delete fOrientationStd_OBJ_;
    delete fWidth_OBJ_;
    delete flength_OBJ_;
    delete eClassification_OBJ_;
    delete fVrelX_OBJ_;
    delete fVrelY_OBJ_;
    delete uiID_OBJ_;
  }

  int32Value *eRefPointPos_OBJ_;
  floatValue *fDistX_OBJ_;
  floatValue *fDistY_OBJ_;
  floatValue *fOrientationStd_OBJ_;
  floatValue *fWidth_OBJ_;
  floatValue *flength_OBJ_;
  int32Value *eClassification_OBJ_;
  floatValue *fVrelX_OBJ_;
  floatValue *fVrelY_OBJ_;
  floatValue *uiID_OBJ_;

  virtual canTp Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    bool error = StructExtractorBasic::SetUrls(url);
    error &= eRefPointPos_OBJ_->SetUrls(".eRefPointPos_OBJ_");
    error &= fDistX_OBJ_->SetUrls(".fDistX_OBJ_");
    error &= fDistY_OBJ_->SetUrls(".fDistY_OBJ_");
    error &= fOrientationStd_OBJ_->SetUrls(".fOrientationStd_OBJ_");
    error &= fWidth_OBJ_->SetUrls(".fWidth_OBJ_");
    error &= flength_OBJ_->SetUrls(".flength_OBJ_");
    error &= eClassification_OBJ_->SetUrls(".eClassification_OBJ_");
    error &= fVrelX_OBJ_->SetUrls(".fVrelX_OBJ_");
    error &= fVrelY_OBJ_->SetUrls(".fVrelY_OBJ_");
    error &= uiID_OBJ_->SetUrls(".uiID_OBJ_");
    return error;
  }
};

TEST_F(ExtractionStructUdexTestFixture, testSigHeaderStructWithUDex) {
  eCAL::Util::EnableLoopback(true);

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo("testStructWithUDex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("ARS5xx.sdl", true)) << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  auto explorer = next::udex::explorer::SignalExplorer();
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  VehDynSigHeaderTest veh(emptyRequestBasic);

  constexpr int struct_size = 50;
  void *testmemory_basic = malloc(sizeof(char) * struct_size);
  memset(testmemory_basic, 0, struct_size);

  veh.SetUrls("testStructWithUDex.AlgoVehCycle.VehDyn.sSigHeader");
  unsigned long test_uiTimeStamp = 30lu;
  unsigned short test_uiMeasurementCounter = 22u;
  unsigned short test_uiCycleCounter = 15u;
  unsigned char test_eSigStatus = 10;
  unsigned char test_a_reserve[3] = {10, 11, 12};

  constexpr size_t offset_timestamp = 4;
  ((unsigned long *)((char *)testmemory_basic + offset_timestamp))[0] = test_uiTimeStamp;

  constexpr size_t offset_uiMeasurementCounter = 8;
  ((unsigned short *)((char *)testmemory_basic + offset_uiMeasurementCounter))[0] = test_uiMeasurementCounter;

  constexpr size_t offset_uiCycleCounter = 10;
  ((unsigned short *)((char *)testmemory_basic + offset_uiCycleCounter))[0] = test_uiCycleCounter;

  constexpr size_t offset_eSigStatus = 12;
  ((unsigned char *)((char *)testmemory_basic + offset_eSigStatus))[0] = test_eSigStatus;

  constexpr size_t offset_a_reserve_0 = 13;
  ((unsigned char *)((char *)testmemory_basic + offset_a_reserve_0))[0] = test_a_reserve[0];
  ((unsigned char *)((char *)testmemory_basic + offset_a_reserve_0))[1] = test_a_reserve[1];
  ((unsigned char *)((char *)testmemory_basic + offset_a_reserve_0))[2] = test_a_reserve[2];

  veh.SetAddress((char *)testmemory_basic, sizeof(char) * struct_size);

  auto test_uitime_single = veh.uiTimeStamp->Get();
  EXPECT_EQ(test_uitime_single, test_uiTimeStamp) << "uiTimeStamp failed for single tp";

  auto test_mes_counter_single = veh.uiMeasurementCounter->Get();
  EXPECT_EQ(test_mes_counter_single, test_uiMeasurementCounter) << "uiMeasurementCounter failed for single tp";

  auto test_cycle_cunter_single = veh.uiCycleCounter->Get();
  EXPECT_EQ(test_cycle_cunter_single, test_uiCycleCounter) << "uiCycleCounter failed for single tp";

  auto test_esigstatus_single = veh.eSigStatus->Get();
  EXPECT_EQ(test_esigstatus_single, test_eSigStatus) << "eSigStatus failed for single tp";

  ASSERT_EQ(veh.a_reserve.size(), 3u) << "a_reserve not set up properly";

  EXPECT_EQ(veh.a_reserve[0]->Get(), test_a_reserve[0]);
  EXPECT_EQ(veh.a_reserve[1]->Get(), test_a_reserve[1]);
  EXPECT_EQ(veh.a_reserve[2]->Get(), test_a_reserve[2]);
}

class VehDynSigHeaderTestCast : public StructExtractor<VehDynSigHeaderTestCast> {
public:
  VehDynSigHeaderTestCast(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractor(parent, signal_info_provider_in) {
    uiTimeStamp_castToint64 = new int64Value(this);
    uiTimeStamp_castToFloat = new floatValue(this);
    uiTimeStamp_castToChar = new charValue(this);
  }
  VehDynSigHeaderTestCast(StructExtractorBasic *parent) : StructExtractor(parent) {
    uiTimeStamp_castToint64 = new int64Value(this);
    uiTimeStamp_castToFloat = new floatValue(this);
    uiTimeStamp_castToChar = new charValue(this);
  }
  int64Value *uiTimeStamp_castToint64;
  floatValue *uiTimeStamp_castToFloat;
  charValue *uiTimeStamp_castToChar;

  virtual VehDynSigHeaderTestCast Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    bool error = StructExtractorBasic::SetUrls(url);
    error &= uiTimeStamp_castToint64->SetUrls(".uiTimeStamp");
    error &= uiTimeStamp_castToFloat->SetUrls(".uiTimeStamp");
    error &= uiTimeStamp_castToChar->SetUrls(".uiTimeStamp");
    return error;
  }
};

// TODO CHECK ALL TYPES
TEST_F(ExtractionStructUdexTestFixture, TestUDexExtractionWithCast) {
  eCAL::Util::EnableLoopback(true);

  auto publisher = next::udex::publisher::DataPublisher("testStructWithUDex");

  //  need to do this so the SDL is registered under ARS5xx device
  ASSERT_TRUE(publisher.SetDataSourceInfo("testStructWithUDex", 22, 33)) << "SetDataSourceInfo failed";
  ASSERT_TRUE(publisher.RegisterDataDescription("ARS5xx.sdl", true)) << "RegisterDataDescription failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  auto explorer = next::udex::explorer::SignalExplorer();
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  VehDynSigHeaderTestCast veh(emptyRequestBasic);
  void *testmemory_basic = malloc(sizeof(char) * 50);

  veh.SetUrls("testStructWithUDex.AlgoVehCycle.VehDyn.sSigHeader");

  struct test_data {
    unsigned long uiTimeStamp;
    unsigned short uiMeasurementCounter;
    unsigned short uiCycleCounter;
    unsigned char eSigStatus;
    unsigned char a_reserve[3];
  };
  test_data test;
  // Fill test Memory
  test.uiTimeStamp = 30;

  memcpy((char *)(testmemory_basic) + 4, &test, sizeof(test));
  veh.SetAddress((char *)testmemory_basic, sizeof(testmemory_basic));

  auto test_uitime_single = veh.uiTimeStamp_castToint64->Get();
  EXPECT_EQ(test_uitime_single, 30) << "uiTimeStamp cast to larger int64 failed for single tp";
  auto test_uitime_singlechar = veh.uiTimeStamp_castToChar->Get();
  EXPECT_EQ(test_uitime_singlechar, 30) << "uiTimeStamp cast to char failed for single tp";
  auto test_uitime_singlefloat = veh.uiTimeStamp_castToFloat->Get();
  EXPECT_EQ(test_uitime_singlefloat, 30) << "uiTimeStamp cast to float failed for single tp";
}

} // namespace udex
} // namespace next

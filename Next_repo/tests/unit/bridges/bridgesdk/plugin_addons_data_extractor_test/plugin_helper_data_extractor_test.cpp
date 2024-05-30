/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_helper_data_extractor_test.cpp
 *  @brief    Testing the plugin helper data extractor
 */
#include <array>
#include <numeric> //std::iota
#include <utility>
#include <vector>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>

NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4244)

TEST(GetCppTypeFromSignalTypeTest, TestValidSignals) {
  std::vector<std::pair<std::string, next::bridgesdk::plugin_addons::SignalType>> signals_to_test{
      // bool is so far not implemented, why?
      // {typeid(bool).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_BOOL},
      {typeid(char).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR},
      {typeid(unsigned char).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8},
      {typeid(uint8_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8},
      {typeid(signed char).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8},
      {typeid(int8_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8},
      {typeid(unsigned short).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16},
      {typeid(uint16_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16},
      {typeid(short).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16},
      {typeid(int16_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16},
      {typeid(unsigned int).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32},
      {typeid(uint32_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32},
      {typeid(int).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32},
      {typeid(int32_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32},
      {typeid(uint64_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64},
      {typeid(int64_t).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64},
      {typeid(float).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT},
      {typeid(double).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE}};

  if constexpr (sizeof(unsigned long) == 8) {
    signals_to_test.push_back(
        {typeid(unsigned long).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64});
    signals_to_test.push_back({typeid(long).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64});
  } else if constexpr (sizeof(unsigned long long) == 8) {
    signals_to_test.push_back(
        {typeid(unsigned long long).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64});
    signals_to_test.push_back(
        {typeid(long long).name(), next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64});
  }

  for (const auto &test_signal : signals_to_test) {
    EXPECT_EQ(next::bridgesdk::plugin_addons::GetCppTypeFromSignalType(test_signal.second), test_signal.first);
  }
}

TEST(GetCppTypeFromSignalTypeTest, TestInvalidSignals) {
  std::vector<std::pair<std::string, next::bridgesdk::plugin_addons::SignalType>> signals_to_test{
      // bool is so far not implemented, why?
      {"none", next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_BOOL}};
  for (const auto &test_signal : signals_to_test) {
    EXPECT_EQ(next::bridgesdk::plugin_addons::GetCppTypeFromSignalType(test_signal.second), test_signal.first);
  }
}

TEST(simpleTypeConsistencyCheck, TestValidNames) {
// testing here with the valid names
#if defined(_MSC_VER)
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<char>("char"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint8_t>("unsigned char"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int8_t>("signed char"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint16_t>("unsigned short"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int16_t>("short"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint32_t>("unsigned int"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int32_t>("int"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint64_t>("unsigned __int64"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int64_t>("__int64"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<float>("float"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<double>("double"));
#elif defined(__linux__)
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<char>("c"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint8_t>("h"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int8_t>("a"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint16_t>("t"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int16_t>("s"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint32_t>("j"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int32_t>("i"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint64_t>("m"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int64_t>("l"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<float>("f"));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<double>("d"));
#endif
}

TEST(simpleTypeConsistencyCheck, TestInvalidNames) {
  // and now here with invalid names, for simplicity just a few
  // we have to add here negative tests, otherwise simpleTypeConsistencyCheck could always return true
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<char>("unsigned char"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<char>("signed char"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint8_t>("signed char"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint8_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int8_t>("unsigned char"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int8_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint16_t>("signed short"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint16_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int16_t>("long"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int16_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint32_t>("signed int"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint32_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int32_t>("unsigned int"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int32_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint64_t>("__int64"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<uint64_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int64_t>("unsigned __int64"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<int64_t>("float"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<float>("double"));
  EXPECT_FALSE(next::bridgesdk::plugin_addons::simpleTypeConsistencyCheck<double>("float"));
}

TEST(CheckTypeTemplateAndSignalMatching, TestNativeNames) {
  // this function does not work so far in all cases, this is why a lot of them are expected to fail
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<char>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<unsigned char>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<signed char>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<unsigned short>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<short>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<unsigned int>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<int>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<float>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<double>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE));

  if constexpr (sizeof(unsigned long) == 8) {
    EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<unsigned long>(
        next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64));
    EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<long>(
        next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64));
  } else if constexpr (sizeof(unsigned long long) == 8) {
    EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<unsigned long long>(
        next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64));
    EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<long long>(
        next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64));
  }
}

TEST(CheckTypeTemplateAndSignalMatching, TestTypedefs) {
  // this function does not work so far in all cases, this is why a lot of them are expected to fail
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<uint8_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<int8_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<uint16_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<int16_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<uint32_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<int32_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<uint64_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64));
  EXPECT_TRUE(next::bridgesdk::plugin_addons::CheckTypeTemplateAndSignalMatching<int64_t>(
      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64));
}

using BridgeSdkSignalType = next::bridgesdk::plugin_addons::SignalType;
template <typename T>
BridgeSdkSignalType GetSignalName() {
  return BridgeSdkSignalType::SIGNAL_TYPE_BOOL;
}
template <>
BridgeSdkSignalType GetSignalName<char>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_CHAR;
}
template <>
BridgeSdkSignalType GetSignalName<signed char>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_INT8;
}
template <>
BridgeSdkSignalType GetSignalName<unsigned char>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_UINT8;
}
template <>
BridgeSdkSignalType GetSignalName<unsigned short>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_UINT16;
}
template <>
BridgeSdkSignalType GetSignalName<short>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_INT16;
}
template <>
BridgeSdkSignalType GetSignalName<unsigned int>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_UINT32;
}
template <>
BridgeSdkSignalType GetSignalName<int>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_INT32;
}
template <>
BridgeSdkSignalType GetSignalName<unsigned long long>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_UINT64;
}
template <>
BridgeSdkSignalType GetSignalName<long long>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_INT64;
}
template <>
BridgeSdkSignalType GetSignalName<float>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_FLOAT;
}
template <>
BridgeSdkSignalType GetSignalName<double>() {
  return BridgeSdkSignalType::SIGNAL_TYPE_DOUBLE;
}

template <typename T>
class transformValueTest : public ::testing::Test {};
using transformValueTestTypes =
    ::testing::Types<unsigned char, unsigned short, unsigned int, unsigned long long, char, short, int, long long>;
TYPED_TEST_CASE(transformValueTest, transformValueTestTypes);

// in this test we have different arrays (of char, short...) but always want to have it read into a long long
TYPED_TEST(transformValueTest, readingFromDifferentSizedArray) {
  std::array<TypeParam, 4> truth;
  next::bridgesdk::plugin_addons::RawSignalInfo info;

  if (std::is_signed<TypeParam>()) {
    // in case of testing usigned, we have to give also negative numbers
    std::iota(truth.begin(), truth.end(), -21);
  } else {
    std::iota(truth.begin(), truth.end(), 42);
  }

  info.sig_type_ = GetSignalName<TypeParam>();
  info.signal_offset = 0;
  info.payload_size_ = sizeof(truth[0]);
  info.array_length_ = 1; // we are only interested in one value
  for (size_t i = 0; truth.size() > i; ++i) {
    info.signal_offset = i * sizeof(truth[0]);
    unsigned long long result = 0;

    EXPECT_TRUE(next::bridgesdk::plugin_addons::transformValueToTemplateType(
        info, reinterpret_cast<const char *>(truth.data()), result));
    NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4389)
    EXPECT_EQ(result, truth[i]);
    NEXT_RESTORE_WARNINGS_WINDOWS
  }
}

// this time we do it the other way, always reading from byte array, but into differnt data types
TYPED_TEST(transformValueTest, readingIntoDifferentVariableTypes) {
  std::array<signed char, 4> truth;
  next::bridgesdk::plugin_addons::RawSignalInfo info;

  if (std::is_signed<TypeParam>()) {
    // in case of testing usigned, we have to give also negative numbers
    std::iota(truth.begin(), truth.end(), -21);
    info.sig_type_ = BridgeSdkSignalType::SIGNAL_TYPE_INT8;
  } else {
    std::iota(truth.begin(), truth.end(), 42);
    info.sig_type_ = BridgeSdkSignalType::SIGNAL_TYPE_UINT8;
  }

  info.signal_offset = 0;
  info.payload_size_ = sizeof(truth[0]);
  info.array_length_ = 1; // we are only interested in one value
  for (size_t i = 0; truth.size() > i; ++i) {
    info.signal_offset = i * sizeof(truth[0]);
    TypeParam result = 0;

    EXPECT_TRUE(next::bridgesdk::plugin_addons::transformValueToTemplateType(
        info, reinterpret_cast<const char *>(truth.data()), result));
    NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4389)
    EXPECT_EQ(result, truth[i]);
    NEXT_RESTORE_WARNINGS_WINDOWS
  }
}

static constexpr size_t TEST_DATA_ELEMENTS = 4;
// Testing ParseDataFromBinaryBlob (single value variant)
/* Here we have to create one helper class, ParseDataFromBinaryBlobTestHelper.
 * This helper class is used as base class for our two test cases:
 * a) valid types
 * b) invalid types
 *
 * a) used types:   ParseDataFromBinaryBlobValidTypes
 *    test fixture: ParseDataFromBinaryBlobTestValidTypes
 * b) used types:   ParseDataFromBinaryBlobInvalidTypes
 *    test fixture: ParseDataFromBinaryBlobTestInvalidTypes
 */
template <typename Tup>
class ParseDataFromBinaryBlobTestHelper : public ::testing::Test {
public:
  // the type we use as input in our array
  using T1 = typename std::tuple_element_t<0, Tup>;
  // the type we want the data be converted into
  using T2 = typename std::tuple_element_t<1, Tup>;

  // this struct is placed at the beginning of our data structure, just to check that the offset is used correctly
  struct OtherData {
    float f_{0.f};
    long long l_{0};
  };

protected:
  ParseDataFromBinaryBlobTestHelper() {
    std::array<T1, TEST_DATA_ELEMENTS> data;
    std::iota(data.begin(), data.end(), T1(21));
    if (std::is_signed<T1>()) {
      assert(data.size() >= 2);
      std::transform(data.begin(), data.begin() + data.size() / 2, data.begin(),
                     [](const T1 &value) { return (value - T1(42)); });
    }
    // filling the beginning with the dummy data
    *reinterpret_cast<OtherData *>(raw_data_.data()) = OtherData();
    // copy the prepared array behind it
    std::copy_n(reinterpret_cast<const std::byte *>(data.data()), TEST_DATA_ELEMENTS * sizeof(T1),
                raw_data_.begin() + sizeof(OtherData));

    channel_message_pointers_.data = raw_data_.data();
    channel_message_pointers_.channel_name = channel_name_;
    channel_message_pointers_.size = raw_data_.size();
    channel_message_pointers_.time = 0x12345; // just some random time, not needed here

    // we will create here the entries for "channel_name.values[0..n]"
    // these entries are used for the single value variant
    for (size_t i = 0; data.size() > i; ++i) {
      next::bridgesdk::plugin_addons::RawSignalInfo signal_info;
      signal_info.signal_offset = sizeof(OtherData) + i * sizeof(T1);
      signal_info.payload_size_ = sizeof(T1);
      signal_info.sig_type_ = GetSignalName<T1>();
      signal_info.array_length_ = 1;
      signal_list_
          .raw_signal_info[std::string(channel_name_).append("." + field_name_ + "[") + std::to_string(i) + "]"] =
          signal_info;
    }
    // and now we add entries for "channel_name.values"
    // this is used for the vector variant
    next::bridgesdk::plugin_addons::RawSignalInfo signal_info;
    signal_info.signal_offset = sizeof(OtherData);
    signal_info.payload_size_ = sizeof(T1);
    signal_info.sig_type_ = GetSignalName<T1>();
    signal_info.array_length_ = data.size();
    signal_list_.raw_signal_info[std::string(channel_name_).append("." + field_name_)] = signal_info;
  }

  std::array<std::byte, sizeof(OtherData) + TEST_DATA_ELEMENTS * sizeof(T1)> raw_data_;
  T1 const *const data_ptr_ = reinterpret_cast<T1 *>(raw_data_.data() + sizeof(OtherData));
  next::bridgesdk::ChannelMessagePointers channel_message_pointers_;
  next::bridgesdk::plugin_addons::Signals signal_list_;
  const T2 def_ret_value_ = 0x5A;                      // = 90 (base 10), 0101 1010b
  T2 ret_value_ = def_ret_value_;                      // for testing the single value variant
  const std::vector<T2> def_ret_values_{T2(0), T2(1)}; // for testing the vector variant
  std::vector<T2> ret_values_{def_ret_values_};
  const char *channel_name_ = "test_data"; // has to be char*, it is used in ChannelMessagePointers structure
  const std::string field_name_{"values"};
};

template <typename Tup>
class ParseDataFromBinaryBlobTestValidTypes : public ParseDataFromBinaryBlobTestHelper<Tup> {};

template <typename Tup>
class ParseDataFromBinaryBlobTestInvalidTypes : public ParseDataFromBinaryBlobTestHelper<Tup> {};

template <typename Tup>
class ParseDataFromBinaryBlobVecTestValidTypes : public ParseDataFromBinaryBlobTestHelper<Tup> {};

using ParseDataFromBinaryBlobValidTypes =
    // commented out cases are not working, has to be debugged
    ::testing::Types<
        // first the easy cases - input and output type is the same
        std::tuple<char, char>, std::tuple<signed char, signed char>, std::tuple<unsigned char, unsigned char>,
        std::tuple<short, short>, std::tuple<unsigned short, unsigned short>, std::tuple<int, int>,
        std::tuple<unsigned int, unsigned int>, std::tuple<unsigned long long, unsigned long long>,
        std::tuple<long long, long long>, std::tuple<float, float>, std::tuple<double, double>,
        // and now the cases where input and output types are different, still same sign
        // from char
        std::tuple<char, short>, std::tuple<char, int>, std::tuple<char, long long>, std::tuple<char, float>,
        std::tuple<char, double>, std::tuple<unsigned char, unsigned short>, std::tuple<unsigned char, unsigned int>,
        std::tuple<unsigned char, unsigned long long>, std::tuple<unsigned char, float>,
        std::tuple<unsigned char, double>, std::tuple<signed char, signed short>, std::tuple<signed char, signed int>,
        std::tuple<signed char, signed long long>, std::tuple<signed char, float>, std::tuple<signed char, double>,
        // from short
        std::tuple<short, int>, std::tuple<short, long long>, std::tuple<short, float>, std::tuple<short, double>,
        std::tuple<unsigned short, unsigned int>, std::tuple<unsigned short, unsigned long long>,
        std::tuple<unsigned short, float>, std::tuple<unsigned short, double>,
        // from int
        std::tuple<int, long long>, std::tuple<int, float>, std::tuple<int, double>,
        std::tuple<unsigned int, unsigned long long>, std::tuple<unsigned int, float>, std::tuple<unsigned int, double>,
        // from long long (already critical, long long cast to double will lose precision)
        std::tuple<long long, double>, std::tuple<unsigned long long, double>,
        // from float
        std::tuple<float, double>

        // TODO we need some fix for these checks or is it intended?
        // in case of signed <-> unsigned we will lose precision / sign (they are not all listed here)
        // std::tuple<unsigned char, char>
        // std::tuple<unsigned char, signed char>
        // std::tuple<signed char, unsigned char>
        // std::tuple<short, unsigned int>
        // std::tuple<short, unsigned short>
        // std::tuple<unsigned long long, double>
        // but C is doing implicit casts, this is why
        //    char c{-21};
        //    unsigned int i(c); // i will be 4294967275, it is binary(-21)+numeric_limits<unsigned int>::max()
        //    assert(i == 4294967275);
        >;
TYPED_TEST_CASE(ParseDataFromBinaryBlobTestValidTypes, ParseDataFromBinaryBlobValidTypes);

TYPED_TEST(ParseDataFromBinaryBlobTestValidTypes, validData) {
  for (size_t i = 0; TEST_DATA_ELEMENTS > i; ++i) {
    this->ret_value_ = this->def_ret_value_;
    bool success = false;
    std::string signal_name(std::string(this->channel_name_).append(".values[") + std::to_string(i) + "]");
    success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(
        &this->channel_message_pointers_, this->signal_list_, signal_name, this->ret_value_);
    // use assert for success here, the value cannot be correct if the success reports false
    ASSERT_TRUE(success) << "failed to parse data for array position " << i;
    EXPECT_EQ(this->data_ptr_[i], this->ret_value_);
  }
}

TYPED_TEST(ParseDataFromBinaryBlobTestValidTypes, tooSmallSourceDataField) {
  for (size_t i = 0; TEST_DATA_ELEMENTS > i; ++i) {
    this->ret_value_ = this->def_ret_value_;
    bool success = false;
    std::string signal_name(std::string(this->channel_name_).append(".values[") + std::to_string(i) + "]");

    // now introduce the error, our raw data field is too small
    // instead of having sizeof(OtherData) + TEST_DATA_ELEMENTS*sizeof(T1) it is only having size of one value
    // so parsing the data has to fail for all cases
    typename ParseDataFromBinaryBlobTestHelper<TypeParam>::T1 T1;
    this->channel_message_pointers_.size = sizeof(T1);

    success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(
        &this->channel_message_pointers_, this->signal_list_, signal_name, this->ret_value_);
    // use assert for success here, the value cannot be correct if the success reports false
    ASSERT_FALSE(success) << "parsed data on position " << i << " but should have failed";
    EXPECT_EQ(this->def_ret_value_, this->ret_value_);
  }
}

TYPED_TEST(ParseDataFromBinaryBlobTestValidTypes, rawDataNullptr) {
  this->ret_value_ = this->def_ret_value_;
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values[1]"));

  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(nullptr, this->signal_list_, signal_name,
                                                                    this->ret_value_);
  // use assert for success here, the value cannot be correct if the success reports false
  ASSERT_FALSE(success) << "should have failed since nullptr provided";
  EXPECT_EQ(this->def_ret_value_, this->ret_value_);
}

TYPED_TEST(ParseDataFromBinaryBlobTestValidTypes, dataNullptr) {
  this->ret_value_ = this->def_ret_value_;
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values[1]"));

  // setting the data pointer to null
  this->channel_message_pointers_.data = nullptr;

  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(&this->channel_message_pointers_,
                                                                    this->signal_list_, signal_name, this->ret_value_);
  // use assert for success here, the value cannot be correct if the success reports false
  ASSERT_FALSE(success) << "should have failed since nullptr provided";
  EXPECT_EQ(this->def_ret_value_, this->ret_value_);
}

// now the invalid cases
using ParseDataFromBinaryBlobInvalidTypes = ::testing::Types<
    // copying into smaller datatypes will not work
    // into char
    std::tuple<short, char>, std::tuple<int, char>, std::tuple<long long, char>,
    // into short
    std::tuple<int, short>, std::tuple<long long, short>,
    // into int
    std::tuple<long long, int>,
    // into float
    std::tuple<double, float>
    // and now floating point numbers into integers
    // from float/double
    // std::tuple<float, int>, std::tuple<float, long long>
    // std::tuple<double, int>, std::tuple<double, long long>
    // TODO from signed to unsinged / unsigned to signed
    >;
TYPED_TEST_CASE(ParseDataFromBinaryBlobTestInvalidTypes, ParseDataFromBinaryBlobInvalidTypes);

TYPED_TEST(ParseDataFromBinaryBlobTestInvalidTypes, invalidTypes) {
  for (size_t i = 0; TEST_DATA_ELEMENTS > i; ++i) {
    this->ret_value_ = this->def_ret_value_;
    bool success = false;
    std::string signal_name(std::string(this->channel_name_).append(".values[") + std::to_string(i) + "]");
    success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(
        &this->channel_message_pointers_, this->signal_list_, signal_name, this->ret_value_);
    // use EXPECT_FALSE here, the value shall be unchanged
    EXPECT_FALSE(success) << "data was parsed, but shouldn't " << i;
    EXPECT_EQ(this->ret_value_, this->def_ret_value_) << "the value should be unchanged";
  }
}

// testing the vector version of ParseDataFromBinaryBlob function
// reusing the same types as for the single value variant
TYPED_TEST_CASE(ParseDataFromBinaryBlobVecTestValidTypes, ParseDataFromBinaryBlobValidTypes);

TYPED_TEST(ParseDataFromBinaryBlobVecTestValidTypes, validData) {
  this->ret_values_.clear(); // TODO de we have to do this?
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values"));
  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(&this->channel_message_pointers_,
                                                                    this->signal_list_, signal_name, this->ret_values_);
  // use assert for success here, the values cannot be correct if the success reports false
  ASSERT_TRUE(success) << "ParseDataFromBinaryBlob() failed";
  ASSERT_EQ(TEST_DATA_ELEMENTS, this->ret_values_.size());
  // TODO check vector content
  for (size_t i = 0; TEST_DATA_ELEMENTS > i; ++i) {
    EXPECT_EQ(this->data_ptr_[i], this->ret_values_[i]) << "value on position " << i << " does not match";
  }
}

// we will now set the data within the signal information to incorrect values
TYPED_TEST(ParseDataFromBinaryBlobVecTestValidTypes, tooBigSourceArray) {
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values"));

  // now introduce the error, the provided raw data length is less than the array size
  this->signal_list_.raw_signal_info[std::string(this->channel_name_).append(".values")].array_length_ = 1000000;

  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(&this->channel_message_pointers_,
                                                                    this->signal_list_, signal_name, this->ret_values_);
  // use assert for success here, the values cannot be correct if the success reports false
  ASSERT_FALSE(success) << "ParseDataFromBinaryBlob() should have been failing";
  EXPECT_EQ(this->ret_values_, this->def_ret_values_) << "the vector should be unchanged";
}

// we will now set the data within the signal information to incorrect values
TYPED_TEST(ParseDataFromBinaryBlobVecTestValidTypes, tooSmallSourceDataField) {
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values"));

  // now introduce the error, the provided data size is just too small
  typename ParseDataFromBinaryBlobTestHelper<TypeParam>::T1 T1;
  this->channel_message_pointers_.size = sizeof(T1);

  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(&this->channel_message_pointers_,
                                                                    this->signal_list_, signal_name, this->ret_values_);
  // use assert for success here, the values cannot be correct if the success reports false
  ASSERT_FALSE(success) << "ParseDataFromBinaryBlob() should have been failing";
  EXPECT_EQ(this->ret_values_, this->def_ret_values_) << "the vector should be unchanged";
}

// we will now set the data within the signal information to incorrect values
TYPED_TEST(ParseDataFromBinaryBlobVecTestValidTypes, rawDataNullptr) {
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values"));

  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(nullptr, this->signal_list_, signal_name,
                                                                    this->ret_values_);
  // use assert for success here, the values cannot be correct if the success reports false
  ASSERT_FALSE(success) << "ParseDataFromBinaryBlob() should have been failing (nullptr provided)";
  EXPECT_EQ(this->ret_values_, this->def_ret_values_) << "the vector should be unchanged";
}

// we will now set the data within the signal information to incorrect values
TYPED_TEST(ParseDataFromBinaryBlobVecTestValidTypes, dataNullptr) {
  bool success = false;
  std::string signal_name(std::string(this->channel_name_).append(".values"));

  // now introduce the error, the provided data size is just too small
  this->channel_message_pointers_.data = nullptr;

  success = next::bridgesdk::plugin_addons::ParseDataFromBinaryBlob(&this->channel_message_pointers_,
                                                                    this->signal_list_, signal_name, this->ret_values_);
  // use assert for success here, the values cannot be correct if the success reports false
  ASSERT_FALSE(success) << "ParseDataFromBinaryBlob() should have been failing (nullptr provided)";
  EXPECT_EQ(this->ret_values_, this->def_ret_values_) << "the vector should be unchanged";
}

NEXT_RESTORE_WARNINGS_WINDOWS

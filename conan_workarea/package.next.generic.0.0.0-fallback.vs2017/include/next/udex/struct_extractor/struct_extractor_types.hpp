/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     struct_extractor_types.hpp
 * @brief    default types for struct extractor
 */

#ifndef NEXT_UDEX_STRUCT_EXTRACTOR_TYPES_H
#define NEXT_UDEX_STRUCT_EXTRACTOR_TYPES_H

#include <next/udex/data_types.hpp>
#include <next/udex/struct_extractor/struct_extractor.hpp>

#include <next/udex/type_casting.hpp>

namespace next {
namespace udex {

class floatValue : public extractor::StructExtractor<float> {
public:
  floatValue(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<float>(parent) {}
  virtual float Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    float return_value{0.f};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class doubleValue : public extractor::StructExtractor<double> {
public:
  doubleValue(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<double>(parent) {}
  virtual double Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    double return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class uCharValue : public extractor::StructExtractor<unsigned char> {
public:
  uCharValue(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<unsigned char>(parent) {}
  virtual unsigned char Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    unsigned char return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class charValue : public extractor::StructExtractor<char> {
public:
  charValue(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<char>(parent) {}
  virtual char Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    char return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class uint16Value : public extractor::StructExtractor<uint16_t> {
public:
  uint16Value(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<uint16_t>(parent) {}
  virtual uint16_t Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    uint16_t return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};
class int16Value : public extractor::StructExtractor<int16_t> {
public:
  int16Value(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<int16_t>(parent) {}
  virtual int16_t Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    int16_t return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class uint32Value : public extractor::StructExtractor<uint32_t> {
public:
  uint32Value(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<uint32_t>(parent) {}
  virtual uint32_t Get() override {
    size_t max{0};
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    uint32_t return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class int32Value : public extractor::StructExtractor<int32_t> {
public:
  int32Value(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<int32_t>(parent) {}
  virtual int32_t Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    int32_t return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class uint64Value : public extractor::StructExtractor<uint64_t> {
public:
  uint64Value(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<uint64_t>(parent) {}
  virtual uint64_t Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    uint64_t return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

class int64Value : public extractor::StructExtractor<int64_t> {
public:
  int64Value(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<int64_t>(parent) {}
  virtual int64_t Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    int64_t return_value{0};
    casting::castValue(data_type, data_address, max, return_value);
    return return_value;
  }
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }
};

} // namespace udex
} // namespace next

#endif // NEXT_UDEX_STRUCT_EXTRACTOR_TYPES_H

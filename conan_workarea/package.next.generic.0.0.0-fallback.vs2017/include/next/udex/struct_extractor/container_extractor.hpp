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

#ifndef NEXT_UDEX_STRUCT_EXTRACTOR_EXTRACTOR_TREE_H
#define NEXT_UDEX_STRUCT_EXTRACTOR_EXTRACTOR_TREE_H

#include <string>

#include <next/udex/data_types.hpp>
#include <next/udex/struct_extractor/struct_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>

namespace next {
namespace udex {

union ValueUnion {
  bool asBool;
  char asChar;
  uint8_t asUint8;
  int8_t asInt8;
  uint16_t asUint16;
  int16_t asInt16;
  uint32_t asUint32;
  int32_t asInt32;
  uint64_t asUint64;
  int64_t asInt64;
  float asFloat;
  double asDouble;
};

/*! @brief AnyValue holds the original data in its original form. No casting is done.
 *
 * Values can be retrieved via the ValueUnion and the respecitive type can be accessed via GetTypeOfAnyValue
 * Also handles Values which are arrays. For size of Array use ArraySize.
 * If no memory is available or Array request is out of bounds a default 0-value is returned.
 */
class AnyValue : public extractor::StructExtractor<ValueUnion> {
public:
  AnyValue(extractor::StructExtractorBasic *parent) : extractor::StructExtractor<ValueUnion>(parent) {}
  // safe the parent if it's a shared_ptr as we have dynamic containers here.
  AnyValue(std::shared_ptr<extractor::StructExtractorBasic> parent)
      : extractor::StructExtractor<ValueUnion>(parent.get()) {
    // safe reference to ensure shared-ptr lifetime
    parent_save_ = parent;
  }

  //! Get the value based on a union. To access the right value use GetTypeOfAnyValue
  virtual ValueUnion Get() override {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    ValueUnion return_value{};
    return_value.asUint64 = 0; // make sure to set bits to zero
    size_t num_bytes_copy = GetSizeFromSignal(data_type, max);
    if (num_bytes_copy) {
      memcpy(&return_value, data_address, GetSizeFromSignal(data_type, max));
    }
    return return_value;
  }

  //! Get the address of the value(s)
  void *GetAddress(size_t &max_size) { return (void *)GetValueAddress(max_size); }

  //! Set the memory value based on a union.
  virtual bool Set(ValueUnion value) {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    size_t size = GetSizeFromSignal(data_type, max);
    if (size <= max) {
      memcpy(data_address, &value, size);
      return true;
    } else {
      return false;
    }
  }

  //! Access the values if the signal was an array on leaf-level. Use ArraySize for Array information
  ValueUnion Get(size_t index) {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    ValueUnion return_value{};
    return_value.asUint64 = 0; // make sure to set bits to zero
    size_t size_single_signal = GetSizeFromSignal(data_type, max);
    size_t offset = size_single_signal * index;
    if (size_single_signal * (index + 1) <= max) {
      memcpy(&return_value, (char *)data_address + offset, GetSizeFromSignal(data_type, max));
    }
    return return_value;
  }

  //! Set the values based on a union if the signal was an array on leaf-level.
  bool Set(ValueUnion value, size_t index) {
    size_t max;
    auto data_type = GetType();
    void *data_address = (void *)GetValueAddress(max);
    size_t size_single_signal = GetSizeFromSignal(data_type, max);
    size_t offset = size_single_signal * index;
    if (size_single_signal * (index + 1) <= max) {
      memcpy((char *)data_address + offset, &value, GetSizeFromSignal(data_type, max));
      return true;
    } else {
      return false;
    }
  }

  //! Get the array size. If no information is there 0 is returned. Can be used to see if AnyValue is set up properly
  size_t ArraySize() { return StructExtractor::GetArraySize(); }

  //! Get Type. If no information is there STRUCT_TYPE is returned. Can be used to see if AnyValue is set up properly
  SignalType GetTypeOfAnyValue() { return GetType(); }

  //! get the full url of the AnyValue concatenating the urls of all parent containers and the child url.
  const std::string &GetFullUrl() { return StructExtractor::GetFullUrl(); }

  //! Set the url of the leaf. Only use the subpart of the leaf itslef
  //! i.E. ADC5xx.EMF.Timestamp.uiCounter -> to create child only use SetUrls("uiCounter").
  virtual bool SetUrls(const std::string &url) override { return extractor::StructExtractorBasic::SetUrls(url); }

private:
  inline size_t GetSizeFromSignal(const SignalType &type, size_t max) {
    size_t type_size = 0;
    switch (type) {
    case SignalType::SIGNAL_TYPE_CHAR:
      type_size = sizeof(char);
      break;
    case SignalType::SIGNAL_TYPE_UINT8:
      type_size = sizeof(uint8_t);
      break;
    case SignalType::SIGNAL_TYPE_INT8:
      type_size = sizeof(int8_t);
      break;
    case SignalType::SIGNAL_TYPE_UINT16:
      type_size = sizeof(uint16_t);
      break;
    case SignalType::SIGNAL_TYPE_INT16:
      type_size = sizeof(int16_t);
      break;
    case SignalType::SIGNAL_TYPE_UINT32:
      type_size = sizeof(uint32_t);
      break;
    case SignalType::SIGNAL_TYPE_INT32:
      type_size = sizeof(int32_t);
      break;
    case SignalType::SIGNAL_TYPE_UINT64:
      type_size = sizeof(uint64_t);
      break;
    case SignalType::SIGNAL_TYPE_INT64:
      type_size = sizeof(int64_t);
      break;
    case SignalType::SIGNAL_TYPE_FLOAT:
      type_size = sizeof(float);
      break;
    case SignalType::SIGNAL_TYPE_DOUBLE:
      type_size = sizeof(double);
      break;
    default:
      type_size = 0;
    }
    if (type_size > max) {
      return 0;
    } else {
      return type_size;
    }
  }
  std::shared_ptr<extractor::StructExtractorBasic> parent_save_;
};

/*! @brief A Container to group and handle complex tree-like structures automatically
 *
 * The ExtractorContainer has no own Signal but can hold either other ExtractorContainers or AnyValues.
 * Extensively used in the PackageTreeExtractor.
 */
class ExtractorContainer : public extractor::StructExtractor<ExtractorContainer> {
public:
  /*! @brief Child description. Contains either a AnyValue or a ExtractorContainer. Defined by the flag
   * isLeaf. True -> AnyValue, False -> ExtractorContainer
   */
  struct ChildExtractor {
    bool isLeaf;
    std::string name;
    std::shared_ptr<ExtractorContainer> container;
    std::shared_ptr<AnyValue> leaf;
  };

public:
  ExtractorContainer(extractor::StructExtractorBasic *parent) : StructExtractor(parent) {}
  ExtractorContainer(std::shared_ptr<extractor::StructExtractorBasic> parent) : StructExtractor(parent.get()) {
    // this safes a reference to the shared pointer to have a safe deletion
    parent_save_ = parent;
  }

  // Set a specific Child to the Container
  bool SetChild(const ChildExtractor &child) {
    if (child.isLeaf) {
      if (!child.leaf) {
        return false;
      }
      child.leaf->SetUrls(child.name);
    } else {
      if (!child.container) {
        return false;
      }
      child.container->SetUrls(child.name);
    }
    children_.push_back(child);
    return true;
  }
  virtual ExtractorContainer Get() { return *this; }
  virtual bool SetUrls(const std::string &url) override { return StructExtractorBasic::SetUrls(url); }

  //! Get all current children of the container
  std::vector<ChildExtractor> &GetChildren() { return children_; }

private:
  std::vector<ChildExtractor> children_;
  std::shared_ptr<extractor::StructExtractorBasic> parent_save_;
};

} // namespace udex
} // namespace next
#endif // NEXT_UDEX_STRUCT_EXTRACTOR_EXTRACTOR_TREE_H

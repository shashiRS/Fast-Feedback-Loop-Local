
#include <next/udex/struct_extractor/struct_extractor_basic.hpp>

#include <next/sdk/logger/logger.hpp>
#include <next/udex/explorer/signal_explorer.hpp>

#include "struct_info_provider_udex.hpp"

namespace next {
namespace udex {
namespace extractor {

StructExtractorBasic::StructExtractorBasic(StructExtractorBasic *parent) {
  if (parent == nullptr) {
    SetUpAsRoot();
    signal_info_provider_ = std::make_shared<SignalInfoProviderUDex>();
  } else {
    SetUpAsChild(parent);
    signal_info_provider_ = parent_->signal_info_provider_;
  }
}

StructExtractorBasic::StructExtractorBasic(StructExtractorBasic *parent,
                                           std::shared_ptr<ISignalInfoProvider> signal_info_provider_in) {
  if (parent == nullptr) {
    SetUpAsRoot();
  } else {
    SetUpAsChild(parent);
  }
  signal_info_provider_ = signal_info_provider_in;
}

void StructExtractorBasic::SetAddress(const char *root_address, const size_t size) {
  if (is_root_ && (root_address != nullptr)) {
    root_address_->address = (char *)root_address;
    root_address_->size = size;
  } else {
    warn(__FILE__, "error setting address to non root");
  }
}

char *StructExtractorBasic::GetValueAddress(size_t &max_size) {
  if (root_address_ == nullptr || offset_ == UINT64_MAX || root_address_->size == 0 ||
      root_address_->address == nullptr) {
    return GetEmptyMemory(max_size);
  }
  if (signal_info_provider_->deserializationRequired()) {
    return GetDynamicDeserialized(max_size);
  }
  return SimpleFlatMemory(max_size);
}

bool StructExtractorBasic::SetUrls(const std::string &url) {
  array_url_ = url;
  original_url = url;
  if (is_root_) {
    url_ = url;
  } else {
    if (parent_->url_ == "[UNDEFINED]") {
      offset_ = 0;
      return false;
    }
    parent_url_ = parent_->url_;
    url_ = parent_url_ + url;
  }
  next::udex::SignalInfo signal_info = signal_info_provider_->getSignalInfo(url_);
  offset_ = signal_info.offset;
  CopyStructInfo(signal_info.signal_type, signal_info.array_size);
  return true;
}

SignalType StructExtractorBasic::GetType() { return type_; }

size_t StructExtractorBasic::GetOffset() { return offset_; }

size_t StructExtractorBasic::GetArraySize() { return array_length_; }

const std::string &StructExtractorBasic::GetFullUrl() { return url_; }

const std::string &StructExtractorBasic::GetOriginalUrl() { return original_url; }

inline char *StructExtractorBasic::GetEmptyMemory(size_t &max_size) {
  max_size = 0;
  return nullptr;
}

inline char *StructExtractorBasic::SimpleFlatMemory(size_t &max_size) {
  max_size = root_address_->size - offset_;
  return root_address_->address + offset_;
}

inline char *StructExtractorBasic::GetDynamicDeserialized(size_t &max_size) {

  memoryBlock address_deserialized{};
  bool success = signal_info_provider_->getDeserializedMemory(address_deserialized);
  if (!success) {
    return GetEmptyMemory(max_size);
  }

  // was sucessfull so parse for the offset
  size_t offset_deserialized = signal_info_provider_->getOffsetByUrl(url_);
  max_size = root_address_->size - offset_deserialized;

  // offset not valid
  if (max_size < address_deserialized.size) {
    return GetEmptyMemory(max_size);
  }
  // write output
  return address_deserialized.address + offset_deserialized;
}

void StructExtractorBasic::SetUpAsRoot() {
  is_root_ = true;
  memoryBlock mem_block{nullptr, 0};
  root_address_ = std::make_shared<memoryBlock>(mem_block);
}
void StructExtractorBasic::SetUpAsChild(StructExtractorBasic *parent) {
  is_root_ = false;
  root_address_ = parent->root_address_;
  parent->children_.push_back(this);
  parent_ = parent;
  parent_url_ = parent->url_;
  signal_info_provider_ = parent->signal_info_provider_;
}

void StructExtractorBasic::CopyStructInfo(const next::udex::SignalType type, const size_t array_length) {
  this->type_ = type;
  this->array_length_ = array_length;
}

} // namespace extractor
} // namespace udex
} // namespace next

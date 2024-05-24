#ifndef NEXT_UDEX_STRUCT_INFO_PROVIDER_UDEX_H
#define NEXT_UDEX_STRUCT_INFO_PROVIDER_UDEX_H

#include <string>
#include <unordered_map>

#include "next/udex/data_types.hpp"
#include "next/udex/extractor/data_extractor.hpp"
#include "next/udex/struct_extractor/struct_info_provider.hpp"

class SignalInfoProviderUDex : public ISignalInfoProvider {
public:
  SignalInfoProviderUDex() { extractor_ = std::make_shared<next::udex::extractor::DataExtractor>(); };

  next::udex::SignalInfo getSignalInfo(const std::string &url) override { return extractor_->GetInfo(url); }
  size_t getOffsetByUrl(const std::string &url) override {
    auto signal_info = extractor_->GetInfo(url);
    return signal_info.offset;
  }

  bool deserializationRequired() override { return false; }
  bool getDeserializedMemory(memoryBlock &) override { return true; }

private:
  std::shared_ptr<next::udex::extractor::DataExtractor> extractor_;
};

#endif // NEXT_UDEX_STRUCT_INFO_PROVIDER_UDEX_H

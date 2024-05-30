/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     struct_extractor_basic.hpp *
 * @brief    base class for struct extractor to handle backend and low level
 * functinos
 */

#ifndef NEXT_UDEX_STRUCT_EXTRACTOR_BASIC_HPP
#define NEXT_UDEX_STRUCT_EXTRACTOR_BASIC_HPP

#include <next/sdk/sdk_macros.h>

#include <next/udex/udex_config.h>
#include "struct_info_provider.hpp"

namespace next {
namespace udex {
namespace extractor {

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class UDEX_LIBRARY_EXPORT StructExtractorBasic {
public:
  friend class StructExtractorArrayFactory;

protected:
  virtual ~StructExtractorBasic() = default;
  StructExtractorBasic() = delete;

  StructExtractorBasic(StructExtractorBasic *parent);
  StructExtractorBasic(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in);

  void SetAddress(const char *root_address, const size_t size);
  char *GetValueAddress(size_t &max_size);
  SignalType GetType(void);
  bool SetUrls(const std::string &url);
  size_t GetOffset();
  size_t GetArraySize();
  const std::string &GetFullUrl();
  const std::string &GetOriginalUrl();

private:
  inline char *GetEmptyMemory(size_t &max_size);
  inline char *SimpleFlatMemory(size_t &max_size);
  inline char *GetDynamicDeserialized(size_t &max_size);

  void SetUpAsRoot();
  void SetUpAsChild(StructExtractorBasic *parent);

  void CopyStructInfo(const SignalType type, const size_t array_length);

  bool is_root_ = false;

  std::string url_ = "[UNDEFINED]";
  std::string array_url_ = "[UNDEFINED]";
  std::string original_url = "[UNDEFINED]";
  std::string parent_url_ = "[UNDEFINED]";

  std::vector<StructExtractorBasic *> children_ = {};
  StructExtractorBasic *parent_ = nullptr;
  std::shared_ptr<memoryBlock> root_address_ = nullptr;
  SignalType type_;
  size_t array_length_;
  size_t offset_ = UINT64_MAX;

  std::shared_ptr<ISignalInfoProvider> signal_info_provider_ = nullptr;

public:
  friend class StructExtractorBasicTest;
};

constexpr static StructExtractorBasic *emptyRequestBasic = nullptr;

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace extractor
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_STRUCT_EXTRACTOR_BASIC_HPP

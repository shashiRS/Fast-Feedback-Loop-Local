/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     struct_extractor.hpp
 * @brief    class to create fast extraction for cpp-like structs
 */

#ifndef NEXT_UDEX_STRUCT_EXTRACTOR_H
#define NEXT_UDEX_STRUCT_EXTRACTOR_H

#include <string>

#include <next/udex/data_types.hpp>
#include <next/udex/struct_extractor/struct_extractor_basic.hpp>
#include <next/udex/struct_extractor/struct_info_provider.hpp>

namespace next {
namespace udex {
namespace extractor {

/*! @brief StructExtractor is a templated class to allow for fast and pointer-based access to values within NEXT
 *
 * To create a StructExtractor you can either hand in a empty parent (emptyRequestBasic) or chain multiple
 * StructExtractor together to create a c-like struct data structure.
 *
 * Setting up a new StructExtractor with an non-empty parent results in coupling of respective memory used to read out
 * data, as well as the concatination of urls.
 *
 * For examples on how to use it look for "struct_extractor_types.hpp" or any unit tests within
 * "tests/unit/udex/struct_extractor_test"
 *
 * ADVANCED:
 * You can even create your full own parsing of memory slots using a custom ISignalInfoProvider. This provider
 * has the possibility to deserialize the memory upfront and provide offset and types based on an url-schema.
 *
 * Example for such an ISignalInfoProvider can be seen in the unit tests as well
 * */
template <class valueType>
class StructExtractor : public StructExtractorBasic {
public:
  //! default constructor deleted to ensure a parent
  StructExtractor() = delete;

  //! uses the default NEXT-UDex based ISignalInfoProvider
  explicit StructExtractor(StructExtractorBasic *parent) : StructExtractorBasic(parent) {}

  //! allows for a custom ISignalInfoProvider. Does only need to be configured on the root element
  //! (set up with emptyRequestBasic)
  StructExtractor(StructExtractorBasic *parent, std::shared_ptr<ISignalInfoProvider> signal_info_provider_in)
      : StructExtractorBasic(parent, signal_info_provider_in) {}

  virtual ~StructExtractor() = default;

  /*! @brief get the respective declared return value
   *
   * takes case to provide the templated type with help of casting and object parsing
   * (i.E. for a complex struct return struct itself, for basic values like float take care of casting value correct)
   * @return valueType templated type of requestValue (i.E. float, int, struct, tpObject etc)
   */
  virtual valueType Get() = 0;

  /*! call this to set the url
   *
   * Make sure to call the requestBasic::SetUrls first before initializing children!
   *
   * @param url
   */
  virtual bool SetUrls(const std::string &url) = 0;

  /*! Sets up the current memory block to the StructExtractor. Only possible on the root element
   *
   * @param root_address pointer towards the current interpretable memory block
   * @param size size of the block itself. Used for minimalistic error checking
   */
  virtual void SetAddress(const char *root_address, const size_t size) final {
    StructExtractorBasic::SetAddress(root_address, size);
  }
};

} // namespace extractor
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_STRUCT_EXTRACTOR_H

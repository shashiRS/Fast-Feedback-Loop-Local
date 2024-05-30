/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file     struct_extractor_array_factory.h
 * @brief    creates and prepares a vector of struct_extractor to work as an array
 *
 **/

#ifndef NEXT_UDEX_STRUCT_EXTRACTOR_ARRAY_FACTORY_HPP
#define NEXT_UDEX_STRUCT_EXTRACTOR_ARRAY_FACTORY_HPP

#include <next/udex/udex_config.h>
#include "struct_extractor_basic.hpp"

namespace next {
namespace udex {
namespace extractor {

class UDEX_LIBRARY_EXPORT StructExtractorArrayFactory {
public:
  StructExtractorArrayFactory(){};

  /*! Update a vector of simple non-array struct extractors to match an array
   *
   * takes in a plain sturct extractor vector with uninialized struct extractors
   * transforms there url as well as ther offset and type to match
   * propagates offset and type though wihtout requesting the signal information provider all the time
   *
   * @param raw_object_vector
   * @return bool true if successfull
   */
  // use this function to create an actual vector of structs by a single struct set up
  bool UpdateUrlInVector(std::vector<std::shared_ptr<StructExtractorBasic>> &raw_object_vector);

private:
  /*! shifts the offset of the StructExtractoBasic
   *
   * @param obj object to shift
   * @param shift shift in bytes
   */
  void shiftOffset(std::shared_ptr<StructExtractorBasic> obj, size_t shift);
  void shiftOffset(StructExtractorBasic *obj, size_t shift);

  /*! copies over offset and type
   *
   * checks if objects are matching (children are the same etc)
   *
   * @param obj_from object which is original
   * @param obj_to object to copy to
   * @return true if successfull
   */
  bool copyOffsetsAndType(std::shared_ptr<StructExtractorBasic> obj_from, std::shared_ptr<StructExtractorBasic> obj_to);
  bool copyOffsetsAndType(StructExtractorBasic *obj_from, StructExtractorBasic *obj_to);

  /*! Updates the offset if a new url was computed (after updateUrlAsArray for example)
   *
   * @param obj updated object
   */
  void UpdateOffset(std::shared_ptr<StructExtractorBasic> obj);
  void UpdateOffset(StructExtractorBasic *obj);

  /*! updates the object with the respecitve index
   * introduces index to url
   * shifts offset
   * requests new offset
   *
   * @param obj
   * @param index
   */
  void UpdateUrlAsArray(std::shared_ptr<StructExtractorBasic> obj, int index);

  /*! updates the parent url if needed
   *
   * @param obj
   */
  void UpdateParentUrl(StructExtractorBasic *obj);

public:
  friend class StructExtractorArrayFactoryTest;
};

} // namespace extractor
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_STRUCT_EXTRACTOR_ARRAY_FACTORY_HPP

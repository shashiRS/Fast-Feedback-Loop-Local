#ifndef MOCK_COMPONENT_H_
#define MOCK_COMPONENT_H_

#include <iostream>
#include <string>

namespace next {
namespace integration_test {
// Struct of the signal structure of the sdl file
typedef struct {
  signed char const_signed_char[2] = {std::numeric_limits<signed char>::min(), std::numeric_limits<signed char>::max()};
  unsigned char const_unsigned_char[2] = {std::numeric_limits<unsigned char>::min(),
                                          std::numeric_limits<unsigned char>::max()};
  short const_short[2] = {std::numeric_limits<short>::min(), std::numeric_limits<short>::max()};
  unsigned short const_unsigned_short[2] = {std::numeric_limits<unsigned short>::min(),
                                            std::numeric_limits<unsigned short>::max()};
  long const_long[2] = {std::numeric_limits<long>::min(), std::numeric_limits<long>::max()};
  unsigned long const_unsigned_long[2] = {std::numeric_limits<unsigned long>::min(),
                                          std::numeric_limits<unsigned long>::max()};
  int64_t const_int64_t[2] = {std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()};
  uint64_t const_uint64_t[2] = {std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()};
  float const_float[2] = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()};
  double const_double[2] = {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()};
} ConstantVariables;

typedef struct {
  uint64_t counter_uint64_t = 0;
  double counter_double = 1.000000;
} CounterVariables;

typedef struct {
  unsigned long versionNumber = 0;
  ConstantVariables constant_variables;
  CounterVariables counter_variables;
} Group1;

} // namespace integration_test
} // namespace next

#endif

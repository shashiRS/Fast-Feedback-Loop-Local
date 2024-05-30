// COMPANY:   Continental Automotive
// COMPONENT: Eco
#ifndef ECO_HIGH_LOW_FRACTION_TRANSFORM_H_
#define ECO_HIGH_LOW_FRACTION_TRANSFORM_H_

#include "Platform_Types.h"
#include "eco/cpp11_compatibility.h"

namespace eco {
/// This class provides helper functions to combine 2 given 32 bit values to a single 64 bit value and to split a
/// 64 bit value into two 32 bit values.
class HighLowFractionTransform {

public:
  /// This function combines the two given values into a single 64 bit value. The given highPart is shifted into the
  /// upper 32 bits and the given lowPart is set into lower 32 bits of the resulting 64 bit value.
  /// @param[in] highPart The value which shall be put into the upper 32 bits of the result.
  /// @param[in] lowPart The value which shall be put into the lower 32 bits of the result.
  /// @return The combination of both given values as single uint64 number.
  template <typename HIGH, typename LOW>
  static uint64 combine(const HIGH highPart, const LOW lowPart) {
    static_assert(sizeof(highPart) <= sizeof(uint32), "highPart to big for HighLowFractionTransform::combine(...)");
    static_assert(sizeof(lowPart) <= sizeof(uint32), "lowPart to big for HighLowFractionTransform::combine(...)");

    uint64 result = (static_cast<uint64>(highPart) << 32UL) + static_cast<uint64>(lowPart);
    return result;
  }

  /// This function splits the given 64 bit value into two values. The upper 32 bits are used for the highPart result
  /// and the lower 32 bit are used for the lowPart result. The types of highPart and lowPart are defined by the
  /// template types.
  /// @param[out] highPart The resulting highPart number containing the upper 32 bits of the given highLowFraction.
  /// @param[out] lowPart The resulting lowPart number containing the lower 32 bits of the given highLowFraction.
  /// @param[in] highLowFraction The 64 bit value which shall be split into 2 numbers.
  template <typename HIGH, typename LOW>
  static void split(HIGH &highPart, LOW &lowPart, uint64 highLowFraction) {
    static const bool lowPartIsSigned = LOW(-1) < LOW(0);

    if (lowPartIsSigned) {
      lowPart = static_cast<LOW>(static_cast<sint32>(highLowFraction));
    } else {
      lowPart = static_cast<LOW>(static_cast<uint32>(highLowFraction));
    }

    highPart = static_cast<HIGH>(highLowFraction >> 32UL);
    if (lowPart < static_cast<LOW>(0)) {
      highPart += static_cast<HIGH>(1);
    }
  }
};

} // namespace eco

#endif // ECO_HIGH_LOW_FRACTION_TRANSFORM_H_

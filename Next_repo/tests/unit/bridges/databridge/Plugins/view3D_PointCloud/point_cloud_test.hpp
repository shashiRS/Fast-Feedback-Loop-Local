#ifndef NEXT_BRIDGES_POINTCLOUDTEST_HPP
#define NEXT_BRIDGES_POINTCLOUDTEST_HPP
#define _USE_MATH_DEFINES
#include <math.h>

#include <next/sdk/sdk.hpp>

namespace next {
namespace testing {
constexpr float pi = static_cast<float>(M_PI);
constexpr float tolerance = 1e-3f;
} // namespace testing

class PointCloudPluginTest : public sdk::testing::TestUsingEcal {
public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

  PointCloudPluginTest() { this->instance_name_ = "PointCloudPluginTest"; }
};

class PointCloudArrayTest : public sdk::testing::TestUsingEcal {
public:
  PointCloudArrayTest() { this->instance_name_ = "PointCloudArrayTest"; }

  /**
   * @brief Sets the spherical coordinates at the specified index.
   *
   * @param radius The radius value.
   * @param theta The theta angle value in radians.
   * @param phi The phi angle value in radians.
   * @param index The index where the coordinates will be stored.
   */
  void SetSphericalCoordinates(float radius, float theta, float phi, size_t index) {
    constexpr size_t radius_offset{7048};
    float *myRadius = reinterpret_cast<float *>(&dummblock_[radius_offset]);
    myRadius[index] = radius;

    constexpr size_t theta_offset{11272};
    float *myTheta = reinterpret_cast<float *>(&dummblock_[theta_offset]);
    myTheta[index] = theta;

    constexpr size_t phy_offset{15496};
    float *myPhy = reinterpret_cast<float *>(&dummblock_[phy_offset]);
    myPhy[index] = phi;
  }

  char *GetDummBlock() { return dummblock_; }
  size_t GetDummBlockSize() { return dummblock_size_; }

private:
  static constexpr size_t dummblock_size_{100000};
  char dummblock_[dummblock_size_];
};

} // namespace next

#endif // NEXT_BRIDGES_POINTCLOUDTEST_HPP

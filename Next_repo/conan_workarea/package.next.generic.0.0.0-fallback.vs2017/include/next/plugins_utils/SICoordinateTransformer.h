#ifndef NEXT_PLUGINS_UTILS_SI_COORDINATE_TRANSFORMER_H_
#define NEXT_PLUGINS_UTILS_SI_COORDINATE_TRANSFORMER_H_

#include <assert.h>
#include <math.h>
#include <map>
#include <string>

namespace next {
namespace plugins_utils {
namespace parking {
namespace si {

constexpr float LSM_PI{3.141592653589F};     /**< Constant value for pi */
constexpr float LSM_TWO_PI{6.283185307179F}; /**< LSM_PI * 2.0f */

struct Point {
  float x;
  float y;
};

class Position {
private:
  Point mPoint;
  float mAngle;

public:
  Position() : mPoint{0.0F, 0.0F}, mAngle{0.0F} {}
  explicit Position(const Point &point, const float angle) : mPoint{point}, mAngle{angle} {}
  Point point() { return this->mPoint; }
  float &angle() { return this->mAngle; }
  void setAngle(float ang) { this->mAngle = ang; }
  void setPoint(Point new_point) { this->mPoint = new_point; }
};

struct SIComponentInformation {
  Position reset_origin_position;
  uint_fast8_t reset_origin_counter;
  uint_fast8_t origin_counter_internal;
  bool is_si_system_;
};

namespace transformer {

class SITransformer2D final {
public:
  // Constructors area
  SITransformer2D() : position{Position{{0.0F, 0.0F}, 0.0F}}, sinVal{0.0F}, cosVal{0.0F} {}
  explicit SITransformer2D(Position &ref) : position{ref}, sinVal{sinf(ref.angle())}, cosVal{cosf(ref.angle())} {}
  SITransformer2D(float x, float y, float angle) {
    this->position = Position{{x, y}, angle};
    this->cosVal = cosf(angle);
    this->sinVal = sinf(angle);
  }

  // inverse transformation
  Point inverseTransform(Point input) {
    const Point tmp{(input.x - position.point().x), (input.y - position.point().y)};
    const Point res{rotateSinCos(tmp, -this->sinVal, this->cosVal)};
    return res;
  }

  Position inverseTransform(Position input) {
    return Position{inverseTransform(input.point()), radMod(input.angle() - position.angle())};
  }

  // rotation
  Point rotateSinCos(Point input, float sinAng, float cosAng) {
    const float x{(input.x * cosAng) - (input.y * sinAng)};
    const float y{(input.x * sinAng) + (input.y * cosAng)};
    return Point{x, y};
  }

  // update functions
  void setPosition(float x, float y, float angle) {
    const Position newP = Position{{x, y}, angle};
    this->position = newP;
    this->sinVal = sinf(angle);
    this->cosVal = cosf(angle);
  }
  void setPosition(Position newPosition) {
    this->position = newPosition;
    this->sinVal = sinf(newPosition.angle());
    this->cosVal = cosf(newPosition.angle());
  }

  float radMod(const float value_rad) {
    // assert(std::isfinite(value_rad));
    // assert(std::abs(value_rad) < (20.0F * LSM_TWO_PI)); // strong indication that something goes wrong
    float result_rad{value_rad};
    if (std::isfinite(value_rad)) { // only relevant if compiling NDEBUG
      while (result_rad < -LSM_PI) {
        result_rad += LSM_TWO_PI;
      }
      while (result_rad >= LSM_PI) {
        result_rad -= LSM_TWO_PI;
      }
    }
    return result_rad;
  }

private:
  Position position;
  float sinVal;
  float cosVal;
};

} // namespace transformer
} // namespace si
} // namespace parking
} // namespace plugins_utils
} // namespace next

#endif // NEXT_PLUGINS_UTILS_SI_COORDINATE_TRANSFORMER_H_

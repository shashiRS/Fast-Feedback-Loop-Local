#ifndef NEXT_PLUGINS_UTILS_ODOMETRY_TRANSFORMER_H_
#define NEXT_PLUGINS_UTILS_ODOMETRY_TRANSFORMER_H_

#include <math.h>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>

namespace next {
namespace plugins_utils {
namespace parking {

struct Point {
  float x;
  float y;
};

#define M_PI 3.14159265358979323846

namespace odo {
using namespace boost::numeric;

float qRadiansToDegrees(float radians) { return radians * (180 / M_PI); }

class TranslateRotatorOdo {
private:
  float x;
  float y;
  float yaw_angle;
  ublas::matrix<float> trans_rotate_matrix;

public:
  TranslateRotatorOdo() {
    this->x = 0;
    this->y = 0;
    this->yaw_angle = 0;
    this->trans_rotate_matrix = ublas::matrix<float>(4, 4);
  }
  void translate(float x, float y, float yaw_angle) {
    trans_rotate_matrix(0, 0) = cos(yaw_angle);
    trans_rotate_matrix(0, 1) = sin(yaw_angle);
    trans_rotate_matrix(0, 2) = 0.F;
    trans_rotate_matrix(0, 3) = x;

    trans_rotate_matrix(1, 0) = -sin(yaw_angle);
    trans_rotate_matrix(1, 1) = cos(yaw_angle);
    trans_rotate_matrix(1, 2) = 0.F;
    trans_rotate_matrix(1, 3) = -y;

    trans_rotate_matrix(2, 0) = 0.F;
    trans_rotate_matrix(2, 1) = 0.F;
    trans_rotate_matrix(2, 2) = 1.F;
    trans_rotate_matrix(2, 3) = 0.F;

    trans_rotate_matrix(3, 0) = 0.F;
    trans_rotate_matrix(3, 1) = 0.F;
    trans_rotate_matrix(3, 2) = 0.F;
    trans_rotate_matrix(3, 3) = 1.F;
  }
  ublas::matrix<float> rotate(float x, float y) {
    ublas::matrix<float> point(4, 1);
    point(0, 0) = x;
    point(1, 0) = y;
    point(2, 0) = 0.F;
    point(3, 0) = 1.F;

    return ublas::prod(trans_rotate_matrix, point);
  }
};

} // namespace odo
} // namespace parking
} // namespace plugins_utils
} // namespace next

#endif // NEXT_PLUGINS_UTILS_ODOMETRY_TRANSFORMER_H_

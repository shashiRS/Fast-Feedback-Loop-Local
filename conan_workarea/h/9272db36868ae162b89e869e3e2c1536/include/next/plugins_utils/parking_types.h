#ifndef NEXT_PLUGINS_UTILS_PARKING_TYPES_H_
#define NEXT_PLUGINS_UTILS_PARKING_TYPES_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace next {
namespace plugins_utils {
namespace parking {
namespace types {

struct VehicleShapeParams {
  float width;
  float length;
  float height;
  uint8_t AP_V_NUM_STANDARD_SHAPE_PTS; // Number of valid vertices of the standard ego vehicle shape polygon
  std::vector<float>
      AP_V_STANDARD_SHAPE_X_M; // x-Values of the standard ego vehicle shape polygon. Center of rear axle is at origin.
  std::vector<float> AP_V_STANDARD_SHAPE_Y_M;
  uint8_t AP_V_MIRROR_SHAPE_SIZE_NU;
  std::vector<float> AP_V_LEFT_MIRROR_SHAPE_X_M;  // x-values of the left mirror shape
  std::vector<float> AP_V_LEFT_MIRROR_SHAPE_Y_M;  // y-values of the left mirror shape
  std::vector<float> AP_V_RIGHT_MIRROR_SHAPE_X_M; // x-values of the right mirror shape
  std::vector<float> AP_V_RIGHT_MIRROR_SHAPE_Y_M; // y-values of the right mirror shape
  float AP_V_OVERHANG_REAR_M;

  bool ready = false;
};

struct TransformationToOdometry {
  float x;
  float y;
  float yaw;

  bool is_set = false;
};

struct CounterState // TO DO: to find a better name
{
  uint8_t current_counter;
  uint8_t old_counter;
};

struct TrajectoryInfo {
  bool enable_trajectory;
};

enum PoseSelectionStatus : uint8_t {
  PSS_NO_SELECTION = 0U,
  PSS_PLANNER_PRESELECTION = 1U,
  PSS_DRIVER_SELECTION = 2U,
  MAX_NUM_POSE_SEL_STATUS_TYPES = 3U
};

enum PoseReachedStatus : uint8_t {
  NO_TP_REACHED_STATUS = 0U,
  TP_REACHED = 1U,
  TP_NOT_REACHED = 2U,
  MAX_NUM_POSE_REACHED_STATUS_TYPES = 3U
};

enum PoseReachableStatus : uint8_t {
  TP_NOT_VALID = 0U,
  TP_NOT_REACHABLE = 1U,
  TP_FULLY_REACHABLE = 2U,
  TP_SAFE_ZONE_REACHABLE = 3U,
  TP_MANUAL_FWD_REACHABLE = 4U,
  TP_MANUAL_BWD_REACHABLE = 5U,
  MAX_NUM_POSE_REACHABLE_STATUS = 6U
};

struct ParkingSpotInfo {
  std::string color;
  std::string extra_text;
  bool filled;
  float opacity;
};

enum class ParkingScenarioTypes : uint8_t {
  PARALLEL_PARKING = 0U,
  PERPENDICULAR_PARKING = 1U,
  ANGLED_PARKING_OPENING_TOWARDS_BACK = 2U,
  ANGLED_PARKING_OPENING_TOWARDS_FRONT = 3U,
  GARAGE_PARKING = 4U,
  DIRECT_PARKING = 5U,
  EXTERNAL_TAPOS_PARALLEL = 6U,
  EXTERNAL_TAPOS_PERPENDICULAR = 7U,
  EXTERNAL_TAPOS_PARALLEL_OUT = 8U,
  EXTERNAL_TAPOS_PERPENDICULAR_OUT = 9U,
  MAX_NUM_PARKING_SCENARIO_TYPES = 10U
};

enum PCLLineTypes : uint8_t {
  UNKNOW_TYPE = 0U,
  SOLID_LINE = 1U,
  DASHED_LINE = 2U,
  PARTLY_COVERED_LINE = 3U,
  CURBSTONE = 4U,
  PARKING_CAR = 5U,
  GUARDRAIL = 6U,
  WHEEL_STOPPER = 7U
};

struct ParkingCoordinates {
  std::vector<std::vector<std::array<float, 3>>> global_pos;
};

struct EgoVehCoordinate {
  float x;
  float y;
  float yaw;
};

} // namespace types
} // namespace parking
} // namespace plugins_utils
} // namespace next

#endif // NEXT_PLUGINS_UTILS_PARKING_TYPES_H_

#ifndef PLUGIN_VIEW3D_ARRAY_TPOBJECTS_DATA_TYPE_HPP
#define PLUGIN_VIEW3D_ARRAY_TPOBJECTS_DATA_TYPE_HPP

#include <array>
namespace next {
namespace databridge {

struct Fdp21POjectData {
  uint32_t time_object{0lu};
  std::array<uint32_t, 100> id{0lu};
  std::array<uint8_t, 100> padding{0lu};
  std::array<float, 100> position_x{0.F};
  std::array<float, 100> position_y{0.F};
  std::array<float, 100> position_xvar{0.F};
  std::array<float, 100> position_yvar{0.F};
  std::array<float, 100> velocity_x{0.F};
  std::array<float, 100> velocity_y{0.F};
  std::array<float, 100> velocity_xvar{0.F};
  std::array<float, 100> velocity_yvar{0.F};
  std::array<float, 100> accel_x{0.F};
  std::array<float, 100> accel_y{0.F};
  std::array<uint8_t, 100> reference_point{0U};
  std::array<float, 100> box_width{0.F};
  std::array<float, 100> box_length{0.F};
  std::array<float, 100> box_yaw_angle{0.F};
  std::array<float, 100> box_width_var{0.F};
  std::array<float, 100> box_length_var{0.F};
  std::array<float, 100> box_height{0.F};
  std::array<float, 100> box_yaw_angle_var{0.F};
  std::array<uint8_t, 100> camera_catch{0U};
  std::array<uint8_t, 100> front_ctr_catch{0U};
  std::array<uint8_t, 100> front_left_radar_catch{0U};
  std::array<uint8_t, 100> front_right_radar_catch{0U};
  std::array<uint8_t, 100> rear_left_radar_catch{0U};
  std::array<uint8_t, 100> rear_right_radar_catch{0U};
  std::array<uint8_t, 100> predicted_object{0U};
  std::array<uint8_t, 100> classification{0U};
  std::array<uint8_t, 100> turn_indicator{0U};
  std::array<uint8_t, 100> brake_light{0U};
  std::array<uint8_t, 100> lane_assignment{0U};
  std::array<uint8_t, 100> quick_movement{0U};
  std::array<uint8_t, 100> confidence{0U};
  std::array<uint32_t, 100> age{0lu};
};

} // namespace databridge
} // namespace next

#endif // PLUGIN_VIEW3D_ARRAY_TPOBJECTS_DATA_TYPE_HPP

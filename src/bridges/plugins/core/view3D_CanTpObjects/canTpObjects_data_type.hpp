#ifndef ARS_OBJECT_DATA_TYPE_HPP
#define ARS_OBJECT_DATA_TYPE_HPP

#include <iostream>

namespace databridge_ambiguity {

struct Data_Ars {
  Data_Ars() = default;
  ~Data_Ars() = default;

  // field types and members
  uint16_t e2_e_crc16_obj_00_01;
  uint8_t e2_e_seq_counter_obj_00_01;
  uint8_t ui_msg_counter_obj_00_01;
  uint8_t ui_id_obj_00;
  uint8_t ui_split_id_obj_00;
  uint8_t e_maintenance_state_obj_00;
  float f_arel_ystd_obj_00;
  float f_arel_y_obj_00;
  float f_arel_xstd_obj_00;
  float f_arel_x_obj_00;
  float f_vrel_ystd_obj_00;
  float f_vrel_y_obj_00;
  float f_vrel_xstd_obj_00;
  float f_vrel_x_obj_00;
  float f_dist_ystd_obj_00;
  float f_dist_y_obj_00;
  float f_dist_xstd_obj_00;
  float f_dist_x_obj_00;
  float f_orientation_obj_00;
  float f_width_obj_00;
  uint8_t e_fovoverlap_far_obj_00;
  uint8_t ui_dyn_confidence_obj_00;
  uint8_t ui_obstacle_probability_obj_00;
  float f_rcs_obj_00;
  float f_snr_obj_00;
  uint8_t b_ped_micro_doppler_obj_00;
  uint8_t e_classification_obj_00;
  uint8_t ui_class_confidence_obj_00;
  uint8_t ui_probability_of_existence_obj_00;
  float f_orientation_std_obj_00;
  float flength_obj_00;
  uint8_t e_ref_point_pos_obj_00;
  uint8_t e_box_cut_status_obj_00;
  uint8_t ui_life_cycles_obj_00;
  uint8_t e_dynamic_property_obj_00;
  uint8_t ui_id_obj_01;
  uint8_t ui_split_id_obj_01;
  uint8_t e_maintenance_state_obj_01;
  float f_arel_ystd_obj_01;
  float f_arel_y_obj_01;
  float f_arel_xstd_obj_01;
  float f_arel_x_obj_01;
  float f_vrel_ystd_obj_01;
  float f_vrel_y_obj_01;
  float f_vrel_xstd_obj_01;
  float f_vrel_x_obj_01;
  float f_dist_ystd_obj_01;
  float f_dist_y_obj_01;
  float f_dist_xstd_obj_01;
  float f_dist_x_obj_01;
  float f_orientation_obj_01;
  float f_width_obj_01;
  uint8_t e_fovoverlap_far_obj_01;
  uint8_t ui_dyn_confidence_obj_01;
  uint8_t ui_obstacle_probability_obj_01;
  float f_rcs_obj_01;
  float f_snr_obj_01;
  uint8_t b_ped_micro_doppler_obj_01;
  uint8_t e_classification_obj_01;
  uint8_t ui_class_confidence_obj_01;
  uint8_t ui_probability_of_existence_obj_01;
  float f_orientation_std_obj_01;
  float flength_obj_01;
  uint8_t e_ref_point_pos_obj_01;
  uint8_t e_box_cut_status_obj_01;
  uint8_t ui_life_cycles_obj_01;
  uint8_t e_dynamic_property_obj_01;
};

struct ArsObjectDatastructure {
  ArsObjectDatastructure() = default;
  ~ArsObjectDatastructure() = default;
  Data_Ars data{};
};

} // namespace databridge_ambiguity
#endif // ARS_OBJECT_DATA_TYPE_HPP

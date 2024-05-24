#ifndef NEXT_PLUGINS_CAN_TPOBJECTS_STRUCT_HPP_
#define NEXT_PLUGINS_CAN_TPOBJECTS_STRUCT_HPP_

#include <next/plugins_utils/objects/traffic_participant.hpp>
#include <next/udex/struct_extractor/struct_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

namespace next::plugins {

/**
 * @class CanTpObjects
 * @brief Handles Can Tp Objects.
 */
class CanTpObjects : public udex::extractor::StructExtractor<CanTpObjects> {
public:
  std::unordered_map<std::string, std::string> urls_ = {{"ref_tmp_", ".eRefPointPos_OBJ"},
                                                        {"pos_x_", ".fDistX_OBJ"},
                                                        {"pos_y_", ".fDistY_OBJ"},
                                                        {"yaw_", ".fOrientationStd_OBJ"},
                                                        {"width_", ".fWidth_OBJ"},
                                                        {"length_", ".flength_OBJ"},
                                                        {"classification_ars_", ".eClassification_OBJ"},
                                                        {"vel_x_", ".fVrelX_OBJ"},
                                                        {"vel_y_", ".fVrelY_OBJ"},
                                                        {"id_", ".uiID_OBJ"}};
  CanTpObjects(StructExtractorBasic *parent);
  virtual ~CanTpObjects() = default;
  virtual CanTpObjects Get() override;
  CanTpObjectsList GetCanTpOjects() const;
  virtual bool SetUrls(const std::string &url) override;
  void GetIds(const std::string &url, std::string &first_id, std::string &second_id);

private:
  CanTpObjectsList can_tp_objects;

  template <typename member>
  bool SetUrlSafe(std::shared_ptr<member> value, const std::string &key);
  std::unordered_map<std::string, std::string> AddIdToUrls(const std::unordered_map<std::string, std::string> &urls,
                                                           const std::string &id);
};

} // namespace next::plugins

#endif // NEXT_PLUGINS_CAN_TPOBJECTS_STRUCT_HPP_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     ego_vehicle_struct.hpp
 * @brief    defining a ego vehicle object to request data
 *
 **/

#ifndef NEXT_BRIDGES_EGO_VEHICLE_STRUCT_H_
#define NEXT_BRIDGES_EGO_VEHICLE_STRUCT_H_

#include <next/udex/struct_extractor/struct_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

namespace next {
namespace plugins {

class EgoVehicle : public next::udex::extractor::StructExtractor<EgoVehicle> {
public:
  EgoVehicle(StructExtractorBasic *parent) : StructExtractor<EgoVehicle>(parent) {
    ego_x_ = std::make_shared<next::udex::floatValue>(this);
    ego_y_ = std::make_shared<next::udex::floatValue>(this);
    ego_yaw_ = std::make_shared<next::udex::floatValue>(this);
  }

  virtual ~EgoVehicle() {}
  virtual EgoVehicle Get() override { return nullptr; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }
    if (!setUrlSave(ego_x_, "ego_x_position")) {
      return false;
    }
    if (!setUrlSave(ego_y_, "ego_y_position")) {
      return false;
    }
    if (!setUrlSave(ego_yaw_, "ego_yaw_angle")) {
      return false;
    }
    return true;
  }

  std::shared_ptr<next::udex::floatValue> ego_x_;
  std::shared_ptr<next::udex::floatValue> ego_y_;
  std::shared_ptr<next::udex::floatValue> ego_yaw_;

  std::map<std::string, std::string> urls_ = {
      {"ego_x_position", ".xPosition_m"}, {"ego_y_position", ".yPosition_m"}, {"ego_yaw_angle", ".yawAngle_rad"}};

private:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if (auto it = urls_.find(key); urls_.end() != it) {
      return value->SetUrls(urls_[key]);
    }
    return false;
  }
};
} // namespace plugins
} // namespace next

#endif // !NEXT_BRIDGES_EGO_VEHICLE_STRUCT_H_

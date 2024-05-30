/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     ego_vehicle.hpp
 * @brief    defining a ego vehicle object to request data
 *
 **/
#ifndef NEXT_BRIDGES_EGOVEHICLE_H
#define NEXT_BRIDGES_EGOVEHICLE_H

#include <map>

#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

namespace next {
namespace plugins {

class EgoVehicle : public next::udex::extractor::StructExtractor<EgoVehicle> {
public:
  EgoVehicle(StructExtractorBasic *parent) : StructExtractor<EgoVehicle>(parent) {
    pos_x_ = std::make_shared<next::udex::floatValue>(this);
    pos_y_ = std::make_shared<next::udex::floatValue>(this);
    yaw_angle_ = std::make_shared<next::udex::floatValue>(this);

    vel_x_ = std::make_shared<next::udex::floatValue>(this);
    yaw_rate_ = std::make_shared<next::udex::floatValue>(this);

    timestamp_ = std::make_shared<next::udex::uint64Value>(this);
  }

  ~EgoVehicle() = default;

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }
    if (!setUrlSave(pos_x_, "ego_pos_x")) {
      return false;
    }
    if (!setUrlSave(pos_y_, "ego_pos_y")) {
      return false;
    }
    if (!setUrlSave(yaw_angle_, "ego_yaw_angle")) {
      return false;
    }
    if (!setUrlSave(vel_x_, "ego_vel_x")) {
      return false;
    }
    if (!setUrlSave(yaw_rate_, "ego_yaw_rate")) {
      return false;
    }
    if (!setUrlSave(timestamp_, "timestamp")) {
      return false;
    }

    return true;
  }

  virtual EgoVehicle Get() override { return nullptr; }

  std::shared_ptr<next::udex::floatValue> pos_x_;
  std::shared_ptr<next::udex::floatValue> pos_y_;
  std::shared_ptr<next::udex::floatValue> yaw_angle_;

  std::shared_ptr<next::udex::floatValue> vel_x_;
  std::shared_ptr<next::udex::floatValue> yaw_rate_;

  std::shared_ptr<next::udex::uint64Value> timestamp_;

  std::unordered_map<std::string, std::string> urls_ = {
      {"ego_pos_x", ".xPosition_m"},   {"ego_pos_y", ".yPosition_m"},      {"ego_yaw_angle", ".yawAngle_rad"},
      {"ego_vel_x", ".xVelocity_mps"}, {"ego_yaw_rate", ".yawRate_radps"}, {"timestamp", ".timestamp_us"},
  };

  std::mutex ego_vehicle_protector_{};

private:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }
};

} // namespace plugins
} // namespace next

#endif // NEXT_BRIDGES_EGOVEHICLE_H

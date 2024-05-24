/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     traffic_participant.hpp
 * @brief    defining a traffic participant to request data
 *
 **/
#ifndef NEXT_BRIDGES_TRAFFICPARTICIPANT_H
#define NEXT_BRIDGES_TRAFFICPARTICIPANT_H

#include <cmath>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

namespace next {
namespace plugins {

enum class TrafficParticipantType { UNDEFINED, CAN, STRUCT };

class TrafficObject : public next::udex::extractor::StructExtractor<TrafficObject> {
public:
  TrafficObject(StructExtractorBasic *parent, TrafficParticipantType tp_type) : StructExtractor<TrafficObject>(parent) {
    tp_type_ = tp_type;
    pos_x_ = std::make_shared<next::udex::floatValue>(this);
    pos_y_ = std::make_shared<next::udex::floatValue>(this);
    vel_x_ = std::make_shared<next::udex::floatValue>(this);
    vel_y_ = std::make_shared<next::udex::floatValue>(this);
    yaw_ = std::make_shared<next::udex::floatValue>(this);

    classification_ = std::make_shared<next::udex::uCharValue>(this);
    life_time_ = std::make_shared<next::udex::floatValue>(this);
    shapepoint_0_pos_x_ = std::make_shared<next::udex::floatValue>(this);
    shapepoint_0_pos_y_ = std::make_shared<next::udex::floatValue>(this);
    shapepoint_1_pos_x_ = std::make_shared<next::udex::floatValue>(this);
    shapepoint_1_pos_y_ = std::make_shared<next::udex::floatValue>(this);
    shapepoint_2_pos_x_ = std::make_shared<next::udex::floatValue>(this);
    shapepoint_2_pos_y_ = std::make_shared<next::udex::floatValue>(this);

    id_ = std::make_shared<next::udex::int32Value>(this);
    ref_tmp_ = std::make_shared<next::udex::int32Value>(this);
    width_ = std::make_shared<udex::floatValue>(this);
    length_ = std::make_shared<udex::floatValue>(this);
    classification_ars_ = std::make_shared<next::udex::int32Value>(this);
  }

  TrafficObject(StructExtractorBasic *parent, TrafficParticipantType tp_type,
                std::unordered_map<std::string, std::string> &urls)
      : TrafficObject(parent, tp_type) {
    urls_.clear();
    urls_ = urls;
  }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!SetCommonUrls()) {
      return false;
    }

    if (tp_type_ == TrafficParticipantType::STRUCT) {
      if (!SetStrucTpUrls()) {
        return false;
      }
    } else if (tp_type_ == TrafficParticipantType::CAN) {
      if (!SetCanTpUrls()) {
        return false;
      }
    }

    return true;
  }

  virtual TrafficObject Get() override { return *this; }

  float GetLength() const {
    if (tp_type_ == TrafficParticipantType::STRUCT) {
      float length_x = shapepoint_1_pos_x_->Get() - shapepoint_2_pos_x_->Get();
      float length_y = shapepoint_1_pos_y_->Get() - shapepoint_2_pos_y_->Get();

      return std::sqrt(length_x * length_x + length_y * length_y);
    } else if (tp_type_ == TrafficParticipantType::CAN) {
      return length_->Get();
    }

    return 0.0f;
  }

  float GetWidth() const {
    if (tp_type_ == TrafficParticipantType::STRUCT) {
      float width_x = shapepoint_0_pos_x_->Get() - shapepoint_1_pos_x_->Get();
      float width_y = shapepoint_0_pos_y_->Get() - shapepoint_1_pos_y_->Get();
      return std::sqrt(width_x * width_x + width_y * width_y);
    } else if (tp_type_ == TrafficParticipantType::CAN) {
      return width_->Get();
    }

    return 0.0f;
  }

  int32_t GetRefTmp() const { return ref_tmp_->Get(); }
  float GetPosX() const { return pos_x_->Get(); }
  float GetPosY() const { return pos_y_->Get(); }
  float GetYaw() const { return yaw_->Get(); }
  unsigned char GetClassification() const { return classification_->Get(); }
  int32_t GetClassificationArs() const { return classification_ars_->Get(); }
  float GetVelX() const { return vel_x_->Get(); }
  float GetVelY() const { return vel_y_->Get(); }
  int32_t GetId() const { return id_->Get(); }
  float GetLifeTime() const { return life_time_->Get(); }
  float GetShape0X() const { return shapepoint_0_pos_x_->Get(); }
  float GetShape0Y() const { return shapepoint_0_pos_y_->Get(); }
  float GetShape1X() const { return shapepoint_1_pos_x_->Get(); }
  float GetShape1Y() const { return shapepoint_1_pos_y_->Get(); }

  TrafficParticipantType tp_type_;

  std::unordered_map<std::string, std::string> urls_ = {
      {"pos_x_", ".kinematic.fDistX"},
      {"pos_y_", ".kinematic.fDistY"},
      {"life_time_", ".general.fLifeTime"},
      {"vel_x_", ".kinematic.fVabsX"},
      {"vel_y_", ".kinematic.fVabsY"},
      {"yaw_", ".addKinematic.fYaw"},
      {"classification_", ".classification"},
      {"shapepoint_0_pos_x_", ".shapePoints.aShapePointCoordinates[0].fPosX"},
      {"shapepoint_0_pos_y_", ".shapePoints.aShapePointCoordinates[0].fPosY"},
      {"shapepoint_1_pos_x_", ".shapePoints.aShapePointCoordinates[1].fPosX"},
      {"shapepoint_1_pos_y_", ".shapePoints.aShapePointCoordinates[1].fPosY"},
      {"shapepoint_2_pos_x_", ".shapePoints.aShapePointCoordinates[2].fPosX"},
      {"shapepoint_2_pos_y_", ".shapePoints.aShapePointCoordinates[2].fPosY"}};

private:
  // Common
  std::shared_ptr<next::udex::floatValue> pos_x_;
  std::shared_ptr<next::udex::floatValue> pos_y_;
  std::shared_ptr<next::udex::floatValue> vel_x_;
  std::shared_ptr<next::udex::floatValue> vel_y_;
  std::shared_ptr<next::udex::floatValue> yaw_;
  std::shared_ptr<next::udex::floatValue> width_;
  std::shared_ptr<next::udex::floatValue> length_;

  // Struct
  std::shared_ptr<next::udex::uCharValue> classification_;
  std::shared_ptr<next::udex::floatValue> life_time_;
  std::shared_ptr<next::udex::floatValue> shapepoint_0_pos_x_;
  std::shared_ptr<next::udex::floatValue> shapepoint_0_pos_y_;
  std::shared_ptr<next::udex::floatValue> shapepoint_1_pos_x_;
  std::shared_ptr<next::udex::floatValue> shapepoint_1_pos_y_;
  std::shared_ptr<next::udex::floatValue> shapepoint_2_pos_x_;
  std::shared_ptr<next::udex::floatValue> shapepoint_2_pos_y_;

  // Can
  std::shared_ptr<udex::int32Value> ref_tmp_;
  std::shared_ptr<next::udex::int32Value> classification_ars_;
  std::shared_ptr<udex::int32Value> id_;

  bool SetCommonUrls() {
    if (!SetUrlSafe(pos_x_, "pos_x_")) {
      return false;
    }

    if (!SetUrlSafe(pos_y_, "pos_y_")) {
      return false;
    }

    if (!SetUrlSafe(vel_x_, "vel_x_")) {
      return false;
    }

    if (!SetUrlSafe(vel_y_, "vel_y_")) {
      return false;
    }

    if (!SetUrlSafe(yaw_, "yaw_")) {
      return false;
    }

    return true;
  }

  bool SetStrucTpUrls() {
    if (!SetUrlSafe(classification_, "classification_")) {
      return false;
    }
    if (!SetUrlSafe(life_time_, "life_time_")) {
      return false;
    }
    if (!SetUrlSafe(shapepoint_0_pos_x_, "shapepoint_0_pos_x_")) {
      return false;
    }
    if (!SetUrlSafe(shapepoint_0_pos_y_, "shapepoint_0_pos_y_")) {
      return false;
    }
    if (!SetUrlSafe(shapepoint_1_pos_x_, "shapepoint_1_pos_x_")) {
      return false;
    }
    if (!SetUrlSafe(shapepoint_1_pos_y_, "shapepoint_1_pos_y_")) {
      return false;
    }
    if (!SetUrlSafe(shapepoint_2_pos_x_, "shapepoint_2_pos_x_")) {
      return false;
    }
    if (!SetUrlSafe(shapepoint_2_pos_y_, "shapepoint_2_pos_y_")) {
      return false;
    }

    return true;
  }

  bool SetCanTpUrls() {
    if (!SetUrlSafe(id_, "id_")) {
      return false;
    }
    if (!SetUrlSafe(ref_tmp_, "ref_tmp_")) {
      return false;
    }
    if (!SetUrlSafe(width_, "width_")) {
      return false;
    }
    if (!SetUrlSafe(length_, "length_")) {
      return false;
    }
    if (!SetUrlSafe(classification_ars_, "classification_ars_")) {
      return false;
    }

    return true;
  }

  template <typename member>
  inline bool SetUrlSafe(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }

    return true;
  }
};

class TrafficList : public next::udex::extractor::StructExtractor<TrafficList> {
public:
  TrafficList(StructExtractorBasic *parent) : StructExtractor(parent) {
    num_objects_ = std::make_shared<next::udex::int32Value>(this);
  }
  TrafficList(StructExtractorBasic *parent, uint32_t max_size_list) : TrafficList(parent) {
    max_size_list_ = max_size_list;
  }
  virtual ~TrafficList() = default;

  virtual TrafficList Get() override { return nullptr; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }
    if (!SetUrlSafe(num_objects_, "num_objects_")) {
      return false;
    }

    if (urls_.find("aObject_") == urls_.end()) {
      return false;
    }

    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
    for (unsigned int i = 0; i < max_size_list_; i++) {
      auto tp_object_general = (std::make_shared<TrafficObject>(this, TrafficParticipantType::STRUCT, urls_));
      tp_object_general->SetUrls(urls_["aObject_"]);
      tp_objects.push_back(tp_object_general);
      cast_vector.push_back(tp_objects[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory = next::udex::extractor::StructExtractorArrayFactory();
    factory.UpdateUrlInVector(cast_vector);

    return true;
  }

  uint32_t GetMaxSizeTrafficList() { return max_size_list_; }

  std::shared_ptr<next::udex::int32Value> num_objects_;
  std::vector<std::shared_ptr<TrafficObject>> tp_objects;
  std::unordered_map<std::string, std::string> urls_ = {{"num_objects_", ".HeaderObjList.iNumOfUsedObjects"},
                                                        {"aObject_", ".aObject"}};

private:
  uint32_t max_size_list_{0};

  template <typename member>
  inline bool SetUrlSafe(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }
};

struct CanTpObject {
  std::shared_ptr<TrafficObject> object_1;
  std::shared_ptr<TrafficObject> object_2;
};

using CanTpObjectsList = std::unordered_map<std::string, CanTpObject>;

} // namespace plugins
} // namespace next

#endif // NEXT_BRIDGES_TRAFFICPARTICIPANT_H

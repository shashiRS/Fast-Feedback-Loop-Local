#pragma once
/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     tp_objects.hpp
 * @brief    defining a array traffic object to request data
 *
 **/
#ifndef NEXT_BRIDGES_ARRAYTPOBJECT_H
#define NEXT_BRIDGES_ARRAYTPOBJECT_H

#include <map>
#include <memory>
#include <vector>

#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>
#include "url_mapper.h"

namespace next {
namespace plugins {

class ArrayTpList : public next::udex::extractor::StructExtractor<ArrayTpList> {
public:
  ArrayTpList(StructExtractorBasic *parent) : StructExtractor<ArrayTpList>(parent) {}
  ArrayTpList(StructExtractorBasic *parent, uint32_t max_arraytp_size)
      : StructExtractor<ArrayTpList>(parent), max_array_size_(max_arraytp_size) {

    for (auto const &key : urls_) {
      urls_keys_.push_back(key.first);
    }
  }
  virtual ~ArrayTpList() = default;

  virtual ArrayTpList Get() override { return nullptr; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    for (const auto &key : urls_keys_) {
      if (urls_.find(key) == urls_.end()) {
        return false;
      }
    }

    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_id_.push_back(std::make_shared<next::udex::uint32Value>(this));
      list_id_[i]->SetUrls(urls_.at("id_"));
      cast_vector.push_back(list_id_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_id =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_id.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_reference_point_.push_back(std::make_shared<next::udex::uCharValue>(this));
      list_reference_point_[i]->SetUrls(urls_.at("reference_point_"));
      cast_vector.push_back(list_reference_point_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_referencePoint =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_referencePoint.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_position_x_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_position_x_[i]->SetUrls(urls_.at("position_x_"));
      cast_vector.push_back(list_position_x_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_positionX =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_positionX.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_position_y_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_position_y_[i]->SetUrls(urls_.at("position_y_"));
      cast_vector.push_back(list_position_y_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_positionY =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_positionY.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_bow_yaw_angle_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_bow_yaw_angle_[i]->SetUrls(urls_.at("bow_yaw_angle_"));
      cast_vector.push_back(list_bow_yaw_angle_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_boxYawAngle =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_boxYawAngle.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_box_width_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_box_width_[i]->SetUrls(urls_.at("box_width_"));
      cast_vector.push_back(list_box_width_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_boxWidth =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_boxWidth.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_box_length_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_box_length_[i]->SetUrls(urls_.at("box_length_"));
      cast_vector.push_back(list_box_length_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_boxLength =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_boxLength.UpdateUrlInVector(cast_vector);

    if (urls_.find("classification_") == urls_.end()) {
      return false;
    }
    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_classification_.push_back(std::make_shared<next::udex::uCharValue>(this));
      list_classification_[i]->SetUrls(urls_.at("classification_"));
      cast_vector.push_back(list_classification_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_classification =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_classification.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_velocity_x_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_velocity_x_[i]->SetUrls(urls_.at("velocity_x_"));
      cast_vector.push_back(list_velocity_x_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_velocityX =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_velocityX.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < max_array_size_; i++) {
      list_velocity_y_.push_back(std::make_shared<next::udex::floatValue>(this));
      list_velocity_y_[i]->SetUrls(urls_.at("velocity_y_"));
      cast_vector.push_back(list_velocity_y_[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_velocityY =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_velocityY.UpdateUrlInVector(cast_vector);

    return true;
  }
  std::mutex tp_protector_{};
  std::vector<std::shared_ptr<next::udex::uint32Value>> list_id_;
  std::vector<std::shared_ptr<next::udex::uCharValue>> list_reference_point_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_position_x_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_position_y_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_bow_yaw_angle_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_box_width_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_box_length_;
  std::vector<std::shared_ptr<next::udex::uCharValue>> list_classification_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_velocity_x_;
  std::vector<std::shared_ptr<next::udex::floatValue>> list_velocity_y_;

  std::unordered_map<std::string, std::string> urls_{{"id_", ".id"},
                                                     {"reference_point_", ".referencePoint"},
                                                     {"position_x_", ".positionX"},
                                                     {"position_y_", ".positionY"},
                                                     {"bow_yaw_angle_", ".boxYawAngle"},
                                                     {"box_width_", ".boxWidth"},
                                                     {"box_length_", ".boxLength"},
                                                     {"classification_", ".classification"},
                                                     {"velocity_x_", ".velocityX"},
                                                     {"velocity_y_", ".velocityY"}};

private:
  const uint32_t max_array_size_{0};
  std::vector<std::string> urls_keys_;
};

} // namespace plugins
} // namespace next

#endif // NEXT_BRIDGES_ARRAYTPOBJECT_H

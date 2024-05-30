/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     pointCloudStruct.hpp
 * @brief    defining a point cloud object to request data
 *
 **/
#ifndef NEXT_BRIDGES_POINTCLOUDSTRUCT_H
#define NEXT_BRIDGES_POINTCLOUDSTRUCT_H

#include <cmath>

#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/struct_extractor/package_tree_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor.hpp>
#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

namespace next {
namespace plugins {

struct CloudPoint : public next::udex::extractor::StructExtractor<CloudPoint> {
public:
  CloudPoint(StructExtractorBasic *parent) : next::udex::extractor::StructExtractor<CloudPoint>(parent) {
    pos_x_ = std::make_shared<next::udex::floatValue>(this);
    pos_y_ = std::make_shared<next::udex::floatValue>(this);
    pos_z_ = std::make_shared<next::udex::floatValue>(this);
  }

  virtual ~CloudPoint() {}

  virtual CloudPoint Get() override { return *this; }
  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }
    if (!setUrlSave(pos_x_, "xpos")) {
      return false;
    }
    if (!setUrlSave(pos_y_, "ypos")) {
      return false;
    }
    if (!setUrlSave(pos_z_, "zpos")) {
      return false;
    }
    return true;
  }
  float GetPosX() { return pos_x_->Get(); }
  float GetPosY() { return pos_y_->Get(); }
  float GetPosZ() { return pos_z_->Get(); }

  std::map<std::string, std::string> urls_ = {{"xpos", "xPosition_m"}, {"ypos", "yPosition_m"}, {"zpos", ""}};

private:
  std::shared_ptr<next::udex::floatValue> pos_x_;
  std::shared_ptr<next::udex::floatValue> pos_y_;
  std::shared_ptr<next::udex::floatValue> pos_z_;

  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if (auto it = urls_.find(key); urls_.end() != it) {
      return value->SetUrls(urls_[key]);
    }
    return false;
  }
};

/**
 * @class CloudPointAsArray
 * @brief Handles arrays of cloud points in a spherical coordinate system.
 */
class CloudPointAsArray : public next::udex::extractor::StructExtractor<CloudPointAsArray> {
public:
  CloudPointAsArray(StructExtractorBasic *parent) : next::udex::extractor::StructExtractor<CloudPointAsArray>(parent) {
    radius_ = std::make_shared<next::udex::AnyValue>(this);
    theta_ = std::make_shared<next::udex::AnyValue>(this);
    phi_ = std::make_shared<next::udex::AnyValue>(this);
    extractor_ = std::make_shared<next::udex::extractor::DataExtractor>();
  }
  virtual ~CloudPointAsArray() {}
  virtual CloudPointAsArray Get() override { return *this; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!GetNumberOfPoints(num_points_)) {
      return false;
    }

    SetUrlSafe(radius_, "radius_");
    SetUrlSafe(theta_, "theta_");
    SetUrlSafe(phi_, "phi_");

    return true;
  }

  std::map<std::string, std::string> urls_ = {
      {"xpos", "xPosition_m"},   {"ypos", "yPosition_m"}, {"zpos", ""},
      {"radius_", "a_RangeRad"}, {"phi_", "a_ElevAng"},   {"theta_", "a_AzAng"}};
  size_t num_points_ = 0;

  std::shared_ptr<next::udex::AnyValue> radius_; /**< The radius of the sphere. */
  std::shared_ptr<next::udex::AnyValue> theta_;  /**< The azimuthal angle. */
  std::shared_ptr<next::udex::AnyValue> phi_;    /**< The polar angle. */

  float GetPosX(size_t index) {
    if (index > num_points_ || num_points_ == 0) {
      return 0;
    }
    return radius_->Get(index).asFloat * std::sin(phi_->Get(index).asFloat) * std::cos(theta_->Get(index).asFloat);
  }

  float GetPosY(size_t index) {
    if (index > num_points_ || num_points_ == 0) {
      return 0;
    }
    return radius_->Get(index).asFloat * std::sin(phi_->Get(index).asFloat) * std::sin(theta_->Get(index).asFloat);
  }

  float GetPosZ(size_t index) {
    if (index > num_points_ || num_points_ == 0) {
      return 0;
    }
    return radius_->Get(index).asFloat * std::cos(phi_->Get(index).asFloat);
  }

private:
  std::shared_ptr<udex::extractor::DataExtractor> extractor_;

  template <typename member>
  inline bool SetUrlSafe(member &value, const std::string &key) {
    if (auto it = urls_.find(key); urls_.end() != it) {
      return value->SetUrls(urls_[key]);
    }
    return false;
  }

  bool GetNumberOfPoints(size_t &num_points) {
    if (auto it = urls_.find("radius_"); urls_.end() != it) {
      auto signal_info = extractor_->GetInfo(StructExtractorBasic::GetFullUrl() + urls_["radius_"]);
      num_points = signal_info.array_size;
      return true;
    }
    return false;
  }
};

struct Cloud : public next::udex::extractor::StructExtractor<Cloud> {

public:
  Cloud(StructExtractorBasic *parent) : next::udex::extractor::StructExtractor<Cloud>(parent) {
    number_of_points_ = std::make_shared<next::udex::uint32Value>(this);
  }
  virtual ~Cloud() {}

  virtual Cloud Get() override { return nullptr; }
  bool SetNumberOfPoints(size_t num_objects) {
    num_objects_ = num_objects;
    return true;
  }
  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!array_of_struct_) {

      points_as_array_ = std::make_shared<CloudPointAsArray>(this);
      points_as_array_->urls_ = urls_;

      if (auto it = urls_.find("arrays"); it != urls_.end()) {
        points_as_array_->SetUrls(urls_["arrays"]);
      }
    } else {
      if (!setUrlSave(number_of_points_, "num_points")) {
        return false;
      }
      std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
      for (size_t i = 0; i < num_objects_; i++) {
        points_.push_back(std::make_shared<CloudPoint>(this));
        points_[i]->urls_.clear();
        points_[i]->urls_ = urls_;
        points_[i]->SetUrls(urls_["point_"]);
        cast_vector.push_back(points_[i]);
      }
      next::udex::extractor::StructExtractorArrayFactory factory = next::udex::extractor::StructExtractorArrayFactory();
      factory.UpdateUrlInVector(cast_vector);
    }
    info(__FILE__, "Finished setting up urls for PointCloud url: {0}", url);
    return true;
  }

  virtual float GetPosX(size_t index) {
    if (!array_of_struct_) {
      return points_as_array_->GetPosX(index);
    } else {
      if (index > points_.size()) {
        return 0;
      }
      return points_[index]->GetPosX();
    }
  }

  virtual float GetPosY(size_t index) {
    if (!array_of_struct_) {
      return points_as_array_->GetPosY(index);
    } else {
      if (index > points_.size()) {
        return 0;
      }
      return points_[index]->GetPosY();
    }
  }

  virtual float GetPosZ(size_t index) {
    if (!array_of_struct_) {
      return points_as_array_->GetPosZ(index);
    } else {
      if (index > points_.size()) {
        return 0;
      }
      return points_[index]->GetPosZ();
    }
  }

  size_t GetNumberOfPointsFromCloudPointAsArray() { return points_as_array_->num_points_; }

  std::shared_ptr<next::udex::uint32Value> number_of_points_;
  std::vector<std::shared_ptr<CloudPoint>> points_;
  std::shared_ptr<CloudPointAsArray> points_as_array_;
  bool array_of_struct_ = false;

  std::map<std::string, std::string> urls_ = {
      {"num_points", ".numberOfPoints"}, {"point_", ".points"}, {"arrays", ".ClusterListArrays"}};

private:
  size_t num_objects_ = 100;

  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if (urls_.find(key) == urls_.end()) {
      return false;
    }
    if (!value->SetUrls(urls_[key])) {
      return false;
    }
    return true;
  }
};

} // namespace plugins
} // namespace next

#endif // NEXT_BRIDGES_POINTCLOUDSTRUCT_H

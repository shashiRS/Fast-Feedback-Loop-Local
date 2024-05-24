/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_roadmodel.hpp
 * @brief    defining a base road model object to request data
 *
 **/
#ifndef NEXT_BRIDGES_BASE_ROAD_MODEL_H
#define NEXT_BRIDGES_BASE_ROAD_MODEL_H

#include <next/udex/struct_extractor/struct_extractor_array_factory.hpp>
#include <next/udex/struct_extractor/struct_extractor_types.hpp>

namespace next {
namespace plugins {

struct LaneBoundaryMaxSize {
  size_t left_{0};
  size_t right_{0};
};

struct RoadModelMaxSize {
  size_t max_size_points_{0};
  size_t max_size_linearObj_{0};
  size_t max_size_laneSegments_{0};
  LaneBoundaryMaxSize max_size_laneBoundary;
};

class BaseRoadGeneral : public next::udex::extractor::StructExtractor<BaseRoadGeneral> {
public:
  BaseRoadGeneral(StructExtractorBasic *parent) : StructExtractor(parent) {
    ego_lane_ = std::make_shared<next::udex::uCharValue>(this);
    ego_position_ = std::make_shared<next::udex::floatValue>(this);
    num_connections_ = std::make_shared<next::udex::uCharValue>(this);
    num_lane_segments_ = std::make_shared<next::udex::uCharValue>(this);
    num_points_ = std::make_shared<next::udex::uint16Value>(this);
    num_linear_obj_ = std::make_shared<next::udex::uCharValue>(this);
  }
  virtual ~BaseRoadGeneral() {}
  virtual BaseRoadGeneral Get() override { return *this; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!setUrlSave(ego_lane_, "ego_lane_")) {
      return false;
    }
    if (!setUrlSave(ego_position_, "ego_position_")) {
      return false;
    }
    if (!setUrlSave(num_connections_, "num_connections_")) {
      return false;
    }
    if (!setUrlSave(num_lane_segments_, "num_lane_segments_")) {
      return false;
    }
    if (!setUrlSave(num_points_, "num_points_")) {
      return false;
    }
    if (!setUrlSave(num_linear_obj_, "num_linear_obj_")) {
      return false;
    }

    return true;
  }

  std::shared_ptr<next::udex::uCharValue> ego_lane_;
  std::shared_ptr<next::udex::floatValue> ego_position_;
  std::shared_ptr<next::udex::uCharValue> num_connections_;
  std::shared_ptr<next::udex::uCharValue> num_lane_segments_;
  std::shared_ptr<next::udex::uint16Value> num_points_;
  std::shared_ptr<next::udex::uCharValue> num_linear_obj_;
  std::unordered_map<std::string, std::string> urls_ = {{"ego_lane_", ".laneTopology.egoLane"},
                                                        {"ego_position_", ".laneTopology.egoPosition"},
                                                        {"num_connections_", ".laneTopology.numConnections"},
                                                        {"num_lane_segments_", ".laneTopology.numLaneSegments"},
                                                        {"num_points_", ".numPoints"},
                                                        {"num_linear_obj_", ".numLinearObjects"}};

protected:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }
};

class BaseRoadPointList : public next::udex::extractor::StructExtractor<BaseRoadPointList> {
public:
  BaseRoadPointList(StructExtractorBasic *parent) : StructExtractor(parent) {
    point_x_ = std::make_shared<next::udex::floatValue>(this);
    point_y_ = std::make_shared<next::udex::floatValue>(this);
  }
  virtual ~BaseRoadPointList() {}
  virtual BaseRoadPointList Get() override { return *this; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!setUrlSave(point_x_, "point_x_")) {
      return false;
    }
    if (!setUrlSave(point_y_, "point_y_")) {
      return false;
    }

    return true;
  }

  std::shared_ptr<next::udex::floatValue> point_x_;
  std::shared_ptr<next::udex::floatValue> point_y_;

  std::unordered_map<std::string, std::string> urls_ = {{"point_x_", ".x"}, {"point_y_", ".y"}};

protected:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }
};

class BaseRoadlinearObList : public next::udex::extractor::StructExtractor<BaseRoadlinearObList> {
public:
  BaseRoadlinearObList(StructExtractorBasic *parent) : StructExtractor(parent) {
    geo_start_index_ = std::make_shared<next::udex::uint16Value>(this);
    geo_end_index_ = std::make_shared<next::udex::uint16Value>(this);
  }
  virtual ~BaseRoadlinearObList() {}
  virtual BaseRoadlinearObList Get() override { return *this; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!setUrlSave(geo_start_index_, "geo_start_index_")) {
      return false;
    }
    if (!setUrlSave(geo_end_index_, "geo_end_index_")) {
      return false;
    }

    return true;
  }

  std::shared_ptr<next::udex::uint16Value> geo_start_index_;
  std::shared_ptr<next::udex::uint16Value> geo_end_index_;

  std::unordered_map<std::string, std::string> urls_ = {{"geo_start_index_", ".geometry.startIndex"},
                                                        {"geo_end_index_", ".geometry.endIndex"}};

protected:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }
};

class BaseRoadLaneTopList : public next::udex::extractor::StructExtractor<BaseRoadLaneTopList> {
public:
  BaseRoadLaneTopList(StructExtractorBasic *parent, LaneBoundaryMaxSize max_size_laneBoundary)
      : StructExtractor(parent), max_size_laneBoundary_(max_size_laneBoundary) {
    line_start_index_ = std::make_shared<next::udex::uint16Value>(this);
    line_end_index_ = std::make_shared<next::udex::uint16Value>(this);
    id_ = std::make_shared<next::udex::uint32Value>(this);
    left_lane_ = std::make_shared<next::udex::uCharValue>(this);
    right_lane_ = std::make_shared<next::udex::uCharValue>(this);
    num_left_boundary_ = std::make_shared<next::udex::uCharValue>(this);
    num_right_boundary_ = std::make_shared<next::udex::uCharValue>(this);
  }
  virtual ~BaseRoadLaneTopList() {}
  virtual BaseRoadLaneTopList Get() override { return *this; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!setUrlSave(line_start_index_, "line_start_index_")) {
      return false;
    }
    if (!setUrlSave(line_end_index_, "line_end_index_")) {
      return false;
    }
    if (!setUrlSave(id_, "id_")) {
      return false;
    }
    if (!setUrlSave(left_lane_, "left_lane_")) {
      return false;
    }
    if (!setUrlSave(right_lane_, "right_lane_")) {
      return false;
    }
    if (!setUrlSave(num_left_boundary_, "num_left_boundary_")) {
      return false;
    }
    if (!setUrlSave(num_right_boundary_, "num_right_boundary_")) {
      return false;
    }

    if (urls_.find("leftBoundaryParts_") == urls_.end()) {
      return false;
    }
    if (urls_.find("rightBoundaryParts_") == urls_.end()) {
      return false;
    }

    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;
    for (unsigned int j = 0; j < max_size_laneBoundary_.left_; j++) {
      base_object_left.push_back(std::make_shared<next::udex::uCharValue>(this));
      if (!base_object_left[j]->SetUrls(urls_["leftBoundaryParts_"])) {
        return false;
      }
      cast_vector.push_back(base_object_left[j]);
    }

    next::udex::extractor::StructExtractorArrayFactory factory_laneTopology_left =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_laneTopology_left.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int j = 0; j < max_size_laneBoundary_.right_; j++) {
      base_object_right.push_back(std::make_shared<next::udex::uCharValue>(this));
      if (!base_object_right[j]->SetUrls(urls_["rightBoundaryParts_"])) {
        return false;
      }
      cast_vector.push_back(base_object_right[j]);
    }

    next::udex::extractor::StructExtractorArrayFactory factory_laneTopology_right =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_laneTopology_right.UpdateUrlInVector(cast_vector);

    return true;
  }

  std::shared_ptr<next::udex::uint16Value> line_start_index_;
  std::shared_ptr<next::udex::uint16Value> line_end_index_;
  std::shared_ptr<next::udex::uint32Value> id_;
  std::shared_ptr<next::udex::uCharValue> left_lane_;
  std::shared_ptr<next::udex::uCharValue> right_lane_;
  std::shared_ptr<next::udex::uCharValue> num_left_boundary_;
  std::shared_ptr<next::udex::uCharValue> num_right_boundary_;
  std::vector<std::shared_ptr<next::udex::uCharValue>> base_object_left;
  std::vector<std::shared_ptr<next::udex::uCharValue>> base_object_right;

  std::unordered_map<std::string, std::string> urls_ = {{"line_start_index_", ".centerline.startIndex"},
                                                        {"line_end_index_", ".centerline.endIndex"},
                                                        {"id_", ".id"},
                                                        {"left_lane_", ".leftLane"},
                                                        {"right_lane_", ".rightLane"},
                                                        {"num_left_boundary_", ".numLeftBoundaryParts"},
                                                        {"num_right_boundary_", ".numRightBoundaryParts"},
                                                        {"leftBoundaryParts_", ".leftBoundaryParts"},
                                                        {"rightBoundaryParts_", ".rightBoundaryParts"}};

protected:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }

private:
  LaneBoundaryMaxSize max_size_laneBoundary_;
};

class BaseRoadBoundaryParts : public next::udex::extractor::StructExtractor<BaseRoadBoundaryParts> {
public:
  BaseRoadBoundaryParts(StructExtractorBasic *parent) : StructExtractor(parent) {
    leftBoundaryParts_ = std::make_shared<next::udex::uCharValue>(this);
    rightBoundaryParts_ = std::make_shared<next::udex::uCharValue>(this);
  }
  virtual ~BaseRoadBoundaryParts() {}
  virtual BaseRoadBoundaryParts Get() override { return *this; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (!setUrlSave(leftBoundaryParts_, ".leftBoundaryParts")) {
      return false;
    }
    if (!setUrlSave(rightBoundaryParts_, ".rightBoundaryParts")) {
      return false;
    }

    return true;
  }

  std::shared_ptr<next::udex::uCharValue> leftBoundaryParts_;
  std::shared_ptr<next::udex::uCharValue> rightBoundaryParts_;

  std::unordered_map<std::string, std::string> urls_ = {{"leftBoundaryParts_", ".leftBoundaryParts"},
                                                        {"rightBoundaryParts_", ".rightBoundaryParts"}};

protected:
  template <typename member>
  inline bool setUrlSave(std::shared_ptr<member> value, const std::string &key) {
    if ((urls_.find(key) == urls_.end()) || (!value->SetUrls(urls_[key]))) {
      return false;
    }
    return true;
  }
};

class BaseRoadModelList : public next::udex::extractor::StructExtractor<BaseRoadModelList> {

  RoadModelMaxSize road_model_max_size_;

public:
  BaseRoadModelList(StructExtractorBasic *parent) : StructExtractor(parent) {}
  BaseRoadModelList(StructExtractorBasic *parent, RoadModelMaxSize road_model_max_size)
      : StructExtractor(parent), road_model_max_size_(road_model_max_size) {}
  virtual ~BaseRoadModelList() {}
  virtual BaseRoadModelList Get() override { return nullptr; }

  virtual bool SetUrls(const std::string &url) override {
    if (!StructExtractorBasic::SetUrls(url)) {
      return false;
    }

    if (urls_.find("points_") == urls_.end()) {
      return false;
    }
    if (urls_.find("linearObjects_") == urls_.end()) {
      return false;
    }
    if (urls_.find("laneTopology_laneSegments_") == urls_.end()) {
      return false;
    }

    std::vector<std::shared_ptr<StructExtractorBasic>> cast_vector;

    base_object_general = std::make_shared<BaseRoadGeneral>(this);
    base_object_general->urls_.clear();
    base_object_general->urls_ = urls_;
    // use parent url
    if (!base_object_general->SetUrls("")) {
      return false;
    }

    for (unsigned int i = 0; i < road_model_max_size_.max_size_points_; i++) {
      base_object_points.push_back(std::make_shared<BaseRoadPointList>(this));
      base_object_points[i]->urls_.clear();
      base_object_points[i]->urls_ = urls_;
      if (!base_object_points[i]->SetUrls(urls_["points_"])) {
        return false;
      }
      cast_vector.push_back(base_object_points[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_points =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_points.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < road_model_max_size_.max_size_linearObj_; i++) {
      base_object_linearOb.push_back(std::make_shared<BaseRoadlinearObList>(this));
      base_object_linearOb[i]->urls_.clear();
      base_object_linearOb[i]->urls_ = urls_;
      if (!base_object_linearOb[i]->SetUrls(urls_["linearObjects_"])) {
        return false;
      }
      cast_vector.push_back(base_object_linearOb[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_linearOb =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_linearOb.UpdateUrlInVector(cast_vector);

    cast_vector.clear();
    for (unsigned int i = 0; i < road_model_max_size_.max_size_laneSegments_; i++) {
      base_object_laneTopology.push_back(
          std::make_shared<BaseRoadLaneTopList>(this, road_model_max_size_.max_size_laneBoundary));
      base_object_laneTopology[i]->urls_.clear();
      base_object_laneTopology[i]->urls_ = urls_;
      if (!base_object_laneTopology[i]->SetUrls(urls_["laneTopology_laneSegments_"])) {
        return false;
      }
      cast_vector.push_back(base_object_laneTopology[i]);
    }
    next::udex::extractor::StructExtractorArrayFactory factory_laneTopology =
        next::udex::extractor::StructExtractorArrayFactory();
    factory_laneTopology.UpdateUrlInVector(cast_vector);

    return true;
  }

  std::shared_ptr<BaseRoadGeneral> base_object_general;
  std::vector<std::shared_ptr<BaseRoadPointList>> base_object_points;
  std::vector<std::shared_ptr<BaseRoadlinearObList>> base_object_linearOb;
  std::vector<std::shared_ptr<BaseRoadLaneTopList>> base_object_laneTopology;
  std::unordered_map<std::string, std::string> urls_{{"laneTopology_laneSegments_", ".laneTopology.laneSegments"},
                                                     {"points_", ".points"},
                                                     {"linearObjects_", ".linearObjects"}};
};

} // namespace plugins
} // namespace next

#endif // NEXT_BRIDGES_BASE_ROAD_MODEL_H

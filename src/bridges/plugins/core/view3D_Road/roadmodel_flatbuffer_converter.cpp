#include <json/json.h>

#include <next/bridgesdk/schema/3d_view/groundline_generated.h>
#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>

#include "roadmodel_flatbuffer_converter.h"

namespace next {
namespace plugins {

// get laneTopology
bool RoadModelFlatBufferConverter::extractLaneTopologyFromRawData(std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                                                  RoadModelMaxSize roadModelMaxSize) {

  road_model_.laneTopology.egoLane = base_roadmodel->base_object_general->ego_lane_->Get();
  road_model_.laneTopology.egoPosition = base_roadmodel->base_object_general->ego_position_->Get();
  road_model_.laneTopology.numConnections = base_roadmodel->base_object_general->num_connections_->Get();
  road_model_.laneTopology.numLaneSegments = base_roadmodel->base_object_general->num_lane_segments_->Get();

  if (road_model_.laneTopology.numLaneSegments > roadModelMaxSize.max_size_laneSegments_) {
    warn(__FILE__,
         "Number of lange segments are exceeded in Fdp Base road. Only the first {0} lane segements are used.",
         roadModelMaxSize.max_size_laneSegments_);
  }

  for (size_t i = 0; i < road_model_.laneTopology.numLaneSegments && i < roadModelMaxSize.max_size_laneSegments_; i++) {
    road_model_.laneTopology.laneSegments[i].centerline.startIndex =
        base_roadmodel->base_object_laneTopology[i]->line_start_index_->Get();
    road_model_.laneTopology.laneSegments[i].centerline.endIndex =
        base_roadmodel->base_object_laneTopology[i]->line_end_index_->Get();
    road_model_.laneTopology.laneSegments[i].id = base_roadmodel->base_object_laneTopology[i]->id_->Get();
    road_model_.laneTopology.laneSegments[i].leftLane = base_roadmodel->base_object_laneTopology[i]->left_lane_->Get();
    road_model_.laneTopology.laneSegments[i].rightLane =
        base_roadmodel->base_object_laneTopology[i]->right_lane_->Get();
    road_model_.laneTopology.laneSegments[i].numLeftBoundaryParts =
        base_roadmodel->base_object_laneTopology[i]->num_left_boundary_->Get();

    if (road_model_.laneTopology.laneSegments[i].numLeftBoundaryParts > roadModelMaxSize.max_size_laneBoundary.left_) {
      warn(__FILE__,
           "Number of left boundary parts are exceeded in Fdp Base road. Only the first {0} left boundary parts are "
           "used.",
           roadModelMaxSize.max_size_laneBoundary.left_);
    }

    for (size_t j = 0; j < road_model_.laneTopology.laneSegments[i].numLeftBoundaryParts &&
                       j < roadModelMaxSize.max_size_laneBoundary.left_;
         j++) {
      road_model_.laneTopology.laneSegments[i].leftBoundaryParts[j] =
          base_roadmodel->base_object_laneTopology[i]->base_object_left[j]->Get();
    }

    road_model_.laneTopology.laneSegments[i].numRightBoundaryParts =
        base_roadmodel->base_object_laneTopology[i]->num_right_boundary_->Get();

    if (road_model_.laneTopology.laneSegments[i].numRightBoundaryParts >
        roadModelMaxSize.max_size_laneBoundary.right_) {
      warn(__FILE__,
           "Number of right boundary parts are exceeded in Fdp Base road. Only the first {0} right boundary parts are "
           "used.",
           roadModelMaxSize.max_size_laneBoundary.right_);
    }

    for (size_t j = 0; j < road_model_.laneTopology.laneSegments[i].numRightBoundaryParts &&
                       j < roadModelMaxSize.max_size_laneBoundary.right_;
         j++) {
      road_model_.laneTopology.laneSegments[i].rightBoundaryParts[j] =
          base_roadmodel->base_object_laneTopology[i]->base_object_right[j]->Get();
    }
  }

  return true;
}

// get numPoints
bool RoadModelFlatBufferConverter::extractRoadPointsFromRawData(std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                                                RoadModelMaxSize roadModelMaxSize) {

  road_model_.numPoints = base_roadmodel->base_object_general->num_points_->Get();

  if (road_model_.numPoints > roadModelMaxSize.max_size_points_) {
    warn(__FILE__, "Number of points are exceeded in Fdp Base road. Only the first {0} points are used.",
         roadModelMaxSize.max_size_points_);
  }

  for (size_t i = 0; i < road_model_.numPoints && i < roadModelMaxSize.max_size_points_; i++) {
    road_model_.points[i].x = base_roadmodel->base_object_points[i]->point_x_->Get();
    road_model_.points[i].y = base_roadmodel->base_object_points[i]->point_y_->Get();
  }

  return true;
}

// get linearObjects
bool RoadModelFlatBufferConverter::extractLinearObjectFromRawData(std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                                                  RoadModelMaxSize roadModelMaxSize) {

  road_model_.numLinearObjects = base_roadmodel->base_object_general->num_linear_obj_->Get();

  if (road_model_.numLinearObjects > roadModelMaxSize.max_size_linearObj_) {
    warn(__FILE__,
         "Number of linear objects are exceeded in Fdp Base road. Only the first {0} linear objects are used.",
         roadModelMaxSize.max_size_linearObj_);
  }

  for (size_t i = 0; i < road_model_.numLinearObjects && i < roadModelMaxSize.max_size_linearObj_; i++) {
    road_model_.linearObjects[i].geometry.startIndex = base_roadmodel->base_object_linearOb[i]->geo_start_index_->Get();
    road_model_.linearObjects[i].geometry.endIndex = base_roadmodel->base_object_linearOb[i]->geo_end_index_->Get();
  }

  return true;
}

bool RoadModelFlatBufferConverter::convertRawDataToRoadModelAndSend(next::plugins_utils::SensorConfig sensor_config,
                                                                    std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                                                    RoadModelMaxSize roadModelMaxSize,
                                                                    uint64_t timestamp) {
  // getPoints
  if (!extractRoadPointsFromRawData(base_roadmodel, roadModelMaxSize))
    return false;

  // getlinearObjects
  if (!extractLinearObjectFromRawData(base_roadmodel, roadModelMaxSize))
    return false;

  // getLaneTopology
  if (!extractLaneTopologyFromRawData(base_roadmodel, roadModelMaxSize))
    return false;

  calculate(sensor_config, roadModelMaxSize, timestamp);
  return true;
}

void RoadModelFlatBufferConverter::sendJSDataFromFlatBuffer(uint8_t *flatbuf,
                                                            const next::plugins_utils::SensorConfig &sensor_config,
                                                            size_t buff_size, uint64_t timestamp) {

  bridgesdk::plugin::SensorInfoForFbs sensor_info;
  sensor_info.cache_name = sensor_config.data_cache_name;
  sensor_info.sensor_id = sensor_config.sensor_id;
  sensor_info.data_view = sensor_config.data_view_name;
  sensor_info.timestamp_microsecond = timestamp;

  bridgesdk::plugin::FlatbufferPackage fbs;
  fbs.fbs_binary = flatbuf;
  fbs.fbs_size = buff_size;

  plugin_.sendFlatBufferDataToParentPlugin(fbs, {"GroundLines", {"0.0.1"}}, sensor_info);
}

void RoadModelFlatBufferConverter::calculate(const next::plugins_utils::SensorConfig &sensor_config,
                                             RoadModelMaxSize roadModelMaxSize, uint64_t timestamp) {

  uint8_t ego_lane_index = road_model_.laneTopology.egoLane;

  if (road_model_.numPoints <= 0) {
    return;
  }
  /// Initialize variables to store the five lane segments from the RoadModel msg
  /// The RMF Lane building can be found here:
  /// https://confluence-adas.zone2.agileci.conti.de/display/TRC/How+it%27s+done%3A+RMF+Lane+Building

  std::vector<flatbuffers::Offset<GuiSchema::GroundLine>> groundlines_vec;

  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  if (ego_lane_index >= roadModelMaxSize.max_size_laneSegments_) {
    warn(__FILE__, "egoLane index {0} exceeds number of laneSegments in Fdp Base road. Lane is skipped.",
         ego_lane_index);
    return;
  }

  uint8_t left_lane = road_model_.laneTopology.laneSegments[ego_lane_index].leftLane;
  uint8_t right_lane = road_model_.laneTopology.laneSegments[ego_lane_index].rightLane;
  uint8_t num_left_boundaries = road_model_.laneTopology.laneSegments[ego_lane_index].numLeftBoundaryParts;
  uint8_t num_right_boundaries = road_model_.laneTopology.laneSegments[ego_lane_index].numRightBoundaryParts;
  uint16_t center_start = road_model_.laneTopology.laneSegments[ego_lane_index].centerline.startIndex;
  uint16_t center_end = road_model_.laneTopology.laneSegments[ego_lane_index].centerline.endIndex;

  std::vector<float> x_coord_ego_center; // cordinates of center ego lane
  std::vector<float> y_coord_ego_center; // cordinates of center ego lane

  if (road_model_.numPoints > roadModelMaxSize.max_size_points_) {
    warn(__FILE__, "Number of points are exceeded in Fdp Base road. Only the first {0} points are used.",
         roadModelMaxSize.max_size_points_);
  }
  if (center_end >= roadModelMaxSize.max_size_points_) {
    warn(__FILE__,
         "Index center_end exceeds the number of points in Fdp Base road. Only points till the index {0} are used.",
         roadModelMaxSize.max_size_points_ - 1);
  }

  // From start to end, for Ego center line, add the x and y coordinates
  for (uint16_t i = center_start; i < road_model_.numPoints && i < roadModelMaxSize.max_size_points_ && i <= center_end;
       i++) {
    x_coord_ego_center.push_back(road_model_.points[i].x);
    y_coord_ego_center.push_back(road_model_.points[i].y);
  }

  if (x_coord_ego_center.size() != 0 && y_coord_ego_center.size() != 0) {
    // auto groundLine_centre = processLane(sensor_config, x_coord_ego_center, y_coord_ego_center, 0);

    auto groundLine_centre = GuiSchema::CreateGroundLine(builder, 0, builder.CreateVector<float>(x_coord_ego_center),
                                                         builder.CreateVector<float>(y_coord_ego_center), false);
    builder.Finish(groundLine_centre);
    groundlines_vec.push_back(groundLine_centre);
  }
  /// Left lane information extraction from the signal
  if (num_left_boundaries > roadModelMaxSize.max_size_laneBoundary.left_) {
    warn(
        __FILE__,
        "Number of left boundary parts are exceeded in Fdp Base road. Only the first {0} left boundary parts are used.",
        roadModelMaxSize.max_size_laneBoundary.left_);
  }

  std::vector<uint8_t> left_lane_boundary_index; // Boundary index for the left lane
  for (uint8_t i = 0; i < num_left_boundaries && i < roadModelMaxSize.max_size_laneBoundary.left_; ++i) {
    left_lane_boundary_index.push_back(road_model_.laneTopology.laneSegments[ego_lane_index].leftBoundaryParts[i]);
  }

  std::vector<float> x_left_lane; // cordinates of left line
  std::vector<float> y_left_lane; // cordinates of left line

  for (size_t i = 0; i < left_lane_boundary_index.size(); ++i) {
    if (left_lane_boundary_index[i] >= roadModelMaxSize.max_size_linearObj_) {
      warn(__FILE__,
           "Left boundary index {0} for linear objects exceeds the max size {1} in Fdp Base road. Left boundary part "
           "is skipped.",
           left_lane_boundary_index[i], roadModelMaxSize.max_size_linearObj_);
      continue;
    }

    uint16_t start_index = road_model_.linearObjects[left_lane_boundary_index[i]].geometry.startIndex;
    uint16_t end_index = road_model_.linearObjects[left_lane_boundary_index[i]].geometry.endIndex;

    if (end_index >= roadModelMaxSize.max_size_points_) {
      warn(__FILE__,
           "Index end_index for linearObjects left exceeds the number of points in Fdp Base road. Only points till the "
           "index {0} are used.",
           roadModelMaxSize.max_size_points_ - 1);
    }

    for (uint16_t j = start_index; j <= end_index && j < roadModelMaxSize.max_size_points_; j++) {
      x_left_lane.push_back(road_model_.points[j].x);
      y_left_lane.push_back(road_model_.points[j].y);
    }
  }

  if (x_left_lane.size() != 0 && y_left_lane.size() != 0) {
    auto groundLine_left_lane = GuiSchema::CreateGroundLine(builder, 1, builder.CreateVector<float>(x_left_lane),
                                                            builder.CreateVector<float>(y_left_lane), false);
    builder.Finish(groundLine_left_lane); // finish takes some time, performance increase needed
    groundlines_vec.push_back(groundLine_left_lane);
  }

  if (num_right_boundaries > roadModelMaxSize.max_size_laneBoundary.right_) {
    warn(__FILE__,
         "Number of right boundary parts are exceeded in Fdp Base road. Only the first {0} right boundary parts are "
         "used.",
         roadModelMaxSize.max_size_laneBoundary.right_);
  }

  /// Right lane information extraction from the signal
  std::vector<uint8_t> right_lane_boundary_index; // Boundary index for the right lane
  for (uint8_t i = 0; i < num_right_boundaries && i < roadModelMaxSize.max_size_laneBoundary.right_; ++i) {
    right_lane_boundary_index.push_back(road_model_.laneTopology.laneSegments[ego_lane_index].rightBoundaryParts[i]);
  }

  std::vector<float> x_right_lane; // cordinates of right lane
  std::vector<float> y_right_lane; // cordinates of right lane

  for (size_t i = 0; i < right_lane_boundary_index.size(); ++i) {
    if (right_lane_boundary_index[i] >= roadModelMaxSize.max_size_linearObj_) {
      warn(__FILE__,
           "Right boundary index {0} for linear objects exceeds the max size {1} in Fdp Base road. Right boundary part "
           "is skipped.",
           right_lane_boundary_index[i], roadModelMaxSize.max_size_linearObj_);
      continue;
    }

    uint16_t start_index = road_model_.linearObjects[right_lane_boundary_index[i]].geometry.startIndex;
    uint16_t end_index = road_model_.linearObjects[right_lane_boundary_index[i]].geometry.endIndex;

    if (end_index >= roadModelMaxSize.max_size_points_) {
      warn(__FILE__,
           "Index end_index for linearObjects right exceeds the number of points in Fdp Base road. Only points till "
           "the index {0} are used.",
           roadModelMaxSize.max_size_points_ - 1);
    }

    for (uint16_t j = start_index; j <= end_index && j < roadModelMaxSize.max_size_points_; j++) {
      x_right_lane.push_back(road_model_.points[j].x);
      y_right_lane.push_back(road_model_.points[j].y);
    }
  }

  if (x_right_lane.size() != 0 && y_right_lane.size() != 0) {
    /// Store the results of right line in a JSON Structure
    // auto groundLine_right_lane = processLane(sensor_config, x_right_lane, y_right_lane, 2);
    auto groundLine_right_lane = GuiSchema::CreateGroundLine(builder, 2, builder.CreateVector<float>(x_right_lane),
                                                             builder.CreateVector<float>(y_right_lane), false);
    builder.Finish(groundLine_right_lane);
    groundlines_vec.push_back(groundLine_right_lane);
  }
  /// Next left lane information extraction from the signal
  std::vector<uint8_t> next_left_boundary_parts;
  if (left_lane < roadModelMaxSize.max_size_laneSegments_) {
    uint8_t num_left_boundary_parts = road_model_.laneTopology.laneSegments[left_lane].numLeftBoundaryParts;
    if (num_left_boundary_parts > roadModelMaxSize.max_size_laneBoundary.left_) {
      warn(__FILE__,
           "Number of left boundary parts are exceeded in Fdp Base road. Only the first {0} left boundary parts are "
           "used.",
           roadModelMaxSize.max_size_laneBoundary.left_);
    }

    for (uint8_t i = 0; i < num_left_boundary_parts && i < roadModelMaxSize.max_size_laneBoundary.left_; ++i) {
      next_left_boundary_parts.push_back(road_model_.laneTopology.laneSegments[left_lane].leftBoundaryParts[i]);
    }

    std::vector<float> x_next_left; // cordinates of left line
    std::vector<float> y_next_left; // cordinates of left line

    for (size_t i = 0; i < next_left_boundary_parts.size(); ++i) {
      uint16_t start_index = road_model_.linearObjects[next_left_boundary_parts[i]].geometry.startIndex;
      uint16_t end_index = road_model_.linearObjects[next_left_boundary_parts[i]].geometry.endIndex;
      if (end_index >= roadModelMaxSize.max_size_points_) {
        warn(__FILE__,
             "Index end_index of left boundary exceeds the number of points in Fdp Base road. Only points till the "
             "index {0} are used.",
             roadModelMaxSize.max_size_points_ - 1);
      }

      for (uint16_t j = start_index; j <= end_index && roadModelMaxSize.max_size_points_; j++) {
        x_next_left.push_back(road_model_.points[j].x);
        y_next_left.push_back(road_model_.points[j].y);
      }
    }

    if (y_next_left.size() != 0 && x_next_left.size() != 0) {

      auto groundLine_next_left_lane = GuiSchema::CreateGroundLine(builder, 3, builder.CreateVector<float>(x_next_left),
                                                                   builder.CreateVector<float>(y_next_left), false);
      builder.Finish(groundLine_next_left_lane);
      groundlines_vec.push_back(groundLine_next_left_lane);
    }
  } else if (left_lane != roadModelMaxSize.max_size_laneSegments_) {
    // Hint: Default value for left lane is max_size_laneSegments_. Only if the value is larger max_size_laneSegments_
    // it's an error for sure
    warn(__FILE__, "left_lane index {0} exceeds number of laneSegments {1} in Fdp Base road. Lane is skipped.",
         left_lane, roadModelMaxSize.max_size_laneSegments_);
  }

  /// Next right lane information extraction from the signal
  std::vector<int> next_right_boundary_parts;
  if (right_lane < roadModelMaxSize.max_size_laneSegments_) {
    uint8_t num_right_boundary_parts = road_model_.laneTopology.laneSegments[right_lane].numRightBoundaryParts;
    if (num_right_boundary_parts > roadModelMaxSize.max_size_laneBoundary.right_) {
      warn(__FILE__,
           "Number of right boundary parts are exceeded in Fdp Base road. Only the first {0} right boundary parts are "
           "used.",
           roadModelMaxSize.max_size_laneBoundary.right_);
    }

    for (uint8_t i = 0; i < num_right_boundary_parts && i < roadModelMaxSize.max_size_laneBoundary.right_; ++i) {
      next_right_boundary_parts.push_back(road_model_.laneTopology.laneSegments[right_lane].rightBoundaryParts[i]);
    }

    std::vector<float> x_next_right; // cordinates of next right line
    std::vector<float> y_next_right; // cordinates of next right line

    for (size_t i = 0; i < next_right_boundary_parts.size(); ++i) {
      uint16_t start_index = road_model_.linearObjects[next_right_boundary_parts[i]].geometry.startIndex;
      uint16_t end_index = road_model_.linearObjects[next_right_boundary_parts[i]].geometry.endIndex;
      if (end_index >= roadModelMaxSize.max_size_points_) {
        warn(__FILE__,
             "Index end_index for right boundary exceeds the number of points in Fdp Base road. Only points till the "
             "index {0} are used.",
             roadModelMaxSize.max_size_points_ - 1);
      }

      for (uint16_t j = start_index; j <= end_index && j < roadModelMaxSize.max_size_points_; j++) {
        x_next_right.push_back(road_model_.points[j].x);
        y_next_right.push_back(road_model_.points[j].y);
      }
    }

    if (x_next_right.size() != 0 && y_next_right.size() != 0) {
      auto groundLine_next_right_lane = GuiSchema::CreateGroundLine(
          builder, 3, builder.CreateVector<float>(x_next_right), builder.CreateVector<float>(y_next_right), false);
      builder.Finish(groundLine_next_right_lane);

      groundlines_vec.push_back(groundLine_next_right_lane);
    }
  } else if (right_lane != roadModelMaxSize.max_size_laneSegments_) {
    // Hint: Default value for right lane is max_size_laneSegments_. Only if the value is larger max_size_laneSegments_
    // it's an error for sure
    warn(__FILE__, "right_lane index {0} exceeds number of laneSegments {1} in Fdp Base road. Lane is skipped.",
         right_lane, roadModelMaxSize.max_size_laneSegments_);
  }

  // create GroundLines
  auto ground_lines_vec = builder.CreateVector<flatbuffers::Offset<GuiSchema::GroundLine>>(groundlines_vec);

  auto groundLines = CreateGroundLineList(builder, ground_lines_vec);
  builder.Finish(groundLines);

  sendJSDataFromFlatBuffer(builder.GetCurrentBufferPointer(), sensor_config, builder.GetSize(), timestamp);
}

} // namespace plugins
} // namespace next

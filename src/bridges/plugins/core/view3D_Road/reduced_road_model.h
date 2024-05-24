#ifndef REDUCED_ROAD_MODEL_H
#define REDUCED_ROAD_MODEL_H
#pragma once
namespace next {
namespace plugins {

struct RoadPoint {
  float x;
  float y;
};

struct Polyline {
  uint16_t startIndex;
  uint16_t endIndex;
};

struct LaneSegment {
  Polyline centerline;
  uint32_t id;
  uint8_t leftLane;
  uint8_t rightLane;
  uint8_t numLeftBoundaryParts;
  uint8_t leftBoundaryParts[4];
  uint8_t numRightBoundaryParts;
  uint8_t rightBoundaryParts[4];
};

struct LinearObject {
  Polyline geometry;
};

struct LaneTopology {
  uint8_t egoLane;
  float egoPosition;
  uint8_t numLaneSegments;
  uint8_t numConnections;
  LaneSegment laneSegments[5];
};

struct RoadModel {
  uint8_t numLinearObjects;
  uint16_t numPoints;
  RoadPoint points[2000];
  LinearObject linearObjects[20];
  LaneTopology laneTopology;
};

} // namespace plugins
} // namespace next
#endif // REDUCED_ROAD_MODEL_H

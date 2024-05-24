#ifndef NEXT_PLUGINS_FDP_BASE_URL_MAPPER_H_
#define NEXT_PLUGINS_FDP_BASE_URL_MAPPER_H_

#include <string>
#include <unordered_map>

const std::unordered_map<std::string, std::string> fdp_base{
    {"ego_lane_", ".laneTopology.egoLane"},
    {"ego_position_", ".laneTopology.egoPosition"},
    {"num_connections_", ".laneTopology.numConnections"},
    {"num_lane_segments_", ".laneTopology.numLaneSegments"},
    {"laneTopology_laneSegments_", ".laneTopology.laneSegments"},
    /**/ {"line_start_index_", ".centerline.startIndex"},
    /**/ {"line_end_index_", ".centerline.endIndex"},
    /**/ {"id_", ".id"},
    /**/ {"left_lane_", ".leftLane"},
    /**/ {"right_lane_", ".rightLane"},
    /**/ {"num_left_boundary_", ".numLeftBoundaryParts"},
    /**/ {"num_right_boundary_", ".numRightBoundaryParts"},
    /**/ {"leftBoundaryParts_", ".leftBoundaryParts"},
    /**/ {"rightBoundaryParts_", ".rightBoundaryParts"},
    {"num_points_", ".numPoints"},
    {"points_", ".points"},
    /**/ {"point_x_", ".x"},
    /**/ {"point_y_", ".y"},
    {"num_linear_obj_", ".numLinearObjects"},
    {"linearObjects_", ".linearObjects"},
    /**/ {"geo_start_index_", ".geometry.startIndex"},
    /**/ {"geo_end_index_", ".geometry.endIndex"}};

#endif // NEXT_PLUGINS_FDP_BASE_URL_MAPPER_H_

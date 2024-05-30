#ifndef NEXT_PLUGINS_POINT_CLOUD_URL_MAPPER_H_
#define NEXT_PLUGINS_POINT_CLOUD_URL_MAPPER_H_

#include <map>
#include <string>

std::map<std::string, std::string> parking_odo_estimation{
    {"ego_x_position", ".xPosition_m"},
    {"ego_y_position", ".yPosition_m"},
    {"ego_yaw_angle", ".yawAngle_rad"},
};

#endif // NEXT_PLUGINS_POINT_CLOUD_URL_MAPPER_H_

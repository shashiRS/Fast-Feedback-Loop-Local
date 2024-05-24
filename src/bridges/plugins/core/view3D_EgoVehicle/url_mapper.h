#ifndef NEXT_PLUGINS_VIEW3D_FDPBASEEGOVEHICLEKINEMATICS_URL_MAPPER_H_
#define NEXT_PLUGINS_VIEW3D_FDPBASEEGOVEHICLEKINEMATICS_URL_MAPPER_H_

#include <map>
#include <string>

std::map<std::string, std::string> parking_odo_estimation_port_general{
    {"ego_pos_x", ".xPosition_m"},   {"ego_pos_y", ".yPosition_m"},      {"ego_yaw_angle", ".yawAngle_rad"},
    {"ego_vel_x", ".xVelocity_mps"}, {"ego_yaw_rate", ".yawRate_radps"}, {"timestamp", ".timestamp_us"}};

std::map<std::string, std::string> parking_odo_estimation_port_Cobolt{
    {"ego_pos_x", ".xPosition_m"},   {"ego_pos_y", ".yPosition_m"},      {"ego_yaw_angle", ".yawAngle_rad"},
    {"ego_vel_x", ".xVelocity_mps"}, {"ego_yaw_rate", ".yawRate_radps"}, {"timestamp", ".sSigHeader.uiTimeStamp"}};

#endif // NEXT_PLUGINS_VIEW3D_FDPBASEEGOVEHICLEKINEMATICS_URL_MAPPER_H_

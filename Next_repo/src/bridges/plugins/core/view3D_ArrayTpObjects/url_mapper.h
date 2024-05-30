#ifndef NEXT_PLUGINS_TPF_ARRAY_TPOBJECTS_URL_MAPPER_H_
#define NEXT_PLUGINS_TPF_ARRAY_TPOBJECTS_URL_MAPPER_H_

#include <string>
#include <unordered_map>

const std::unordered_map<std::string, std::string> fdp_21p{{"id_", ".id"},
                                                           {"reference_point_", ".referencePoint"},
                                                           {"position_x_", ".positionX"},
                                                           {"position_y_", ".positionY"},
                                                           {"bow_yaw_angle_", ".boxYawAngle"},
                                                           {"box_width_", ".boxWidth"},
                                                           {"box_length_", ".boxLength"},
                                                           {"classification_", ".classification"},
                                                           {"velocity_x_", ".velocityX"},
                                                           {"velocity_y_", ".velocityY"}};

#endif // NEXT_PLUGINS_TPF_ARRAY_TPOBJECTS_URL_MAPPER_H_

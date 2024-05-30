#ifndef NEXT_PLUGINS_TPF_BASE_TPOBJECTS_URL_MAPPER_H_
#define NEXT_PLUGINS_TPF_BASE_TPOBJECTS_URL_MAPPER_H_

#include <map>
#include <string>

std::map<std::string, std::string> parking_odo_estimation{
    {"ego_x_position", ".xPosition_m"},
    {"ego_y_position", ".yPosition_m"},
    {"ego_yaw_angle", ".yawAngle_rad"},
};

const std::unordered_map<std::string, std::string> csb_coding{
    {"num_objects_", ".HeaderObjList.iNumOfUsedObjects"},
    {"aObject_", ".aObject"},
    /**/ {"pos_x_", ".kinematic.fDistX"},
    /**/ {"pos_y_", ".kinematic.fDistY"},
    /**/ {"life_time_", ".general.fLifeTime"},
    /**/ {"vel_x_", ".kinematic.fVabsX"},
    /**/ {"vel_y_", ".kinematic.fVabsY"},
    /**/ {"yaw_", ".addKinematic.fYaw"},
    /**/ {"classification_", ".classification"},
    /**/ {"shapepoint_0_pos_x_", ".shapePoints.aShapePointCoordinates[0].fPosX"},
    /**/ {"shapepoint_0_pos_y_", ".shapePoints.aShapePointCoordinates[0].fPosY"},
    /**/ {"shapepoint_1_pos_x_", ".shapePoints.aShapePointCoordinates[1].fPosX"},
    /**/ {"shapepoint_1_pos_y_", ".shapePoints.aShapePointCoordinates[1].fPosY"},
    /**/ {"shapepoint_2_pos_x_", ".shapePoints.aShapePointCoordinates[2].fPosX"},
    /**/ {"shapepoint_2_pos_y_", ".shapePoints.aShapePointCoordinates[2].fPosY"}};

const std::unordered_map<std::string, std::string> base_coding{
    {"num_objects_", ".HeaderObjList.iNumOfUsedObjects"},
    {"aObject_", ".aObject"},
    /**/ {"pos_x_", ".kinematic.fDistX"},
    /**/ {"pos_y_", ".kinematic.fDistY"},
    /**/ {"life_time_", ".general.fLifeTime"},
    /**/ {"vel_x_", ".kinematic.fVabsX"},
    /**/ {"vel_y_", ".kinematic.fVabsY"},
    /**/ {"yaw_", ".addKinematic.fYaw"},
    /**/ {"classification_", ".classification.eClassification"},
    /**/ {"shapepoint_0_pos_x_", ".shapePoints.aShapePointCoordinates[0].fPosX"},
    /**/ {"shapepoint_0_pos_y_", ".shapePoints.aShapePointCoordinates[0].fPosY"},
    /**/ {"shapepoint_1_pos_x_", ".shapePoints.aShapePointCoordinates[1].fPosX"},
    /**/ {"shapepoint_1_pos_y_", ".shapePoints.aShapePointCoordinates[1].fPosY"},
    /**/ {"shapepoint_2_pos_x_", ".shapePoints.aShapePointCoordinates[2].fPosX"},
    /**/ {"shapepoint_2_pos_y_", ".shapePoints.aShapePointCoordinates[2].fPosY"}};

const std::unordered_map<std::string, std::string> mos_coding{{"num_objects_", ".numOfObjects"},
                                                              {"aObject_", ".obj"},
                                                              /**/ {"pos_x_", ".distX"},
                                                              /**/ {"pos_y_", ".distY"},
                                                              /**/ {"life_time_", ".age"},
                                                              /**/ {"vel_x_", "absVelX"},
                                                              /**/ {"vel_y_", "absVelY"},
                                                              /**/ {"yaw_", ".heading"},
                                                              /**/ {"classification_", ""},
                                                              /**/ {"shapepoint_0_pos_x_", ".length"},
                                                              /**/ {"shapepoint_0_pos_y_", ".width"},
                                                              /**/ {"shapepoint_1_pos_x_", ".length"},
                                                              /**/ {"shapepoint_1_pos_y_", ""},
                                                              /**/ {"shapepoint_2_pos_x_", ""},
                                                              /**/ {"shapepoint_2_pos_y_", ""}};

const std::unordered_map<std::string, std::string> llf_tp_coding{
    {"num_objects_", ".objListHeader.numOfUsedObjects"},
    {"aObject_", ".objects"},
    /**/ {"pos_x_", ".objectDynamics.position.posX"},
    /**/ {"pos_y_", ".objectDynamics.position.posY"},
    /**/ {"life_time_", ""}, // Not available in TPO structure
    /**/ {"vel_x_", ".objectDynamics.dynamics.velX"},
    /**/ {"vel_y_", ".objectDynamics.dynamics.velY"},
    /**/ {"yaw_", ".objectDynamics.orientation.yaw"},
    /**/ {"classification_", ".classification"},
    /**/ {"shapepoint_0_pos_x_", ".geometry.length"},
    /**/ {"shapepoint_0_pos_y_", ".geometry.width"},
    /**/ {"shapepoint_1_pos_x_", ".geometry.length"},
    /**/ {"shapepoint_1_pos_y_", ""},
    /**/ {"shapepoint_2_pos_x_", ""},
    /**/ {"shapepoint_2_pos_y_", ""}};

const std::unordered_map<std::string, std::string> cem200_config = csb_coding;

#endif // NEXT_PLUGINS_TPF_BASE_TPOBJECTS_URL_MAPPER_H_

#ifndef NEXT_PLUGINS_TPF_BASE_TPOBJECTS_URL_MAPPER_H_
#define NEXT_PLUGINS_TPF_BASE_TPOBJECTS_URL_MAPPER_H_

#include <map>
#include <string>

/**********************************************************************/
//** map generalized object attribute names to sensor specific names **/
/**********************************************************************/
const std::map<std::string, std::string> csb_coding{
    {"CycleCounter", ".sigHeader.uiCycleCounter"},
    {"NumUsedObjects", ".HeaderObjList.iNumOfUsedObjects"},
    {"LifeTime", "general.fLifeTime"},
    {"DistX", "kinematic.fDistX"},
    {"DistY", "kinematic.fDistY"},
    {"fVabsX", "kinematic.fVabsX"},
    {"fVabsY", "kinematic.fVabsY"},
    {"shapePointCoordinates[0]", "shapePoints.aShapePointCoordinates[0]"},
    {"shapePointCoordinates[1]", "shapePoints.aShapePointCoordinates[1]"},
    {"shapePointCoordinates[2]", "shapePoints.aShapePointCoordinates[2]"},
    {"shapePointCoordinates[3]", "shapePoints.aShapePointCoordinates[3]"},
    {"classification", "classification"},
    {"fYaw", "addKinematic.fYaw"},
};

const std::map<std::string, std::string> base_coding = {
    {"CycleCounter", ".sSigHeader.uiCycleCounter"},
    {"NumUsedObjects", ".HeaderObjList.iNumOfUsedObjects"},
    {"LifeTime", "general.fLifeTime"},
    {"DistX", "kinematic.fDistX"},
    {"DistY", "kinematic.fDistY"},
    {"fVabsX", "kinematic.fVabsX"},
    {"fVabsY", "kinematic.fVabsY"},
    {"shapePointCoordinates[0]", "shapePoints.aShapePointCoordinates[0]"},
    {"shapePointCoordinates[1]", "shapePoints.aShapePointCoordinates[1]"},
    {"shapePointCoordinates[2]", "shapePoints.aShapePointCoordinates[2]"},
    {"shapePointCoordinates[3]", "shapePoints.aShapePointCoordinates[3]"},
    {"classification", "classification.eClassification"},
    {"fYaw", "addKinematic.fYaw"},
};

const std::map<std::string, std::string> cem200_config = {
    {"CycleCounter", ".sSigHeader.uiCycleCounter"},
    {"NumUsedObjects", ".HeaderObjList.iNumOfUsedObjects"},
    {"LifeTime", "general.fLifeTime"},
    {"DistX", "kinematic.fDistX"},
    {"DistY", "kinematic.fDistY"},
    {"fVabsX", "kinematic.fVabsX"},
    {"fVabsY", "kinematic.fVabsY"},
    {"shapePointCoordinates[0]", "shapePoints.aShapePointCoordinates[0]"},
    {"shapePointCoordinates[1]", "shapePoints.aShapePointCoordinates[1]"},
    {"shapePointCoordinates[2]", "shapePoints.aShapePointCoordinates[2]"},
    {"shapePointCoordinates[3]", "shapePoints.aShapePointCoordinates[3]"},
    {"classification", "classification"},
    {"fYaw", "addKinematic.fYaw"},
};

#endif // NEXT_PLUGINS_TPF_BASE_TPOBJECTS_URL_MAPPER_H_

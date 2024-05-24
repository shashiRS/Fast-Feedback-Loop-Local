set(Plugin_NAME view3D_EgoVehicle)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    ego_vehicle.h
    ego_vehicle.cpp
    ego_vehicle.hpp
    url_mapper.h
  PRIVATEINCLUDEDIRS
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/plugin_addons
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    flatbuffers::flatbuffers
    jsoncpp_static
    next_flatbuffers_3d_view
    Boost::boost
    next_sdk::next_appsupport
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/egovehicle.fbs
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/primitive.fbs
    ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view/egovehicle.bfbs
    ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view/primitive.bfbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
  )

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_3d_view)

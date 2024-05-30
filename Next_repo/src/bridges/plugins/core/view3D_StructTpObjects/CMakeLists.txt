set(Plugin_NAME view3D_StructTpObjects)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    structTpObjects.h
    url_mapper.h
    structTpObjects.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    flatbuffers::flatbuffers
    jsoncpp_static
    next_plugin
    next_flatbuffers_3d_view
    Boost::boost
    next_sdk::next_appsupport
    next_udex::next_udex
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/trafficparticipant.fbs
    ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view/trafficparticipant.bfbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_3d_view)

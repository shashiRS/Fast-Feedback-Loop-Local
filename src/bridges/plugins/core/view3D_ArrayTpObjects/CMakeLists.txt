set(Plugin_NAME view3D_ArrayTpObjects)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    array_tp_objects.h
    array_tp_objects.cpp
    tp_objects.hpp
    url_mapper.h
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    flatbuffers::flatbuffers
    jsoncpp_static
    next_flatbuffers_3d_view
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/trafficparticipant.fbs
    ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view/trafficparticipant.bfbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_3d_view)

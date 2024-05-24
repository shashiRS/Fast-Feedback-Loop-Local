set(Plugin_NAME view3D_Road)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    base_roadmodel.hpp
    url_mapper.h
    roadmodel.h
    roadmodel.cpp
    roadmodel_flatbuffer_converter.cpp
    roadmodel_flatbuffer_converter.h
    reduced_road_model.h
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    Boost::boost
    next_sdk::next_appsupport
    jsoncpp_static
    flatbuffers::flatbuffers
    next_udex::next_udex
    next_flatbuffers_3d_view
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/groundline.fbs
    ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view/groundline.bfbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
  )

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_3d_view)

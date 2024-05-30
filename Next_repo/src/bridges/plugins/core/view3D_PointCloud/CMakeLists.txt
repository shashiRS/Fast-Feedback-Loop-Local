set(Plugin_NAME view3D_PointCloud)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    point_cloud.h
    point_cloud.cpp
    url_mapper.h
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    jsoncpp_static
    next_plugin
    flatbuffers::flatbuffers
    next_flatbuffers_3d_view
    Boost::boost
    next_sdk::next_appsupport
    next_udex::next_udex
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/pointcloud.fbs
    ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view/pointcloud.bfbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_3d_view)

target_compile_definitions(${Plugin_NAME}
  PRIVATE
    # silence some C++17 deprecation warnings, produced by boost numerics ublas (e.g. std::allocator<_Ty>::size_type)
    _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
    _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
)

set(Plugin_NAME example_PointCloud)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    point_cloud_example.h
    point_cloud_example.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    jsoncpp_static
    next_plugin
    flatbuffers::flatbuffers
    Boost::boost
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/3d_view/pointcloud.fbs
  TYPE
    example
)

target_compile_definitions(${Plugin_NAME}
  PRIVATE
    # silence some C++17 deprecation warnings, produced by boost numerics ublas (e.g. std::allocator<_Ty>::size_type)
    _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
    _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
)

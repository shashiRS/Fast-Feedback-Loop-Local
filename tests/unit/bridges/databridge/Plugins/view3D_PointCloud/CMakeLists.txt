set(TEST_PACKAGE view3D_PointCloud_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    point_cloud_test.cpp
    ${PLUGINS_SRC_DIR}/core/view3D_PointCloud/point_cloud.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/SRR630_RR.sdl
  PRIVATEINCLUDEDIRS
    ${PLUGINS_SRC_DIR}/core/view3D_PointCloud
    ${PLUGINS_INTERFACE_DIR}
    ${UNIT_TEST_COMMON_DIR}
    ${DATABRIDGE_INTERFACE_DIR}
  PRIVATELINKLIBS
    Boost::boost
    flatbuffers::flatbuffers
    jsoncpp_static
    next_plugin
    next_udex::next_udex
    next_flatbuffers_3d_view
    next_sdk::next_appsupport
  INSTALLTARGETS
    mts_sentinel::client
    mts_core_lib::modules
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
    # silence some C++17 deprecation warnings, produced by boost numerics ublas (e.g. std::allocator<_Ty>::size_type)
    _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
)

add_dependencies(${TEST_PACKAGE}
  GENERATE_next_flatbuffers_3d_view
)

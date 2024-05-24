set(TEST_PACKAGE view3D_CanTpObjects_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    canTpObjects_test.cpp
    ${PLUGINS_SRC_DIR}/core/view3D_CanTpObjects/canTpObjects.cpp
    ${PLUGINS_SRC_DIR}/core/view3D_CanTpObjects/canTpObjects_struct.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_SRC_DIR}/core/view3D_CanTpObjects
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_udex
    jsoncpp_static
    next_plugin
    flatbuffers::flatbuffers
    next_flatbuffers_3d_view
    next_sdk::next_appsupport
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ARS510HA22_Private.dbc
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

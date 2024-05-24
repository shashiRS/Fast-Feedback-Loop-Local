next_create_test(
  NAME view3D_StructTpObjects_test
  SRCS
    structTpObjects_test.cpp
    ${PLUGINS_SRC_DIR}/core/view3D_StructTpObjects/structTpObjects.cpp
  PRIVATEINCLUDEDIRS
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/plugin_addons
    ${PLUGINS_SRC_DIR}/core/view3D_StructTpObjects
    ${PLUGINS_INTERFACE_DIR}
    ${UNIT_TEST_COMMON_DIR}
    ${DATABRIDGE_INTERFACE_DIR}
  PRIVATELINKLIBS
    flatbuffers::flatbuffers
    jsoncpp_static
    next_udex::next_udex
    next_plugin
    next_flatbuffers_3d_view
    Boost::boost
    next_sdk::next_appsupport
  USESDB
  INSTALLTARGETS
    mts_sentinel::client
    mts_core_lib::modules
)

add_dependencies(view3D_StructTpObjects_test
  GENERATE_next_flatbuffers_3d_view
)

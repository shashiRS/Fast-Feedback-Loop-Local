set (TEST_PACKAGE core_flatbuffer_parsing_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    flatbuffer_core_signal_test.cpp
    ${DATABRIDGE_SRC_DIR}/DataManager/OutputManager/plugin_data_class_parser.h
  RESOURCES
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/core/signal_update.fbs
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/core/signal.fbs
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/core/types.fbs
  PUBLICLINKLIBS
    flatbuffers::flatbuffers
    next_flatbuffers_core
    next_flatbuffers_3d_view
    Boost::boost
    jsoncpp_static
    databridge_lib
  INSTALLTARGETS
    mts_sentinel::client
)

add_dependencies(${TEST_PACKAGE}
  GENERATE_next_flatbuffers_core
)

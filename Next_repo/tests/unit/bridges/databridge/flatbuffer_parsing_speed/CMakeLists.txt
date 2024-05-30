set(TEST_PACKAGE flatbuffer_parsing_speed_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
  flatbuffer_parsing_speed_test.cpp
  ${DATABRIDGE_SRC_DIR}/DataManager/OutputManager/plugin_data_class_parser.h
  RESOURCES
  "${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/trafficparticipant.fbs"
  PUBLICLINKLIBS
  databridge_lib
  Boost::boost
  Boost::date_time
  flatbuffers::flatbuffers
  jsoncpp_static
  next_control::next_control
  next_flatbuffers_3d_view
  next_flatbuffers_profiler
  next_flatbuffers_core
  INSTALLTARGETS
  mts_sentinel::client
)

add_dependencies(${TEST_PACKAGE}
  GENERATE_next_flatbuffers_profiler
)

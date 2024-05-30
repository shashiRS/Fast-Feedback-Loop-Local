set (TEST_PACKAGE data_manager_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_manager_test.cpp
    ${DATABRIDGE_SRC_DIR}/DataManager/data_manager_impl.cpp
  PRIVATEINCLUDEDIRS
    ${Boost_INCLUDE_DIRS}
  PRIVATELINKLIBS
    databridge_lib
    Boost::date_time
    flatbuffers::flatbuffers
    jsoncpp_static
    next_control::next_control
    next_flatbuffers_profiler
    next_flatbuffers_3d_view
    next_flatbuffers_core
  RESOURCES
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/profiler/basic_info.fbs
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/profiler/cpu_profiler.fbs
  INSTALLTARGETS
    mts_sentinel::client
)

add_dependencies(${TEST_PACKAGE}
  GENERATE_next_flatbuffers_3d_view
  GENERATE_next_flatbuffers_profiler
  GENERATE_next_flatbuffers_core
)


set(TEST_PACKAGE base_ctrl_config_reader_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    base_ctrl_config_reader_test.cpp
    ${ORCHESTRATOR_SRC_DIR}/base_ctrl_data_generator/base_ctrl_config_reader.hpp 
  PRIVATELINKLIBS
    Boost::boost 
    next_sdk::next_sdk
    next_sdk::next_appsupport
  PRIVATEINCLUDEDIRS
    ${ORCHESTRATOR_SRC_DIR}/base_ctrl_data_generator
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/sig_header.sdl
    ${CMAKE_CURRENT_LIST_DIR}/cfg_test_control_data.json
    ${CMAKE_CURRENT_LIST_DIR}/bad_cfg_test_control_data.json
  INSTALLTARGETS
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
  CONTROL_LIBRARY_BUILD
)

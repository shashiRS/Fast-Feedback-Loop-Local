set(TEST_PACKAGE control_data_generator_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    control_data_generator_test.cpp
    ${ORCHESTRATOR_SRC_DIR}/base_ctrl_data_generator/control_data_generator.cpp
  PRIVATELINKLIBS
    next_sdk::next_sdk
    next_sdk::next_appsupport
    next_udex::next_udex
    Boost::filesystem
    Boost::boost
  PRIVATEINCLUDEDIRS
    ${ORCHESTRATOR_SRC_DIR}
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/test_control_data_config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/sig_header.sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/ARS5xx.sdl
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
  CONTROL_LIBRARY_BUILD
)

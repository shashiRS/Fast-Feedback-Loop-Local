set(TEST_PACKAGE orchestrator_master_impl_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    control_orchestrator_master_impl_test.cpp
    ${ORCHESTRATOR_SRC_DIR}/orchestrator_master_impl.cpp
    ${ORCHESTRATOR_SRC_DIR}/base_ctrl_data_generator/control_data_generator.cpp
    ${CONTROL_DIR}/src/event_control/orchestrator_event_binary.cpp
  PRIVATELINKLIBS
    Boost::boost
    Boost::filesystem
    jsoncpp_static # because of orchestrator_master_impl.cpp
    next_sdk::next_sdk
    next_udex::next_udex
    next_sdk::next_appsupport 
  PRIVATEINCLUDEDIRS
    ${ORCHESTRATOR_SRC_DIR}
    ${CONTROL_INTERFACE_DIR}
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/test_player_config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/test_control_data_with_file_link.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/flow_config_file.json
  USE_CUSTOM_ECAL_DOMAIN
  INSTALLTARGETS
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
    mts_core_lib::modules
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
  CONTROL_LIBRARY_BUILD
)

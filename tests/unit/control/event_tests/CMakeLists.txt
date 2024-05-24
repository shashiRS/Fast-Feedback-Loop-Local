set(TEST_PACKAGE event_tests)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    open_recording_status_event_test.cpp
    orchestrator_event_tst.cpp
    player_command_test.cpp
    player_state_event_tst.cpp
    ${EVENTCONTROL_SRC_DIR}/open_recording_status_event.cpp
    ${EVENTCONTROL_SRC_DIR}/orchestrator_event_binary.cpp
    ${EVENTCONTROL_SRC_DIR}/player_command.cpp
    ${EVENTCONTROL_SRC_DIR}/player_state_event.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/test_input_json.json
	${CMAKE_CURRENT_LIST_DIR}/test_input_json_singlePath.json
	${CMAKE_CURRENT_LIST_DIR}/test_input_json_multiplePaths.json

  PRIVATEINCLUDEDIRS
    ${CONTROL_INTERFACE_DIR}
  PRIVATELINKLIBS
    eCAL::core
    eCAL::pb
    next_sdk::next_sdk
    jsoncpp_static
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

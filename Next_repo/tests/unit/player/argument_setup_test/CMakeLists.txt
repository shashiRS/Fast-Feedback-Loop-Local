set(TEST_PACKAGE argument_setup_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS 
    ${PLAYER_SRC_DIR}/publisher/publisher_manager.cpp
    ${PLAYER_SRC_DIR}/publisher/speed_factor.cpp
    ${PLAYER_SRC_DIR}/publisher/trigger_mapper.cpp
    ${PLAYER_SRC_DIR}/resource_manager/cycle_manager.cpp
    ${PLAYER_SRC_DIR}/resource_manager/resource_manager.cpp
    ${PLAYER_SRC_DIR}/resource_manager/udex_service.cpp
    argument_setup_test.cpp
  PUBLICINCLUDEDIRS
    ${PLAYER_SRC_DIR}/publisher/
    ${PLAYER_INTERFACE_DIR}
    ${PLAYER_SRC_DIR}/resource_manager/
  PRIVATELINKLIBS 
    next_control::next_control
    next_sdk::next_appsupport
    next_udex::next_udex
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tst.json
    ${CMAKE_CURRENT_SOURCE_DIR}/tst.rrec
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  #    Tracy::TracyClient
)

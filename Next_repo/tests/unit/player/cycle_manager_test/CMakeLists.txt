next_create_test(
  NAME player_cycle_manager_test
  SRCS
    cycle_manager_test.cpp
    ${PLAYER_SRC_DIR}/resource_manager/cycle_manager.cpp
  PRIVATELINKLIBS
    next_sdk::next_sdk
  PRIVATEINCLUDEDIRS
    ${PROJECT_BINARY_DIR}
    ${PLAYER_INTERFACE_DIR}
    ${PLAYER_SRC_DIR}
  USE_CUSTOM_ECAL_DOMAIN
)

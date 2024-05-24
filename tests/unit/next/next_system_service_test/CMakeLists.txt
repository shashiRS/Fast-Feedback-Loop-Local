set(TEST_PACKAGE next_system_service_test)

set(TEST_SRC
  next_system_service_test.cpp
  ${NEXT_SRC_DIR}/next_inport.cpp
  ${NEXT_SRC_DIR}/sync_manager/sync_manager.cpp
  ${NEXT_SRC_DIR}/sync_manager/syncref_handler.cpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PRIVATEINCLUDEDIRS
  ${NEXT_SRC_DIR}
  ${NEXT_INTERFACE_DIR}
  PRIVATELINKLIBS
  next_udex
  next_sdk
  next_appsupport
  next_control
  USE_CUSTOM_ECAL_DOMAIN
  INSTALLTARGETS
  mts_core_lib::modules
  mts_sentinel::sentinel
  mts_sentinel::msdia
  RESOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/ARS5xx.sdl
)

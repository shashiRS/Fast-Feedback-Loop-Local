set(TEST_PACKAGE next_inport_test)

set(TEST_SRC
  next_inport_test.cpp
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
  INSTALLTARGETS
  mts_core_lib::modules
  mts_sentinel::client
  RESOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/arrays.sdl
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/syncref.sdl
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/config_sync_ref.json
)

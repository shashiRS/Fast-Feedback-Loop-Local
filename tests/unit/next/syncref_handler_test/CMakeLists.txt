set(TEST_PACKAGE syncref_handler_test)

set(TEST_SRC
  syncref_handler_test.cpp
  ${NEXT_SRC_DIR}/sync_manager/syncref_handler.cpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PRIVATEINCLUDEDIRS
  ${NEXT_INTERFACE_DIR}
  ${NEXT_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/include
  PRIVATELINKLIBS
  next_control::next_control
  next_sdk::next_sdk
  next_sdk::next_appsupport
  next_udex::next_udex
  jsoncpp_static
  INSTALLTARGETS
  mts_core_lib::modules
  mts_sentinel::sentinel
  mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)
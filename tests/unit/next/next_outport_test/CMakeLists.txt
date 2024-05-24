set(TEST_PACKAGE next_outport_test)

set(TEST_SRC
  next_outport_test.cpp
  ${NEXT_SRC_DIR}/next_outport.cpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PRIVATEINCLUDEDIRS
  ${NEXT_SRC_DIR}
  ${NEXT_INTERFACE_DIR}
  PRIVATELINKLIBS
  Boost::filesystem
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
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/arrays.sdl
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/test_signal.sdl
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/test_signal2.sdl
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/test_signal3.sdl
)

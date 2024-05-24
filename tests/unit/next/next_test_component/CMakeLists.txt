set(TEST_PACKAGE next_test_component)

set(TEST_SRC
  ifc_component_test.cpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PRIVATELINKLIBS
  next
  next_udex
  eCAL::core
  INSTALLTARGETS
  mts_core_lib::modules
  mts_sentinel::sentinel
  mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
  RESOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/arrays.sdl
)

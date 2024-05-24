next_create_test(
  NAME hash_manager_test
  SRCS hash_manager_test.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ARS5xx.sdl
    ${CMAKE_CURRENT_LIST_DIR}/MultigroupInPackage.sdl
  PRIVATEINCLUDEDIRS ${UDEX_SRC_DIR}/src/publisher
  PRIVATELINKLIBS next_udex
  INSTALLTARGETS mts_core_lib::modules
  USE_CUSTOM_ECAL_DOMAIN
)

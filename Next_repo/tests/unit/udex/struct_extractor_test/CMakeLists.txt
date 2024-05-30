next_create_test(
  NAME signal_structure_extractor_test
  SRCS
    struct_extractor_test.cpp
    struct_extractor_udex_test.cpp
    struct_extractor_array_factory_test.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ARS5xx.sdl
  PRIVATEINCLUDEDIRS
  ${UDEX_SRC_DIR}/src
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
  USE_CUSTOM_ECAL_DOMAIN
)
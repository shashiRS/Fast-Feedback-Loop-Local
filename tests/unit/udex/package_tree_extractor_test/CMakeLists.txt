next_create_test(
  NAME struct_extractor_tree_test
  SRCS
  package_tree_extractor_test.cpp
  RESOURCES
  ${CMAKE_CURRENT_LIST_DIR}/arrays_tree.sdl
  PRIVATELINKLIBS
  next_udex
  INSTALLTARGETS
  mts_core_lib::modules
  USE_CUSTOM_ECAL_DOMAIN
)
set(TEST_PACKAGE tree_extractor_publisher_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    tree_extractor_publisher_test.cpp
  PRIVATELINKLIBS
    Boost::boost
    next_sdk::next_sdk
    next_udex::next_udex
    next_appsupport 
  PRIVATEINCLUDEDIRS
    ${ORCHESTRATOR_SRC_DIR}
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../resources/sig_header.sdl
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
  CONTROL_LIBRARY_BUILD
)


set(TEST_PACKAGE core_lib_reader_test)

set(TEST_SRC
  ${PLAYER_SRC_DIR}/plugin/core_lib_reader/core_lib_reader.cpp
  core_lib_reader_test.cpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/SwMultigroupInPackageCoreLibTest.sdl
    ${CMAKE_CURRENT_LIST_DIR}/HwMultigroupInPackageCoreLibTest.sdl
    ${CMAKE_CURRENT_LIST_DIR}/EthernetFibexPackageCoreLibTest.xml
  PUBLICINCLUDEDIRS
    ${PLAYER_SRC_DIR}
  ${PLAYER_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_control::next_control
    next_sdk::next_appsupport
    next_udex::next_udex
    Boost::system
    jsoncpp_static
    mtscpp::mtscpp # Magno SDK as link dependency
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  #    Tracy::TracyClient
  USE_CUSTOM_ECAL_DOMAIN
)

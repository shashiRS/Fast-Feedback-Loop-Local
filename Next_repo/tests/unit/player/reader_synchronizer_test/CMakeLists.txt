set(TEST_PACKAGE reader_synchronizer_test)

set(TEST_SRC
  ${PLAYER_SRC_DIR}/plugin/core_lib_reader/core_lib_reader.cpp
  ${PLAYER_SRC_DIR}/reader/reader_manager.cpp
  ${PLAYER_SRC_DIR}/reader/reader_synchronizer.cpp
  reader_synchronizer_test.cpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  RESOURCES
  PUBLICINCLUDEDIRS
    ${PLAYER_SRC_DIR}
    ${PLAYER_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_sdk::next_appsupport
    next_udex::next_udex
    Boost::system
    Boost::filesystem
    jsoncpp_static
    mtscpp::mtscpp # Only the Magno SDK as link dependency
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
    mts_sentinel::sentinel
    mts_sentinel::msdia
  #    Tracy::TracyClient
  USE_CUSTOM_ECAL_DOMAIN
)

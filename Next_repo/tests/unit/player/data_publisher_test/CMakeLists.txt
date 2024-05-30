set(TEST_PACKAGE data_publisher_test)

file(GLOB_RECURSE  SRC_FILES
  ${PLAYER_SRC_DIR}/publisher/*.cpp
  ${PLAYER_SRC_DIR}/publisher/*.c
  ${PLAYER_SRC_DIR}/publisher/*.h
  ${PLAYER_SRC_DIR}/publisher/*.hpp
  ${PLAYER_SRC_DIR}/reader/*.cpp
  ${PLAYER_SRC_DIR}/reader/*.c
  ${PLAYER_SRC_DIR}/reader/*.h
  ${PLAYER_SRC_DIR}/reader/*.hpp
  ${PLAYER_SRC_DIR}/sdk_deprecated/*.cpp
  ${PLAYER_SRC_DIR}/sdk_deprecated/*.c
  ${PLAYER_SRC_DIR}/sdk_deprecated/*.h
  ${PLAYER_SRC_DIR}/sdk_deprecated/*.hpp
  ${PLAYER_SRC_DIR}/state_machine/*.cpp
  ${PLAYER_SRC_DIR}/state_machine/*.c
  ${PLAYER_SRC_DIR}/state_machine/*.h
  ${PLAYER_SRC_DIR}/state_machine/*.hpp
  ${PLAYER_SRC_DIR}/player_plugin/*.cpp
  ${PLAYER_SRC_DIR}/player_plugin/*.c
  ${PLAYER_SRC_DIR}/player_plugin/*.h
  ${PLAYER_SRC_DIR}/player_plugin/*.hpp
)

set(TEST_SRC
  ../../common/resource_manager_moc.cpp
  ../../common/resource_manager_moc.hpp
  ${PLAYER_SRC_DIR}/resource_manager/buffer_queue.hpp
  ../../common/udex_service_moc.hpp
  ../../common/udex_service_moc.cpp
  data_publisher_test.cpp
  ${SRC_FILES}
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PUBLICINCLUDEDIRS
    ${PLAYER_SRC_DIR}
    ../../common
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

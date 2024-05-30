set(TEST_PACKAGE buffer_queue_test)

set(TEST_SRC
  buffer_queue_test.cpp
  ${PLAYER_SRC_DIR}/resource_manager/buffer_queue.hpp
)
next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PUBLICINCLUDEDIRS ${PLAYER_SRC_DIR} 
  PRIVATELINKLIBS
    next_udex::next_udex
    mtscpp::mtscpp # Magno SDK as link dependency
    next_sdk::next_appsupport
  INSTALLTARGETS
    mts_core_lib::modules
  #    Tracy::TracyClient
)

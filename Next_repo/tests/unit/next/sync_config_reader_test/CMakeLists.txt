set(TEST_PACKAGE sync_config_reader_test)

set(TEST_SRC
  sync_config_reader_test.cpp
  ${NEXT_SRC_DIR}/sync_manager/sync_config_reader.hpp
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  PRIVATEINCLUDEDIRS
    ${NEXT_SRC_DIR}
    ${NEXT_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_sdk
    next_udex
    next_control
    next_appsupport
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/sync_config_reader.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../ressources/sync_config_reader_v1.json
)
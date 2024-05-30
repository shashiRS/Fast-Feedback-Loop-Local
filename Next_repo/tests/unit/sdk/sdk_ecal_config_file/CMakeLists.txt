
set(TEST_RESOURCES
  ${CMAKE_CURRENT_LIST_DIR}/ecal.ini
)

next_create_test(
  NAME sdk_ecal_config_file_test
  SRCS sdk_ecal_config_file_test.cpp
  RESOURCES ${TEST_RESOURCES}
  PRIVATELINKLIBS 
    next_sdk
)

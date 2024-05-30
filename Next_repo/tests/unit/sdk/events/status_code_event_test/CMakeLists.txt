next_create_test(
  NAME status_code_event_test
  SRCS 
    status_code_event_test.cpp 
    next_status_code_test.cpp
  PRIVATEINCLUDEDIRS
    ${PROJECT_BINARY_DIR}
  PRIVATELINKLIBS
    next_sdk
    next_sdk::next_appsupport
  USE_CUSTOM_ECAL_DOMAIN
)
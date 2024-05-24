next_create_test(
  NAME status_code_log_test
  SRCS
  status_code_log_test.cpp
  PRIVATEINCLUDEDIRS
  ${APPSUPPORT_SRC_DIR}/src/next_status_code
  PRIVATELINKLIBS
  next_sdk
  next_sdk::next_appsupport
  USE_CUSTOM_ECAL_DOMAIN
)

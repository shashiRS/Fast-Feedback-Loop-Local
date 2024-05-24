next_create_test(
  NAME request_config_from_client_test
  SRCS req_conf_from_client_test.cpp
  PRIVATELINKLIBS
  next_sdk
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  TIMEOUT 30
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

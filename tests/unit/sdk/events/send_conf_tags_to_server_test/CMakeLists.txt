next_create_test(
  NAME send_config_tags_to_server_test
  SRCS send_conf_tags_to_server_test.cpp
  PRIVATELINKLIBS
  next_sdk
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  TIMEOUT 30
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

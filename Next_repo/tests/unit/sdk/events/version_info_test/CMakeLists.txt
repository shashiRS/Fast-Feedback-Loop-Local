next_create_test(
  NAME version_info_test
  SRCS version_info_test.cpp
  PRIVATELINKLIBS
  next_sdk
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  TIMEOUT 30
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

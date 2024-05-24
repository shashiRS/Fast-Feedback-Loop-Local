next_create_test(
  NAME request_eventupdate_test
  SRCS req_eventupdate_test.cpp
  PRIVATELINKLIBS
  next_sdk
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  TIMEOUT 120
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

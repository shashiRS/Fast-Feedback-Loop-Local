next_create_test(
  NAME multiple_event_test
  SRCS multiple_event_test.cpp
  PRIVATELINKLIBS
  next_sdk
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  TIMEOUT 60 # typical run time is ~28 seconds
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

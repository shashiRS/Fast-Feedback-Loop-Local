next_create_test(
  NAME event_override_test
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/event_override_test.cpp
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  PRIVATELINKLIBS
  eCAL::pb
  next_sdk
  TIMEOUT 60 # typical run time is ~25 seconds
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

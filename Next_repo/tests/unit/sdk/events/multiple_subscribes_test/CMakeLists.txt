next_create_test(
  NAME event_multiple_subscribes_test
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/multiple_subscribes_test.cpp
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  PRIVATELINKLIBS
  eCAL::pb
  next_sdk
  TIMEOUT 30
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

next_create_test(
  NAME event_manager_test
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/event_manager_test.cpp
  ${SDK_SRC_DIR}/src/event_base/event_publisher.cpp
  ${SDK_SRC_DIR}/src/event_base/event_subscriber.cpp
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  PRIVATELINKLIBS
  eCAL::pb
  next_sdk
  TIMEOUT 60
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

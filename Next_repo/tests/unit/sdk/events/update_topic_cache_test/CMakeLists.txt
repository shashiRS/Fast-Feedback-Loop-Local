next_create_test(
  NAME update_topic_cache_test
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/update_topic_cache_test.cpp
  PRIVATEINCLUDEDIRS
  ${SDK_SRC_DIR}/src/event_base
  PRIVATELINKLIBS
  eCAL::pb
  next_sdk
  TIMEOUT 30
  USE_CUSTOM_ECAL_DOMAIN
  USESDB
)

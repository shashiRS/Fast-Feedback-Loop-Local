next_create_test(
  NAME lifecycle_state_event_test
  SRCS lifecycle_state_event_test.cpp
  RESOURCES
    ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    jsoncpp_static
    next_sdk
)

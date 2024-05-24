next_create_test(
  NAME cycle_event_test
  SRCS cycle_event_test.cpp
  RESOURCES
    ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    jsoncpp_static
    next_sdk
)

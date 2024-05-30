next_create_test(
  NAME cycle_information_event_test
  SRCS cycle_information_event_test.cpp
  RESOURCES
    ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk
)

next_create_test(
  NAME session_event_test
  SRCS session_event_test.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk
)

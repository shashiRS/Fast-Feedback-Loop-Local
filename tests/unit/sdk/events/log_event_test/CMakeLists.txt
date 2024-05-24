next_create_test(
  NAME log_event_test
  SRCS log_event_test.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk
)

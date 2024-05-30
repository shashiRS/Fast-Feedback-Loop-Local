next_create_test(
  NAME sdk_ecal_singleton_initalization_test
  SRCS ecal_singleton_initalization_test.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS 
    next_sdk
)

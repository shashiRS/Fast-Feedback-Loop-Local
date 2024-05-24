next_create_test(
  NAME signal_description_handler_test
  SRCS
    signal_description_handler_test.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_description_handler.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    jsoncpp_static
    next_udex
  PRIVATEINCLUDEDIRS ${UDEX_SRC_DIR}/src/signal_storage
  USESDB
)

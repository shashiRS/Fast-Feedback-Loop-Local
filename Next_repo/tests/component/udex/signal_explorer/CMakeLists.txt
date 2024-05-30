next_create_test(
  NAME compnent_signal_explorer_component
  SRCS signal_explorer_test.cpp
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USESDB
)

next_create_test(
  NAME core_lib_data_description_parser_test
  SRCS
  core_lib_data_description_parser_test.cpp
  ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_descriptor_parser.hpp
  ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_descriptor_parser.cpp
  PRIVATEINCLUDEDIRS
  ${UDEX_INTERFACE_DIR}
  ${UDEX_SRC_DIR}/src/signal_storage
  PRIVATELINKLIBS
  Boost::boost
  mtscpp::mtscpp
  next_sdk
  INSTALLTARGETS
  mts_core_lib::modules
  mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)

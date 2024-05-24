next_create_test(
  NAME signal_watcher_test
  SRCS
    signal_watcher_test.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_watcher.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_tree_module.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/udex_dynamic_publisher.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_description_handler.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_descriptor_parser.cpp
  PRIVATELINKLIBS
    eCAL::pb
    next_sdk::next_appsupport
    next_udex
    jsoncpp_static
    Boost::json
  PRIVATEINCLUDEDIRS ${UDEX_SRC_DIR}/src/signal_storage
  USE_CUSTOM_ECAL_DOMAIN
)

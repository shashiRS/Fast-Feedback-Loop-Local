next_create_test(
  NAME dynamic_subscriber_test
  SRCS
    dynamic_subscriber_test.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_watcher.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_tree_module.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/udex_dynamic_publisher.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/udex_dynamic_subscriber.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_description_handler.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_descriptor_parser.cpp
  PRIVATELINKLIBS
    next_udex
    Boost::json
    eCAL::pb
    jsoncpp_static
    next_sdk::next_sdk
    next_sdk::next_appsupport
    mts_core_lib::libraries
  PRIVATEINCLUDEDIRS
    ${UDEX_SRC_DIR}/src/signal_storage
    ${UDEX_INTERFACE_DIR}
  USE_CUSTOM_ECAL_DOMAIN
)

# workaround for setting the LD_LIBRARY_PATH
# should be set by the next_create_test function
# TODO fix it in next_create_test function
set_tests_properties(dynamic_subscriber_test PROPERTIES
    #ENVIRONMENT "LD_LIBRARY_PATH=../lib"
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/tests/cases/lib"
)

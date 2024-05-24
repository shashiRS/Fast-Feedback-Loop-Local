next_create_test(
  NAME dynamic_publisher_test
  SRCS
    dynamic_publisher_test.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_tree_module.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_watcher.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/udex_dynamic_publisher.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/udex_dynamic_subscriber.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/signal_description_handler.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_descriptor_parser.cpp
  PRIVATELINKLIBS
    Boost::json
    eCAL::pb
    jsoncpp_static
    next_sdk::next_appsupport
    next_udex
    mts_core_lib::libraries
    next_sdk::next_sdk
  PRIVATEINCLUDEDIRS ${UDEX_SRC_DIR}/src/signal_storage
  USE_CUSTOM_ECAL_DOMAIN
)

# workaround for setting the LD_LIBRARY_PATH
# should be set by the next_create_test function
# TODO fix it in next_create_test function
set_tests_properties(dynamic_publisher_test PROPERTIES
    #ENVIRONMENT "LD_LIBRARY_PATH=../lib"
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/tests/cases/lib"
)

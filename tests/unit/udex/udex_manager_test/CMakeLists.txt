add_library(test_processor SHARED
  test_processor.cpp
)

target_link_libraries(test_processor
  PUBLIC
  next_sdk
)
target_include_directories(test_processor
  PRIVATE
  ${UDEX_INTERFACE_DIR}
)


next_create_test(
  NAME udex_manager_test
  SRCS
    udex_manager_test.cpp
    ${UDEX_SRC_DIR}/src/udex_manager.cpp
  PRIVATEINCLUDEDIRS
    ${UDEX_INTERFACE_DIR}
    ${UDEX_SRC_DIR}/src
  PRIVATELINKLIBS
    next_sdk
    Boost::filesystem
    dummy_processor
    test_processor
  USE_CUSTOM_ECAL_DOMAIN
  RESOURCES
    $<TARGET_FILE:test_processor>
    $<TARGET_FILE:dummy_processor>
)
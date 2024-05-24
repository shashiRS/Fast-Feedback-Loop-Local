
# disable deprecation warnings for the unit tests
add_compile_options(
  "$<$<CXX_COMPILER_ID:MSVC>:$<BUILD_INTERFACE:;/wd4996>>"
)

###########################
# add here all the tests, try to keep them sorted alphabetically (helps preventing merge conflicts)

add_subdirectory(core_lib_data_description_parser_test)
add_subdirectory(data_extractor_test)
add_subdirectory(data_extractor_sync_async_test)
#add_subdirectory(data_publisher_next_package_test)
#add_subdirectory(data_publisher_test)
add_subdirectory(data_subscriber_test)
add_subdirectory(dynamic_publisher_test)
add_subdirectory(dynamic_subscriber_test)
add_subdirectory(ecal_stress_test)
if(NOT ${BRICKS_BUILD_PLATFORM_NAME} MATCHES "^linux64.*")
add_subdirectory(gia_test)
endif()
add_subdirectory(hash_manager_test)
add_subdirectory(helper_functions_test)
add_subdirectory(hw_data_extractor_test)
add_subdirectory(package_tree_extractor_test)
add_subdirectory(register_data_description_test)
add_subdirectory(signal_description_handler_test)
add_subdirectory(signal_explorer_test)
add_subdirectory(signal_watcher_test)
add_subdirectory(struct_extractor_test)
add_subdirectory(udex_manager_test)
add_subdirectory(udex_queue)

###########################
# do not remove the following empty line (eof)
#

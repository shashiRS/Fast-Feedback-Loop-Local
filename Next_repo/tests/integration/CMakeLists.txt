
add_executable(minimal_json_send
  ecal_msg_publisher.cpp
)

target_link_libraries(minimal_json_send
  PRIVATE
    eCAL::core
    jsoncpp_static
)

set_target_properties(minimal_json_send PROPERTIES FOLDER "tests/integration")

add_subdirectory(helper_files)

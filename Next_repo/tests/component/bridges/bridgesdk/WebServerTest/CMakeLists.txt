set(TEST_PACKAGE component_websocket_server_test)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    web_server_test.cpp
    ${BRIDGES_COMPONENT_TESTS_COMMON_INC_DIR}/websocket_client.hpp
  PRIVATELINKLIBS
    Boost::boost
    next_sdk
    web_server
  PRIVATEINCLUDEDIRS
    ${BRIDGES_COMPONENT_TESTS_COMMON_INC_DIR}
)

target_compile_definitions(${TEST_PACKAGE}
  PRIVATE
    # silence some C++17 deprecation warnings, produced by boost asio (e.g. std::allocator<void>)
    # https://github.com/chriskohlhoff/asio/issues/290#issuecomment-377727614
    _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
    _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
)

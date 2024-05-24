next_create_test(
  NAME http_post_test
  SRCS 
    http_post_test.cpp
  PRIVATEINCLUDEDIRS
    ${CONTROLBRIDGE_SRC_DIR}/http_post
    ${UNIT_TEST_COMMON_DIR}
  PRIVATELINKLIBS
    Boost::system
    controlbridge_lib
)

target_compile_definitions(http_post_test
  PRIVATE
    # silence some C++17 deprecation warnings, produced by boost asio (e.g. std::allocator<void>)
    # https://github.com/chriskohlhoff/asio/issues/290#issuecomment-377727614
    _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
    _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
)

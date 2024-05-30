next_create_test(
  NAME sdk_init_chain_test
  SRCS
    init_chain_tests.cpp
    ${SDK_INTERFACE_DIR}/next/sdk/init_chain/simple_init_chain.hpp
    ${SDK_SRC_DIR}/src/simple_init_chain.cpp
  PRIVATEINCLUDEDIRS
    ${SDK_INTERFACE_DIR}
)

target_compile_definitions(sdk_init_chain_test
  PRIVATE NEXT_SDK_LIBRARY_STATIC
)

target_compile_features(sdk_init_chain_test
  PRIVATE
    cxx_std_17
)

target_compile_options(sdk_init_chain_test
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
)

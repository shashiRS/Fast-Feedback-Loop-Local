add_library(random_generator SHARED
  random_generator.hpp
  random_generator.cpp
)

target_link_libraries(random_generator
  PUBLIC
    next_sdk
)

set_target_properties(random_generator PROPERTIES FOLDER tests/component/sdk/initchain/libs)
target_compile_features(random_generator PRIVATE cxx_std_17)

next_create_test(
  NAME initchain_test
  SRCS initchain_test.cpp
  PRIVATELINKLIBS
    random_generator
)

target_compile_features(initchain_test PRIVATE cxx_std_17)

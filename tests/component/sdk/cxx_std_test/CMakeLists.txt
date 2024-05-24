# set the default CXX standard to 14
set(CMAKE_CXX_STANDARD 14)

add_library(sdk_def_cxx_std SHARED
  def_cxx_std.cpp
  def_cxx_std.hpp
)

set_target_properties(sdk_def_cxx_std PROPERTIES FOLDER tests/component/sdk/cxx_std_test/libs)

next_create_test(
  NAME sdk_cxx_std_test
  SRCS cxx_std_test.cpp
  PRIVATELINKLIBS
    next_sdk
    sdk_def_cxx_std
)

#target_compile_features(sdk_cxx_std_test PRIVATE cxx_std_14)
#set_property(TARGET sdk_cxx_std_test PROPERTY CXX_STANDARD 14)

# reset the default CXX standard to 17
set(CMAKE_CXX_STANDARD 17)

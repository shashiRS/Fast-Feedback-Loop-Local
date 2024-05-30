set(TEST_PACKAGE profiler)
set(LIBRARY_PACKAGE profiler_library)

add_library(${LIBRARY_PACKAGE} SHARED
    profiling_library.cpp
    profiling_library.hpp
)

target_link_libraries(${LIBRARY_PACKAGE}
    PUBLIC
        next_sdk
)

#target_compile_definitions(${LIBRARY_PACKAGE}
#   PUBLIC
#     TRACY_ENABLE
#)

set_target_properties(${LIBRARY_PACKAGE} PROPERTIES FOLDER tests/component/sdk/profiler/libs)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS
    profiler_test.cpp
  PRIVATELINKLIBS
    ${LIBRARY_PACKAGE}
    Boost::filesystem
)

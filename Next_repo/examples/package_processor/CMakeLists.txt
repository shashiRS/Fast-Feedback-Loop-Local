add_library(dummy_processor SHARED
  dummy_processor.cpp
)

target_link_libraries(dummy_processor
  PUBLIC
  next_sdk
)
target_include_directories(dummy_processor
  PRIVATE
  ${UDEX_INTERFACE_DIR}
)
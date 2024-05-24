add_library(base64 OBJECT
  base64.cpp
  interface/base64.h
)

target_include_directories(base64
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
)

set_target_properties(base64 PROPERTIES FOLDER "Bridges/BridgeSDK/thirdparty")

if(UNIX)
  target_compile_options(base64 PRIVATE "-Wno-sign-conversion")
endif()

project(tpobjects)

set(PLUGIN_NAME ${PROJECT_NAME})
set(PLUGIN_TYPE example)

add_library(${PLUGIN_NAME} SHARED
    tpobjects.h
    url_mapper.h
    tpobjects.cpp
)

target_link_libraries(${PLUGIN_NAME}
    PRIVATE
        flatbuffers::flatbuffers
        jsoncpp_static
        next_plugin
)

target_include_directories(${PLUGIN_NAME}
    PRIVATE
        ${PLUGINS_INTERFACE_DIR}
        ${BRIDGESDK_INTERFACE_DIR}
)

target_compile_definitions(${PLUGIN_NAME}
    PRIVATE
        BRIDGESDKSHARED
)

set_target_properties(${PLUGIN_NAME}
    PROPERTIES
        FOLDER "Plugins/${PLUGIN_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DATABRIDGE_PLUGINS_OUT_DIR}/${PLUGIN_TYPE}/${PLUGIN_NAME}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${DATABRIDGE_PLUGINS_OUT_DIR}/${PLUGIN_TYPE}/${PLUGIN_NAME}"
)

install(
    TARGETS ${PLUGIN_NAME}
    COMPONENT TARGET_COMPONENT
    DESTINATION bin/plugins/${PLUGIN_TYPE}/${PLUGIN_NAME}
)

install(
    FILES "${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/trafficparticipant.fbs"
    COMPONENT TARGET_COMPONENT
    DESTINATION bin/plugins/${PLUGIN_TYPE}/${PLUGIN_NAME}
)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:${PLUGIN_NAME}> DESTINATION bin/plugins/${PLUGIN_TYPE}/${PLUGIN_NAME} OPTIONAL)
endif()

add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/trafficparticipant.fbs" $<TARGET_FILE_DIR:${PLUGIN_NAME}>
)

# set the paths where to look for other .so files (libraries which are used by the plugins)
# first create a backup of the current setting, then set it to a new value and at the end of the
# file it is set back
set(CMAKE_INSTALL_RPATH_BACKUP ${CMAKE_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH "$\{ORIGIN\};$\{ORIGIN\}/../../../../lib")

message("Default output directory for plugins (if not specified differently): ${DATABRIDGE_PLUGINS_OUT_DIR}")

# keep this list alphabetically sorted, makes it easier for merging
add_subdirectory(core)
add_subdirectory(example)

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH_BACKUP})

install(
    DIRECTORY ${PLUGINS_INTERFACE_DIR}/
    COMPONENT BRIDGES_COMPONENT
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
)

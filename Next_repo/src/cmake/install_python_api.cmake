macro(install_python_api)
    message("====================================================================")
    message("Installing Python API to ${M_INST_PATH}")
    message("====================================================================")
    install(
        DIRECTORY
          py_api/
        COMPONENT NEXT_COMPONENT
        DESTINATION next_python
        FILES_MATCHING
        PATTERN "*"
        PATTERN "__pycache__" EXCLUDE
    )
endmacro()
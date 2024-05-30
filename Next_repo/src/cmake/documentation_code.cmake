find_package(Doxygen REQUIRED)
find_package(Python REQUIRED)

add_custom_target(licenses_file
  ${Python_EXECUTABLE} license_parser.py
  BYPRODUCTS ${CMAKE_SOURCE_DIR}/doc/licenses.md
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/licenses
)

# Quickfix for local plantuml inclusion
set(DOXYGEN_IMAGE_PATH "${CMAKE_SOURCE_DIR}/doc/images")

find_file(DOXYGEN_PLANTUML_JAR_PATH
  plantuml.jar
  "C:/cip_tools/plantuml/1.2019.3"
  "C:/LegacyApp/plantuml"
  "C:/Tools/plantuml"
  /usr/share/plantuml
)
message("-- Plantuml is at " ${DOXYGEN_PLANTUML_JAR_PATH})

# Stylesheet
set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_SOURCE_DIR}/doc/style/style.css)
set(DOXYGEN_HTML_EXTRA_FILES
  ${CMAKE_SOURCE_DIR}/doc/style/doxygen-awesome-darkmode-toggle.js
  ${CMAKE_SOURCE_DIR}/doc/style/doxygen-awesome-fragment-copy-button.js
  ${CMAKE_SOURCE_DIR}/doc/style/doxygen-awesome-paragraph-link.js
  ${CMAKE_SOURCE_DIR}/doc/style/doxygen-awesome-interactive-toc.js
  ${CMAKE_SOURCE_DIR}/doc/style/doxygen-awesome-tabs.js
  ${CMAKE_SOURCE_DIR}/doc/style/toggle-alternative-theme.js
  ${CMAKE_SOURCE_DIR}/doc/style/navtree.js
  ${CMAKE_SOURCE_DIR}/doc/style/background.jpg
  ${CMAKE_SOURCE_DIR}/doc/style/ams-visual-data.jpg
  ${CMAKE_SOURCE_DIR}/doc/style/ContinentalStagSans-Book.otf
  ${CMAKE_SOURCE_DIR}/doc/style/ContinentalStagSans-Medium.otf
)

set(DOXYGEN_ALIASES
  [[glos{1}=[(?)](\ref GLOSSARY_\1)]]
  [[named{1}=__\1__]])

# Templates
set(DOXYGEN_HTML_HEADER ${CMAKE_SOURCE_DIR}/doc/style/headerFile.html)
set(DOXYGEN_HTML_FOOTER ${CMAKE_SOURCE_DIR}/doc/style/footerFile.html)

set(DOXYGEN_HAVE_DOT NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_GENERATE_HTML YES)

# Logo
set(DOXYGEN_PROJECT_LOGO ${CMAKE_SOURCE_DIR}/doc/style/Conti_Logo_with_tagline_black_sRGB.png)

# Doxygen extensions mapping pytest
set(DOXYGEN_EXTENSION_MAPPING pytest=Python)
set(DOXYGEN_FILE_PATTERNS
  *.c, *.cc, *.cxx, *.cpp, *.c++, *.java, *.ii, *.ixx,
  *.ipp, *.i++, *.inl, *.idl, *.ddl, *.odl, *.h, *.hh,
  *.hxx, *.hpp, *.h++, *.l, *.cs, *.d, *.php, *.php4, *.php5,
  *.phtml, *.inc, *.m, *.markdown, *.md, *.mm, *.dox, *.py,
  *.pytest, *.pyw, *.f90, *.f95, *.f03, *.f08, *.f18, *.f, *.for,
  *.vhd, *.vhdl, *.ucf, *.qsf,*.ice)

if (DELIVERY_RELEASE)

# Doxygen Layout
set(DOXYGEN_LAYOUT_FILE ${CMAKE_SOURCE_DIR}/doc/style/DoxygenLayoutDelivery.xml)

doxygen_add_docs(DOCUMENTATION
  ${CMAKE_SOURCE_DIR}/doc/main.md
  ${CMAKE_SOURCE_DIR}/doc/licenses.md
  ${CMAKE_SOURCE_DIR}/doc/glossary.md
  ${CMAKE_SOURCE_DIR}/doc/licenses
  ${CMAKE_SOURCE_DIR}/doc/images
  ${CMAKE_SOURCE_DIR}/doc/tutorial
  ${CMAKE_SOURCE_DIR}/doc/overview
)

else()

# Doxygen Layout
set(DOXYGEN_LAYOUT_FILE ${CMAKE_SOURCE_DIR}/doc/style/DoxygenLayout.xml)

doxygen_add_docs(DOCUMENTATION
  ${CMAKE_SOURCE_DIR}/doc
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/licenses
)

endif()

add_dependencies(DOCUMENTATION
  licenses_file
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
  COMPONENT DOCUMENTATION
  DESTINATION doc/${PROJECT_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/style/
  COMPONENT DOCUMENTATION
  DESTINATION doc/${PROJECT_NAME}/html
)

if(NOT "${DEFAULT_PROJECT_VERSION}" STREQUAL "0.0.0")  
  set(BUILD_VERSION ${DEFAULT_PROJECT_VERSION})
  set(DOCU_ZIP_FILE ../docu.zip)

  install(
    CODE [[
      message("########################  Installing documentation ${BUILD_VERSION} on CIP documentation server")
      message("Uploading with user: $ENV{HTD_CREDS_USR}")
      message("Uploading version: ${BUILD_VERSION}")
      message("packing docu from ${CMAKE_INSTALL_PREFIX}/doc/${PROJECT_NAME}/html")
      message("packing zip file ${DOCU_ZIP_FILE}")
      execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar -cf ${DOCU_ZIP_FILE} --format=zip .
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/doc/${PROJECT_NAME}/html
      )
      message("#############")
      execute_process(
        COMMAND curl -X POST
          -u $ENV{HTD_CREDS_USR}:$ENV{HTD_CREDS_PSW}
          --ssl-no-revoke
          -v
          -F filedata=@${DOCU_ZIP_FILE}
          -F name=\"Next\"
          -F version=\"${BUILD_VERSION}\"
          -F description=\"Next Simulation Framework\"
          https://cip-docs.cmo.conti.de/hmfd
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/doc/${PROJECT_NAME}/html
      )
      message("#############")
    ]]
    COMPONENT DOCUMENTATION
  )
endif(!DEFAULT_PROJECT_VERSION = "0.0.0")
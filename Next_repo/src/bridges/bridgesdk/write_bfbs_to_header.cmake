file(READ ${CMAKE_CURRENT_BINARY_DIR}/view_update.bfbs FBS_VIEW_BINARY HEX)

set(GENERATED_HEADER_FILE_NAME VIEW_UPDATE_GENERATED_HPP)

string(REGEX MATCHALL "([A-Fa-f0-9][A-Fa-f0-9])" SEPARATED_HEX ${FBS_VIEW_BINARY})

# Create a counter so that we only have 16 hex bytes per line
set(counter 0)
# Iterate through each of the bytes from the source file
foreach (hex IN LISTS SEPARATED_HEX)
  math(EXPR counter "${counter}+1")
  if (counter GREATER 16)
    # Write a newline so that all of the array initializer
    # gets spread across multiple lines.
    string(APPEND BFBS_BINARY_STRING "\n    ")
    set(counter 0)
  endif ()
  # Write the hex string to the line with an 0x prefix
  # and a , postfix to seperate the bytes of the file.
  string(APPEND BFBS_BINARY_STRING "0x${hex},")
  # Increment the element counter before the newline.

endforeach ()
string(APPEND BFBS_BINARY_STRING "::")

string(REPLACE ",::" "" BFBS_BINARY_STRING_CLEAN ${BFBS_BINARY_STRING})

set(GENERATED_HEADER_CONTENT "
#ifndef ${GENERATED_HEADER_FILE_NAME}_H
#define ${GENERATED_HEADER_FILE_NAME}_H
#include \"stdint.h\"

inline const unsigned char bfbs_data[] = {
    ${BFBS_BINARY_STRING_CLEAN}
};
#endif // ${GENERATED_HEADER_FILE_NAME}_H
")


file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/view_update.hpp "${GENERATED_HEADER_CONTENT}")

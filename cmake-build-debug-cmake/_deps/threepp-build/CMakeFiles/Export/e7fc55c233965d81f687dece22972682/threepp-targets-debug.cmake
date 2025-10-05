#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "threepp::threepp" for configuration "Debug"
set_property(TARGET threepp::threepp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(threepp::threepp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libthreepp.a"
  )

list(APPEND _cmake_import_check_targets threepp::threepp )
list(APPEND _cmake_import_check_files_for_threepp::threepp "${_IMPORT_PREFIX}/lib/libthreepp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

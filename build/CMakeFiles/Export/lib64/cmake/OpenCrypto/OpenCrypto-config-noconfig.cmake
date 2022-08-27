#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OpenCrypto::OpenCrypto" for configuration ""
set_property(TARGET OpenCrypto::OpenCrypto APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(OpenCrypto::OpenCrypto PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libOpenCrypto.so"
  IMPORTED_SONAME_NOCONFIG "libOpenCrypto.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS OpenCrypto::OpenCrypto )
list(APPEND _IMPORT_CHECK_FILES_FOR_OpenCrypto::OpenCrypto "${_IMPORT_PREFIX}/lib64/libOpenCrypto.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

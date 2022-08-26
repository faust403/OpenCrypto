cmake_minimum_required(VERSION 3.17)

project(
      OpenCrypto
      VERSION 1.0.0
      DESCRIPTION "An C++ library for crpyto, which is partically supporting SIMD and functions for breaking the ciphers"
      LANGUAGES CXX C
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

set(PathToHeaders include/opencrypto)
set(PathToSources src)
set(
      Headers
      ${PathToHeaders}/block.h
      #...
)
set(
      Sources
      ${Headers}
      ${PathToSources}/block.cxx
      #...
)
set(
      Tests
      None
      #...
)
set(
      Benchmarks
      None
      #...
)

option(BUILD_SHARED_LIBS "Build shared libs?" ON)

add_library(OpenCrypto)

target_sources(OpenCrypto PRIVATE ${Sources})

target_include_directories(
      OpenCrypto PUBLIC
      $<BUILD_INTERFACE: ${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_options(
      OpenCrypto PUBLIC
      $<$<CONFIG:DEBUG>: -ggdb; -g3; -O0; -march=native; -lstdc++; -std=c++23>
      $<$<CONFIG:RELEASE>: -O3; -march=native; -lstdc++; -std=c++23>
      $<$<CONFIG:RELEASEWITHDEBUG>: -ggdb; -g3; -O3; -march=native; -lstdc++; -std=c++23>
)

target_compile_definitions(
      OpenCrypto PRIVATE
      $<$<COMPILE_LANG_AND_ID:CXX,Clang>:COMPILING_CXX_WITH_CLANG
      $<$<COMPILE_LANG_AND_ID:CXX,AppleClang>:COMPILING_CXX_WITH_APPLECLANG
      $<$<COMPILE_LANG_AND_ID:CXX,GNU>:COMPILING_CXX_WITH_GCC
      $<$<COMPILE_LANG_AND_ID:CXX,Intel>:COMPILING_CXX_WITH_INTEL
)

install(
      TARGETS OpenCrypto
      EXPORT OpenCryptoPackageConfig
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

export(
      TARGETS OpenCrypto
      FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/opencrpyto-config.cmake
)

install(
      EXPORT OpenCryptoPackageConfig
      DESTINATION ${CMAKE_INSTALL_DATADIR}/OpenCrpyto/cmake
)

write_basic_package_version_file(
      ${CMAKE_CURRENT_BINARY_DIR}/cmake/OpenCryptoVersion.cmake
      VERSION ${OpenCrypto_VERSION}
      COMPATIBILITY AnyNewerVersion
)

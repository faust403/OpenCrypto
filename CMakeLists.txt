cmake_minimum_required(VERSION 3.17)

project(
      OpenCrypto
      VERSION 1.0.0
      DESCRIPTION "An C++ library for crpyto, which is partically supporting SIMD and functions for breaking the ciphers"
      LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(PathToHeaders include/opencrypto)
set(PathToSources src)
set(
      Headers
      ${PathToHeaders}/block.h
      ${PathToHeaders}/constants.h
      #...
)
set(
      Sources
      ${Headers}
      ${PathToSources}/block.cxx
      #...
)

option(BUILD_SHARED_LIBS "Build shared libs?" ON)
option(OPENCRYPTO_INSTALL "Install?" ON)

add_library(OpenCrypto)

target_sources(OpenCrypto PRIVATE ${Sources})

target_include_directories(
      OpenCrypto PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
      $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:COMPILING_CXX_WITH_MSVC
)

if(OPENCRYPTO_INSTALL)
      write_basic_package_version_file(
            ${PROJECT_BINARY_DIR}/cmake/opencrypto-config-version.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion
      )
      install(
            DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      )
      install(
            TARGETS OpenCrypto
            EXPORT opencrypto-config
      )
      install(
            EXPORT opencrypto-config
            NAMESPACE OpenCrypto::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenCrypto
      )
      install(
            FILES ${PROJECT_BINARY_DIR}/cmake/opencrypto-config-version.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OpenCrypto
      )

endif()

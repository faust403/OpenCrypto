cmake_minimum_required(VERSION 3.17)

project(libcryptocxx VERSION 1.0.0 LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(headers
      include/libcryptocxx/block.h
      #...
)
set(sources
      ${headers}
      src/block.cxx
      #...
)

# -D опции указываются на этапе конфигурирования(т.е. перед cmake --build {...})
# cmake -S ./ -B build -D BUILD_SHARED_LIBS=OFF -----=> cmake --build {build, build-so}
# cmake -S ./ -B build-so -D BUILD_SHARED_LIBS=ON --/
option(BUILD_SHARED_LIBS "Shared building mode" ON)
option(DEBUG "Release version" ON)
option(CMAKE_BUILD_TYPE "Library's build type (Debug/Release)" OFF)

# Создаю таргет библиотеки
add_library(libcryptocxx)

# Добавляю сурсы
# INTERFACE - говорит что будет использоваться только снаружи моей цели
# PRIVATE - говорит что будет использоваться только внутри моей цели
# PUBLIC - говорит что будет использоваться и внутри и снаружи моей цели
# скрываю не библиотеки, а исходники
# libcryptocxx:
# public:
#   private:
#      sources [src/block.cxx]
#   interface:
#      includes ["{cmake_current_source_dir}/include"
target_sources(
      libcryptocxx PRIVATE
      ${sources}
)

# Для красивого <libcryptocxx/header.h>
target_include_directories(
      libcryptocxx PUBLIC
      ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(
      libcryptocxx PRIVATE -Wall -march=native -lstdc++
)

if(DEBUG)
      target_compile_options(libcryptocxx PRIVATE -O0 -g)
else()
      target_compile_options(libcryptocxx PRIVATE -O3)
endif()

configure_package_config_file(
      libcryptocxxConfig.cmake.in
      ${PROJECT_SOURCE_DIR}/libcryptocxxConfig.cmake
      INSTALL_DESTINATION ${PROJECT_SOURCE_DIR}
)

cmake_minimum_required(VERSION 3.17)

project(libcryptocxx VERSION 1.0.0 LANGUAGES CXX C)

set(headers
      include/libcryptocxx/block.h
      #...
)
set(sources
      ${headers}
      lib/block.cxx
      #...
)

option(BUILD_SHARED "Shared building" ON)
option(DEBUG "Debug version" ON)

if(BUILD_SHARED)
      add_library(libcryptocxx SHARED)
else()
      add_library(libcryptocxx STATIC)
endif()

if(DEBUG)
      target_compile_optoins(libcryptocxx PUBLIC -Wall -march=native -O0 -g)
else()
      target_compile_optoins(libcryptocxx PUBLIC -Wall -march=native -O3)
endif()

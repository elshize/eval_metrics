cmake_minimum_required(VERSION 3.9)
project(irm VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(irm_BUILD_TESTS "whether or not to build the test" ON)
option(irm_BUILD_TOOLS "whether or not to build the test" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

include(cmake/conantools.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -march=native")

add_library(irm INTERFACE)
add_library(irm::irm ALIAS irm)

target_include_directories(irm INTERFACE
    $<BUILD_INTERFACE:${irm_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
install(DIRECTORY include/irm DESTINATION include)
install(FILES include/irm.hpp DESTINATION include)
install(TARGETS irm EXPORT irm DESTINATION lib)
install(EXPORT irm DESTINATION lib/irm)
export(PACKAGE irm)

if (irm_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if (irm_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


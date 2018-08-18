cmake_minimum_required(VERSION 3.9)
project(eval_metrics VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(eval_metrics_BUILD_TEST "whether or not to build the test" ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

find_package(GTest)

#set(IRKit_CXX_PRIVATE_FEATURES cxx_std_17)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -fno-strict-aliasing -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -march=native")
set(Boost_USE_STATIC_LIBS ON)

add_library(eval_metrics INTERFACE)
add_library(eval_metrics::eval_metrics ALIAS eval_metrics)

target_include_directories(eval_metrics INTERFACE
    $<BUILD_INTERFACE:${eval_metrics_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
install(DIRECTORY include DESTINATION include)
install(TARGETS eval_metrics EXPORT eval_metrics DESTINATION lib)
install(EXPORT eval_metrics DESTINATION lib/eval_metrics)
export(PACKAGE eval_metrics)

if (eval_metrics_BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

#if (IRKit_BUILD_BENCHMARKS)
#    add_subdirectory(benchmarks)
#endif()

cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

# find_package(GTest)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

include(GoogleTest)

set(CMAKE_CXX_FLAGS "${IRM_BUILD_FLAGS}")
add_executable(test_precision "test_precision.cpp")
target_link_libraries(test_precision irm gtest_main gmock_main)
target_compile_features(test_precision PRIVATE cxx_std_17)
gtest_add_tests(TARGET test_precision)

add_executable(test_trec test_trec.cpp)
target_link_libraries(test_trec irm gtest_main gmock_main)
target_compile_features(test_trec PRIVATE cxx_std_17)
gtest_add_tests(TARGET test_trec)

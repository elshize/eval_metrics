cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

find_package(GTest)
include(GoogleTest)

add_executable(test_precision "test_precision.cpp")
target_link_libraries(test_precision irm GTest::GTest)
target_compile_features(test_precision PRIVATE cxx_std_17)
gtest_add_tests(TARGET test_precision)

add_executable(test_trec test_trec.cpp)
target_link_libraries(test_trec irm GTest::GTest)
target_compile_features(test_trec PRIVATE cxx_std_17)
gtest_add_tests(TARGET test_trec)

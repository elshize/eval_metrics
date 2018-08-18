cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

include(GoogleTest)

macro(add_test TEST_NAME)
    add_executable(${TEST_NAME} "${TEST_NAME}.cpp")
    target_link_libraries(${TEST_NAME} eval_metrics gtest)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)
    #gtest_add_tests(TARGET ${TEST_NAME} TEST_PREFIX "xxx")
endmacro()

add_test(test_precision)

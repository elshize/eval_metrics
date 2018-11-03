find_package(CLI11)

set(CMAKE_CXX_FLAGS "${IRM_BUILD_FLAGS}")
add_executable(irm-eval "irm.cpp")
target_link_libraries(irm-eval irm CLI11::CLI11)
target_compile_features(irm-eval PRIVATE cxx_std_17)
set_target_properties(irm-eval PROPERTIES OUTPUT_NAME irm)

install(TARGETS irm-eval DESTINATION bin)

PROJECT(PackageTest)
cmake_minimum_required(VERSION 2.8.12)

find_package(irm REQUIRED)
add_executable(example example.cpp)
target_link_libraries(example irm::irm)
target_compile_features(example PRIVATE cxx_std_17)

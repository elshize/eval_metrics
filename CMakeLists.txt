cmake_minimum_required(VERSION 3.9)
project(irm VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(irm_BUILD_TESTS "whether or not to build the test" ON)
option(irm_BUILD_TOOLS "whether or not to build the test" ON)

##
## CONFIGURATION
##
set(IRM_TARGET_NAME               ${PROJECT_NAME})
set(IRM_CONFIG_INSTALL_DIR        "lib/cmake/${PROJECT_NAME}"
  CACHE INTERNAL "")
set(IRM_INCLUDE_INSTALL_DIR       "include")
set(IRM_TARGETS_EXPORT_NAME       "${PROJECT_NAME}Targets")
set(IRM_CMAKE_CONFIG_TEMPLATE     "cmake/config.cmake.in")
set(IRM_CMAKE_CONFIG_DIR          "${CMAKE_CURRENT_BINARY_DIR}")
set(IRM_CMAKE_VERSION_CONFIG_FILE "${IRM_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(IRM_CMAKE_PROJECT_CONFIG_FILE "${IRM_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake")
set(IRM_CMAKE_PROJECT_TARGETS_FILE "${IRM_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake")

add_library(irm INTERFACE)
add_library(irm::irm ALIAS irm)

find_package(Boost REQUIRED)
target_link_libraries(irm INTERFACE Boost::boost)

target_include_directories(irm INTERFACE
    $<BUILD_INTERFACE:${irm_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

set(IRM_BUILD_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -march=native")
if (irm_BUILD_TOOLS)
    add_subdirectory(tools)
endif()
if (irm_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

##
## INSTALL
## install header files, generate and install cmake config files for find_package()
##
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${IRM_CMAKE_VERSION_CONFIG_FILE} COMPATIBILITY SameMajorVersion
)
configure_file(
    ${IRM_CMAKE_CONFIG_TEMPLATE}
    ${IRM_CMAKE_PROJECT_CONFIG_FILE}
    @ONLY
)

install(
    DIRECTORY ${IRM_INCLUDE_BUILD_DIR}
    DESTINATION ${IRM_INCLUDE_INSTALL_DIR}
)
install(
    FILES ${IRM_CMAKE_PROJECT_CONFIG_FILE} ${IRM_CMAKE_VERSION_CONFIG_FILE}
    DESTINATION ${IRM_CONFIG_INSTALL_DIR}
)
export(
    TARGETS ${IRM_TARGET_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${IRM_CMAKE_PROJECT_TARGETS_FILE}
)
install(
    TARGETS ${IRM_TARGET_NAME}
    EXPORT ${IRM_TARGETS_EXPORT_NAME}
    INCLUDES DESTINATION ${IRM_INCLUDE_INSTALL_DIR}
)
install(
    EXPORT ${IRM_TARGETS_EXPORT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${IRM_CONFIG_INSTALL_DIR}
)

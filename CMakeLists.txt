cmake_minimum_required(VERSION 3.10)
project(boxed-cpp VERSION "1.2.0" LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(ThirdParties)
include(ClangTidy)
include(PedanticCompiler)

set(boxed_cpp_HEADERS
    ${PROJECT_SOURCE_DIR}/include/boxed-cpp/boxed.hpp
)
add_library(boxed-cpp INTERFACE)
add_library(boxed-cpp::boxed-cpp ALIAS boxed-cpp)

target_compile_features(boxed-cpp INTERFACE cxx_std_20)
target_include_directories(boxed-cpp INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Generate the version, config and target files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/boxed-cpp-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(boxed-cpp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/boxed-cpp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boxed-cpp
)

install(FILES ${boxed_cpp_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/boxed-cpp
)
install(TARGETS boxed-cpp
    EXPORT boxed-cpp-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/boxed-cpp-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/boxed-cpp-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boxed-cpp
)
install(EXPORT boxed-cpp-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boxed-cpp
    NAMESPACE boxed-cpp::
)

# ---------------------------------------------------------------------------
# unit tests

option(BOXED_CPP_TESTS "Enables building of unittests for boxed-cpp [default: OFF]" OFF)
if(BOXED_CPP_TESTS)
    find_package(Catch2 3.4.0 QUIET)
    if(NOT Catch2_FOUND)
        ThirdPartiesAdd_Catch2()
    endif()
    enable_testing()
    add_executable(test-boxed-cpp
        test-boxed-cpp.cpp
    )
    target_link_libraries(test-boxed-cpp boxed-cpp Catch2::Catch2WithMain)
    add_test(test-boxed-cpp ./test-boxed-cpp)
endif()
message(STATUS "[boxed-cpp] Compile unit tests: ${BOXED_CPP_TESTS}")

cmake_minimum_required(VERSION 3.10)
project(boxed-cpp VERSION "1.0.1" LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ThirdParties)
include(ClangTidy)
include(PedanticCompiler)

set(boxed_cpp_HEADERS
    include/boxed-cpp/boxed.hpp
)
add_library(boxed-cpp INTERFACE)

target_compile_features(boxed-cpp INTERFACE cxx_std_20)
target_include_directories(boxed-cpp INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
)

# ---------------------------------------------------------------------------
# unit tests

option(BOXED_CPP_TESTS "Enables building of unittests for boxed-cpp [default: ON]" OFF)
if(BOXED_CPP_TESTS)
    ThirdPartiesAdd_Catch2()
    enable_testing()
    add_executable(test-boxed-cpp
        test-boxed-cpp.cpp
        test-main.cpp
    )
    target_link_libraries(test-boxed-cpp boxed-cpp Catch2::Catch2)
    add_test(test-boxed-cpp ./test-boxed-cpp)
endif()
message(STATUS "[boxed-cpp] Compile unit tests: ${BOXED_CPP_TESTS}")

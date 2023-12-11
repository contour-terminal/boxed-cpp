set(THIRDPARTIES_HAS_FETCHCONTENT ON)
# if(${CMAKE_VERSION} VERSION_LESS 3.11)
#     set(THIRDPARTIES_HAS_FETCHCONTENT OFF)
# endif()

if(THIRDPARTIES_HAS_FETCHCONTENT)
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
else()
    include(DownloadProject)
endif()

if(NOT FETCHCONTENT_BASE_DIR STREQUAL "")
    set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/3rdparty")
endif()

set(3rdparty_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_downloads" CACHE FILEPATH "3rdparty download directory.")
message(STATUS "base dir: ${FETCHCONTENT_BASE_DIR}")
message(STATUS "dnld dir: ${3rdparty_DOWNLOAD_DIR}")

macro(ThirdPartiesAdd_fmtlib)
    set(3rdparty_fmtlib_VERSION "8.0.0" CACHE STRING "fmtlib version")
    set(3rdparty_fmtlib_CHECKSUM "SHA256=7bce0e9e022e586b178b150002e7c2339994e3c2bbe44027e9abb0d60f9cce83" CACHE STRING "fmtlib checksum")
    set(3rdparty_fmtlib_URL "https://github.com/fmtlib/fmt/archive/refs/tags/${3rdparty_fmtlib_VERSION}.tar.gz")
    if(THIRDPARTIES_HAS_FETCHCONTENT)
        FetchContent_Declare(
            fmtlib
            URL "${3rdparty_fmtlib_URL}"
            URL_HASH "${3rdparty_fmtlib_CHECKSUM}"
            DOWNLOAD_DIR "${3rdparty_DOWNLOAD_DIR}"
            DOWNLOAD_NAME "fmtlib-${3rdparty_fmtlib_VERSION}.tar.gz"
            EXCLUDE_FROM_ALL
        )
        FetchContent_MakeAvailable(fmtlib)
    else()
        download_project(
            PROJ fmtlib
            URL "${3rdparty_fmtlib_URL}"
            URL_HASH "${3rdparty_fmtlib_CHECKSUM}"
            PREFIX "${FETCHCONTENT_BASE_DIR}/fmtlib-${3rdparty_fmtlib_VERSION}"
            DOWNLOAD_DIR "${3rdparty_DOWNLOAD_DIR}"
            DOWNLOAD_NAME "fmtlib-${3rdparty_fmtlib_VERSION}.tar.gz"
            EXCLUDE_FROM_ALL
            UPDATE_DISCONNECTED 1
        )
    endif()
endmacro()

macro(ThirdPartiesAdd_Catch2)
    set(3rdparty_Catch2_VERSION "3.4.0" CACHE STRING "Embedded catch2 version")
    set(3rdparty_Catch2_CHECKSUM "SHA256=122928b814b75717316c71af69bd2b43387643ba076a6ec16e7882bfb2dfacbb" CACHE STRING "Embedded catch2 checksum")
    set(3rdparty_Catch2_URL "https://github.com/catchorg/Catch2/archive/refs/tags/v${3rdparty_Catch2_VERSION}.tar.gz")
    set(CATCH_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(CATCH_BUILD_EXTRA_TESTS OFF CACHE INTERNAL "")
    set(CATCH_BUILD_TESTING OFF CACHE INTERNAL "")
    set(CATCH_ENABLE_WERROR OFF CACHE INTERNAL "")
    set(CATCH_INSTALL_DOCS OFF CACHE INTERNAL "")
    set(CATCH_INSTALL_HELPERS OFF CACHE INTERNAL "")
    if(THIRDPARTIES_HAS_FETCHCONTENT)
        FetchContent_Declare(
            Catch2
            URL "${3rdparty_Catch2_URL}"
            URL_HASH "${3rdparty_Catch2_CHECKSUM}"
            DOWNLOAD_DIR "${3rdparty_DOWNLOAD_DIR}"
            DOWNLOAD_NAME "catch2-${3rdparty_Catch2_VERSION}.tar.gz"
            EXCLUDE_FROM_ALL
            DOWNLOAD_EXTRACT_TIMESTAMP 1
        )
        FetchContent_MakeAvailable(Catch2)
    else()
        download_project(
            PROJ Catch2
            URL "${3rdparty_Catch2_URL}"
            URL_HASH "${3rdparty_Catch2_CHECKSUM}"
            PREFIX "${FETCHCONTENT_BASE_DIR}/Catch2-${3rdparty_Catch2_VERSION}"
            DOWNLOAD_DIR "${3rdparty_DOWNLOAD_DIR}"
            DOWNLOAD_NAME "catch2-${3rdparty_Catch2_VERSION}.tar.gz"
            EXCLUDE_FROM_ALL
            DOWNLOAD_EXTRACT_TIMESTAMP 1
        )
    endif()
endmacro()

macro(ThirdPartiesAdd_range_v3)
    set(3rdparty_range_v3_VERSION "0.11.0" CACHE STRING "Embedded range-v3 version")
    set(3rdparty_range_v3_CHECKSUM "MD5=97ab1653f3aa5f9e3d8200ee2a4911d3" CACHE STRING "Embedded range-v3 hash")
    if(THIRDPARTIES_HAS_FETCHCONTENT)
        FetchContent_GetProperties(range_v3)
        if(NOT "${range_v3_POPULATED}")
            FetchContent_Populate(
                range_v3
                URL "https://github.com/ericniebler/range-v3/archive/${3rdparty_range_v3_VERSION}.tar.gz"
                URL_HASH "${3rdparty_range_v3_CHECKSUM}"
                DOWNLOAD_DIR "${3rdparty_DOWNLOAD_DIR}"
                DOWNLOAD_NAME "range-v3-${3rdparty_range_v3_VERSION}.tar.gz"
                EXCLUDE_FROM_ALL
            )
            add_subdirectory(${range_v3_SOURCE_DIR} ${range_v3_BINARY_DIR} EXCLUDE_FROM_ALL)
            # ^^^ That's the only way to avoid installing this dependency during install step.
        endif()
    else()
        download_project(
            PROJ range-v3
            URL "https://github.com/ericniebler/range-v3/archive/${3rdparty_range_v3_VERSION}.tar.gz"
            URL_HASH "${3rdparty_range_v3_CHECKSUM}"
            DOWNLOAD_DIR "${3rdparty_DOWNLOAD_DIR}"
            DOWNLOAD_NAME "range-v3-${3rdparty_range_v3_VERSION}.tar.gz"
            EXCLUDE_FROM_ALL
            PREFIX "${FETCHCONTENT_BASE_DIR}/range-v3-${3rdparty_range_v3_VERSION}"
        )
    endif()
endmacro()

add_definitions("-DHAVE_UTILS_CONFIG")

cmake_minimum_required(VERSION 3.5)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

option(DISABLE_XDGBASEDIR "disable searching for libxdg-basedir" OFF)
option(ENABLE_TESTS "build unit tests" ON)
option(COLORED_OUTPUT "Colored output" ON)
option(STANDALONE "Not used as a submodule of another project" OFF)
option(COVERAGE "Enable code coverage" OFF)
option(PERF_TRACE "Enable time doctor performance traces" OFF)

find_package(PkgConfig)

if (NOT DISABLE_XDGBASEDIR)
    pkg_check_modules(XDGBASEDIR_PKG libxdg-basedir)
    if (XDGBASEDIR_PKG_FOUND)
        option(HAVE_XDGBASEDIR "xdg-basedir support" ON)

        find_path(XDGBASEDIR_INCLUDE_DIR
            NAMES basedir.h
            PATHS ${XDGBASEDIR_PKG_INCLUDE_DIRS}
        )

        find_library(XDGBASEDIR_LIBRARY
            NAMES ${XDGBASEDIR_PKG_LIBRARIES}
            PATHS ${XDGBASEDIR_PKG_LIBRARY_DIRS}
        )
    endif ()
endif ()

pkg_check_modules(LIBUNWIND_PKG libunwind)
if (LIBUNWIND_PKG_FOUND)
    find_path(LIBUNWIND_INCLUDE_DIR
        NAMES libunwind.h
        PATHS ${LIBUNWIND_PKG_INCLUDE_DIRS}
    )

    find_library(LIBUNWIND_LIBRARY
        NAMES ${LIBUNWIND_PKG_LIBRARIES}
        PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
    )

    find_library(LIBUNWINDARM_LIBRARY
        NAMES unwind-arm
        PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
    )

    if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        find_library(LIBUNWINDNATIVE_LIBRARY
            NAMES unwind-x86_64
            PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
        )
    ELSE ()
        find_library(LIBUNWINDNATIVE_LIBRARY
            NAMES unwind-x86
            PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
        )
    endif()

    if (NOT APPLE)
        option(HAVE_LIBUNWIND "libunwind support" ON)
        set(LIBUNWIND_LIBRARIES ${LIBUNWIND_LIBRARY})
        # insert native library before generic library to avoid linker errors
        if (LIBUNWINDARM_LIBRARY)
            list(INSERT LIBUNWIND_LIBRARIES 0 ${LIBUNWINDARM_LIBRARY})
        endif ()
        if (LIBUNWINDNATIVE_LIBRARY)
            list(INSERT LIBUNWIND_LIBRARIES 0 ${LIBUNWINDNATIVE_LIBRARY})
        endif ()
    endif ()
endif ()

SET (UTILS_SOURCES
    inc/utils/bufferedreader.h      src/bufferedreader.cpp
    inc/utils/fileoperations.h      src/fileoperations.cpp
    inc/utils/filereader.h          src/filereader.cpp
    inc/utils/format.h              src/format.cpp
    inc/utils/functiontraits.h
    inc/utils/log.h                 src/log.cpp
    inc/utils/numericoperations.h
    inc/utils/readerinterface.h
    inc/utils/readerfactory.h       src/readerfactory.cpp
    inc/utils/signal.h
    inc/utils/simplesubscriber.h
    inc/utils/stringoperations.h
    inc/utils/subscriber.h
    inc/utils/timeoperations.h
    inc/utils/timerthread.h
    inc/utils/threadpool.h          src/threadpool.cpp
    inc/utils/trace.h               src/trace.cpp
    inc/utils/workerthread.h        src/workerthread.cpp
    inc/utils/backtrace.h           src/backtrace.cpp
)

ADD_LIBRARY(utils STATIC
    ${UTILS_SOURCES}
)

if (STANDALONE)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if (NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
    endif ()
endif()

if (COVERAGE)
    message(STATUS "Coverage enabled")
    include(CodeCoverage)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_COVERAGE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_COVERAGE}")
endif()

target_include_directories(utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_BINARY_DIR}
)

target_include_directories(utils SYSTEM PRIVATE
    ${XDGBASEDIR_INCLUDE_DIR}
    ${LIBUNWIND_INCLUDE_DIR}
)

find_package(Threads)
target_link_libraries(utils ${BFD_LIBRARIES} ${XDGBASEDIR_LIBRARY} ${LIBUNWIND_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utilsconfig.h.in ${CMAKE_BINARY_DIR}/utilsconfig.h)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test" AND ENABLE_TESTS)
    add_subdirectory(test)
endif ()
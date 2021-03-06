add_definitions("-DHAVE_UTILS_CONFIG")

cmake_minimum_required(VERSION 3.1)
enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(DISABLE_XDGBASEDIR "disable searching for libxdg-basedir" OFF)
option(ENABLE_TESTS "build unit tests" ON)
option(COLORED_OUTPUT "Colored output" ON)
option(STANDALONE "Not used as a submodule of another project" OFF)
option(COVERAGE "Enable code coverage" OFF)
option(PERF_TRACE "Enable time doctor performance traces" OFF)
option(UTILS_ENABLE_BENCHMARKS "Enable performance benchmarks" OFF)

if (STANDALONE)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if (NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
    endif ()
endif()

find_package(PkgConfig)
find_package(fmt CONFIG REQUIRED)
find_package(Libunwind)

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

if (TARGET Libunwind::Libunwind)
    set(HAVE_LIBUNWIND TRUE)
endif ()

ADD_LIBRARY(utils STATIC
    inc/utils/bufferedreader.h      src/bufferedreader.cpp
    inc/utils/enumflags.h
    inc/utils/fileoperations.h      src/fileoperations.cpp
    inc/utils/filereader.h          src/filereader.cpp
    inc/utils/format.h
    inc/utils/functiontraits.h
    inc/utils/log.h                 src/log.cpp
    inc/utils/readerinterface.h
    inc/utils/readerfactory.h       src/readerfactory.cpp
    inc/utils/signal.h
    inc/utils/simplesubscriber.h
    inc/utils/stringoperations.h    src/stringoperations.cpp
    inc/utils/subscriber.h
    inc/utils/timeoperations.h
    inc/utils/timerthread.h
    inc/utils/threadpool.h          src/threadpool.cpp
    inc/utils/trace.h               src/trace.cpp
    inc/utils/traits.h
    inc/utils/workerthread.h        src/workerthread.cpp
    inc/utils/backtrace.h           src/backtrace.cpp
)

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
)

find_package(Threads)
target_link_libraries(utils
    fmt::fmt
    $<$<BOOL:${Libunwund_FOUND}>:Libunwind::Libunwind>
    ${BFD_LIBRARIES}
    ${XDGBASEDIR_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utilsconfig.h.in ${CMAKE_BINARY_DIR}/utilsconfig.h)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test" AND ENABLE_TESTS)
    add_subdirectory(test)
endif ()

if (UTILS_ENABLE_BENCHMARKS)
    add_subdirectory(bench)
endif ()

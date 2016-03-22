ADD_DEFINITIONS("-DHAVE_UTILS_CONFIG")

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_TESTING()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

OPTION(DISABLE_XDGBASEDIR "disable searching for libxdg-basedir" OFF)
OPTION(ENABLE_TESTS "build unit tests" ON)
OPTION(COLORED_OUTPUT "Colored output" ON)
OPTION(STANDALONE "Not used as a submodule of another project" OFF)
OPTION(COVERAGE "Enable code coverage" OFF)
OPTION(PERF_TRACE "Enable time doctor performance traces" OFF)

FIND_PACKAGE(PkgConfig)

IF (NOT DISABLE_XDGBASEDIR)
    PKG_CHECK_MODULES(XDGBASEDIR_PKG libxdg-basedir)
    IF (XDGBASEDIR_PKG_FOUND)
        OPTION(HAVE_XDGBASEDIR "xdg-basedir support" ON)

        FIND_PATH(XDGBASEDIR_INCLUDE_DIR
            NAMES basedir.h
            PATHS ${XDGBASEDIR_PKG_INCLUDE_DIRS}
        )

        FIND_LIBRARY(XDGBASEDIR_LIBRARY
            NAMES ${XDGBASEDIR_PKG_LIBRARIES}
            PATHS ${XDGBASEDIR_PKG_LIBRARY_DIRS}
        )
    ENDIF ()
ENDIF ()

PKG_CHECK_MODULES(LIBUNWIND_PKG libunwind)
IF (LIBUNWIND_PKG_FOUND)
    FIND_PATH(LIBUNWIND_INCLUDE_DIR
        NAMES libunwind.h
        PATHS ${LIBUNWIND_PKG_INCLUDE_DIRS}
    )

    FIND_LIBRARY(LIBUNWIND_LIBRARY
        NAMES ${LIBUNWIND_PKG_LIBRARIES}
        PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
    )

    FIND_LIBRARY(LIBUNWINDARM_LIBRARY
        NAMES unwind-arm
        PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
    )

    IF ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        FIND_LIBRARY(LIBUNWINDNATIVE_LIBRARY
            NAMES unwind-x86_64
            PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
        )
    ELSE ()
        FIND_LIBRARY(LIBUNWINDNATIVE_LIBRARY
            NAMES unwind-x86
            PATHS ${LIBUNWIND_PKG_LIBRARY_DIRS}
        )
    ENDIF()

    IF (NOT APPLE)
        OPTION(HAVE_LIBUNWIND "libunwind support" ON)
        SET(LIBUNWIND_LIBRARIES ${LIBUNWIND_LIBRARY})
        IF (LIBUNWINDARM_LIBRARY)
            LIST(APPEND LIBUNWIND_LIBRARIES ${LIBUNWINDARM_LIBRARY})
        ENDIF ()
        IF (LIBUNWINDNATIVE_LIBRARY)
            LIST(APPEND LIBUNWIND_LIBRARIES ${LIBUNWINDNATIVE_LIBRARY})
        ENDIF ()
    ENDIF ()
ENDIF ()

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

IF (STANDALONE AND NOT MSVC)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")

    ADD_DEFINITIONS("-std=c++1y")
    IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND APPLE)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    ENDIF ()
ENDIF()

IF (COVERAGE)
    MESSAGE(STATUS "Coverage enabled")
    INCLUDE(CodeCoverage)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_COVERAGE}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_COVERAGE}")
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc ${XDGBASEDIR_INCLUDE_DIR} ${LIBUNWIND_INCLUDE_DIR})

SET (UTILS_LIBRARIES ${BFD_LIBRARIES} ${XDGBASEDIR_LIBRARY} ${LIBUNWIND_LIBRARIES})
IF (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android" AND NOT MSVC)
    LIST(APPEND UTILS_LIBRARIES -lpthread)
ENDIF ()
TARGET_LINK_LIBRARIES(utils ${UTILS_LIBRARIES})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/utilsconfig.h.in ${CMAKE_BINARY_DIR}/utilsconfig.h)

IF (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test" AND ENABLE_TESTS)
    ADD_SUBDIRECTORY(test)
ENDIF ()
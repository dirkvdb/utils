CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_TESTING()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

OPTION(DISABLE_XDGBASEDIR "disable searching for libxdg-basedir" OFF)
OPTION(BACKWARD "enable backtrace support" OFF)

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

IF (BACKWARD)
    ADD_SUBDIRECTORY(src/backward-cpp)
    LIST(APPEND UTILS_SOURCES
        ${backward_ENABLE}
    )
ENDIF()

ADD_LIBRARY(utils STATIC
    ${UTILS_SOURCES}
)

OPTION(COLORED_OUTPUT "Colored output" ON)
OPTION(STANDALONE "Not used as a submodule of another project" OFF)
OPTION(COVERAGE "Enable code coverage" OFF)
OPTION(PERF_TRACE "Enable time doctor performance traces" OFF)

ADD_DEFINITIONS("-DHAVE_UTILS_CONFIG")

IF (NOT MSVC)
    SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wfatal-errors")

    ADD_DEFINITIONS("-std=c++1y")
    IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND APPLE)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    ENDIF ()

    IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Gcc")
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -rdynamic ")
    ENDIF ()
ENDIF()

IF (COVERAGE)
    MESSAGE(STATUS "Coverage enabled")
    INCLUDE(CodeCoverage)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    IF(CMAKE_COMPILER_IS_GNUCXX)
        SET(COVERAGE_LIBRARY gcov)
        SET(CMAKE_LDFLAGS ${CMAKE_LDFLAGS} -fprofile-arcs -ftest-coverage)
    ELSE ()
        SET(CMAKE_LDFLAGS ${CMAKE_LDFLAGS} --coverage)
    ENDIF()
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc ${XDGBASEDIR_INCLUDE_DIR})

SET (UTILS_LIBRARIES ${BFD_LIBRARIES} ${XDGBASEDIR_LIBRARY})
IF (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android" AND NOT MSVC)
    LIST(APPEND UTILS_LIBRARIES -lpthread)
ENDIF ()
TARGET_LINK_LIBRARIES(utils ${UTILS_LIBRARIES})

IF (UNIX OR APPLE)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONSOLE_SUPPORTS_COLOR")
ENDIF (UNIX OR APPLE)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/utilsconfig.h)

ADD_SUBDIRECTORY(test)

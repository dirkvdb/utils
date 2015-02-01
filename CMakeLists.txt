CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_TESTING()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

FIND_PACKAGE(PkgConfig)
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
ENDIF (XDGBASEDIR_PKG_FOUND)

FIND_LIBRARY(BFD_LIBRARY
    NAMES bfd
)

IF (BFD_LIBRARY)
    ADD_DEFINITIONS(-DBACKWARD_HAS_BFD=1)
    SET(BFD_LIBRARIES ${BFD_LIBRARY})
ENDIF()

ADD_LIBRARY(utils STATIC
	inc/utils/backtrace.h      		src/backtrace.cpp
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
    inc/utils/types.h
    inc/utils/workerthread.h        src/workerthread.cpp
)

OPTION(COLORED_OUTPUT "Colored output" ON)
OPTION(STANDALONE "Not used as a submodule of another project" OFF)
OPTION(COVERAGE "Enable code coverage" OFF)
OPTION(PERF_TRACE "Enable time doctor performance traces" OFF)

ADD_DEFINITIONS("-DHAVE_UTILS_CONFIG")
ADD_DEFINITIONS("-std=c++1y")
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
ENDIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Gcc")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -rdynamic ")
ENDIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Gcc")

SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wfatal-errors")

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
SET (UTILS_INCLUDE_DIRS utils ${XDGBASEDIR_INCLUDE_DIR})

SET (UTILS_LIBRARIES utils ${BFD_LIBRARIES} ${XDGBASEDIR_LIBRARY})
IF (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    LIST(APPEND UTILS_LIBRARIES -lpthread)
ENDIF ()

IF (UNIX OR APPLE)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONSOLE_SUPPORTS_COLOR")
ENDIF (UNIX OR APPLE)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/utilsconfig.h)

ADD_SUBDIRECTORY(test)

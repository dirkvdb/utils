CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ENABLE_TESTING()

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(XDGBASEDIR libxdg-basedir)

IF (XDGBASEDIR_FOUND)
    OPTION(HAVE_XDGBASEDIR "xdg-basedir support" ON)
ENDIF (XDGBASEDIR_FOUND)

ADD_LIBRARY(utils STATIC
    inc/utils/fileoperations.h
    src/fileoperations.cpp
    inc/utils/functiontraits.h
    inc/utils/log.h
    src/log.cpp
    inc/utils/numericoperations.h
    inc/utils/readerinterface.h
    inc/utils/signal.h
    inc/utils/simplesubscriber.h
    inc/utils/stringoperations.h
    inc/utils/subscriber.h
    inc/utils/timeoperations.h
    inc/utils/timerthread.h
    inc/utils/trace.h
    inc/utils/types.h
    inc/utils/workerthread.h
)

OPTION(COLORED_OUTPUT "Colored output" ON)

ADD_DEFINITIONS("-DHAVE_UTILS_CONFIG")
ADD_DEFINITIONS("-std=c++11")
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "-stdlib=libc++")
ENDIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/inc ${XDGBASEDIR_INCLUDE_DIRS})
SET (UTILS_LIBRARIES utils ${XDGBASEDIR_LIBRARIES})
SET (UTILS_INCLUDE_DIRS utils ${XDGBASEDIR_INCLUDE_DIRS})

SET (CMAKE_CXX_FLAGS_DEBUG "-g -rdynamic -O0 -Wall -Wfatal-errors -Wno-deprecated-declarations")

IF (UNIX OR APPLE)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCONSOLE_SUPPORTS_COLOR")
ENDIF (UNIX OR APPLE)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/utilsconfig.h)

ADD_SUBDIRECTORY(test)

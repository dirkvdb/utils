CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_LIBRARY(utils STATIC
    fileoperations.h fileoperations.cpp
    functiontraits.h
    log.h
    numericoperations.h
    signal.h
    simplesubscriber.h
    stringoperations.h
    subscriber.h
    timeoperations.h
    trace.h
    types.h
)

SET (CMAKE_CXX_FLAGS "-std=c++0x")
SET (CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Werror -Wfatal-errors -Wno-deprecated-declarations")
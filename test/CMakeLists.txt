CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(
	${XDGBASEDIR_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../inc/
)

LINK_DIRECTORIES(
	${XDGBASEDIR_LIBRARY_DIRS}
)

ADD_EXECUTABLE(utilstest EXCLUDE_FROM_ALL
    bufferedreadertest.cpp
    fileoperationstest.cpp
    gmock-gtest-all.cpp
    main.cpp
    signaltest.cpp
    stringoperationstest.cpp
    tracetest.cpp
    threadpooltest.cpp
    workerthreadtest.cpp
)

IF (COVERAGE)
    SETUP_TARGET_FOR_COVERAGE(utils_coverage utilstest coverage)
ENDIF ()

TARGET_LINK_LIBRARIES(utilstest
	utils
    ${COVERAGE_LIBRARY}
)

IF (STANDALONE)
    ADD_CUSTOM_TARGET(check)
ENDIF (STANDALONE)

IF(TARGET check)
    ADD_TEST(NAME UtilsTests COMMAND utilstest)
    ADD_DEPENDENCIES(check utilstest)
ENDIF()
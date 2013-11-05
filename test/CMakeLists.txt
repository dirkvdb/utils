CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
GET_DIRECTORY_PROPERTY(UTILS_LIBRARIES  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.." DEFINITION UTILS_LIBRARIES)

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
    gmock-gtest-all.cc
    main.cpp
    signaltest.cpp
    stringoperationstest.cpp
    threadpooltest.cpp
    workerthreadtest.cpp
)

TARGET_LINK_LIBRARIES(utilstest
	-lpthread
    ${UTILS_LIBRARIES}
	${XDGBASEDIR_LIBRARIES}
)

ADD_TEST(UtilsTests utilstest)
ADD_DEPENDENCIES(check utilstest)
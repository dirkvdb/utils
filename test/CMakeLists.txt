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

ADD_EXECUTABLE(utilstest
	bufferedreadertest.cpp
	gmock-gtest-all.cc
	main.cpp
	signaltest.cpp
	stringoperationstest.cpp
	fileoperationstest.cpp
)

TARGET_LINK_LIBRARIES(utilstest
	utils
	-lpthread
	${XDGBASEDIR_LIBRARIES}
)

ADD_TEST(UtilsTests utilstest)
 

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ENABLE_TESTING()

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(XDGBASEDIR libxdg-basedir REQUIRED)

INCLUDE_DIRECTORIES(
	${XDGBASEDIR_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/../inc/
)

ADD_EXECUTABLE(unittest
	gmock-gtest-all.cc
	main.cpp
	signaltest.cpp
	stringoperationstest.cpp
	fileoperationstest.cpp
)

SET (CMAKE_CXX_FLAGS "-std=c++0x")

TARGET_LINK_LIBRARIES(unittest
	utils
	-lpthread
	${XDGBASEDIR_LIBRARIES}
)

ADD_TEST(UtilsTests unittest)
 

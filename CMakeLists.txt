cmake_minimum_required(VERSION 3.2)

set (PROJECT_NAME TinyHttp)
project(${PROJECT_NAME}  CXX)

option(USE_OPENRC "Install openrc run script" ON)
option(BUILD_TESTS "Build unit tests" ON)

find_package(Poco REQUIRED Foundation Net Util JSON XML)
#set (LIBS ${LIBS} ${Poco_LIBRARIES})

find_package(Boost 1.56 COMPONENTS program_options locale system filesystem REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )
#set (LIBS ${LIBS} ${Boost_LIBRARIES})

add_definitions("-std=c++11"
                "-fno-elide-constructors"
                "-pedantic-errors"
                "-Wextra"
                "-Wall"
                "-Winit-self"
                "-Wold-style-cast"
                "-Woverloaded-virtual"
                "-Wuninitialized"
                "-Wmissing-declarations"
                "-Wstrict-aliasing")

add_subdirectory(src)
add_subdirectory(resources)

if (BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif(BUILD_TESTS)

configure_file(${CMAKE_SOURCE_DIR}/files/server.xml.in ${CMAKE_BINARY_DIR}/src/server.xml)
configure_file(${CMAKE_SOURCE_DIR}/files/localhost.xml.in ${CMAKE_BINARY_DIR}/src/vhosts.d/localhost.xml)
configure_file(${CMAKE_SOURCE_DIR}/files/resources.xml.in ${CMAKE_BINARY_DIR}/src/vhosts.d/resources.xml)

install(FILES ${CMAKE_BINARY_DIR}/src/server.xml DESTINATION etc/tinyhttp.d)
install(FILES ${CMAKE_BINARY_DIR}/src/vhosts.d/localhost.xml DESTINATION etc/tinyhttp.d/vhosts.d)
install(FILES ${CMAKE_BINARY_DIR}/src/vhosts.d/resources.xml DESTINATION etc/tinyhttp.d/vhosts.d)

if (USE_OPENRC)
	configure_file(${CMAKE_SOURCE_DIR}/files/launch/openrc/tinyhttpd.in ${CMAKE_BINARY_DIR}/files/launch/openrc/tinyhttpd)
	install(FILES ${CMAKE_BINARY_DIR}/files/launch/openrc/tinyhttpd
		DESTINATION etc/init.d
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)
endif(USE_OPENRC)

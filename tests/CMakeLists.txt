include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

set (TESTS
	Fake
	FileRange
	HeaderParsers
	HttpServer
	StreamCopy
	Server
)

foreach(test ${TESTS})
	add_test(NAME ${test}
			COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests_main
			--gtest_filter="Test${test}.*")
	set (TEST_SOURCES ${TEST_SOURCES} Test${test}.cpp)
endforeach()

add_executable(tests_main ${TEST_SOURCES})
target_link_libraries(tests_main
	${GTEST_LIBRARIES}
	${GTEST_MAIN_LIBRARY}
	${LIBS}
	tinyhttp_daemon
	tinyhttp_cpsp
	tinyhttp_api
)
install(TARGETS tests_main RUNTIME DESTINATION var/lib/tinyhttp.d/tests)

configure_file(${CMAKE_SOURCE_DIR}/files/server.xml.in ${CMAKE_CURRENT_BINARY_DIR}/server.xml)
configure_file(${CMAKE_SOURCE_DIR}/files/localhost.xml.in ${CMAKE_CURRENT_BINARY_DIR}/vhosts.d/localhost.xml)
configure_file(${CMAKE_SOURCE_DIR}/files/resources.xml.in ${CMAKE_CURRENT_BINARY_DIR}/vhosts.d/resources.xml)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/server.xml DESTINATION var/lib/tinyhttp.d/tests)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vhosts.d/localhost.xml DESTINATION var/lib/tinyhttp.d/tests/vhosts.d)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vhosts.d/resources.xml DESTINATION var/lib/tinyhttp.d/tests/vhosts.d)

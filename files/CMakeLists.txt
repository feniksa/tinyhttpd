configure_file(${CMAKE_SOURCE_DIR}/files/server.xml.in ${CMAKE_BINARY_DIR}/src/server.xml)
configure_file(${CMAKE_SOURCE_DIR}/files/localhost.xml.in ${CMAKE_BINARY_DIR}/src/vhosts.d/localhost.xml)
configure_file(${CMAKE_SOURCE_DIR}/files/resources.xml.in ${CMAKE_BINARY_DIR}/src/vhosts.d/resources.xml)

install(FILES ${CMAKE_BINARY_DIR}/src/server.xml DESTINATION etc/tinyhttp.d)
install(FILES ${CMAKE_BINARY_DIR}/src/vhosts.d/localhost.xml DESTINATION etc/tinyhttp.d/vhosts.d)
install(FILES ${CMAKE_BINARY_DIR}/src/vhosts.d/resources.xml DESTINATION etc/tinyhttp.d/vhosts.d)

if (USE_OPENRC)
        configure_file(${CMAKE_SOURCE_DIR}/files/launch/openrc/tinyhttpd.in 
        	${CMAKE_BINARY_DIR}/files/launch/openrc/tinyhttpd)

		install(FILES ${CMAKE_BINARY_DIR}/files/launch/openrc/tinyhttpd
                DESTINATION etc/init.d
                PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE)
endif(USE_OPENRC)


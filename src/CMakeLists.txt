include_directories(${CMAKE_BINARY_DIR}/src)

set (SOURCES
	Configuration.cpp
	RequestHandlerFactory.cpp
	HttpServer.cpp
	HttpServerConf.cpp
	mime.cpp
	HandleNotFound.cpp
	HandleStaticFile.cpp
	StreamCopy.cpp
	HttpHeaderParse.cpp
	FileRange.cpp
	WebDirectory.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
#target_link_libraries(${PROJECT_NAME}_daemon ${PROJECT_NAME}_cpsp ${PROJECT_NAME}_api)

add_executable(${PROJECT_NAME}d main.cpp)
target_link_libraries(${PROJECT_NAME}d  ${PROJECT_NAME} ${LIBS} tinyhttp_cpsp tinyhttp_api)
install(TARGETS ${PROJECT_NAME}d RUNTIME DESTINATION usr/bin)

add_subdirectory(cpsp)
add_subdirectory(api)

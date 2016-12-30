include_directories(${CMAKE_BINARY_DIR}/src)

set (LIB_SRCS
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

add_library(tinyhttp_daemon STATIC ${LIB_SRCS})
#target_link_libraries(${PROJECT_NAME}_daemon ${PROJECT_NAME}_cpsp ${PROJECT_NAME}_api)

add_executable( tinyhttpd main.cpp)
target_link_libraries( tinyhttpd  ${LIBS} tinyhttp_daemon tinyhttp_cpsp tinyhttp_api)
install(TARGETS tinyhttpd RUNTIME DESTINATION usr/bin)

add_subdirectory(cpsp)
add_subdirectory(api)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src)

set (API_SOURCES_CXX
	DirectoryHandler.cpp
)

add_library(tinyhttp_api STATIC ${API_SOURCES_CXX})
#target_link_libraries(${PROJECT_NAME}_api ${PROJECT_NAME}_cpsp) # <- it should be compiled first as it have generation rules


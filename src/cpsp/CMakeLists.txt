include_directories(${CMAKE_SOURCE_DIR}/src)

set (CPSP_SRCS
	AccessDeniedHandler.cpsp
	NotFoundHandler.cpsp
	FileBrowserHandler.cpsp
	DirectoryHandler.cpsp
)

foreach(CPSP_FILE ${CPSP_SRCS})
	get_filename_component(CPSP_FILE_BASE ${CPSP_FILE} NAME_WE)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CPSP_FILE_BASE}.h ${CMAKE_CURRENT_BINARY_DIR}/${CPSP_FILE_BASE}.cpp
		COMMAND cpspc
		ARGS --output-dir="${CMAKE_CURRENT_BINARY_DIR}" ${CPSP_FILE}
 		DEPENDS ${CPSP_FILE}
		COMMENT "Compile Servlet page ${CPSP_FILE}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)

	set (CPSP_CXX ${CPSP_CXX} ${CMAKE_CURRENT_BINARY_DIR}/${CPSP_FILE_BASE}.cpp)
endforeach(CPSP_FILE)

add_library(tinyhttp_cpsp STATIC ${CPSP_CXX})

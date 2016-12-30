cmake_minimum_required(VERSION 3.2)

project(tinyhttp  CXX)

# --- Options
option(USE_OPENRC "Install openrc run script" ON)
option(BUILD_TESTS "Build unit tests" ON)

# --- helper for youcompleteme
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
if( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
  execute_process( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
endif()

# --- Libraries section
find_package(Poco REQUIRED Foundation Net Util JSON XML)
set (LIBS ${LIBS} ${Poco_LIBRARIES})

find_package(Boost 1.56 COMPONENTS program_options locale system filesystem REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )
set (LIBS ${LIBS} ${Boost_LIBRARIES})

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
add_subdirectory(files)

if (BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif(BUILD_TESTS)



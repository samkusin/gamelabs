cmake_minimum_required(VERSION 2.8.10)

project( voronoi )

# setup options and needed variables.
if( DEFINED GAMELABS_PROJECTS_DIR )
    list( APPEND CMAKE_MODULE_PATH "${GAMELABS_PROJECTS_DIR}/cmake" )
endif( )

include( GameLabsBuild )

#
# set sources
#
set( PROJECT_INCLUDES
     "${CMAKE_CURRENT_SOURCE_DIR}/voronoi.hpp" )
set( PROJECT_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/voronoi.cpp" )

#
# Build Project
#
find_library( OPENGL_LIBRARY OpenGL )
find_library( GLFW3_LIBRARY GLFW3 )
find_path( GLFW3_INCLUDE_DIR GLFW3/glfw3.h )

include_directories( "${GAMELABS_PROJECTS_DIR}" )
include_directories( "${GLFW3_INCLUDE_DIR}" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable( simple
	${PROJECT_SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/samples/simple.cpp"
	${PROJECT_INCLUDES} )
set_target_properties( simple PROPERTIES COMPILE_FLAGS
	${LOCAL_CPP_COMPILE_FLAGS} )
set_target_properties( simple PROPERTIES LINK_FLAGS
	${LOCAL_CPP_LINK_FLAGS} )

if( OPENGL_LIBRARY AND GLFW3_LIBRARY )
	add_executable( glgraph
		${PROJECT_SOURCES}
		"${CMAKE_CURRENT_SOURCE_DIR}/samples/glgraph.cpp"
		${PROJECT_INCLUDES} )
	set_target_properties( glgraph PROPERTIES COMPILE_FLAGS
		${LOCAL_CPP_COMPILE_FLAGS} )
	set_target_properties( glgraph PROPERTIES LINK_FLAGS
		${LOCAL_CPP_LINK_FLAGS} )
	target_link_libraries( glgraph
		${OPENGL_LIBRARY} ${GLFW3_LIBRARY} )
else( )
	message( WARNING "OpenGL and GLFW3 required to build sample" )
endif( )

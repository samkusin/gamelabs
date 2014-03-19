cmake_minimum_required(VERSION 2.8.10)

project( genroom )

# setup options and needed variables.
if( DEFINED GAMELABS_PROJECTS_DIR )
    list( APPEND CMAKE_MODULE_PATH "${GAMELABS_PROJECTS_DIR}/cmake" )
endif( )

include( GameLabsBuild )

#
# set sources
#

set( PROJECT_INCLUDES 
     "${CMAKE_CURRENT_SOURCE_DIR}/grid.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/map.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/maptypes.hpp" 
     "${CMAKE_CURRENT_SOURCE_DIR}/genroom.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/tiledatabase.hpp" )

set( PROJECT_SOURCES
	 "${GAMELABS_PROJECTS_DIR}/voronoi/voronoi.cpp"
     "${GAMELABS_PROJECTS_DIR}/jobqueue/jobqueue.cpp"
     "${GAMELABS_PROJECTS_DIR}/jobqueue/jobscheduler.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/map.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/genroom.cpp" 
     "${CMAKE_CURRENT_SOURCE_DIR}/tiledatabase.cpp" )

#
# Build Project
#
find_library( OPENGL_LIBRARY OpenGL )
find_library( GLFW3_LIBRARY GLFW3 )

include_directories( "${GAMELABS_PROJECTS_DIR}" )
include_directories( "${GAMELABS_EXT_PACKAGE_DIR}/include" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

if( OPENGL_LIBRARY AND GLFW3_LIBRARY )
	add_executable( glgenroom
		${PROJECT_SOURCES}
        "${GAMELABS_EXT_PACKAGE_DIR}/source/stb/stb_image.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/samples/bitmap.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/samples/graphics.cpp" 
		"${CMAKE_CURRENT_SOURCE_DIR}/samples/glgenroom.cpp"
		${PROJECT_INCLUDES}
        "${CMAKE_CURRENT_SOURCE_DIR}/samples/bitmap.hpp" )
	set_target_properties( glgenroom PROPERTIES COMPILE_FLAGS
		${LOCAL_CPP_COMPILE_FLAGS} )
	set_target_properties( glgenroom PROPERTIES LINK_FLAGS
		${LOCAL_CPP_LINK_FLAGS} )
	target_link_libraries( glgenroom 
		${OPENGL_LIBRARY} ${GLFW3_LIBRARY} )
else( )
	message( WARNING "OpenGL and GLFW3 required to build sample" )
endif( )




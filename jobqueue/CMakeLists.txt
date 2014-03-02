cmake_minimum_required(VERSION 2.8.10)

project( jobqueue )

# setup options and needed variables.
if( DEFINED GAMELABS_PROJECTS_DIR )
    list( APPEND CMAKE_MODULE_PATH "${GAMELABS_PROJECTS_DIR}/cmake" )
endif( )

include( GameLabsBuild )

#
# set sources
#
set( PROJECT_INCLUDES
     "${CMAKE_CURRENT_SOURCE_DIR}/jobtypes.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/job.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/jobqueue.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/jobscheduler.hpp" )
set( PROJECT_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/jobqueue.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/jobscheduler.cpp" )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

#
# Build Project
#
add_executable( simgame
	${PROJECT_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/samples/gameobjects.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/samples/simgame.cpp"
	${PROJECT_INCLUDES} )
set_target_properties( simgame PROPERTIES COMPILE_FLAGS
	${LOCAL_CPP_COMPILE_FLAGS} )
set_target_properties( simgame PROPERTIES LINK_FLAGS
	${LOCAL_CPP_LINK_FLAGS} )




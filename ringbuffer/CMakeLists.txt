cmake_minimum_required(VERSION 2.8.10)

project( ringbuffer )

# setup options and needed variables.
if( DEFINED GAMELABS_PROJECTS_DIR )
    list( APPEND CMAKE_MODULE_PATH "${GAMELABS_PROJECTS_DIR}/cmake" )
endif( )

include( GameLabsBuild )

#
# Build Project
#
set( PROJECT_INCLUDES
     "${CMAKE_CURRENT_SOURCE_DIR}/packetbuffer.hpp" )
set( PROJECT_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/packetbuffer.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" )

find_library( PTHREAD_LIBRARY pthread )

set( PROJECT_LIBRARIES ${PTHREAD_LIBRARY} )

add_executable( streamer ${PROJECT_SOURCES} ${PROJECT_INCLUDES} )
set_target_properties( streamer PROPERTIES COMPILE_FLAGS ${LOCAL_CPP_COMPILE_FLAGS} )
if( LOCAL_COMPILE_DEFINES )
	set_target_properties( streamer PROPERTIES COMPILE_DEFINITIONS ${LOCAL_COMPILE_DEFINES} )
endif( )
set_target_properties( streamer PROPERTIES LINK_FLAGS ${LOCAL_CPP_LINK_FLAGS} )
target_link_libraries( streamer ${PROJECT_LIBRARIES} )




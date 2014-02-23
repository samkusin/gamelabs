cmake_minimum_required(VERSION 2.8.10)

project( ringbuffer )

###############################################################################
# CMake custom variables used to set up the build process.  All subtargets
# will use these variables to discover packages, set compile settings, or other
# modify other build related settings.

# Setup the external package directory used for compiling the SDK.
if ( NOT DEFINED GAMELABS_PROJECTS_DIR )
     set( GAMELABS_PROJECTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.."
          CACHE FILEPATH "Directory containing any gamelabs projects used by this project" )
endif( )

if ( NOT DEFINED GAMELABS_EXT_PACKAGE_DIR )
    set( GAMELABS_EXT_PACKAGE_DIR "${GAMELABS_PROJECTS_DIR}/external"
         CACHE FILEPATH "Directory containing external packages used by this project" )
endif( )

# Setup compile options
#
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  set( LOCAL_CXX_COMPILER_CLANG TRUE )
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  set( LOCAL_CXX_COMPILER_GNU TRUE )
elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
  set( LOCAL_CXX_COMPILER_MSVC TRUE )
endif( )

if( LOCAL_CXX_COMPILER_CLANG )
    set( LOCAL_CPP_COMPILE_FLAGS "-Wall -Wno-missing-braces -std=c++11 -stdlib=libc++ -fno-exceptions -fno-rtti" )
    set( LOCAL_CPP_LINK_FLAGS "-std=c++11 -stdlib=libc++" )
elseif( LOCAL_CXX_COMPILER_GNU )
	set( LOCAL_CPP_COMPILE_FLAGS "-Wall -Wno-missing-braces -fno-exceptions -fno-rtti" )
	set( LOCAL_CPP_COMPILE_FLAGS "${LOCAL_CPP_COMPILE_FLAGS} -std=c++11 -stdlib=libstdc++" )
else( )
     message( FATAL "Compiler not supported" )
endif( )

# Select platform
#
# platform-independent

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




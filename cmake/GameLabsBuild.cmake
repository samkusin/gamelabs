#
#   GAMELABS_PROJECTS_DIR       - Root directory for all projects
#   GAMELABS_EXT_PACKAGE_DIR    - Root directory for external packages

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
set( GAMELABS_PLATFORM_OSX FALSE )
if( "${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" )
    set( GAMELABS_PLATFORM_OSX TRUE )
    set( LOCAL_COMPILE_DEFINES ${LOCAL_COMPILE_DEFINES} GAMELABS_PLATFORM_OSX )
else( )
    message( WARNING "Platform not discovered" )
endif( )
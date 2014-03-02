cmake_minimum_required(VERSION 2.8.10)

project( gamelabs )

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory( ringbuffer )
add_subdirectory( jobqueue )
add_subdirectory( genroom )
add_subdirectory( voronoi )

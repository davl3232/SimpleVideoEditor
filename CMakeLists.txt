cmake_minimum_required(VERSION 2.8)
project( editor )
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable( lector lector.cpp )
target_link_libraries( lector ${OpenCV_LIBS} )
target_compile_features( lector PRIVATE cxx_range_for )

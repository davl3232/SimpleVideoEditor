cmake_minimum_required(VERSION 2.8)
project( SimpleVideoEditor )
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable( editor editor.cpp )
target_link_libraries( editor ${OpenCV_LIBS} )
target_compile_features( editor PRIVATE cxx_range_for )

cmake_minimum_required(VERSION 2.8)
project( editor )
find_package( OpenCV REQUIRED )

add_executable( editor editor.cpp )
target_link_libraries( editor ${OpenCV_LIBS} )
add_executable( lector lector.cpp )
target_link_libraries( lector ${OpenCV_LIBS} )

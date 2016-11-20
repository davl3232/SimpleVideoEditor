cmake_minimum_required(VERSION 2.8)
project( proyecto )
find_package( OpenCV REQUIRED )

add_executable( proyecto proyecto.cpp )
target_link_libraries( proyecto ${OpenCV_LIBS} )

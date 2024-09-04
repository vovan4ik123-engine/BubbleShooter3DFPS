cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BUBBLE_SHOOTER_3D_FPS)

include_directories(Beryll_engine/libs)
include_directories(Beryll_engine/src)

add_subdirectory(Beryll_engine) # Engine with all necessary libs.
add_subdirectory(src) # Application code.

#show all included dirs at this CmakeLists.txt level
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#    message(WARNING "dir='${dir}'")
#endforeach()

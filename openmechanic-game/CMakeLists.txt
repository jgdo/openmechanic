cmake_minimum_required(VERSION 2.6)
project(openmechanic-game)

find_package(osgBullet)

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS *.cpp)
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS *.h)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${openmechanic_engine_INCLUDE_DIRS} ${OSGBULLET_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wno-comment -Wno-ignored-attributes -Wno-misleading-indentation -Wno-deprecated-declarations")
 
add_executable(openmechanic-game ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(openmechanic-game openmechanic-engine pthread osg osgViewer osgDB osgUtil osgbCollision)
 

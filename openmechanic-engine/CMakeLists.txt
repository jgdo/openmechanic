cmake_minimum_required(VERSION 2.8)
project(openmechanic-engine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Boost REQUIRED)
find_package(Bullet REQUIRED)

find_package( PkgConfig )
pkg_check_modules( EIGEN3 REQUIRED eigen3 )

include_directories( ${EIGEN3_INCLUDE_DIRS} )
include_directories (${BULLET_INCLUDE_DIRS})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS *.cpp)
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS *.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wno-comment -Wno-ignored-attributes -Wno-misleading-indentation -Wno-deprecated-declarations")

add_library(openmechanic-engine ${SOURCE_FILES} ${HEADER_FILES} ${BULLET_INCLUDE_DIRS})
target_link_libraries(openmechanic-engine GL GLU ${BULLET_LIBRARIES} boost_system )

set(Engine_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

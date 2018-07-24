cmake_minimum_required(VERSION 2.8)

project(FeedHandler)

set(BINARY feedhandler)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.45.0 COMPONENTS system)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_executable(${BINARY} Main.cpp)
  target_link_libraries(${BINARY} ${Boost_LIBRARIES})
endif()


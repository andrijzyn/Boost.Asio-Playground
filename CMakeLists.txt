cmake_minimum_required(VERSION 3.0)
project(Server)

find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(client ${Boost_LIBRARIES})
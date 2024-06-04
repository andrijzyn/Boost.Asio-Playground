cmake_minimum_required(VERSION 3.10)

project(TCPServer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Добавляем исполняемый файл для сервера
add_executable(server server.cpp)

# Добавляем исполняемый файл для клиента
add_executable(client client.cpp)

if(WIN32)
    set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang++.exe")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
endif()

if(UNIX)
    set(CMAKE_CXX_COMPILER "g++-12")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -fmax-errors=5")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

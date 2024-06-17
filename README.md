# TCPServer
This is a simple TCP server and client project written in C++.
## Description
This project provides a basic implementation of a TCP server and client, allowing communication over a TCP/IP network.
## Prerequisites
> CMake (minimum version 3.10).  
> C++ compiler (clang++ for Windows, g++-12 for Linux)
## Compilation Instructions
### Windows
Ensure that you have CMake installed and added to your system's PATH.
Open Command Prompt.
Navigate to the project directory.
Run the following commands:

    mkdir build
    cd build
    cmake ..
    cmake --build .

### Linux (Debian, Deb-based)
Ensure that you have CMake and g++-12 installed.
On Ubuntu, you can install g++-12 with the following command:

    sudo apt update && sudo apt upgrade
    sudo apt install g++-12

Open Terminal.
Navigate to the project directory.
Run the following commands:

    mkdir build
    cd build
    cmake ..
    cmake --build .

## Usage

After successfully compiling the project, you will find the executable files server and client in the build directory.
Run the server executable to start the TCP server.
Run the client executable to connect to the server and communicate.
> [!WARNING] 
> At this point, this project has been abandoned!

> [!TIP]
> If something doesn't work... please fix it and let me know...

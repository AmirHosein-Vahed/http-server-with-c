cmake_minimum_required(VERSION 3.10)
project(http_server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add executable
add_executable(http_server
    src/main.c
    src/http_server.c
)
cmake_minimum_required(VERSION 3.10)
project(http_server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add main executable
add_executable(http_server
    src/main.c
    src/http_server.c
)

# Download and include Unity testing framework
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(unity)

# Add test executable
add_executable(http_server_tests
    tests/test_http_server.c
    src/http_server.c
)

# Link Unity and add its include directory
target_link_libraries(http_server_tests unity)
target_include_directories(http_server_tests PRIVATE ${unity_SOURCE_DIR}/src)

# Add pthread library for threading support
find_package(Threads REQUIRED)
target_link_libraries(http_server_tests Threads::Threads)

# Add test target
enable_testing()
add_test(NAME http_server_tests COMMAND http_server_tests)
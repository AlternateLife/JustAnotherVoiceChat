# Setup base project
cmake_minimum_required(VERSION 3.5)
project(AlternateVoice LANGUAGES CXX)

# Setup basic C++ and project attributes
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add targets
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

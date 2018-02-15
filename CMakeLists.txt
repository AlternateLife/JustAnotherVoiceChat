# Setup base project
cmake_minimum_required(VERSION 3.5)
project(JustAnotherVoiceChat VERSION 0.1.0.0 LANGUAGES CXX)

# Setup basic C++ and project attributes
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

# Add include directories
include_directories(include)
include_directories(thirdparty/ts3/include)
include_directories(thirdparty/enet/include)
include_directories(thirdparty/libmicrohttpd/src/include)
include_directories(thirdparty/cereal/include)
include_directories("${JustAnotherVoiceChat_BINARY_DIR}")

if(WIN32)
  link_directories(${JustAnotherVoiceChat_SOURCE_DIR}/../thirdparty/libmicrohttpd)
else()
  link_directories(${JustAnotherVoiceChat_BINARY_DIR}/lib)
endif()

# Add targets
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

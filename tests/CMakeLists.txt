# Setup test project
project(AlternateVoiceTest LANGUAGES CXX)

# Search required libraries
find_package(Threads)

include(ExternalProject)

include_directories(../include)
include_directories(../thirdparty/ts3/include)
include_directories(../thirdparty/enet/include)
include_directories(../thirdparty/libmicrohttpd/src/include)

if(WIN32)
  link_directories(${AlternateVoiceTest_SOURCE_DIR}/../thirdparty/libmicrohttpd)
else()
  link_directories(${AlternateVoice_BINARY_DIR}/lib)
endif()

file(GLOB SOURCES "./*.cpp")

# Add executable
add_executable(AlternateVoiceTest ${SOURCES})

# link external libraries
target_link_libraries(AlternateVoiceTest AlternateVoice)

target_link_libraries(AlternateVoiceTest enet)
target_link_libraries(AlternateVoiceTest ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
  target_link_libraries(AlternateVoiceTest libmicrohttpd)
  target_link_libraries(AlternateVoiceTest ws2_32)
  target_link_libraries(AlternateVoiceTest winmm)
else()
  target_link_libraries(AlternateVoiceTest microhttpd)

  add_dependencies(AlternateVoiceTest libmicrohttpd)
endif()

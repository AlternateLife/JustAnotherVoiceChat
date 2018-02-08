# Setup library project
project(AlternateVoice VERSION 0.1.0.0 LANGUAGES CXX)

add_definitions(-DALTERNATEVOICE_EXPORTS)

include(ExternalProject)

# Add files to library
include_directories(../include)
include_directories(../thirdparty/ts3/include)
include_directories(../thirdparty/libmicrohttpd/src/include)
include_directories("${AlternateVoice_BINARY_DIR}")

if(WIN32)
  link_directories(${AlternateVoice_SOURCE_DIR}/../thirdparty/libmicrohttpd)
else()
  link_directories(${AlternateVoice_SOURCE_DIR}/../thirdparty/libmicrohttpd/src/microhttpd/.libs)
endif()

# configure header files
configure_file(../include/Version.h.in ${AlternateVoice_BINARY_DIR}/Version.h @ONLY)

file(GLOB SOURCES "./*.cpp")

add_library(AlternateVoice SHARED ${SOURCES})

# link external libraries
if(WIN32)
  target_link_libraries(AlternateVoice libmicrohttpd)
else()
  ExternalProject_Add(libmicrohttpd
    SOURCE_DIR ${AlternateVoice_SOURCE_DIR}/../thirdparty/libmicrohttpd
    CONFIGURE_COMMAND autoreconf -fi && ./configure --prefix ${AlternateVoice_BINARY_DIR}
    INSTALL_DIR ${AlternateVoice_BINARY_DIR}
    BUILD_IN_SOURCE 1
  )

  target_link_libraries(AlternateVoice microhttpd)

  add_dependencies(AlternateVoice libmicrohttpd)
endif()

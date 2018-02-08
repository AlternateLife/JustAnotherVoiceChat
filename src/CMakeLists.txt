# Setup library project
project(AlternateVoice VERSION 0.1.0.0 LANGUAGES CXX)

add_definitions(-DALTERNATEVOICE_EXPORTS)

# Add files to library
include_directories(../include)
include_directories(../thirdparty/ts3/include)
include_directories("${AlternateVoice_BINARY_DIR}")

# configure header files
configure_file(../include/Version.h.in ${AlternateVoice_BINARY_DIR}/Version.h @ONLY)

file(GLOB SOURCES "./*.cpp")

add_library(AlternateVoice SHARED ${SOURCES})

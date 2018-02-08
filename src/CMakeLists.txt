# Setup library project
project(AlternateVoice LANGUAGES CXX)

# Add files to library
include_directories(../include)
include_directories(../thirdparty/ts3/include)

file(GLOB SOURCES "./*.cpp")

add_library(AlternateVoice SHARED ${SOURCES})

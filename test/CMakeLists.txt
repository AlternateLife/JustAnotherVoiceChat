# Setup test project
project(AlternateVoiceTest LANGUAGES CXX)

include_directories(../include)
include_directories(../thirdparty/ts3/include)

file(GLOB SOURCES "./*.cpp")

# Add executable
add_executable(AlternateVoiceTest ${SOURCES})

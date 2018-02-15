# Search required libraries
file(GLOB SOURCES "./*.cpp")

# Add executable
add_executable(AlternateVoiceTS3Mock ${SOURCES})

# link external libraries
target_link_libraries(AlternateVoiceTS3Mock AlternateVoice)

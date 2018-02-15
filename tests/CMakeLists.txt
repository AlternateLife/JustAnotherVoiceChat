# Search required libraries
file(GLOB SOURCES "./*.cpp")

# Add executable
add_executable(AlternateVoiceTest ${SOURCES})

# link external libraries
target_link_libraries(AlternateVoiceTest AlternateVoice)

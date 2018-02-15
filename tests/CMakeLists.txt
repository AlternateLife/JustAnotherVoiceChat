# Search required libraries
file(GLOB SOURCES "./*.cpp")

# Add executable
add_executable(JustAnotherVoiceChatTS3Mock ${SOURCES})

# link external libraries
target_link_libraries(JustAnotherVoiceChatTS3Mock JustAnotherVoiceChat)

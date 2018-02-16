# Setup library project
add_definitions(-DJUSTANOTHERVOICECHAT_EXPORTS)

# Search required libraries
find_package(Threads)

include(ExternalProject)

# configure header files
configure_file(../include/version.h.in ${JustAnotherVoiceChat_BINARY_DIR}/version.h @ONLY)

file(GLOB SOURCES "./*.cpp")

add_library(JustAnotherVoiceChat SHARED ${SOURCES})

# link external libraries
target_link_libraries(JustAnotherVoiceChat enet)
target_link_libraries(JustAnotherVoiceChat ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
  target_link_libraries(JustAnotherVoiceChat libmicrohttpd)
  target_link_libraries(JustAnotherVoiceChat ws2_32)
  target_link_libraries(JustAnotherVoiceChat winmm)
else()
  ExternalProject_Add(libmicrohttpd
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libmicrohttpd
    CONFIGURE_COMMAND autoreconf -fi && ./configure --prefix ${JustAnotherVoiceChat_BINARY_DIR}
    INSTALL_DIR ${JustAnotherVoiceChat_BINARY_DIR}
    BUILD_IN_SOURCE 1
  )

  target_link_libraries(JustAnotherVoiceChat microhttpd)

  add_dependencies(JustAnotherVoiceChat libmicrohttpd)
endif()

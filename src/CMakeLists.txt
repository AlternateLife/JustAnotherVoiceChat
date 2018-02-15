# Setup library project
add_definitions(-DALTERNATEVOICE_EXPORTS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Search required libraries
find_package(Threads)

include(ExternalProject)

# configure header files
configure_file(../include/version.h.in ${AlternateVoice_BINARY_DIR}/version.h @ONLY)

file(GLOB SOURCES "./*.cpp")

add_library(AlternateVoice SHARED ${SOURCES})

# link external libraries
target_link_libraries(AlternateVoice enet)
target_link_libraries(AlternateVoice ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
  target_link_libraries(AlternateVoice libmicrohttpd)
  target_link_libraries(AlternateVoice ws2_32)
  target_link_libraries(AlternateVoice winmm)
else()
  ExternalProject_Add(libmicrohttpd
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libmicrohttpd
    CONFIGURE_COMMAND autoreconf -fi && ./configure --prefix ${AlternateVoice_BINARY_DIR}
    INSTALL_DIR ${AlternateVoice_BINARY_DIR}
    BUILD_IN_SOURCE 1
  )

  target_link_libraries(AlternateVoice microhttpd)

  add_dependencies(AlternateVoice libmicrohttpd)
endif()

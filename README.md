# JustAnotherVoiceChat

## Description

This is the teamspeak 3 plugin whichs connects to the JustAnotherVoiceChat server to control 3D voice communication.

## Usage

Use the provided teamspeak 3 plugin package you can download in the release section.

## Development

>Hint: Make sure to also clone the submodules with `git submodule init && git submodule update`.

### Windows

1. Build the required *libmicrohttpd* library with *Visual Studio* and place the generated `.lib` files in the `thirdparty/libmicrohttpd` directory
2. Create a build directory for cmake `mkdir build && cd build`
3. Generate the build environment `cmake .. -G "Visual Studio 15 2017 Win64"`
4. Build the plugin with *Visual Studio*

### Linux/Mac OS

1. Create a build directory for cmake `mkdir build && cd build`
2. Generate the build environment `cmake ..`
3. Build the plugin `make`

## Authors

* MarkAtk

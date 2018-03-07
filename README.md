![JAVIC logo](docs/images/JAVIC_Wide_250px.png)

[![Build status](https://ci.appveyor.com/api/projects/status/gkt5ejc524w3ckja/branch/master?svg=true)](https://ci.appveyor.com/project/romanzipp/justanothervoicechat/branch/master)
[![Build Status](https://travis-ci.org/AlternateLife/JustAnotherVoiceChat.svg?branch=master)](https://travis-ci.org/AlternateLife/JustAnotherVoiceChat)

## Description

This is the teamspeak 3 plugin which connects to the JustAnotherVoiceChat server to control 3D voice communication.

Feel free to join our Discord Server if you have any further questions.

[![JAVIC Discord](https://discordapp.com/api/guilds/401509555684769802/embed.png?style=banner2)](https://discord.gg/uAVhvBT)

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
* jnkchris

## MIT license

Copyright (c) 2018 JustAnotherVoiceChat

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

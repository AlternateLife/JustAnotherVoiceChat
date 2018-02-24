/*
 * File: tests/ts3mock.cpp
 * Date: 08.02.2018
 *
 * MIT License
 *
 * Copyright (c) 2018 JustAnotherVoiceChat
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>
#include <thread>

#ifdef _WIN32

#else
#include <csignal>
#endif

#include "teamspeakPlugin.h"

static bool running = true;

unsigned int logMessage(const char *message, LogLevel severity, const char *channel, uint64) {
  std::string logType = "";

  switch(severity) {
    case LogLevel_CRITICAL:
      logType = "CRITICAL";
      break;

    case LogLevel_DEBUG:
      logType = "   DEBUG";
      break;

    case LogLevel_DEVEL:
      logType = "   DEVEL";
      break;

    case LogLevel_ERROR:
      logType = "   ERROR";
      break;

    case LogLevel_INFO:
      logType = "    INFO";
      break;

    case LogLevel_WARNING:
      logType = " WARNING";
      break;
  }

  std::cout << channel << " " << logType << " [" << severity << "]: " << message << std::endl;
  return 0;
}

unsigned int spawnNewServerConnectionHandler(int, uint64 *result) {
  *result = 0x1234;
  return 0;
}

unsigned int destroyServerConnectionHandler(uint64) {
  return 0;
}

unsigned int getConnectionStatus(uint64, int *result) {
  *result = 1;
  return 0;
}

unsigned int getClientId(uint64, anyID *result) {
  *result = 4;
  return 0;
}

#ifdef _WIN32

#else
void signalHandler(int) {
  running = false;
}
#endif

int main(int, char **) {
#ifdef _WIN32

#else
  signal(SIGTERM, signalHandler);
  signal(SIGINT, signalHandler);
#endif

  // register functions
  struct TS3Functions functions;
  functions.logMessage = logMessage;
  functions.spawnNewServerConnectionHandler = spawnNewServerConnectionHandler;
  functions.destroyServerConnectionHandler = destroyServerConnectionHandler;
  functions.getConnectionStatus = getConnectionStatus;
  functions.getClientID = getClientId;
  ts3plugin_setFunctionPointers(functions);

  // mockup teamspeak 
  ts3plugin_init();

  while (running) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // cleanup
  ts3plugin_shutdown();

  return EXIT_SUCCESS;
}

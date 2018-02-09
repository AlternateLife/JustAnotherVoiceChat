/*
 * File: Teamspeak.cpp
 * Date: 08.02.2018
 *
 * MIT License
 *
 * Copyright (c) 2018 AlternateVoice
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

#include "Teamspeak.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <teamspeak/public_errors.h>

#include "Version.h"
#include "AlternateVoice.h"

#define PLUGIN_API_VERSION 22;

static struct TS3Functions ts3Functions;
static char versionBuffer[16];
static uint64 serverConnectionHandler = 0;

void ts3_log(std::string message, enum LogLevel severity) {
  ts3Functions.logMessage(message.c_str(), severity, "AlternateVoice", 0);
}

anyID ts3_clientID() {
  // check if connected to the server
  if (serverConnectionHandler == 0) {
    ts3_log("Unable to get client ID when not connected to a server", LogLevel_WARNING);
    return 0;
  }

  int status;
  int result = ts3Functions.getConnectionStatus(serverConnectionHandler, &status);
  if (result != ERROR_ok) {
    ts3_log("Unable to get server connection status", LogLevel_WARNING);
    return 0;
  }

  // 1 = connected, 0 = not connected
  if (status != 1) {
    return 0;
  }

  // get client ID on this server
  anyID clientID;
  result = ts3Functions.getClientID(serverConnectionHandler, &clientID);
  if (result != ERROR_ok) {
    ts3_log("Unable to get client ID", LogLevel_ERROR);
    return 0;
  }

  return clientID;
}

bool ts3_connect(std::string host, uint16_t port, std::string serverPassword) {
  // create connection handler if needed
  int result;

  if (serverConnectionHandler == 0) {
    result = ts3Functions.spawnNewServerConnectionHandler(0, &serverConnectionHandler);
    if (result != ERROR_ok) {
      ts3_log("Unable to spawn server connection handler", LogLevel_ERROR);
      return false;
    }
  }

  char *identity;
  result = ts3Functions.createIdentity(&identity);
  if (result != ERROR_ok) {
    ts3_log("Unable to create a teamspeak identity", LogLevel_ERROR);
    return false;
  }

  result = ts3Functions.startConnection(serverConnectionHandler, identity, host.c_str(), port, "Test", NULL, "", serverPassword.c_str());
  if (result != ERROR_ok) {
    ts3_log(std::to_string(result) + ": Unable to connect to " + host + ":" + std::to_string(port), LogLevel_WARNING);
    return false;
  }

  ts3Functions.freeMemory(identity);
  return true;
}

void ts3_disconnect() {

}

const char *ts3plugin_name() {
  return "AlternateVoice";
}

const char *ts3plugin_version() {
  sprintf(versionBuffer, "%d.%d.%d.%d", ALTERNATEVOICE_VERSION_MAJOR, ALTERNATEVOICE_VERSION_MINOR, ALTERNATEVOICE_VERSION_PATCH, ALTERNATEVOICE_VERSION_BUILD);
  return versionBuffer;
}

int ts3plugin_apiVersion() {
  return PLUGIN_API_VERSION;
}

const char *ts3plugin_author() {
  return "AlternateVoice";
}

const char *ts3plugin_description() {
  return "3D game voice plugin for communicating with the AlternateVoice server";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
  ts3Functions = funcs;
}

int ts3plugin_init() {
  if (AlternateVoice_Start() == false) {
    return 1;
  }

  return 0;
}

void ts3plugin_shutdown() {
  if (serverConnectionHandler != 0) {
    ts3Functions.destroyServerConnectionHandler(serverConnectionHandler);
  }

  AlternateVoice_Stop();
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
  char name[512];
  if (ts3Functions.getClientDisplayName(serverConnectionHandlerID, clientID, name, 512) != ERROR_ok) {
    ts3_log("Unable to get client's display name", LogLevel_WARNING);
    return;
  }

  if (status == STATUS_TALKING) {
    ts3_log(std::string(name) + " starts talking", LogLevel_INFO);
  } else {
    ts3_log(std::string(name) + " stops talking", LogLevel_INFO);
  }
}

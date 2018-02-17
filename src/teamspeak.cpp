/*
 * File: src/teamspeak.cpp
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

#include "teamspeak.h"

#include "teamspeakPlugin.h"

#define BUFFER_LENGTH 256

static uint64 serverConnectionHandler = 0;

void ts3_log(std::string message, enum LogLevel severity) {
  ts3Functions.logMessage(message.c_str(), severity, "JustAnotherVoiceChat", 0);
}

bool ts3_connect(std::string host, uint16_t port, std::string serverPassword) {
  // check if server is already connected
  uint64 *serverList;
  int result = ts3Functions.getServerConnectionHandlerList(&serverList);
  if (result != ERROR_ok) {
    ts3_log("Unable to get server list", LogLevel_ERROR);
    return false;
  }

  // search for host in list
  int index = 0;
  uint64 handle = serverList[index];

  while (handle != NULL) {
    char host[BUFFER_LENGTH] = { '\0' };
    char password[BUFFER_LENGTH] = { '\0' };
    unsigned short port = 0;

    result = ts3Functions.getServerConnectInfo(handle, host, &port, password, BUFFER_LENGTH);
    if (result != ERROR_ok) {
      ts3_log("Unable to get server connection status " + std::to_string(handle), LogLevel_ERROR);
      continue;
    }

    // compare handler host with requested
    ts3_log(std::string("Server ") + host + ":" + std::to_string(port), LogLevel_DEBUG);

    // get next handle
    index++;
    handle = serverList[index];
  }

  // server list needs to be freeded after usage
  ts3Functions.freeMemory(serverList);

  // create connection handler if needed
  if (serverConnectionHandler == 0) {
    result = ts3Functions.spawnNewServerConnectionHandler(0, &serverConnectionHandler);
    if (result != ERROR_ok) {
      ts3_log("Unable to spawn server connection handler", LogLevel_ERROR);
      return false;
    }
  }

  // char *identity;
  // result = ts3Functions.createIdentity(&identity);
  // if (result != ERROR_ok) {
  //   ts3_log("Unable to create a teamspeak identity", LogLevel_ERROR);
  //   return false;
  // }

  // result = ts3Functions.startConnection(serverConnectionHandler, identity, host.c_str(), port, "Test", NULL, "", serverPassword.c_str());
  // if (result != ERROR_ok) {
  //   ts3_log(std::to_string(result) + ": Unable to connect to " + host + ":" + std::to_string(port), LogLevel_WARNING);
  //   return false;
  // }

  // ts3Functions.freeMemory(identity);
  return true;
}

void ts3_disconnect() {
  if (serverConnectionHandler != 0) {
    ts3Functions.destroyServerConnectionHandler(serverConnectionHandler);
  }
}

bool ts3_moveToChannel(std::string name, std::string password) {
    return false;
}

anyID ts3_clientID(uint64 serverConnectionHandlerId) {
  // first try parameter server connection
  uint64 connId = serverConnectionHandlerId;

  if (serverConnectionHandlerId == 0) {
    // try global connection id
    connId = serverConnectionHandler;
  }

  // check if connected to the server
  if (connId == 0) {
    ts3_log("Unable to get client ID when not connected to a server", LogLevel_WARNING);
    return 0;
  }

  int status;
  int result = ts3Functions.getConnectionStatus(connId, &status);
  if (result != ERROR_ok) {
    ts3_log("Unable to get server connection status", LogLevel_WARNING);
    return 0;
  }

  // 1 = connected, 0 = not connected
  if (status <= 0) {
    ts3_log("Not connected to the server " + std::to_string(status), LogLevel_DEBUG);
    return 0;
  }

  // get client ID on this server
  anyID clientID;
  result = ts3Functions.getClientID(connId, &clientID);
  if (result != ERROR_ok) {
    ts3_log("Unable to get client ID", LogLevel_ERROR);
    return 0;
  }

  return clientID;
}

void ts3_setClientVolumeModifier(anyID clientID, float value) {

}

void ts3_setClientPosition(anyID clientID, const struct TS3_Vector *position) {

}

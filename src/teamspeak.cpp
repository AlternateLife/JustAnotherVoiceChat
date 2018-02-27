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

#include <stdlib.h>
#include <thread>
#include <chrono>
#include <teamspeak/public_rare_definitions.h>

#define BUFFER_LENGTH 256

static uint64 _serverConnectionHandler = 0;
static std::set<anyID> _mutedClients;
static std::string _originalNickname = "";

void ts3_log(std::string message, enum LogLevel severity) {
  ts3Functions.logMessage(message.c_str(), severity, "JustAnotherVoiceChat", 0);
}

bool ts3_verifyServer(std::string uniqueIdentifier) {
  // check if server is already connected
  uint64 *serverList;
  auto result = ts3Functions.getServerConnectionHandlerList(&serverList);
  if (result != ERROR_ok) {
    ts3_log("Unable to get server list", LogLevel_ERROR);
    return false;
  }

  // search for host in list, skip entry because it is 1 every time
  int index = 0;
  uint64 handle = serverList[index];
  _serverConnectionHandler = 0;

  while (handle != 0) {
    // get server's unique identifier
    char *uid;
    result = ts3Functions.getServerVariableAsString(handle, VIRTUALSERVER_UNIQUE_IDENTIFIER, &uid);
    if (result == ERROR_ok) {
      if (uniqueIdentifier.compare(uid) == 0) {
        _serverConnectionHandler = handle;
        ts3Functions.freeMemory(uid);

        break;
      }

      ts3Functions.freeMemory(uid);
    } else {
      ts3_log(std::to_string(result) + ": Failed to get server unique identifier", LogLevel_WARNING);
    }

    // get next handle
    index++;
    handle = serverList[index];
  }

  if (_serverConnectionHandler == 0) {
    ts3_log("Unable to find server match for " + uniqueIdentifier, LogLevel_WARNING);
  }

  // server list needs to be freeded after usage
  ts3Functions.freeMemory(serverList);

  return (_serverConnectionHandler != 0);
}

bool ts3_moveToChannel(uint64 channelId, std::string password) {
  // get client id
  auto clientId = ts3_clientId(_serverConnectionHandler);
  if (clientId == 0) {
    ts3_log("Unable to get client id for channel move", LogLevel_WARNING);
    return false;
  }

  auto result = ts3Functions.requestClientMove(_serverConnectionHandler, clientId, channelId, password.c_str(), NULL);
  if (result != ERROR_ok) {
    ts3_log("Unable to move into the channel " + std::to_string(channelId), LogLevel_WARNING);
    return false;
  }

  return true;
}

bool ts3_muteClient(anyID clientId, bool mute) {
  std::set<anyID> clients;
  clients.insert(clientId);

  return ts3_muteClients(clients, mute);
}

bool ts3_muteClients(std::set<anyID> &clients, bool mute) {
  if (clients.empty()) {
    return true;
  }

  // create client list
  anyID *clientIds = (anyID *)malloc((clients.size() + 1) * sizeof(anyID));

  int index = 0;
  for (auto it = clients.begin(); it != clients.end(); it++) {
    clientIds[index++] = *it;
  }

  // terminate array with zero element
  clientIds[index] = 0;

  // apply (un-)mute on clients
  unsigned int result;

  if (mute) {
    result = ts3Functions.requestMuteClients(_serverConnectionHandler, clientIds, NULL);
    if (result == ERROR_ok) {
      // add all new clients to cached list
      for (auto it = clients.begin(); it != clients.end(); it++) {
        ts3_log("Add client to cached list " + std::to_string(*it), LogLevel_DEBUG);
        _mutedClients.insert(*it);
      }
    } else {
      ts3_log("Unable to mute clients", LogLevel_DEBUG);
    }
  } else {
    result = ts3Functions.requestUnmuteClients(_serverConnectionHandler, clientIds, NULL);
    if (result == ERROR_ok) {
      // remove all clients from cached list
      for (auto it = clients.begin(); it != clients.end(); it++) {
        // client to be erased
        anyID clientId = *it;
        
        auto eraseIt = _mutedClients.begin();
        while (eraseIt != _mutedClients.end()) {
          if (*eraseIt == clientId) {
            ts3_log("Remove client from cached list " + std::to_string(*it), LogLevel_DEBUG);
            eraseIt = _mutedClients.erase(eraseIt);
          } else {
            eraseIt++;
          }
        }
      }
    } else {
      ts3_log("Unable to unmute clients", LogLevel_DEBUG);
    }
  }

  // cleanup list
  free(clientIds);

  return result == ERROR_ok;
}

bool ts3_unmuteAllClients() {
  if (_mutedClients.empty()) {
    return true;
  }

  // create client list
  anyID *clientIds = (anyID *)malloc((_mutedClients.size() + 1) * sizeof(anyID));

  int index = 0;
  for (auto it = _mutedClients.begin(); it != _mutedClients.end(); it++) {
    ts3_log("Try to unmute client " + std::to_string(*it), LogLevel_DEBUG);
    clientIds[index++] = *it;
  }

  // terminate array with zero element
  clientIds[index] = 0;

  // apply (un-)mute on clients
  auto result = ts3Functions.requestUnmuteClients(_serverConnectionHandler, clientIds, NULL);

  // cleanup list
  free(clientIds);

  _mutedClients.clear();

  return result == ERROR_ok;
}

std::set<anyID> ts3_clientsInChannel(uint64 channelId) {
  anyID *clientList;
  std::set<anyID> clients;

  // subscribe to channel to be able to get clients 
  uint64 channelIds[2];
  channelIds[0] = channelId;
  channelIds[1] = 0;

  auto result = ts3Functions.requestChannelSubscribe(_serverConnectionHandler, channelIds, NULL);
  if (result != ERROR_ok) {
    ts3_log("Unable to subscribe to channel " + std::to_string(channelId), LogLevel_WARNING);
    return clients;
  }

  int isSubscribed;

  do {
    ts3_log("Wait for channel subscription", LogLevel_DEBUG);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if (ts3Functions.getChannelVariableAsInt(_serverConnectionHandler, channelId, CHANNEL_FLAG_ARE_SUBSCRIBED, &isSubscribed) != ERROR_ok) {
      ts3_log("Unable to get channel's subscrition state " + std::to_string(channelId), LogLevel_WARNING);
      return clients;
    }
  } while(isSubscribed == false);

  result = ts3Functions.getChannelClientList(_serverConnectionHandler, channelId, &clientList);
  if (result != ERROR_ok) {
    ts3_log("Unable to get clients for channel " + std::to_string(channelId), LogLevel_WARNING);
    return clients;
  }

  // put client ids into the set
  int index = 0;
  anyID id = clientList[index];

  while (id != 0) {
    clients.insert(id);

    index++;
    id = clientList[index];
  }

  ts3Functions.freeMemory(clientList);
  return clients;
}

bool ts3_setNickname(std::string nickname) {
  if (_serverConnectionHandler == 0) {
    return false;
  }

  char *currentNickname;
  if (ts3Functions.getClientSelfVariableAsString(_serverConnectionHandler, CLIENT_NICKNAME, &currentNickname) != ERROR_ok) {
    ts3_log("Unable to get original nickname", LogLevel_WARNING);
    _originalNickname = "";
    return false;
  }

  if (nickname.compare(currentNickname) == 0) {
    return true;
  } else if (_originalNickname.compare("") == 0) {
    _originalNickname = std::string(currentNickname);
  }

  // set new nickname
  if (ts3Functions.setClientSelfVariableAsString(_serverConnectionHandler, CLIENT_NICKNAME, nickname.c_str()) != ERROR_ok) {
    ts3_log("Unable to rename client to " + nickname, LogLevel_WARNING);
    return false;
  }

  // update changes
  if (ts3Functions.flushClientSelfUpdates(_serverConnectionHandler, NULL) != ERROR_ok) {
    ts3_log("Error flushing client updates", LogLevel_ERROR);
    return false;
  }

  return true;
}

bool ts3_resetNickname() {
  if (_serverConnectionHandler == 0) {
    return false;
  }

  if (_originalNickname.compare("") == 0) {
    return true;
  }

  if (ts3Functions.setClientSelfVariableAsString(_serverConnectionHandler, CLIENT_NICKNAME, _originalNickname.c_str()) != ERROR_ok) {
    ts3_log("Unable to reset nickname to original " + _originalNickname, LogLevel_WARNING);
    return false;
  }

  // update changes
  if (ts3Functions.flushClientSelfUpdates(_serverConnectionHandler, NULL) != ERROR_ok) {
    ts3_log("Error flushing client updates", LogLevel_ERROR);
    return false;
  }

  _originalNickname = "";

  return true;
}

std::string ts3_getClientIdentity() {
  if (_serverConnectionHandler == 0) {
    return "";
  }

  // get client unique identity
  char *identity;
  if (ts3Functions.getClientSelfVariableAsString(_serverConnectionHandler, CLIENT_UNIQUE_IDENTIFIER, &identity) != ERROR_ok) {
    ts3_log("Unable to get client unique identity", LogLevel_ERROR);
    return "";
  }

  return std::string(identity);
}

bool ts3_setClientPosition(anyID clientId, float x, float y, float z) {
  if (_serverConnectionHandler == 0) {
    return false;
  }

  TS3_VECTOR position;
  position.x = x;
  position.y = y;
  position.z = z;

  if (ts3Functions.channelset3DAttributes(_serverConnectionHandler, clientId, &position) != ERROR_ok) {
    ts3_log("Unable to set position for client " + std::to_string(clientId), LogLevel_WARNING);
    return false;
  }

  return true;
}

bool ts3_resetListenerPosition() {
  if (_serverConnectionHandler == 0) {
    return false;
  }

  TS3_VECTOR position;
  position.x = 0;
  position.y = 0;
  position.z = 0;

  TS3_VECTOR forward;
  forward.x = 0;
  forward.y = 0;
  forward.z = 0;

  TS3_VECTOR up;
  up.x = 0;
  up.y = 0;
  up.z = 1;

  if (ts3Functions.systemset3DListenerAttributes(_serverConnectionHandler, &position, &forward, &up) != ERROR_ok) {
    ts3_log("Unable to reset 3D system settings", LogLevel_WARNING);
    return false;
  }

  return true;
}

bool ts3_set3DSettings(float distanceFactor, float rolloffScale) {
  if (_serverConnectionHandler == 0) {
    return false;
  }

  if (ts3Functions.systemset3DSettings(_serverConnectionHandler, distanceFactor, rolloffScale) != ERROR_ok) {
    ts3_log("Unable to set 3D system settings", LogLevel_DEBUG);
    return false;
  }

  return true;
}

void ts3_resetClients3DPositions() {
  if (_serverConnectionHandler == 0) {
    return;
  }

  auto channelId = ts3_channelId(_serverConnectionHandler);
  auto clients = ts3_clientsInChannel(channelId);

  for (auto it = clients.begin(); it != clients.end(); it++) {
    ts3_setClientPosition(*it, 0, 0, 0);
  }
}

uint64 ts3_serverConnectionHandle() {
  return _serverConnectionHandler;
}

anyID ts3_clientId(uint64 serverConnectionHandlerId) {
  // check if connected to the server
  if (serverConnectionHandlerId == 0) {
    ts3_log("Unable to get client ID when not connected to a server", LogLevel_WARNING);
    return 0;
  }

  int status;
  int result = ts3Functions.getConnectionStatus(serverConnectionHandlerId, &status);
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
  result = ts3Functions.getClientID(serverConnectionHandlerId, &clientID);
  if (result != ERROR_ok) {
    ts3_log("Unable to get client ID", LogLevel_ERROR);
    return 0;
  }

  return clientID;
}

uint64 ts3_channelId(uint64 serverConnectionHandlerId) {
  uint64 channelId;
  auto clientId = ts3_clientId(serverConnectionHandlerId);

  auto result = ts3Functions.getChannelOfClient(serverConnectionHandlerId, clientId, &channelId);
  if (result != ERROR_ok) {
    ts3_log("Unable to get current channel id: " + std::to_string(result), LogLevel_DEBUG);
    return 0;
  }

  return channelId;
}

bool ts3_isInputMuted(uint64 serverConnectionHandlerId) {
  int hardwareStatus;
  int deactivated;
  int muted;

  if (ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerId, CLIENT_INPUT_HARDWARE, &hardwareStatus) != ERROR_ok) {
    return false;
  } else if (hardwareStatus == HARDWAREINPUT_DISABLED) {
    return true;
  }

  if (ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerId, CLIENT_INPUT_DEACTIVATED, &deactivated) != ERROR_ok) {
    return false;
  } else if (deactivated == INPUT_DEACTIVATED) {
    return true;
  }

  if (ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerId, CLIENT_INPUT_MUTED, &muted) != ERROR_ok) {
    return false;
  }

  return muted == MUTEINPUT_MUTED;
}

bool ts3_isOutputMuted(uint64 serverConnectionHandlerId) {
  int hardwareStatus;
  int muted;

  if (ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerId, CLIENT_OUTPUT_HARDWARE, &hardwareStatus) != ERROR_ok) {
    return false;
  } else if (hardwareStatus == HARDWAREOUTPUT_DISABLED) {
    return true;
  }

  if (ts3Functions.getClientSelfVariableAsInt(serverConnectionHandlerId, CLIENT_OUTPUT_MUTED, &muted) != ERROR_ok) {
    return false;
  }

  return muted == MUTEOUTPUT_MUTED;
}

bool ts3_setOutputMuted(uint64 serverConnectionHandlerId, bool muted) {
  if (ts3Functions.setClientSelfVariableAsInt(serverConnectionHandlerId, CLIENT_OUTPUT_MUTED, muted) != ERROR_ok) {
    ts3_log("Unable to change client output mute", LogLevel_WARNING);
    return false;
  }

  // update changes
  if (ts3Functions.flushClientSelfUpdates(_serverConnectionHandler, NULL) != ERROR_ok) {
    ts3_log("Error flushing client updates", LogLevel_ERROR);
    return false;
  }

  return true;
}

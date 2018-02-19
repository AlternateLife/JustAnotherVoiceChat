/*
 * File: src/teamspeakPlugin.cpp
 * Date: 15.02.2018
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

#include "teamspeakPlugin.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "version.h"
#include "justAnotherVoiceChat.h"
#include "teamspeak.h"

#define PLUGIN_API_VERSION 22;

struct TS3Functions ts3Functions;
static char versionBuffer[16];

const char *ts3plugin_name() {
  return "JustAnotherVoiceChat";
}

const char *ts3plugin_version() {
  sprintf(versionBuffer, "%d.%d.%d.%d", JUSTANOTHERVOICECHAT_VERSION_MAJOR, JUSTANOTHERVOICECHAT_VERSION_MINOR, JUSTANOTHERVOICECHAT_VERSION_PATCH, JUSTANOTHERVOICECHAT_VERSION_BUILD);
  return versionBuffer;
}

int ts3plugin_apiVersion() {
  return PLUGIN_API_VERSION;
}

const char *ts3plugin_author() {
  return "JustAnotherVoiceChat";
}

const char *ts3plugin_description() {
  return "3D game voice plugin for communicating with a game server based on JustAnotherVoiceChat.\nFor more information see https://github.com/AlternateLife/JustAnotherVoiceChat";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
  ts3Functions = funcs;
}

int ts3plugin_init() {
  if (JustAnotherVoiceChat_start() == false) {
    return 1;
  }

  return 0;
}

void ts3plugin_shutdown() {
  JustAnotherVoiceChat_stop();
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
  if (serverConnectionHandlerID != ts3_serverConnectionHandle()) {
    return;
  }

  anyID ownId = ts3_clientId(serverConnectionHandlerID);
  if (clientID != ownId) {
    return;
  }

  JustAnotherVoiceChat_updateTalking(status == STATUS_TALKING);
}

void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char* oldValue, const char* newValue) {
  // only listen to input and output mute events
  if (serverConnectionHandlerID != ts3_serverConnectionHandle()) {
    return;
  }

  if (flag != CLIENT_INPUT_MUTED && flag != CLIENT_OUTPUT_MUTED) {
    return;
  }

  // update mute state
  bool mute = strcmp(newValue, "1") == 0;

  if (flag == CLIENT_INPUT_MUTED) {
    JustAnotherVoiceChat_updateMicrophoneMute(mute);
  } else if (flag == CLIENT_OUTPUT_MUTED) {
    JustAnotherVoiceChat_updateSpeakersMute(mute);
  }
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {
  if (serverConnectionHandlerID != ts3_serverConnectionHandle()) {
    return;
  }

  // skip the check for myself
  if (clientID == ts3_clientId(serverConnectionHandlerID)) {
      return;
  }

  // check if client moved into my channel
  auto ownChannel = ts3_channelId(serverConnectionHandlerID);
  if (ownChannel == newChannelID) {
    ts3_log("Mute client on join", LogLevel_DEBUG);
    ts3_muteClient(clientID, true);
    ts3_log("Muted joined client", LogLevel_DEBUG);
    return;
  }

  // check if client moved out of my channel
  if (ownChannel == oldChannelID) {
    ts3_log("Unmute client on leave", LogLevel_DEBUG);
    ts3_muteClient(clientID, false);
    ts3_log("Unmuted left client", LogLevel_DEBUG);
    return;
  }
}

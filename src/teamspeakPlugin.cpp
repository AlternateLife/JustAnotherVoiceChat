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
  return "3D game voice plugin for communicating with the JustAnotherVoiceChat server";
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

/*
 * File: src/justAnotherVoiceChat.cpp
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

#include "justAnotherVoiceChat.h"

#include <iostream>
#include <enet/enet.h>

#include "protocol.h"
#include "httpServer.h"
#include "teamspeak.h"
#include "client.h"

HttpServer *httpServer = nullptr;
Client *client = nullptr;

bool JustAnotherVoiceChat_start() {
  ts3_log("Initialize", LogLevel_INFO);

  if (enet_initialize() != 0) {
    ts3_log("Unable to initialize ENet", LogLevel_ERROR);
    return false;
  }

  if (client != nullptr) {
    delete client;
  }

  client = new Client();
  
  httpServer = new HttpServer();
  httpServer->open(HTTP_PORT);

  ts3_connect("localhost", 9987, "");

  return true;
}

void JustAnotherVoiceChat_stop() {
  ts3_log("Shutting down", LogLevel_INFO);

  httpServer->close();
  delete httpServer;

  client->disconnect();
  delete client;

  enet_deinitialize();

  ts3_log("Shutdown", LogLevel_INFO);
}

bool JustAnotherVoiceChat_connect(std::string host, uint16_t port, uint16_t uniqueIdentifier) {
  if (client == nullptr) {
    return false;
  }

  if (client->connect(host, port, uniqueIdentifier) == false) {
    ts3_log("Unable to connect to " + host + ":" + std::to_string(port), LogLevel_WARNING);
    return false;
  }

  return true;
}

void JustAnotherVoiceChat_disconnect() {
  if (client == nullptr) {
    return;
  }

  client->disconnect();
}

void JustAnotherVoiceChat_updateTalking(bool talking) {
  if (client == nullptr) {
    return;
  }

  client->setTalking(talking);
}

void JustAnotherVoiceChat_updateMicrophoneMute(bool muted) {
  if (client == nullptr) {
    return;
  }

  client->setMicrophoneMuted(muted);
}

void JustAnotherVoiceChat_updateSpeakersMute(bool muted) {
  if (client == nullptr) {
    return;
  }

  client->setSpeakersMuted(muted);
}

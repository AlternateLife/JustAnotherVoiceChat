/*
 * File: include/client.h
 * Date: 09.02.2018
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

#pragma once

#include <enet/enet.h>

#include <string>
#include <thread>

#include "protocol.h"

class Client {
private:
  ENetHost *_client;
  ENetPeer *_peer;

  std::thread *_thread;
  bool _running;
  uint16_t _gameId;
  uint16_t _teamspeakId;
  std::string _host;
  uint16_t _port;
  uint64_t _lastChannelId;

  bool _talking;
  bool _microphoneMuted;
  bool _speakersMuted;

public:
  Client();
  virtual ~Client();

  bool connect(std::string host, uint16_t port, uint16_t uniqueIdentifier);
  void disconnect();
  bool isOpen() const;

  void setTalking(bool talking);
  void setMicrophoneMuted(bool muted);
  void setSpeakersMuted(bool muted);
  bool isTalking() const;
  bool hasMicrophoneMuted() const;
  bool hasSpeakersMuted() const;

private:
  void close();
  void update();
  void abortThread();

  void sendHandshake(int statusCode = STATUS_CODE_OK);
  void sendStatus();
  void handleMessage(ENetEvent &event);
  void handleHandshakeResponse(ENetPacket *packet);
  void handleUpdateMessage(ENetPacket *packet);

  void sendPacket(void *data, size_t length, int channelId, bool reliable = true);
};

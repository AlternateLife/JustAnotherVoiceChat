/*
 * File: include/protocol.h
 * Date: 13.02.2018
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

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

#define ENET_PORT 23332
#define HTTP_PORT 23333

#define NETWORK_CHANNELS 3
#define NETWORK_HANDSHAKE_CHANNEL 0
#define NETWORK_UPDATE_CHANNEL 1
#define NETWORK_STATUS_CHANNEL 2

#define STATUS_CODE_OK 0
#define STATUS_CODE_UNKNOWN_ERROR 1
#define STATUS_CODE_NOT_CONNECTED_TO_SERVER 2
#define STATUS_CODE_NOT_MOVED_TO_CHANNEL 3

typedef struct {
  uint16_t clientId;
  float x;
  float y;
  float z;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(clientId, x, y, z);
  }
} clientPositionUpdate_t;

typedef struct {
  uint16_t clientId;
  bool muted;
  float volume;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(clientId, muted, volume);
  }
} clientVolumeUpdate_t;

typedef struct {
  int statusCode;
  std::string reason;

  std::string teamspeakEndpoint;
  uint16_t teamspeakPort;
  std::string teamspeakPassword;

  uint64_t channelId;
  std::string channelPassword;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(statusCode, reason, teamspeakEndpoint, teamspeakPort, teamspeakPassword, channelId, channelPassword);
  }
} handshakeResponsePacket_t;

typedef struct {
  uint16_t gameId;
  uint16_t teamspeakId;
  int statusCode;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(gameId, teamspeakId);
  }
} handshakePacket_t;

typedef struct {
  std::vector<clientPositionUpdate_t> positions;
  std::vector<clientVolumeUpdate_t> volumes;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(positions, volumes);
  }
} updatePacket_t;

typedef struct {
  bool talking;
  bool microphoneMuted;
  bool speakersMuted;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(talking, microphoneMuted, speakersMuted);
  }
} statusPacket_t;

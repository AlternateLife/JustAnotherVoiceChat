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

#define PROTOCOL_VERSION_MAJOR 1
#define PROTOCOL_VERSION_MINOR 3
#define PROTOCOL_MIN_VERSION_MAJOR 1
#define PROTOCOL_MIN_VERSION_MINOR 3

#define ENET_PORT 23332
#define HTTP_PORT 23333

#define NETWORK_CHANNELS 6
#define NETWORK_PROTOCOL_CHANNEL 0
#define NETWORK_HANDSHAKE_CHANNEL 1
#define NETWORK_UPDATE_CHANNEL 2
#define NETWORK_STATUS_CHANNEL 3
#define NETWORK_CONTROL_CHANNEL 4
#define NETWORK_POSITION_CHANNEL 5

#define STATUS_CODE_OK 0
#define STATUS_CODE_UNKNOWN_ERROR 1
#define STATUS_CODE_NOT_CONNECTED_TO_SERVER 2
#define STATUS_CODE_NOT_MOVED_TO_CHANNEL 3
#define STATUS_CODE_UNABLE_TO_MUTE_CLIENTS 4
#define STATUS_CODE_OUTDATED_PROTOCOL_VERSION 5

#define DISCONNECT_STATUS_OK 0
#define DISCONNECT_STATUS_RECONNECT 1
#define DISCONNECT_STATUS_OUTDATED_SERVER 2
#define DISCONNECT_STATUS_OUTDATED_CLIENT 3
#define DISCONNECT_STATUS_REJECTED 4

typedef struct {
  int versionMajor;
  int versionMinor;
  int minimumVersionMajor;
  int minimumVersionMinor;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(versionMajor), CEREAL_NVP(versionMinor), CEREAL_NVP(minimumVersionMajor), CEREAL_NVP(minimumVersionMinor));
  }
} protocolPacket_t;

typedef struct {
  int statusCode;

  int versionMajor;
  int versionMinor;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(statusCode), CEREAL_NVP(versionMajor), CEREAL_NVP(versionMinor));
  }
} protocolResponsePacket_t;

typedef struct {
  uint16_t teamspeakId;
  float x;
  float y;
  float z;
  float voiceRange;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(teamspeakId), CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z), CEREAL_NVP(voiceRange));
  }
} clientPositionUpdate_t;

typedef struct {
  float x;
  float y;
  float z;
  float rotation;

  std::vector<clientPositionUpdate_t> positions;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z), CEREAL_NVP(rotation), CEREAL_NVP(positions));
  }
} positionPacket_t;

typedef struct {
  uint16_t teamspeakId;
  bool muted;
  float volume;
  std::string filterKey;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(teamspeakId), CEREAL_NVP(muted), CEREAL_NVP(volume));
  }
} clientAudioUpdate_t;

typedef struct {
  int statusCode;
  std::string reason;

  std::string teamspeakServerUniqueIdentifier;

  uint64_t channelId;
  std::string channelPassword;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(statusCode), CEREAL_NVP(reason), CEREAL_NVP(teamspeakServerUniqueIdentifier), CEREAL_NVP(channelId), CEREAL_NVP(channelPassword));
  }
} handshakeResponsePacket_t;

typedef struct {
  uint16_t gameId;
  uint16_t teamspeakId;
  int statusCode;

  std::string teamspeakClientUniqueIdentity;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(gameId), CEREAL_NVP(teamspeakId), CEREAL_NVP(statusCode), CEREAL_NVP(teamspeakClientUniqueIdentity));
  }
} handshakePacket_t;

typedef struct {
  std::vector<clientAudioUpdate_t> audioUpdates;
  std::vector<clientPositionUpdate_t> positionUpdates;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(audioUpdates), CEREAL_NVP(positionUpdates));
  }
} updatePacket_t;

typedef struct {
  bool talking;
  bool microphoneMuted;
  bool speakersMuted;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(talking), CEREAL_NVP(microphoneMuted), CEREAL_NVP(speakersMuted));
  }
} statusPacket_t;

typedef struct {
  std::string nickname;

  template <class Archive>
  void serialize(Archive &ar) {
    ar(CEREAL_NVP(nickname));
  }
} controlPacket_t;

inline bool verifyProtocolVersion(int major, int minor, int minMajor, int minMinor) {
  // check major number
  if (major > minMajor) {
    return true;
  } else if (major < minMajor) {
    return false;
  }

  // check minor number
  if (minor > minMinor) {
    return true;
  } else if (minor < minMinor) {
    return false;
  }

  return true;
}

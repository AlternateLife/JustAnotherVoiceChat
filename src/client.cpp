/*
 * File: src/client.cpp
 * Date: 09.02.2018
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

#include "client.h"

#include "teamspeak.h"
#include "protocol.h"

#define NETWORK_CHANNELS 2

Client::Client() {
  _client = nullptr;
  _peer = nullptr;
  _thread = nullptr;
  _stopping = false;
  _uniqueIdentifier = 0;
}

Client::~Client() {
  close();
}

bool Client::connect(std::string host, uint16_t port, uint16_t uniqueIdentifier) {
  if (isOpen()) {
    return false;
  }

  _client = enet_host_create(NULL, 1, NETWORK_CHANNELS, 0, 0);
  if (_client == NULL) {
    _client = nullptr;
    return false;
  }

  // connect to host
  ENetAddress address;
  enet_address_set_host(&address, host.c_str());
  address.port = port;

  _peer = enet_host_connect(_client, &address, NETWORK_CHANNELS, 0);
  if (_peer == NULL) {
    enet_peer_reset(_peer);
    enet_host_destroy(_client);

    _client = nullptr;
    _peer = nullptr;
    return false;
  }

  _thread = new std::thread(&Client::update, this);
  _uniqueIdentifier = uniqueIdentifier;
  return true;
}

void Client::disconnect() {
  enet_peer_disconnect(_peer, 0);
  _stopping = true;
}

bool Client::isOpen() const {
  return _client != nullptr && _peer != nullptr && _stopping == false;
}

void Client::close() {
  if (_thread != nullptr) {
    delete _thread;
    _thread = nullptr;
  }
  
  if (_client != nullptr) {
    enet_host_destroy(_client);
    _client = nullptr;
  }

  if (_peer != nullptr) {
    enet_peer_reset(_peer);
    _peer = nullptr;
  }
}

void Client::update() {
  ENetEvent event;

  while(true) {
    if (enet_host_service(_client, &event, 1000) > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
          ts3_log("Connection established", LogLevel_DEBUG);

          sendHandshake();
          break;

        case ENET_EVENT_TYPE_DISCONNECT:
          ts3_log("Connection closed", LogLevel_DEBUG);

          close();
          break;

        case ENET_EVENT_TYPE_RECEIVE:
          if (_stopping) {
            enet_packet_destroy(event.packet);
            break;
          }

          handleMessage(event);

          enet_packet_destroy(event.packet);
          break;

        default:

          break;
      }
    }
  }
}

void Client::handleMessage(ENetEvent &event) {
  switch (event.channelID) {
    case NETWORK_HANDSHAKE_CHANNEL:

      break;

    case NETWORK_UPDATE_CHANNEL:

      break;

    default:

      break;
  }
}

void Client::sendHandshake() {
  handshakePacket_t packet;
  packet.gameId = _uniqueIdentifier;
  packet.teamspeakId = ts3_clientID();

  // serialize payload
  std::ostringstream os;

  try {
    cereal::BinaryOutputArchive archive(os);
    archive(packet);
  } catch (std::exception &e) {
    ts3_log(e.what(), LogLevel_ERROR);
    return;
  }

  auto data = os.str();
  sendPacket((void *)data.c_str(), data.size(), NETWORK_HANDSHAKE_CHANNEL);
}

void Client::sendPacket(void *data, size_t length, int channelId, bool reliable) {
  enet_uint32 flags = 0;

  if (reliable) {
    flags |= ENET_PACKET_FLAG_RELIABLE;
  }

  auto packet = enet_packet_create(data, length, flags);
  enet_peer_send(_peer, channelId, packet);
}

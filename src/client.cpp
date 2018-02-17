/*
 * File: src/client.cpp
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

#include "client.h"

#include "teamspeak.h"
#include "protocol.h"

Client::Client() {
  _client = nullptr;
  _peer = nullptr;
  _thread = nullptr;
  _running = false;
  _uniqueIdentifier = 0;
  _talking = false;
  _microphoneMuted = false;
  _speakersMuted = false;
}

Client::~Client() {
  close();
}

bool Client::connect(std::string host, uint16_t port, uint16_t uniqueIdentifier) {
  if (host == _host && _port == port && _uniqueIdentifier == uniqueIdentifier) {
    return true;
  }

  if (isOpen()) {
    disconnect();
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
    close();
    return false;
  }

  // check if connecting is successful
  ENetEvent event;
  if (enet_host_service(_client, &event, 5000) <= 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
    close();
    return false;
  }

  abortThread();

  _uniqueIdentifier = uniqueIdentifier;
  _host = host;
  _port = port;
  _running = true;

  // start update thread
  _thread = new std::thread(&Client::update, this);
  ts3_log("Connection established", LogLevel_DEBUG);

  sendHandshake();
  return true;
}

void Client::disconnect() {
  if (_peer == nullptr) {
    return;
  }

  enet_peer_disconnect(_peer, 0);

  // stop main update thread
  abortThread();

  ENetEvent event;
  while (enet_host_service(_client, &event, 3000) > 0) {
    if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
      break;
    }

    // discard any other event
    if (event.packet != 0) {
      enet_packet_destroy(event.packet);
    }
  }

  ts3_log("Disconnected", LogLevel_INFO);

  _host = "";
  _port = 0;
  _uniqueIdentifier = 0;

  close();
}

bool Client::isOpen() const {
  return _client != nullptr && _peer != nullptr && _running;
}

void Client::setTalking(bool talking) {
  _talking = talking;

  sendStatus();
}

void Client::setMicrophoneMuted(bool muted) {
  _microphoneMuted = muted;

  sendStatus();
}

void Client::setSpeakersMuted(bool muted) {
  _speakersMuted = muted;

  sendStatus();
}

bool Client::isTalking() const {
  return _talking;
}

bool Client::hasMicrophoneMuted() const {
  return _microphoneMuted;
}

bool Client::hasSpeakersMuted() const {
  return _speakersMuted;
}

void Client::close() {
  abortThread();
  
  if (_peer != nullptr) {
    enet_peer_reset(_peer);
    _peer = nullptr;
  }

  if (_client != nullptr) {
    enet_host_destroy(_client);
    _client = nullptr;
  }

  _running = false;
  _host = "";
  _port = 0;
  _uniqueIdentifier = 0;
}

void Client::update() {
  ENetEvent event;

  while(_running) {
    int code = enet_host_service(_client, &event, 100);

    if (code > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_DISCONNECT:
          ts3_log("Connection closed", LogLevel_DEBUG);

          _running = false;
          break;

        case ENET_EVENT_TYPE_RECEIVE:
          if (_running == false) {
            enet_packet_destroy(event.packet);
            break;
          }

          // handle message and delete payload afterwards
          handleMessage(event);
          enet_packet_destroy(event.packet);
          break;

        default:
          break;
      }
    } else if (code < 0) {
      ts3_log("Network error occured " + std::to_string(code), LogLevel_DEBUG);
      _running = false;
    }
  }

  close();
}

void Client::abortThread() {
  // stop thread
  if (_thread == nullptr) {
    return;
  }

  if (_thread->get_id() != std::this_thread::get_id()) {
    if (_thread->joinable()) {
      _running = false;
      _thread->join();
    }

    // delete object
    delete _thread;
    _thread = nullptr;
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

void Client::sendStatus() {
  statusPacket_t packet;
  packet.talking = _talking;
  packet.microphoneMuted = _microphoneMuted;
  packet.speakersMuted = _speakersMuted;

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
  sendPacket((void *)data.c_str(), data.size(), NETWORK_STATUS_CHANNEL);
}

void Client::handleMessage(ENetEvent &event) {
  switch (event.channelID) {
    case NETWORK_HANDSHAKE_CHANNEL:
      handleHandshapeResponse(event.packet);
      break;

    case NETWORK_UPDATE_CHANNEL:
      handleUpdateMessage(event.packet);
      break;

    case NETWORK_STATUS_CHANNEL:
      
      break;

    default:
      ts3_log("Unknown message on channel " + std::to_string(event.channelID), LogLevel_INFO);
      break;
  }
}

void Client::handleHandshapeResponse(ENetPacket *packet) {
  // deserialize payload
  responsePacket_t responsePacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is(data);

  try {
    cereal::BinaryInputArchive archive(is);
    archive(responsePacket);
  } catch (std::exception &e) {
    ts3_log(e.what(), LogLevel_ERROR);
    return;
  }

  if (responsePacket.statusCode != STATUS_CODE_OK) {
    ts3_log("Handshake failed: " + std::to_string(responsePacket.statusCode) + ": " + responsePacket.reason , LogLevel_WARNING);
    return;
  }

  ts3_log("Handshake successful", LogLevel_DEBUG);
}

void Client::handleUpdateMessage(ENetPacket *packet) {
  // deserialize payload
  updatePacket_t updatePacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is;

  try {
    cereal::BinaryInputArchive archive(is);
    archive(updatePacket);
  } catch (std::exception &e) {
    ts3_log(e.what(), LogLevel_ERROR);
    return;
  }

  ts3_log("Update received: " + data, LogLevel_DEBUG);
}

void Client::sendResponse(int statusCode, std::string reason, int channelId) {
  responsePacket_t packet;
  packet.statusCode = statusCode;
  packet.reason = reason;
  
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
  sendPacket((void *)data.c_str(), data.size(), channelId);
}

void Client::sendPacket(void *data, size_t length, int channelId, bool reliable) {
  enet_uint32 flags = 0;

  if (reliable) {
    flags |= ENET_PACKET_FLAG_RELIABLE;
  }

  auto packet = enet_packet_create(data, length, flags);
  enet_peer_send(_peer, channelId, packet);
}

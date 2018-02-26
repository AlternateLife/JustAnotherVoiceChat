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

Client::Client() {
  _client = nullptr;
  _peer = nullptr;
  _thread = nullptr;
  _running = false;
  _gameId = 0;
  _teamspeakId = 0;
  _talking = false;
  _microphoneMuted = false;
  _speakersMuted = false;
  _lastChannelId = 0;
}

Client::~Client() {
  close();
}

bool Client::connect(std::string host, uint16_t port, uint16_t uniqueIdentifier) {
  if (host == _host && _port == port && _gameId == uniqueIdentifier) {
    return true;
  }

  if (isOpen()) {
    disconnect(DISCONNECT_STATUS_RECONNECT);
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

  _gameId = uniqueIdentifier;
  _host = host;
  _port = port;
  _teamspeakId = 0;
  _running = true;

  // start update thread
  _thread = new std::thread(&Client::update, this);
  ts3_log("Connection established", LogLevel_DEBUG);

  sendProtocolMessage();
  return true;
}

void Client::disconnect(uint32_t status) {
  if (_peer == nullptr) {
    return;
  }

  enet_peer_disconnect(_peer, status);

  // stop main update thread
  abortThread();

  ENetEvent event;
  while (_client != nullptr && enet_host_service(_client, &event, 3000) > 0) {
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
  _gameId = 0;
  _teamspeakId = 0;

  close();
}

bool Client::isOpen() const {
  return _client != nullptr && _peer != nullptr && _running;
}

bool Client::isIngame() const {
  return isOpen() && _gameId != 0 && _teamspeakId != 0;
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
  _gameId = 0;
  _teamspeakId = 0;

  // move back to old teamspeak channel
  if (_lastChannelId == 0) {
    return;
  }

  auto serverHandle = ts3_serverConnectionHandle();
  if (serverHandle == 0) {
    return;
  }

  auto channelId = ts3_channelId(serverHandle);
  if (channelId == _lastChannelId) {
    return;
  }

  // TODO: Save channel password
  auto result = ts3_moveToChannel(_lastChannelId, "");
  if (result == false) {
    ts3_log("Unable to move to saved channel " + std::to_string(_lastChannelId), LogLevel_WARNING);
    _lastChannelId = 0;
    return;
  }

  _lastChannelId = 0;

  ts3_resetClients3DPositions();

  // unmute all muted clients after moved out of channel and reset custom nickname
  ts3_unmuteAllClients();
  ts3_resetNickname();
}

void Client::update() {
  ENetEvent event;

  while(_running && _client != nullptr) {
    int code = enet_host_service(_client, &event, 100);

    if (code > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_DISCONNECT:
          ts3_log("Connection closed by the server", LogLevel_DEBUG);

          event.peer->data = NULL;
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

void Client::sendProtocolMessage() {
  protocolPacket_t packet;
  packet.versionMajor = PROTOCOL_VERSION_MAJOR;
  packet.versionMinor = PROTOCOL_VERSION_MINOR;
  packet.minimumVersionMajor = PROTOCOL_MIN_VERSION_MAJOR;
  packet.minimumVersionMinor = PROTOCOL_MIN_VERSION_MINOR;

  // serialize payload
  std::ostringstream os;

  try {
    cereal::BinaryOutputArchive archive(os);
    archive(packet);
  } catch (std::exception &e) {
    ts3_log(std::string("sendProtocolMessage: ") + e.what(), LogLevel_ERROR);
    return;
  }

  auto data = os.str();
  sendPacket((void *)data.c_str(), data.size(), NETWORK_PROTOCOL_CHANNEL);
}

void Client::sendHandshake(int statusCode) {
  handshakePacket_t packet;
  packet.gameId = _gameId;
  packet.teamspeakId = _teamspeakId;
  packet.statusCode = statusCode;

  packet.teamspeakClientUniqueIdentity = ts3_getClientIdentity();

  // serialize payload
  std::ostringstream os;

  try {
    cereal::BinaryOutputArchive archive(os);
    archive(packet);
  } catch (std::exception &e) {
    ts3_log(std::string("sendHandshake: ") + e.what(), LogLevel_ERROR);
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
    ts3_log(std::string("sendStatus: ") + e.what(), LogLevel_ERROR);
    return;
  }

  auto data = os.str();
  sendPacket((void *)data.c_str(), data.size(), NETWORK_STATUS_CHANNEL);
}

void Client::handleMessage(ENetEvent &event) {
  switch (event.channelID) {
    case NETWORK_PROTOCOL_CHANNEL:
      handleProtocolResponse(event.packet);
      break;

    case NETWORK_HANDSHAKE_CHANNEL:
      handleHandshakeResponse(event.packet);
      break;

    case NETWORK_UPDATE_CHANNEL:
      handleUpdateMessage(event.packet);
      break;

    case NETWORK_CONTROL_CHANNEL:
      handleControlMessage(event.packet);
      break;

    case NETWORK_POSITION_CHANNEL:
      handlePositionMessage(event.packet);
      break;

    default:
      ts3_log("Unknown message on channel " + std::to_string(event.channelID), LogLevel_INFO);
      break;
  }
}

void Client::handleProtocolResponse(ENetPacket *packet) {
  // deserialize payload
  protocolResponsePacket_t protocolPacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is(data);

  try {
    cereal::BinaryInputArchive archive(is);
    archive(protocolPacket);
  } catch (std::exception &e) {
    ts3_log(std::string("handleProtocolResponse: ") + e.what(), LogLevel_ERROR);
    return;
  }

  // compare protocol versions
  if (verifyProtocolVersion(protocolPacket.versionMajor, protocolPacket.versionMinor, PROTOCOL_MIN_VERSION_MAJOR, PROTOCOL_MIN_VERSION_MINOR) == false) {
    ts3_log("Server uses an outdated protocol version: " + std::to_string(protocolPacket.versionMajor) + "." + std::to_string(protocolPacket.versionMinor), LogLevel_WARNING);

    disconnect(DISCONNECT_STATUS_OUTDATED_SERVER);
    return;
  }

  if (protocolPacket.statusCode != STATUS_CODE_OK) {
    ts3_log("Client uses an outdated protocol version: " + std::to_string(PROTOCOL_VERSION_MAJOR) + "." + std::to_string(PROTOCOL_VERSION_MINOR), LogLevel_WARNING);

    disconnect(DISCONNECT_STATUS_OUTDATED_CLIENT);
    return;
  }

  // protocol matches, send handshake
  sendHandshake();
}

void Client::handleHandshakeResponse(ENetPacket *packet) {
  // deserialize payload
  handshakeResponsePacket_t responsePacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is(data);

  try {
    cereal::BinaryInputArchive archive(is);
    archive(responsePacket);
  } catch (std::exception &e) {
    ts3_log(std::string("handleHandshakeResponse: ") + e.what(), LogLevel_ERROR);
    return;
  }

  if (responsePacket.statusCode != STATUS_CODE_OK) {
    ts3_log("Handshake failed: " + std::to_string(responsePacket.statusCode) + ": " + responsePacket.reason , LogLevel_WARNING);
    return;
  }

  if (ts3_verifyServer(responsePacket.teamspeakServerUniqueIdentifier) == false) {
    ts3_log(std::string("Unable to find teamspeak server: ") + responsePacket.teamspeakServerUniqueIdentifier, LogLevel_WARNING);
    sendHandshake(STATUS_CODE_NOT_CONNECTED_TO_SERVER);
    return;
  }

  // save old channel for later use
  auto serverHandle =  ts3_serverConnectionHandle();
  auto lastChannelId = ts3_channelId(serverHandle);
  
  if (ts3_moveToChannel(responsePacket.channelId, responsePacket.channelPassword) == false) {
    ts3_log(std::string("Unable to move into channel ") + std::to_string(responsePacket.channelId), LogLevel_WARNING);
    sendHandshake(STATUS_CODE_NOT_MOVED_TO_CHANNEL);
    return;
  }

  // mute all clients by default
  auto clients = ts3_clientsInChannel(responsePacket.channelId);
  for (auto it = clients.begin(); it != clients.end(); it++) {
    ts3_muteClient(*it, true);
  }

  // if (ts3_muteClients(clients, true) == false) {
  //   ts3_log("Unable to mute clients on joining channel " + std::to_string(responsePacket.channelId), LogLevel_WARNING);
  //   sendHandshake(STATUS_CODE_UNABLE_TO_MUTE_CLIENTS);
  //   return;
  // }

  // connection on teamspeak server is valid, save teamspeak id
  _teamspeakId = ts3_clientId(serverHandle);
  _lastChannelId = lastChannelId;

  sendHandshake();
  ts3_log("Handshake successful", LogLevel_DEBUG);

  // get initial sound status
  _microphoneMuted = ts3_isInputMuted(serverHandle);
  _speakersMuted = ts3_isOutputMuted(serverHandle);

  sendStatus();

  ts3_resetListenerPosition();
  ts3_set3DSettings(2.0f, 3.0f);
}

void Client::handleUpdateMessage(ENetPacket *packet) {
  // deserialize payload
  updatePacket_t updatePacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is(data);

  try {
    cereal::BinaryInputArchive archive(is);
    archive(updatePacket);
  } catch (std::exception &e) {
    ts3_log(std::string("handleUpdateMessage: ") + e.what(), LogLevel_ERROR);
    return;
  }

  // handle volume changes
  std::set<anyID> muteClients;
  std::set<anyID> unmuteClients;

  for (auto it = updatePacket.audioUpdates.begin(); it != updatePacket.audioUpdates.end(); it++) {
    if ((*it).muted) {
      ts3_log("Mute teamspeak user " + std::to_string((*it).teamspeakId), LogLevel_DEBUG);

      muteClients.insert((*it).teamspeakId);
    } else {
      ts3_log("Unmute teamspeak user " + std::to_string((*it).teamspeakId), LogLevel_DEBUG);

      unmuteClients.insert((*it).teamspeakId);
    }
  }

  ts3_muteClients(muteClients, true);
  ts3_muteClients(unmuteClients, false);

  // handle position changes
  for (auto it = updatePacket.positionUpdates.begin(); it != updatePacket.positionUpdates.end(); it++) {
    ts3_setClientPosition((*it).teamspeakId, (*it).x, (*it).y, (*it).z);
  }
}

void Client::handleControlMessage(ENetPacket *packet) {
  // deserialize payload
  controlPacket_t controlPacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is(data);

  try {
    cereal::BinaryInputArchive archive(is);
    archive(controlPacket);
  } catch (std::exception &e) {
    ts3_log(std::string("handleControlMessage: ") + e.what(), LogLevel_ERROR);
    return;
  }

  // handle control packet
  if (controlPacket.nickname.compare("") != 0) {
    ts3_setNickname(controlPacket.nickname);
  }
}

void Client::handlePositionMessage(ENetPacket *packet) {
  // deserialize payload
  positionPacket_t positionPacket;

  std::string data((char *)packet->data, packet->dataLength);
  std::istringstream is(data);

  try {
    cereal::BinaryInputArchive archive(is);
    archive(positionPacket);
  } catch (std::exception &e) {
    ts3_log(std::string("handlePositionMessage: ") + e.what(), LogLevel_ERROR);
    return;
  }

  // update all clients
  for (auto it = positionPacket.positions.begin(); it != positionPacket.positions.end(); it++) {
    ts3_setClientPosition((*it).teamspeakId, (*it).x, (*it).y, (*it).z);
  }
}

void Client::sendPacket(void *data, size_t length, int channelId, bool reliable) {
  enet_uint32 flags = 0;

  if (reliable) {
    flags |= ENET_PACKET_FLAG_RELIABLE;
  }

  auto packet = enet_packet_create(data, length, flags);
  enet_peer_send(_peer, (enet_uint8)channelId, packet);
}

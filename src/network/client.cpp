/*
 * File: srcnetwork/client.cpp
 * Date: 09.08.2018
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

#include "network/client.h"

#include "teamspeak.h"

javic::network::Client::Client() {
    _host = nullptr;
    _peer = nullptr;
    _thread = nullptr;
}

bool javic::network::Client::connect(std::string host, uint16_t port) {
    if (isConnected()) {
        ts3_log("Network client is already connected", LogLevel_INFO);

        return true;
    }

    _host = enet_host_create(NULL, 1, PACKET_CHANNELS, 0, 0);
    if (_host == nullptr) {
        ts3_log("Unable to create network host", LogLevel_ERROR);

        close();

        return false;
    }

    // connect to host
    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    _peer = enet_host_connect(_host, &address, PACKET_CHANNELS, 0);
    if (_peer == nullptr) {
        ts3_log("Unable to connect to host address", LogLevel_ERROR);

        close();

        return false;
    }

    // Wait for connect event
    ENetEvent event;
    if (enet_host_service(_host, &event, 5000) <= 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
        ts3_log("Connection to host failed", LogLevel_WARNING);

        close();

        return false;
    }

    // create network thread
    _thread = std::make_shared<std::thread>(&Client::update, this);

    ts3_log("Connection established", LogLevel_DEBUG);

    // TODO: Send protocol packet

    return true;
}

void javic::network::Client::disconnect() {

}

bool javic::network::Client::isConnected() const {

}

bool javic::network::Client::sendPacket(std::shared_ptr<javic::network::NetworkPacket> packet) {

}

void javic::network::Client::update() {
    
}

void javic::network::Client::close() {

}

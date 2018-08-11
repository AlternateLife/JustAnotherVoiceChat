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
#include "taskRunner.h"

javic::network::Client::Client() {
    _host = nullptr;
    _peer = nullptr;
    _thread = nullptr;
    _stopped = true;

    _taskRunner = std::make_shared<TaskRunner>();
}

bool javic::network::Client::connect(std::string host, uint16_t port) {
    if (isConnected()) {
        ts3_log("Network client is already connected", LogLevel_INFO);

        return true;
    }

    _host = enet_host_create(nullptr, 1, PACKET_CHANNELS, 0, 0);
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
    _stopped = false;
    _thread = std::make_shared<std::thread>(&Client::update, this);

    ts3_log("Connection established", LogLevel_DEBUG);

    // TODO: Send protocol packet

    return true;
}

void javic::network::Client::disconnect() {
    if (_peer == nullptr) {
        return;
    }

    ts3_log("Disconnecting", LogLevel_DEBUG);

    stopThread();

    // graceful disconnect
    enet_peer_disconnect(_peer, 0);

    ENetEvent event;
    bool disconnected = false;

    while (_host != nullptr && enet_host_service(_host, &event, 3000) > 0) {
        if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            disconnected = true;
            break;
        }

        if (event.packet != 0) {
            enet_packet_destroy(event.packet);
        }
    }

    if (disconnected) {
        ts3_log("Successful disconnected", LogLevel_DEBUG);
    } else {
        ts3_log("Disconnecting timed out", LogLevel_WARNING);
    }

    close();
}

bool javic::network::Client::isConnected() const {
    return _host != nullptr && _peer != nullptr && _stopped == false;
}

bool javic::network::Client::sendPacket(std::shared_ptr<javic::network::NetworkPacket> packet) {
    auto task = std::make_shared<ResultTask<bool>>([this, packet]() {
        bool result = false;
        auto data = packet->serialize(&result);
        if (result == false) {
            ts3_log("Error serializing network packet", LogLevel_ERROR);

            return false;
        }

        enet_uint32 flags = 0;
        if (packet->reliable()) {
            flags |= ENET_PACKET_FLAG_RELIABLE;
        }

        auto rawPacket = enet_packet_create(data.c_str(), data.length(), flags);
        enet_peer_send(_peer, packet->channel(), rawPacket);

        return true;
    });

    return _taskRunner->runTask(task);
}

void javic::network::Client::update() {
    while (_stopped == false) {
        handleEvents();

        _taskRunner->update();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ts3_log("Network thread stopped", LogLevel_INFO);
}

void javic::network::Client::handleEvents() {
    if (_host == nullptr) {
        return;
    }

    ENetEvent event;
    auto code = enet_host_service(_host, &event, 100);
    if (code < 0) {
        ts3_log("Network error: " + std::to_string(code), LogLevel_WARNING);

        // TODO: Stop network thread?

        return;
    }

    if (code == 0) {
        return;
    }

    switch (event.type) {
        case ENET_EVENT_TYPE_DISCONNECT:

            break;

        case ENET_EVENT_TYPE_RECEIVE:

            break;

        default:
            break;
    }
}

void javic::network::Client::close() {
    ts3_log("Closing network client", LogLevel_DEBUG);

    stopThread();

    if (_peer != nullptr) {
        enet_peer_reset(_peer);
        _peer = nullptr;
    }

    if (_host != nullptr) {
        enet_host_destroy(_host);
        _host = nullptr;
    }

    ts3_log("Closed network client", LogLevel_DEBUG);
}

void javic::network::Client::stopThread() {
    if (_thread == nullptr) {
        return;
    }

    if (_thread->get_id() == std::this_thread::get_id()) {
        return;
    }

    _stopped = true;

    if (_thread->joinable()) {
        _thread->join();
    }

    _thread = nullptr;
}

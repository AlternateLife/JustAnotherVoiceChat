/*
 * File: include/networkPacket.h
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

#include <sstream>
#include <memory>

#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#define ENET_PORT 23332
#define HTTP_PORT 23333

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

namespace javic {
    namespace network {
        const enet_uint8 PACKET_CHANNELS = 6;
        const enet_uint8 PACKET_CHANNEL_PROTOCOL = 0;
        const enet_uint8 PACKET_CHANNEL_HANDSHAKE = 1;
        const enet_uint8 PACKET_CHANNEL_UPDATE = 2;
        const enet_uint8 PACKET_CHANNEL_STATUS = 3;
        const enet_uint8 PACKET_CHANNEL_CONTROL = 4;
        const enet_uint8 PACKET_CHANNEL_POSITION = 5;

        class NetworkPacket {
        protected:
            uint8_t _type;

        public:
            NetworkPacket(uint8_t type) {
                _type = type;
            }

            virtual ~NetworkPacket() = default;

            virtual enet_uint8 channel() const = 0;

            virtual bool reliable() const {
                return true;
            }

            void setType(uint8_t type) {
                _type = type;
            }

            uint8_t type() const {
                return _type;
            }

            std::string serialize(bool *result) {
                std::ostringstream os;

                try {
                    cereal::BinaryOutputArchive archive(os);
                    archive(shared_from_this());
                } catch (std::exception &e) {
                    if (result != nullptr) {
                        *result = false;
                    }

                    return "";
                }

                if (result != nullptr) {
                    *result = true;
                }

                return os.str();
            }

            static std::shared_ptr<NetworkPacket> deserialize(ENetPacket *packet, bool *result) {
                std::string data((char *) packet->data, packet->dataLength);
                std::istringstream is(data);

                std::shared_ptr<NetworkPacket> outputPacket;

                try {
                    cereal::BinaryInputArchive archive(is);
                    archive(outputPacket);
                } catch (std::exception &e) {
                    if (result != nullptr) {
                        *result = false;
                    }

                    return nullptr;
                }

                if (result != nullptr) {
                    *result = true;
                }

                return outputPacket;
            }
        };
    }
}

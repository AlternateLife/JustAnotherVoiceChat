/*
 * File: include/protocolPacket.h
 * Date: 20.07.2018
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

#include "networkPacket.h"

namespace javic {
    namespace network {
        const enet_uint8 PROTOCOL_VERSION_MAJOR = 2;
        const enet_uint8 PROTOCOL_VERSION_MINOR = 0;
        const enet_uint8 PROTOCOL_MIN_VERSION_MAJOR = 2;
        const enet_uint8 PROTOCOL_MIN_VERSION_MINOR = 0;

        typedef enum {
            PROTOCOL_PACKET_TYPE_UNKNOWN = 0,
            PROTOCOL_PACKET_TYPE_REQUEST,
            PROTOCOL_PACKET_TYPE_RESPONSE
        } protocolPacketType_t;

        typedef enum {
            PROTOCOL_PACKET_STATUS_CODE_UNKNOWN = 0,
            PROTOCOL_PACKET_STATUS_CODE_OK,
            PROTOCOL_PACKET_STATUS_CODE_OUTDATED
        } protocolPacketStatusCode_t;

        class ProtocolPacket : public NetworkPacket {
        protected:
            int _versionMajor;
            int _versionMinor;
            int _minimumVersionMajor;
            int _minimumVersionMinor;

            protocolPacketStatusCode_t _statusCode;

        public:
            ProtocolPacket() : NetworkPacket(PROTOCOL_PACKET_TYPE_UNKNOWN) {
                _versionMajor = PROTOCOL_VERSION_MAJOR;
                _versionMinor = PROTOCOL_VERSION_MINOR;
                _minimumVersionMajor = PROTOCOL_MIN_VERSION_MAJOR;
                _minimumVersionMinor = PROTOCOL_MIN_VERSION_MINOR;
                _statusCode = PROTOCOL_PACKET_STATUS_CODE_UNKNOWN;
            }

            ProtocolPacket(uint8_t type) : NetworkPacket(type) {
                _versionMajor = PROTOCOL_VERSION_MAJOR;
                _versionMinor = PROTOCOL_VERSION_MINOR;
                _minimumVersionMajor = PROTOCOL_MIN_VERSION_MAJOR;
                _minimumVersionMinor = PROTOCOL_MIN_VERSION_MINOR;
                _statusCode = PROTOCOL_PACKET_STATUS_CODE_UNKNOWN;
            }

            enet_uint8 channel() const override {
                return PACKET_CHANNEL_PROTOCOL;
            }

            void setStatusCode(protocolPacketStatusCode_t statusCode) {
                _statusCode = statusCode;
            }

            protocolPacketStatusCode_t statusCode() const {
                return _statusCode;
            }

            friend class cereal::access;

            template<class Archive>
            void serialize(Archive &ar) {
                ar(CEREAL_NVP(_type),
                   CEREAL_NVP(_versionMajor),
                   CEREAL_NVP(_versionMinor),
                   CEREAL_NVP(_minimumVersionMajor),
                   CEREAL_NVP(_minimumVersionMinor),
                   CEREAL_NVP(_statusCode)
                );
            }

            static bool verifyProtocolVersion(int major, int minor, int minMajor, int minMinor) {
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
        };
    }
}

CEREAL_REGISTER_TYPE(javic::ProtocolPacket);

CEREAL_REGISTER_POLYMORPHIC_RELATION(javic::NetworkPacket, javic::ProtocolPacket);

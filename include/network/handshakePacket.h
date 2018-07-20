/*
 * File: include/network/handshakePacket.h
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
    typedef enum {
        HANDSHAKE_PACKET_TYPE_UNKNOWN = 0,

    } handshakePacketType_t;

    typedef enum {
        HANDSHAKE_PACKET_STATUS_CODE_UNKNOWN = 0,
    } handshakePacketStatusCode_t;

    class HandshakePacket : public NetworkPacket {
    protected:
        uint16_t _gameId;
        uint16_t _teamspeakId;
        std::string _teamspeakUniqueIdentifier;

        std::string _reason;
        uint64_t _channelId;
        std::string _channelPassword;

        handshakePacketStatusCode_t _statusCode;


    public:
        HandshakePacket() : NetworkPacket(HANDSHAKE_PACKET_TYPE_UNKNOWN) {
            _gameId = 0;
            _teamspeakId = 0;
            _teamspeakUniqueIdentifier = "";
            _reason = "";
            _channelId = 0;
            _channelPassword = "";
            _statusCode = HANDSHAKE_PACKET_STATUS_CODE_UNKNOWN;
        }

        HandshakePacket(handshakePacketType_t type) : NetworkPacket(type) {
            _gameId = 0;
            _teamspeakId = 0;
            _teamspeakUniqueIdentifier = "";
            _reason = "";
            _channelId = 0;
            _channelPassword = "";
            _statusCode = HANDSHAKE_PACKET_STATUS_CODE_UNKNOWN;
        }

        enet_uint8 channel() const override {
            return PACKET_CHANNEL_HANDSHAKE;
        }

        void setGameId(uint16_t gameId) {
            _gameId = gameId;
        }

        uint16_t gameId() const {
            return _gameId;
        }

        void setTeamspeakId(uint16_t teamspeakId) {
            _teamspeakId = teamspeakId;
        }

        uint16_t teamspeakId() const {
            return _teamspeakId;
        }

        void setTeamspeakUniqueIdentifier(std::string identifier) {
            _teamspeakUniqueIdentifier = identifier;
        }

        std::string teamspeakUniqueIdentifier() const {
            return _teamspeakUniqueIdentifier;
        }

        void setReason(std::string reason) {
            _reason = reason;
        }

        std::string reason() const {
            return _reason;
        }

        void setChannelId(uint64_t channelId) {
            _channelId = channelId;
        }

        uint64_t channelId() const {
            return _channelId;
        }

        void setChannelPassword(std::string channelPassword) {
            _channelPassword = channelPassword;
        }

        std::string channelPassword() const {
            return _channelPassword;
        }

        void setStatusCode(handshakePacketStatusCode_t statusCode) {
            _statusCode = statusCode;
        }

        handshakePacketStatusCode_t statusCode() const {
            return _statusCode;
        }

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive &ar) {
            ar(CEREAL_NVP(_type),
               CEREAL_NVP(_gameId),
               CEREAL_NVP(_teamspeakId),
               CEREAL_NVP(_teamspeakUniqueIdentifier),
               CEREAL_NVP(_reason),
               CEREAL_NVP(_channelId),
               CEREAL_NVP(_channelPassword),
               CEREAL_NVP(_statusCode)
            );
        }
    };
}

CEREAL_REGISTER_TYPE(javic::HandshakePacket);

CEREAL_REGISTER_POLYMORPHIC_RELATION(javic::NetworkPacket, javic::HandshakePacket);

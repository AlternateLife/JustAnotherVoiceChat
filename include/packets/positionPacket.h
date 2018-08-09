/*
 * File: include/positionPacket.h
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
#include "positionData.h"

#include <memory>
#include <vector>

namespace javic {
    namespace packets {
        typedef enum {
            POSITION_PACKET_TYPE_UNKNOWN = 0,
        } positionPacketType_t;

        class PositionPacket : public NetworkPacket {
        protected:
            float _x;
            float _y;
            float _z;
            float _rotation;

            std::vector<std::shared_ptr<PositionData>> _positions;

        public:
            PositionPacket : NetworkPacket(POSITION_PACKET_TYPE_UNKNOWN) {
                _x = 0;
                _y = 0;
                _z = 0;
                _rotation = 0;
            }

            PositionPacket(type) : NetworkPacket(type) {
                _x = 0;
                _y = 0;
                _z = 0;
                _rotation = 0;
            }

            enet_uint8 channel() const override {
                return PACKET_CHANNEL_POSITION;
            }

            bool reliable() const override {
                return false;
            }

            void setX(float x) {
                _x = x;
            }

            float x() const {
                return _x;
            }

            void setY(float y) {
                _y = y;
            }

            float y() const {
                return _y;
            }

            void setZ(float z) {
                _z = z;
            }

            float z() const {
                return _z;
            }

            void setRotation(float rotation) {
                _rotation = rotation;
            }

            float rotation() const {
                return _rotation;
            }

            void addPosition(std::shared_ptr<PositionData> position) {
                _positions.push_back(position);
            }

            std::shared_ptr<PositionData> positions() const {
                return _positions;
            }

            friend class cereal::access;

            template<class Archive>
            void serialize(Archive &ar) {
                ar(CEREAL_NVP(_type),
                   CEREAL_NVP(_x),
                   CEREAL_NVP(_y),
                   CEREAL_NVP(_z),
                   CEREAL_NVP(_rotation),
                   CEREAL_NVP(_positions)
                );
            }
        };
    }
}

CEREAL_REGISTER_TYPE(javic::PositionPacket);

CEREAL_REGISTER_POLYMORPHIC_RELATION(javic::NetworkPacket, javic::PositionPacket);

/*
 * File: include/updatePacket.h
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
#include "audioControlData.h"

namespace javic {
    typedef enum {
        UPDATE_PACKET_TYPE_UNKNOWN = 0,
    } updatePacketType_t;

    class UpdatePacket : public NetworkPacket {
    protected:
        std::vector<std::shared_ptr<AudioControlData>> _audioUpdates;
        std::vector<std::shared_ptr<PositionData>> _positionUpdates;

    public:
        UpdatePacket() : NetworkPacket(UPDATE_PACKET_TYPE_UNKNOWN) {

        }

        void addAudioControlData(std::shared_ptr<AudioControlData> audioControlData) {
            _audioUpdates.push_back(audioControlData);
        }

        std::vector<std::shared_ptr<AudioControlData>> audioUpdates() const {
            return _audioUpdates;
        }

        void addPositionData(std::shared_ptr<PositionData> positionData) {
            _positionUpdates.push_back(positionData);
        }

        std::vector<std::shared_ptr<PositionData>> positionUpdates() const {
            return _positionUpdates;
        }

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive &ar) {
            ar(CEREAL_NVP(_type),
               CEREAL_NVP(_audioUpdates),
               CEREAL_NVP(_positionUpdates)
            );
        }
    };
}
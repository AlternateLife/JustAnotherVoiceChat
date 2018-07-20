/*
 * File: include/positionData.h
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

namespace javic {
    class PositionData {
    private:
        uint16_t _teamspeakId;
        float _x;
        float _y;
        float _z;
        float _voiceRange;

    public:
        PositionData() {
            _teamspeakId = 0;
            _x = 0;
            _y = 0;
            _z = 0;
            _voiceRange = 0;
        }

        // TODO: Add Constructor with client pointer

        void setTeamspeakId(uint16_t teamspeakId) {
            _teamspeakId = teamspeakId;
        }

        uint16_t teamspeakId() const {
            return _teamspeakId;
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

        void setVoiceRange(float voiceRange) {
            _voiceRange = voiceRange;
        }

        float voiceRange() const {
            return _voiceRange;
        }

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive &ar) {
            ar(CEREAL_NVP(_teamspeakId),
               CEREAL_NVP(_x),
               CEREAL_NVP(_y),
               CEREAL_NVP(_z),
               CEREAL_NVP(_voiceRange)
            );
        }
    };
}
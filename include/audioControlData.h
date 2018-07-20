/*
 * File: include/audioControlData.h
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

#include <string>

namespace javic {
    class AudioControlData {
    protected:
        uint16_t _teamspeakId;
        bool _muted;
        float _volume;
        std::string _filterKey;

    public:
        AudioControlData() {
            _teamspeakId = 0;
            _muted = 0;
            _volume = 0;
            _filterKey = "";
        }

        void setTeamspeakId(uint16_t teamspeakId) {
            _teamspeakId = teamspeakId;
        }

        uint16_t teamspeakId() const {
            return _teamspeakId;
        }

        void setMuted(bool muted) {
            _muted = muted;
        }

        bool muted() const {
            return _muted;
        }

        void setVolume(float volume) {
            _volume = volume;
        }

        float volume() const {
            return _volume;
        }

        void setFilterKey(std::string filterKey) {
            _filterKey = filterKey;
        }

        std::string filterKey() const {
            return _filterKey;
        }

        friend class cereal::access;

        template <class Archive>
        void serialize(Archive &ar) {
            ar(CEREAL_NVP(_teamspeakId),
               CEREAL_NVP(_muted),
               CEREAL_NVP(_volume),
               CEREAL_NVP(_filterKey)
            );
        }
    };
}
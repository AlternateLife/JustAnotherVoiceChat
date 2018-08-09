/*
 * File: include/task.h
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

#pragma once

#include <functional>
#include <future>

namespace javic {
    class Task {
    public:
        Task() = default;

        virtual void run() = 0;
    };

    class AsyncTask : public Task {
    private:
        std::function<void()> _callback;

    public:
        AsyncTask(std::function<void()> callback) : Task() {
            _callback = callback;
        }

        void run() override {
            _callback();
        }

        std::function<void()> callback() {
            return _callback;
        }
    };

    template<class T>
    class ResultTask : public Task {
    private:
        std::promise<T> _promise;
        std::function<T()> _callback;

    public:
        ResultTask(std::function<T()> callback) : Task() {
            _callback = callback;
        }

        void run() override {
            _promise.set_value(_callback());
        }

        std::promise<T> &promise() {
            return _promise;
        }

        std::function<T()> callback() const {
            return _callback;
        }
    };

    class VoidResultTask : public Task {
    private:
        std::promise<void> _promise;
        std::function<void()> _callback;

    public:
        VoidResultTask(std::function<void()> callback) : Task() {
            _callback = callback;
        }

        void run() override {
            _callback();

            _promise.set_value();
        }

        std::promise<void> &promise() {
            return _promise;
        }

        std::function<void()> callback() {
            return _callback;
        }
    };
}
/*
 * File: include/taskRunner.h
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

#include <vector>
#include <functional>
#include <memory>
#include <mutex>

#include "tasks.h"
#include "teamspeak.h"

namespace javic {
    class TaskRunner {
    private:
        std::vector<std::shared_ptr<Task>> _tasks;
        std::mutex _mutex;
        std::thread::id _threadId;

    public:
        TaskRunner() = default;
        virtual ~TaskRunner() = default;

        template<class T>
        T runTask(std::shared_ptr<ResultTask<T>> task) {
            if (isUpdateThread()) {
                return task->callback()();
            }

            auto future = task->promise().get_future();

            addTask(std::static_pointer_cast<Task>(task));

            return future.get();
        }

        void runTask(std::shared_ptr<VoidResultTask> task) {
            if (isUpdateThread()) {
                task->callback()();

                return;
            }

            auto future = task->promise().get_future();

            addTask(std::static_pointer_cast<Task>(task));

            future.wait();
        }

        void runTask(std::shared_ptr<AsyncTask> task) {
            if (isUpdateThread()) {
                task->callback()();

                return;
            }

            addTask(std::static_pointer_cast<Task>(task));
        }

        void update() {
            if (isUpdateThread() == false) {
                _threadId = std::this_thread::get_id();
            }

            // make a copy of the tasks and clear the actual list
            std::unique_lock<std::mutex> guard(_mutex);

            std::vector<std::shared_ptr<Task>> tasks;
            tasks.swap(_tasks);

            guard.unlock();

            for (auto &task : tasks) {
                try {
                    task->run();
                } catch (std::exception &e) {
                    ts3_log(e.what(), LogLevel_ERROR);
                }
            }
        }

        void clear() {
            std::lock_guard<std::mutex> guard(_mutex);

            _tasks.clear();
        }

    private:
        bool isUpdateThread() const {
            return _threadId == std::this_thread::get_id();
        }

        void addTask(std::shared_ptr<Task> task) {
            std::lock_guard<std::mutex> guard(_mutex);

            _tasks.push_back(task);
        }
    };
}

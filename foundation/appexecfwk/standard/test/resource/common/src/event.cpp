/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "event.h"
#include <cstdio>
#include <iostream>

namespace STtools {
int WaitCompleted(Event &event, const std::string &eventName, const int code, const int timeout)
{
    return event.WaitingMessage(std::to_string(code) + eventName, timeout, false);
}

void Completed(Event &event, const std::string &eventName, const int code)
{
    return event.CompleteMessage(std::to_string(code) + eventName);
}

void CleanMsg(Event &event)
{
    return event.Clean();
}

Event::Event()
{
    waiting_message_ = "";
    complete_message_.clear();
}

Event::~Event()
{
    waiting_message_ = "";
    std::vector<std::string> tmp_vector;
    tmp_vector.swap(complete_message_);
    complete_message_.clear();
}

bool Event::Compare()
{
    if (!waiting_message_.empty()) {
        for (size_t i = 0; i < complete_message_.size(); i++) {
            if (waiting_message_.compare(complete_message_.at(i)) == 0) {
                complete_message_.erase(std::begin(complete_message_) + i, std::begin(complete_message_) + i + 1);
                waiting_message_ = "";
                return true;
            }
        }
    }
    return false;
}

int Event::WaitingMessage(const std::string &message, int timeout_ms, bool locked)
{
    std::unique_lock<std::mutex> lock(mutex_);
    waiting_message_ = message;
    if (Compare()) {
        return 0;
    }

    if (locked) {
        cv_.wait(lock);
        return 0;
    }

    if (cv_.wait_for(lock, std::chrono::seconds(timeout_ms)) == std::cv_status::timeout) {
        waiting_message_ = "";
        return -1;
    }
    return 0;
}

void Event::CompleteMessage(const std::string &message)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (waiting_message_.compare(message) == 0) {
        waiting_message_ = "";
        cv_.notify_all();
        return;
    }
    complete_message_.push_back(message);
    return;
}

void Event::Clean()
{
    waiting_message_ = "";
    complete_message_.clear();
}
}  // namespace STtools
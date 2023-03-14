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

#ifndef UTILS_EVENT_HANDLER_H
#define UTILS_EVENT_HANDLER_H

#include <cstdint>
#include <map>
#include <functional>

namespace OHOS {
namespace Utils {

class EventReactor;

class EventHandler {
public:
    using Callback = std::function<void()>;

    EventHandler(int fd, EventReactor* r);
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&&) = delete;
    EventHandler(const EventHandler&&) = delete;
    ~EventHandler() {}

    int GetHandle() const { return (fd_); }
    uint32_t Events() const { return (events_); }

    void EnableRead();
    void EnableWrite();
    void DisableWrite();
    void DisableAll();

    const EventReactor* GetEventReactor() const { return reactor_; }

    void SetCloseCallback(const Callback& closeCallback) { closeCallback_ = closeCallback; }
    void SetErrorCallback(const Callback& errorCallback) { errorCallback_ = errorCallback; }
    void SetReadCallback(const Callback& readCallback) { readCallback_ = readCallback; }
    void SetWriteCallback(const Callback& writeCallback) { writeCallback_ = writeCallback; }

    void HandleEvents(uint32_t events);

private:
    void Update();

private:
    int             fd_;
    uint32_t        events_;
    EventReactor*   reactor_;

    Callback  readCallback_;
    Callback  writeCallback_;
    Callback  closeCallback_;
    Callback  errorCallback_;
};

}
}
#endif /* UTILS_EVENT_HANDLER_H_ */

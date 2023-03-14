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

#ifndef UTILS_EVENT_DEMULTIPLEXER_H
#define UTILS_EVENT_DEMULTIPLEXER_H

#include <mutex>
#include <cstdint>
#include <map>

namespace OHOS {
namespace Utils {

class EventHandler;

class EventDemultiplexer {
public:
    EventDemultiplexer();
    EventDemultiplexer(const EventDemultiplexer&) = delete;
    EventDemultiplexer& operator=(const EventDemultiplexer&) = delete;
    virtual ~EventDemultiplexer();

    uint32_t StartUp();
    void CleanUp();

    void Polling(int timeout);

    uint32_t UpdateEventHandler(EventHandler* handler);
    uint32_t RemoveEventHandler(EventHandler* handler);

private:
    uint32_t Update(int operation, EventHandler* handler);
    static uint32_t Reactor2Epoll(uint32_t reactorEvent);
    static uint32_t Epoll2Reactor(uint32_t epollEvents);

    int epollFd_;
    int maxEvents_;
    std::recursive_mutex mutex_;
    std::map<int, EventHandler*> eventHandlers_; // guard by mutex_
};

}
}
#endif /* UTILS_EVENT_DEMULTIPLEXER_H_ */

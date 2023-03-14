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

#include "event_handler.h"
#include "event_reactor.h"
#include <sys/epoll.h>

namespace OHOS {
namespace Utils {

EventHandler::EventHandler(int fd, EventReactor* r)
    :fd_(fd), events_(EventReactor::NONE_EVENT), reactor_(r)
{
}

void EventHandler::EnableRead()
{
    events_ |= EventReactor::READ_EVENT;
    Update();
}

void EventHandler::EnableWrite()
{
    events_ |= EventReactor::WRITE_EVENT;
    Update();
}

void EventHandler::DisableWrite()
{
    events_ &= ~EventReactor::WRITE_EVENT;
    Update();
}
void EventHandler::DisableAll()
{
    events_ = EventReactor::NONE_EVENT;
    Update();
}

void EventHandler::HandleEvents(uint32_t events)
{
    if (events & (EventReactor::CLOSE_EVENT)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (events & (EventReactor::ERROR_EVENT)) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (events & (EventReactor::READ_EVENT)) {
        if (readCallback_) {
            readCallback_();
        }
    }

    if (events & (EventReactor::WRITE_EVENT)) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}

void EventHandler::Update()
{
    if (reactor_ != nullptr) {
        reactor_->UpdateEventHandler(this);
    }
}

} // Utils
} // OHOS

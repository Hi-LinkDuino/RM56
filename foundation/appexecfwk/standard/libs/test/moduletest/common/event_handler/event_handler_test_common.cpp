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

#include "event_handler_test_common.h"

namespace OHOS {
namespace AppExecFwk {
std::atomic<bool> CommonUtils::eventRun_ {false};
std::atomic<bool> CommonUtils::taskCalled_ {false};
std::atomic<uint32_t> CommonUtils::eventRunTimes_ {0};

MyEventHandler::MyEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
{}

MyEventHandler::~MyEventHandler()
{}

void MyEventHandler::ProcessEvent(const InnerEvent::Pointer &event)
{
    auto eventId = event->GetInnerEventId();
    if (eventId == STOP_EVENT_ID) {
        GetEventRunner()->Stop();
    }

    if (eventId == RUN_EVENT_ID) {
        CommonUtils::EventRunSet(true);
        CommonUtils::EventRunCountIncrement();
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
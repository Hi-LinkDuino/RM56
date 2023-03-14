/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "mock_scan_state_machine.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_SCAN_LABEL("MockScanStateMachine");

namespace OHOS {
namespace Wifi {
void MockScanStateMachine::SendMessage(int what)
{
    WIFI_LOGD("MockScanStateMachine::SendMessage, what is %{public}d.", what);
}

void MockScanStateMachine::SendMessage(int what, int arg1)
{
    WIFI_LOGD("MockScanStateMachine::SendMessage, what is %{public}d, arg1 is %{public}d.", what, arg1);
}

void MockScanStateMachine::SendMessage(int what, int arg1, int arg2)
{
    WIFI_LOGD("MockScanStateMachine::SendMessage, what is %{public}d, arg1 is %{public}d, arg2 is %{public}d.", what,
        arg1, arg2);
}

void MockScanStateMachine::SendMessage(InternalMessage *msg)
{
    if (msg == nullptr) {
        return;
    }
    WIFI_LOGD("MockScanStateMachine::SendMessage, msg is %{public}d.", msg->GetMessageName());
}

void MockScanStateMachine::StartTimer(int timerName, int64_t interval)
{
    (void)timerName;
    (void)interval;
    WIFI_LOGD("Enter MockScanStateMachine::StartTimer");
}
void MockScanStateMachine::StopTimer(int timerName)
{
    (void)timerName;
    WIFI_LOGD("Enter MockScanStateMachine::StopTimer");
}
} // namespace Wifi
} // namespace OHOS
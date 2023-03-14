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
#include "ap_root_state.h"
#include <typeinfo>
#include "ap_macro.h"
#include "ap_state_machine.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApRootState");
namespace OHOS {
namespace Wifi {
ApRootState::ApRootState() : State("ApRootState")
{}

ApRootState::~ApRootState()
{}

void ApRootState::GoInState()
{
    WIFI_LOGI("%{public}s  GoInState.", GetStateName().c_str());
}

void ApRootState::GoOutState()
{
    WIFI_LOGI("%{public}s  GoOutState.", GetStateName().c_str());
}

bool ApRootState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        WIFI_LOGE("fatal error!");
        return false;
    }
    int msgName = msg->GetMessageName();

    WIFI_LOGI("msg = [%{public}d] is not handled.", msgName);
    return EXECUTED;
}
}  // namespace Wifi
}  // namespace OHOS
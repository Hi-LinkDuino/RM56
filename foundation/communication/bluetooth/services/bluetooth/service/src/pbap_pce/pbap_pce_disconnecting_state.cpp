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

#include "pbap_pce_disconnecting_state.h"
#include "pbap_pce_service.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
PceDisconnectingState::PceDisconnectingState(
    const std::string &name, PbapPceStateMachine &stm, BaseObserverList<IPbapPceObserver> &observerMgrList)
    : PceBaseState(name, stm, observerMgrList)
{}

void PceDisconnectingState::Entry()
{
    PBAP_PCE_LOG_INFO("%{public}s start, Observer()->OnServiceConnectionStateChanged()->DISCONNECTING", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    observerMgrList_.ForEach([device](IPbapPceObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::DISCONNECTING));
    });

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PceDisconnectingState::Exit()
{}

bool PceDisconnectingState::Dispatch(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    uint8_t ret = true;
    switch (msg.what_) {
        case PCE_OBEX_DISCONNECTED:
            Transition(PCE_DISCONNECTED_STATE);
            break;
        case PCE_REQ_SET_TARGET_STATE:
            stm_.SetTargetState(msg.arg1_);
            break;
        case PCE_REQ_TRANSIT_TARGET_STATE:
            stm_.TransitTargetState();
            break;
        default:
            stm_.TryReleasePbapMsg(msg);
            return false;
    }

    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS
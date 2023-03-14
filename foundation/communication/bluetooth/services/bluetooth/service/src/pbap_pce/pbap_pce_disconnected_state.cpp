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

#include "pbap_pce_disconnected_state.h"
#include "btm.h"
#include "pbap_pce_def.h"
#include "pbap_pce_service.h"
#include "pbap_pce_state_machine.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
PceDisconnectedState::PceDisconnectedState(
    const std::string &name, PbapPceStateMachine &stm, BaseObserverList<IPbapPceObserver> &observerMgrList)
    : PceBaseState(name, stm, observerMgrList)
{}

void PceDisconnectedState::Entry()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (stm_.GetGap() != nullptr) {  // when sdpsearch failure, stm_.GetGap() is nullptr
        int retVal = stm_.GetGap()->Deregister();
        if (retVal != BT_NO_ERROR) {
            PBAP_PCE_LOG_ERROR("ProcessDisconnected GAP_Deregister() error");
        }
    }

    PBAP_PCE_LOG_INFO("%{public}s, observer->OnServiceConnectionStateChanged", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_PBAP_PCE, device);
    observerMgrList_.ForEach([device](IPbapPceObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::DISCONNECTED));
    });
    stm_.RemoveBTMLogging();
    stm_.TransitTargetState();
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PceDisconnectedState::Exit()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool PceDisconnectedState::Dispatch(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    switch (msg.what_) {
        case PCE_REQ_SET_TARGET_STATE:
            stm_.SetTargetState(msg.arg1_);
            break;
        case PCE_REQ_TRANSIT_TARGET_STATE:
            stm_.TransitTargetState();
            break;
        default:
            stm_.TryReleasePbapMsg(msg);
            break;
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    return true;
}
}  // namespace bluetooth
}  // namespace OHOS

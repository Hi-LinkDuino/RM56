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

#include "hfp_hf_call_manager.h"

#include "hfp_hf_defines.h"
#include "hfp_hf_service.h"

namespace OHOS {
namespace bluetooth {
HfpHfCallManager::HfpHfCallManager(const std::string &address) : address_(address)
{}

HfpHfCallManager::~HfpHfCallManager()
{
    calls_.clear();
    updateCalls_.clear();
}

HandsFreeUnitCalls *HfpHfCallManager::GetCallById(int id)
{
    std::map<int, HandsFreeUnitCalls>::iterator it = calls_.find(id);
    if (it == calls_.end()) {
        return nullptr;
    }
    return &calls_[id];
}

HandsFreeUnitCalls *HfpHfCallManager::GetCallByState(int state)
{
    std::map<int, HandsFreeUnitCalls>::iterator it;
    for (it = calls_.begin(); it != calls_.end(); ++it) {
        if (it->second.GetState() == state) {
            return &(it->second);
        }
    }
    return nullptr;
}

std::vector<HandsFreeUnitCalls> HfpHfCallManager::GetCurrentCalls()
{
    std::vector<HandsFreeUnitCalls> calls;
    std::map<int, HandsFreeUnitCalls>::iterator it;
    for (it = calls_.begin(); it != calls_.end(); ++it) {
        calls.push_back(it->second);
    }
    return calls;
}

void HfpHfCallManager::UpdateCall(HandsFreeUnitCalls &call)
{
    updateCalls_[call.GetId()] = call;
}

void HfpHfCallManager::UpdateCallDone()
{
    std::map<int, HandsFreeUnitCalls>::iterator iter;
    std::map<int, HandsFreeUnitCalls>::iterator updateiter;
    for (iter = calls_.begin(); iter != calls_.end();) {
        updateiter = updateCalls_.find(iter->first);
        if (updateiter == updateCalls_.end()) {
            iter->second.SetState(HFP_HF_CALL_STATE_FINISHED);
            NotifyCallChanged(iter->second);
            calls_.erase(iter++);
        } else {
            if ((iter->second.GetState() != updateiter->second.GetState()) ||
                (iter->second.GetNumber() != updateiter->second.GetNumber()) ||
                (iter->second.IsMultiParty() != updateiter->second.IsMultiParty())) {
                iter->second.SetState(updateiter->second.GetState());
                iter->second.SetNumber(updateiter->second.GetNumber());
                iter->second.SetMultiParty(updateiter->second.IsMultiParty());
                NotifyCallChanged(iter->second);
            }
            ++iter;
        }
    }

    for (updateiter = updateCalls_.begin(); updateiter != updateCalls_.end(); updateiter++) {
        iter = calls_.find(updateiter->first);
        if (iter == calls_.end()) {
            calls_[updateiter->first] = updateiter->second;
            NotifyCallChanged(updateiter->second);
        }
    }
    updateCalls_.clear();
    return;
}

int HfpHfCallManager::GetCallNumberByState(int state)
{
    int number = 0;
    std::map<int, HandsFreeUnitCalls>::iterator it;
    for (it = calls_.begin(); it != calls_.end(); ++it) {
        if (it->second.GetState() == state) {
            number++;
        }
    }
    return number;
}

void HfpHfCallManager::NotifyCallChanged(const HandsFreeUnitCalls &call)
{
    RawAddress rawAddress(address_);
    HfpHfService *service = HfpHfService::GetService();
    if (service != nullptr) {
        service->NotifyCallChanged(rawAddress, call);
    } else {
        LOG_ERROR("[HFP HF]%{public}s():service is null.", __FUNCTION__);
    }
    return;
}
}  // namespace bluetooth
}  // namespace OHOS
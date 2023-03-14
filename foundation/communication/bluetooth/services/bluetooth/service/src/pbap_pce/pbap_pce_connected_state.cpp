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

#include "pbap_pce_connected_state.h"
#include <codecvt>
#include <locale>
#include "pbap_pce_service.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
PceConnectedState::PceConnectedState(
    const std::string &name, PbapPceStateMachine &stm, BaseObserverList<IPbapPceObserver> &observerMgrList)
    : PceBaseState(name, stm, observerMgrList)
{}

void PceConnectedState::Entry()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PBAP_PCE_LOG_INFO("%{public}s, observer->OnServiceConnectionStateChanged", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_PBAP_PCE, device);
    observerMgrList_.ForEach([device](IPbapPceObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::CONNECTED));
    });
    stm_.TransitTargetState();
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PceConnectedState::Exit()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PceConnectedState::ReqPullPhoneBook(const utility::Message &msg) const
{
    auto pullMsg = static_cast<PbapPcePullPhoneBookMsg *>(msg.arg2_);
    if (!stm_.IsRepositorySupported(pullMsg->GetName())) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        PBAP_PCE_LOG_ERROR("%{public}s request path=<%{public}s> not supported.",
                           __PRETTY_FUNCTION__,
                           converter.to_bytes(pullMsg->GetName()).c_str());
    }
    IPbapPullPhoneBookParam param;
    param.SetName(pullMsg->GetName());
    param.SetPropertySelector(pullMsg->GetPropertySelector());
    param.SetFormat(pullMsg->GetFormat());
    param.SetMaxListCount(pullMsg->GetMaxListCount());
    param.SetListStartOffset(pullMsg->GetListStartOffset());
    param.SetResetNewMissedCalls(pullMsg->GetResetNewMissedCalls());
    param.SetvCardSelector(pullMsg->GetvCardSelector());
    param.SetvCardSelectorOp(pullMsg->GetvCardSelectorOp());
    param.SetSpecifiedBitset(pullMsg->GetSpecifiedBitset());
    stm_.PullPhoneBook(param);
}

void PceConnectedState::ReqSetPhoneBook(const utility::Message &msg) const
{
    auto reqMsg = static_cast<PbapPceSetPhoneBookMsg *>(msg.arg2_);
    stm_.SetPhoneBook(reqMsg->GetName(), reqMsg->GetFlag());
}

void PceConnectedState::ReqPullvCardListing(const utility::Message &msg) const
{
    auto listingMsg = static_cast<PbapPcePullvCardListingMsg *>(msg.arg2_);
    IPbapPullvCardListingParam param;
    param.SetName(listingMsg->GetName());
    param.SetOrder(listingMsg->GetOrder());
    param.SetSearchValue(listingMsg->GetSearchValue());
    param.SetSearchProperty(listingMsg->GetSearchProperty());
    param.SetMaxListCount(listingMsg->GetMaxListCount());
    param.SetListStartOffset(listingMsg->GetListStartOffset());
    param.SetResetNewMissedCalls(listingMsg->GetResetNewMissedCalls());
    param.SetvCardSelector(listingMsg->GetvCardSelector());
    param.SetvCardSelectorOp(listingMsg->GetvCardSelectorOp());
    param.SetSpecifiedBitset(listingMsg->GetSpecifiedBitset());

    stm_.PullvCardListing(param);
}

void PceConnectedState::ReqPullvCardEntry(const utility::Message &msg) const
{
    auto entryMsg = static_cast<PbapPcePullvCardEntryMsg *>(msg.arg2_);
    IPbapPullvCardEntryParam param;
    param.SetName(entryMsg->GetName());
    param.SetPropertySelector(entryMsg->GetPropertySelector());
    param.SetFormat(entryMsg->GetFormat());
    param.SetSpecifiedBitset(entryMsg->GetSpecifiedBitset());
    stm_.PullvCardEntry(param);
}

bool PceConnectedState::DispatchActionCompleted(const utility::Message &msg) const
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;

    switch (msg.what_) {
        case PCE_PULLPHONEBOOKSIZE_COMPLETED:
            stm_.HandlePhoneBookSizeActionCompleted(msg);
            break;
        case PCE_PULLPHONEBOOK_COMPLETED:
            stm_.HandlePullPhoneBookActionCompleted(msg);
            break;
        case PCE_SETPHONEBOOK_COMPLETED:
            stm_.HandleSetPhoneBookActionCompleted(msg);
            break;
        case PCE_PULLVCARDLISTINGSIZE_COMPLETED:
            stm_.HandlePullvCardListingSizeActionCompleted(msg);
            break;
        case PCE_PULLVCARDLISTING_COMPLETED:
            stm_.HandlePullvCardListingActionCompleted(msg);
            break;
        case PCE_PULLVCARDENTRY_COMPLETED:
            stm_.HandlePullvCardEntryActionCompleted(msg);
            break;
        case PCE_ABORTDOWNLOADING_COMPLETED:
            stm_.HandleAbortDownloadingActionCompleted(msg);
            break;
        default:
            ret = false;
            stm_.TryReleasePbapMsg(msg);
            PBAP_PCE_LOG_ERROR("%{public}s msg.what_=%{public}d can't be handled.", __PRETTY_FUNCTION__, msg.what_);
            break;
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    return ret;
}

bool PceConnectedState::Dispatch(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    bool ret = true;

    switch (msg.what_) {
        case PCE_REQ_DISCONNECTED:
            Transition(PCE_DISCONNECTING_STATE);
            stm_.ReqDisconnect();
            break;
        case PCE_REQ_SET_TARGET_STATE:
            stm_.SetTargetState(msg.arg1_);
            break;
        case PCE_REQ_TRANSIT_TARGET_STATE:
            stm_.TransitTargetState();
            break;
        case PCE_OBEX_DISCONNECTED:
            Transition(PCE_DISCONNECTING_STATE);
            Transition(PCE_DISCONNECTED_STATE);
            break;
        case PCE_REQ_PULLPHONEBOOK:
            ReqPullPhoneBook(msg);
            break;
        case PCE_REQ_SETPHONEBOOK:
            ReqSetPhoneBook(msg);
            break;
        case PCE_REQ_PULLVCARDLISTING:
            ReqPullvCardListing(msg);
            break;
        case PCE_REQ_PULLVCARDENTRY:
            ReqPullvCardEntry(msg);
            break;
        case PCE_REQ_ABORTDOWNLOADING:
            stm_.AbortDownloading();
            break;
        default:
            ret = DispatchActionCompleted(msg);
            break;
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS

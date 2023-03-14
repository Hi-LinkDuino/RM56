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

#include "pbap_pce_state_machine.h"
#include <codecvt>
#include <fstream>
#include <locale>
#include <sstream>
#include <vector>
#include "bt_def.h"
#include "btm.h"
#include "obex/obex_utils.h"
#include "pbap_pce_app_params.h"
#include "pbap_pce_connected_state.h"
#include "pbap_pce_connecting_state.h"
#include "pbap_pce_def.h"
#include "pbap_pce_disconnected_state.h"
#include "pbap_pce_disconnecting_state.h"
#include "pbap_pce_service.h"
#include "power_manager.h"

namespace OHOS {
namespace bluetooth {
PbapPceStateMachine::PbapPceStateMachine(const RawAddress &dev, PbapPceService &pceService, PbapPceSdp &pbapPceSdp,
    BaseObserverList<IPbapPceObserver> &observerMgrList)
    : device_(dev),
      pceService_(&pceService),
      pbapPceSdp_(pbapPceSdp),
      observerMgrList_(observerMgrList),
      targetState_(PCE_TARGET_STATE_UNKNOWN)
{
    LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::unique_ptr<State> disconnected =
        std::make_unique<PceDisconnectedState>(PCE_DISCONNECTED_STATE, *this, observerMgrList);
    std::unique_ptr<State> connecting =
        std::make_unique<PceConnectingState>(PCE_CONNECTING_STATE, *this, observerMgrList);
    std::unique_ptr<State> disconnecting =
        std::make_unique<PceDisconnectingState>(PCE_DISCONNECTING_STATE, *this, observerMgrList);
    std::unique_ptr<State> connected = std::make_unique<PceConnectedState>(PCE_CONNECTED_STATE, *this, observerMgrList);
    Move(disconnected);
    Move(connecting);
    Move(disconnecting);
    Move(connected);

    InitState(PCE_CONNECTING_STATE);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool PbapPceStateMachine::IsRepositorySupported(const std::u16string &name) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = false;
    std::vector<std::u16string> localPhoneBook = {u"pb", u"ich", u"och", u"mch", u"cch", u"spd", u"fav"};
    bool isLocalPhoneBook = false;
    for (auto subpath : localPhoneBook) {
        if (std::u16string::npos != name.find(subpath)) {
            isLocalPhoneBook = true;
            break;
        }
    }

    bool isSim = name.find(u"SIM") != std::u16string::npos;
    bool isFav = name.find(u"fav") != std::u16string::npos;
    if (isLocalPhoneBook && (GetSupportedRes() & PBAP_PCE_SUPPORTED_REPOS_LOCALPB)) {
        ret = true;
    } else if (isSim && (GetSupportedRes() & PBAP_PCE_SUPPORTED_REPOS_SIMCARD)) {
        ret = true;
    } else if (isFav && (GetSupportedRes() & PBAP_PCE_SUPPORTED_REPOS_FAV)) {
        ret = true;
    } else {
        ret = false;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void PbapPceStateMachine::PullPhoneBook(const IPbapPullPhoneBookParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (pceObexClient_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_ is null", __PRETTY_FUNCTION__);
        return;
    }
    auto pbReqHeader = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);
    pbReqHeader->AppendItemConnectionId(connectId_);
    pbReqHeader->AppendItemType("x-bt/phonebook");
    pbReqHeader->AppendItemName(param.GetName());
    SetReqVcardFileName(param.GetName());
    PbapPceAppParams appParams;
    if (param.IsSpecified(IPbapPullPhoneBookParam::PROPERTY_SELECTOR)) {
        appParams.SetPropertySelector(param.GetPropertySelector());
    }
    if (param.IsSpecified(IPbapPullPhoneBookParam::FORMAT)) {
        appParams.SetFormat(param.GetFormat());
    }
    bool requestSize = false;
    if (param.IsSpecified(IPbapPullPhoneBookParam::MAX_LIST_COUNT)) {
        appParams.SetMaxListCount(param.GetMaxListCount());
        requestSize = (param.GetMaxListCount() == 0);
    }
    if (param.IsSpecified(IPbapPullPhoneBookParam::LIST_START_OFFSET)) {
        appParams.SetListStartOffset(param.GetListStartOffset());
    }
    if (param.IsSpecified(IPbapPullPhoneBookParam::RESET_NEW_MISSED_CALLS)) {
        appParams.SetResetNewMissedCalls(param.GetResetNewMissedCalls());
    }
    if (param.IsSpecified(IPbapPullPhoneBookParam::VCARD_SELECTOR)) {
        appParams.SetVcardSelector(param.GetvCardSelector());
    }
    if (param.IsSpecified(IPbapPullPhoneBookParam::VCARD_SELECTOROP)) {
        appParams.SetVcardSelectorOperator(param.GetvCardSelectorOp());
    }
    PBAP_PCE_LOG_DEBUG("%{public}s, %{public}s", __PRETTY_FUNCTION__, param.ToDebugString().c_str());
    appParams.AddToObexHeader(*pbReqHeader);
    SetPowerStatusBusy(true);
    pceObexClient_->Get(*pbReqHeader, requestSize ? PCE_REQ_PULLPHONEBOOKSIZE : PCE_REQ_PULLPHONEBOOK);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::SetPhoneBook(const std::u16string &name, int flag)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    std::string nameStr = converter.to_bytes(name);
    PBAP_PCE_LOG_DEBUG("%{public}s name=%{public}s, flag=%{public}d", __PRETTY_FUNCTION__, nameStr.c_str(), flag);
    if (pceObexClient_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_ is null", __PRETTY_FUNCTION__);
        return;
    }
    SetPowerStatusBusy(true);
    pceObexClient_->SetPath(flag, name);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::PullvCardListing(const IPbapPullvCardListingParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (pceObexClient_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s pceObexClient_ is null", __PRETTY_FUNCTION__);
        return;
    }
    auto pbReqHeader = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);
    pbReqHeader->AppendItemConnectionId(connectId_);
    pbReqHeader->AppendItemType("x-bt/vcard-listing");
    pbReqHeader->AppendItemName(param.GetName());

    PbapPceAppParams appParams;
    if (param.IsSpecified(IPbapPullvCardListingParam::ORDER)) {
        appParams.SetOrder(param.GetOrder());
    }
    if (param.IsSpecified(IPbapPullvCardListingParam::SEARCH_VALUE)) {
        std::vector<uint8_t> searchValueUtf8;
        std::string value = param.GetSearchValue();
        searchValueUtf8.assign(value.begin(), value.end());
        appParams.SetSearchValueUtf8(searchValueUtf8);
    }
    if (param.IsSpecified(IPbapPullvCardListingParam::SEARCH_PROPERTY)) {
        appParams.SetSearchProperty(param.GetSearchProperty());
    }
    bool requestSize = false;
    if (param.IsSpecified(IPbapPullvCardListingParam::MAX_LIST_COUNT)) {
        appParams.SetMaxListCount(param.GetMaxListCount());
        requestSize = (param.GetMaxListCount() == 0);
    }
    if (param.IsSpecified(IPbapPullvCardListingParam::LIST_START_OFFSET)) {
        appParams.SetListStartOffset(param.GetListStartOffset());
    }
    if (param.IsSpecified(IPbapPullvCardListingParam::RESET_NEW_MISSED_CALLS)) {
        appParams.SetResetNewMissedCalls(param.GetResetNewMissedCalls());
    }
    if (param.IsSpecified(IPbapPullvCardListingParam::VCARD_SELECTOR)) {
        appParams.SetVcardSelector(param.GetvCardSelector());
    }
    if (param.IsSpecified(IPbapPullvCardListingParam::VCARD_SELECTOROP)) {
        appParams.SetVcardSelectorOperator(param.GetvCardSelectorOp());
    }
    PBAP_PCE_LOG_DEBUG("%{public}s, %{public}s", __PRETTY_FUNCTION__, param.ToDebugString().c_str());
    appParams.AddToObexHeader(*pbReqHeader);
    SetPowerStatusBusy(true);
    pceObexClient_->Get(*pbReqHeader, requestSize ? PCE_REQ_PULLVCARDLISTINGSIZE : PCE_REQ_PULLVCARDLISTING);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::PullvCardEntry(const IPbapPullvCardEntryParam &param)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (pceObexClient_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_ is null", __PRETTY_FUNCTION__);
        return;
    }
    auto pbReqHeader = ObexHeader::CreateRequest(ObexOpeId::GET_FINAL);
    pbReqHeader->AppendItemConnectionId(connectId_);
    pbReqHeader->AppendItemType("x-bt/vcard");
    pbReqHeader->AppendItemName(param.GetName());
    SetReqVcardFileName(param.GetName());

    PbapPceAppParams appParams;
    if (param.IsSpecified(IPbapPullvCardEntryParam::PROPERTY_SELECTOR)) {
        appParams.SetPropertySelector(param.GetPropertySelector());
    }
    if (param.IsSpecified(IPbapPullvCardEntryParam::FORMAT)) {
        appParams.SetFormat(param.GetFormat());
    }
    PBAP_PCE_LOG_DEBUG("%{public}s, %{public}s", __PRETTY_FUNCTION__, param.ToDebugString().c_str());
    appParams.AddToObexHeader(*pbReqHeader);
    SetPowerStatusBusy(true);
    pceObexClient_->Get(*pbReqHeader, PCE_REQ_PULLVCARDENTRY);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::RemoveBTMLogging() const
{
    BtAddr btAddr;
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    GetDevice().ConvertToUint8(btAddr.addr, sizeof(btAddr.addr));
    if (IsGoepL2capPSM()) {
        BTM_RemoveRemoteL2capPsmForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP,
                                           GetGapSecChannel().l2capPsm,
                                           &btAddr);
    } else {
        BTM_RemoveRemoteRfcommScnChannelForLogging(BTM_HCI_LOG_FILTER_MODULE_PBAP,
                                                   GetGapSecChannel().rfcommChannel,
                                                   &btAddr);
    }
}

int PbapPceStateMachine::GetTargetState() const
{
    return targetState_;
}

void PbapPceStateMachine::SetTargetState(int targetState)
{
    targetState_ = targetState;
}

void PbapPceStateMachine::TransitTargetState()
{
    PBAP_PCE_LOG_INFO("%{public}s start target=%{public}d", __PRETTY_FUNCTION__, GetTargetState());
    if (GetTargetState() == PCE_TARGET_STATE_CONNECTED) {
        GetPceService().Connect(GetDevice());
        SetTargetState(PCE_TARGET_STATE_UNKNOWN);
    } else if (GetTargetState() == PCE_TARGET_STATE_DISCONNECTED) {
        GetPceService().Disconnect(GetDevice());
        SetTargetState(PCE_TARGET_STATE_UNKNOWN);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool PbapPceStateMachine::IsBusy() const
{
    PBAP_PCE_LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = false;
    if (pceObexClient_ != nullptr) {
        ret = pceObexClient_->IsBusy();
    } else {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_ OR pceObexClient_->observer_ is null", __PRETTY_FUNCTION__);
    }
    PBAP_PCE_LOG_DEBUG("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

bool PbapPceStateMachine::SetBusy(bool isBusy)
{
    PBAP_PCE_LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = false;
    if (pceObexClient_ != nullptr) {
        pceObexClient_->SetBusy(isBusy);
        ret = true;
    } else {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_ OR pceObexClient_->observer_ is null", __PRETTY_FUNCTION__);
    }
    PBAP_PCE_LOG_DEBUG("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void PbapPceStateMachine::SetPowerStatusBusy(bool isBusy)
{
    PBAP_PCE_LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    if (powerStatusBusy_ == isBusy) {
        return;
    }
    powerStatusBusy_ = isBusy;
    if (isBusy) {
        HILOGI("[%{public}s] PowerStatusUpdate -> BUSY", GetEncryptAddr(device_.GetAddress()).c_str());
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_PBAP_PCE, device_);
    } else {
        HILOGI("[%{public}s] PowerStatusUpdate -> IDLE", GetEncryptAddr(device_.GetAddress()).c_str());
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_PBAP_PCE, device_);
    }
    PBAP_PCE_LOG_DEBUG("%{public}s end", __PRETTY_FUNCTION__);
    return;
}

int PbapPceStateMachine::AbortDownloading()
{
    PBAP_PCE_LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    if (pceObexClient_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_ is null", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    PBAP_PCE_LOG_DEBUG("%{public}s end", __PRETTY_FUNCTION__);
    SetPowerStatusBusy(true);
    return pceObexClient_->Abort();
}

void PbapPceStateMachine::SetConnectId(uint32_t connectId)
{
    connectId_ = connectId;
}

int PbapPceStateMachine::ReqDisconnect(bool withObexReq) const
{
    PBAP_PCE_LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);
    if ((pceObexClient_ == nullptr)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pceObexClient_  is null", __PRETTY_FUNCTION__);
        return RET_BAD_STATUS;
    }
    pceObexClient_->Disconnect(withObexReq);
    PBAP_PCE_LOG_DEBUG("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

bool PbapPceStateMachine::ObexHeaderToDataResult(const ObexHeader &resp, int actionType, IPbapPhoneBookData &retResult)
{
    PBAP_PCE_LOG_DEBUG("%{public}s start", __PRETTY_FUNCTION__);

    bool ret = false;
    if (resp.GetItemAppParams() != nullptr) {
        PbapPceAppParams appParams;
        if (appParams.Init(*(resp.GetItemAppParams()->GetTlvParamters()))) {
            if (appParams.GetPhonebookSize() != nullptr) {
                retResult.phoneBookSize_ = *(appParams.GetPhonebookSize());
            }
            retResult.primaryFolderVersion_ = appParams.GetPrimaryFolderVersion();
            retResult.secondaryFolderVersion_ = appParams.GetSecondaryFolderVersion();
            retResult.databaseIdentifier_ = appParams.GetDatabaseIdentifier();
        }
    } else {
        PBAP_PCE_LOG_INFO("%{public}s resp.GetItemAppParams() is null", __PRETTY_FUNCTION__);
    }
    auto &extendBody = resp.GetExtendBodyObject();
    if (extendBody) {
        if (actionType != PBAP_ACTION_PULLPHONEBOOK) {
            const int bufSize = 100;
            uint8_t buf[bufSize] = {0};
            size_t cnt = 0;
            while ((cnt = extendBody->Read(buf, bufSize)) > 0) {
                retResult.result_.insert(retResult.result_.end(), buf, buf + cnt);
            }
        }
        extendBody->Close();
    }
    ret = true;
    PBAP_PCE_LOG_DEBUG("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

bool PbapPceStateMachine::SplitFilePath(
    const std::u16string &filePath, std::u16string &retPath, std::u16string &retFileName)
{
    bool ret = false;
    size_t pos = filePath.find_last_of(u'/');
    if (pos != std::u16string::npos) {
        retPath = filePath.substr(0, pos);
        retFileName = filePath.substr(pos + 1);
        ret = true;
    }
    return ret;
}

std::string PbapPceStateMachine::Replace(const std::string &str, const std::string &oldStr, const std::string &newStr)
{
    std::string strRet = str;
    for (std::string::size_type pos = 0; pos != std::string::npos; pos += newStr.length()) {
        pos = strRet.find(oldStr, pos);
        if (pos != std::string::npos) {
            strRet.replace(pos, oldStr.length(), newStr);
        } else {
            break;
        }
    }
    return strRet;
}

std::string PbapPceStateMachine::U16ToStr(const std::u16string &str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
    return converter.to_bytes(str);
}

bool PbapPceStateMachine::IsResponeOK(uint8_t respCode) const
{
    if ((respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) ||
        (respCode == static_cast<uint8_t>(ObexRspCode::CONTINUE))) {
        return true;
    }
    return false;
}

void PbapPceStateMachine::HandlePhoneBookSizeActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }
    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    std::string respTxt;
    IPbapPhoneBookData result;
    uint8_t respCode = resp->GetFieldCode();
    if (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        ObexHeaderToDataResult(*resp, PBAP_ACTION_PULLPHONEBOOKSIZE, result);
        respTxt = "PhoneBookSize Success!";
    }
    if (IsResponeOK(respCode)) {
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "PhoneBookSize Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }
    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_PULLPHONEBOOKSIZE, result);
    });
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::HandlePullvCardListingSizeActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }
    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    std::string respTxt;
    IPbapPhoneBookData result;
    uint8_t respCode = resp->GetFieldCode();
    if (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        ObexHeaderToDataResult(*resp, PBAP_ACTION_PULLVCARDLISTINGSIZE, result);
        respTxt = "PullvCardListingSize Success!";
    }
    if (IsResponeOK(respCode)) {
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "PullvCardListingSize Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }
    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_PULLVCARDLISTINGSIZE, result);
    });
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::HandlePullPhoneBookActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }
    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    uint8_t respCode = resp->GetFieldCode();
    std::string respTxt;
    std::string respBodyObject;
    IPbapPhoneBookData result;
    InitIPbapPhoneBookData(result);
    if (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        if (ObexHeaderToDataResult(*resp, PBAP_ACTION_PULLPHONEBOOK, result)) {
#ifdef PBAP_PCE_RECEIVED_BY_OBEX_ARRAY
            respBodyObject.assign(result.result_.begin(), result.result_.end());
            respTxt = "PullPhoneBook Completed:\n" + respBodyObject;
#else
            result.resultLoaded_ = 0;
            result.result_.clear();
            respTxt = "PullPhoneBook Completed:\n";
#endif  // PBAP_PCE_RECEIVED_BY_OBEX_ARRAY
        }
    }

    if (IsResponeOK(respCode)) {
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "PullPhoneBook Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }

    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_PULLPHONEBOOK, result);
    });
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::HandleSetPhoneBookActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }
    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    std::string respTxt;
    uint8_t respCode = resp->GetFieldCode();
    std::u16string name = pceObexMsg->GetName();
    if (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        // PBAP v1.2.3
        // go up - bit0=1; go down - bit0=0; go to root - bit0=0 && Name is empty.
        const uint8_t flagMask = 0x01;
        if (name.find(u'/') != std::u16string::npos) {
            SetCurrentPath(name);
        } else if (pceObexMsg->GetFlags() & flagMask) {
            // go up
            std::u16string currentPath = GetCurrentPath();
            size_t pos = currentPath.find_last_of(u'/');
            if (pos != std::u16string::npos) {
                currentPath = currentPath.substr(0, pos);
                SetCurrentPath(currentPath);
            } else {
                SetCurrentPath(u"");  // root
            }
        } else {
            // go to root
            if (name.empty()) {
                SetCurrentPath(name);
            }
        }
        respTxt = "SetPhoneBook completed!";
    }
    if (IsResponeOK(respCode)) {
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "SetPhoneBook Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }
    IPbapPhoneBookData result;
    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_SETPHONEBOOK, result);
    });
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::HandlePullvCardListingActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }

    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    std::string respTxt;
    std::string respBodyObject;
    IPbapPhoneBookData result;
    uint8_t respCode = resp->GetFieldCode();
    if (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        if (ObexHeaderToDataResult(*resp, PBAP_ACTION_PULLVCARDLISTING, result)) {
            respBodyObject.assign(result.result_.begin(), result.result_.end());
            respTxt = "PullvCardListing Completed:\n" + respBodyObject;
        }
    }
    if (IsResponeOK(respCode)) {
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "PullvCardListing Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }
    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_PULLVCARDLISTING, result);
    });
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::HandlePullvCardEntryActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }

    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    IPbapPhoneBookData result;
    uint8_t respCode = resp->GetFieldCode();
    std::string respTxt;
    std::string respBodyObject;
    if (respCode == static_cast<uint8_t>(ObexRspCode::SUCCESS)) {
        if (ObexHeaderToDataResult(*resp, PBAP_ACTION_PULLVCARDENTRY, result)) {
            respBodyObject.assign(result.result_.begin(), result.result_.end());
        }
        respTxt = "PullvCardEntry Completed:\n" + respBodyObject;
    }
    if (IsResponeOK(respCode)) {
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "PullvCardEntry Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }
    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_PULLVCARDENTRY, result);
    });

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::InitIPbapPhoneBookData(IPbapPhoneBookData &retResult)
{
    std::u16string vcardPath, vcardFileName;
    SplitFilePath(GetReqVcardFileName(), vcardPath, vcardFileName);
    retResult.deviceAddr_ = Replace(GetDevice().GetAddress(), ":", "_");
    retResult.vcardPath_ = Replace(U16ToStr(vcardPath), "/", "_");
    retResult.vcardFileName_ = Replace(U16ToStr(vcardFileName), "/", "_");
    retResult.resultLoaded_ = 0;
}

void PbapPceStateMachine::HandleAbortDownloadingActionCompleted(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PbapPceActionObexMessage *pceObexMsg = static_cast<PbapPceActionObexMessage *>(msg.arg2_);
    if (pceObexMsg == nullptr) {
        PBAP_PCE_LOG_INFO("%{public}s end, pceObexMsg == nullptr!", __PRETTY_FUNCTION__);
        return;
    }

    const ObexHeader *resp = pceObexMsg->GetObexHeader();
    uint8_t respCode = resp->GetFieldCode();
    std::string respTxt;
    IPbapPhoneBookData result;
    InitIPbapPhoneBookData(result);
    if (IsResponeOK(respCode)) {
        respTxt = "AbortDownloading Completed!";
        PBAP_PCE_LOG_INFO("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    } else {
        respTxt = "AbortDownloading Error!";
        PBAP_PCE_LOG_ERROR("%{public}s respCode:0x%x, respTxt:%{public}s", __PRETTY_FUNCTION__, respCode, respTxt.c_str());
    }

    auto &device = GetDevice();
    observerMgrList_.ForEach([device, respCode, result](IPbapPceObserver &observer) {
        observer.OnActionCompleted(device, static_cast<int>(respCode), PBAP_ACTION_ABORT, result);
    });

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::TryReleasePbapMsg(const utility::Message &msg) const
{
    if (msg.what_ == PCE_SDP_FINISH) {
        std::unique_ptr<PbapPceHeaderSdpMsg> sdpMsg(static_cast<PbapPceHeaderSdpMsg *>(msg.arg2_));
        PBAP_PCE_LOG_ERROR("%{public}s msg.what_=[%{public}d] release memory", __PRETTY_FUNCTION__, msg.what_);
    }
    if (msg.what_ == PCE_PASSWORD_INPUT) {
        std::unique_ptr<PbapPcePasswordInputMsg> pwdInputMsg(static_cast<PbapPcePasswordInputMsg *>(msg.arg2_));
        PBAP_PCE_LOG_ERROR("%{public}s msg.what_=[%{public}d] release memory", __PRETTY_FUNCTION__, msg.what_);
    }
}

const RawAddress &PbapPceStateMachine::GetDevice() const
{
    return this->device_;
}

PbapPceService &PbapPceStateMachine::GetPceService() const
{
    return *pceService_;
}

PbapPceObexClient &PbapPceStateMachine::GetObexClient() const
{
    return *pceObexClient_;
}

void PbapPceStateMachine::SetVersionNumber(uint16_t versionNumber)
{
    versionNumber_ = versionNumber;
}

uint16_t PbapPceStateMachine::GetVersionNumber() const
{
    return versionNumber_;
}

void PbapPceStateMachine::SetSupportedRes(uint8_t supportedRes)
{
    supportedRes_ = supportedRes;
}

uint8_t PbapPceStateMachine::GetSupportedRes() const
{
    return supportedRes_;
}

void PbapPceStateMachine::SetSupportedFeature(uint32_t supportedFeature)
{
    supportedFeature_ = supportedFeature;
}

uint32_t PbapPceStateMachine::GetSupportedFeature() const
{
    return supportedFeature_;
}

void PbapPceStateMachine::SetPassword(const std::string &password)
{
    this->password_ = password;
}

const std::string &PbapPceStateMachine::GetPassword() const
{
    return password_;
}

bool PbapPceStateMachine::IsGoepL2capPSM() const
{
    return isGoepL2capPSM_;
}

void PbapPceStateMachine::SetGoepL2capPSM(bool goepL2capPSM)
{
    isGoepL2capPSM_ = goepL2capPSM;
}

const GapSecChannel PbapPceStateMachine::GetGapSecChannel() const
{
    return gapSecChannel_;
}

void PbapPceStateMachine::SetGapSecChannel(const GapSecChannel &gapSecChannel)
{
    gapSecChannel_ = gapSecChannel;
}

bool PbapPceStateMachine::GetFeatureFlag() const
{
    return featureFlag_;
}

void PbapPceStateMachine::SetFeatureFlag(bool featureFlag)
{
    featureFlag_ = featureFlag;
}

PbapPceSdp &PbapPceStateMachine::GetSdp() const
{
    return pbapPceSdp_;
}

PbapPceGap *PbapPceStateMachine::GetGap() const
{
    return pbapPceGap_.get();
}

void PbapPceStateMachine::SetGap(PbapPceGap &gap)
{
    pbapPceGap_.reset(&gap);
}

void PbapPceStateMachine::SetUserId(const std::string &userId)
{
    this->userId_ = userId;
}

const std::string &PbapPceStateMachine::GetUserId() const
{
    return userId_;
}

const std::u16string &PbapPceStateMachine::GetCurrentPath() const
{
    return currentPath_;
}

void PbapPceStateMachine::SetCurrentPath(const std::u16string &path)
{
    currentPath_ = path;
}

std::string PbapPceStateMachine::GetDownloadFileName() const
{
    std::u16string vcardPath, vcardFileName;
    SplitFilePath(GetReqVcardFileName(), vcardPath, vcardFileName);
    std::string device = Replace(GetDevice().GetAddress(), ":", "_");
    std::string strPath = Replace(U16ToStr(vcardPath), "/", "_");
    std::string strVcardFileName = Replace(U16ToStr(vcardFileName), "/", "_");
    std::string strRet = device + strPath + strVcardFileName;
    return strRet;
}

const std::u16string &PbapPceStateMachine::GetReqVcardFileName() const
{
    return reqVcardFileName_;
}

void PbapPceStateMachine::SetReqVcardFileName(const std::u16string &reqVcardFileName)
{
    reqVcardFileName_ = reqVcardFileName;
}

PbapPceStateMachine::~PbapPceStateMachine()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceStateMachine::CreatePceObexClient(const ObexClientConfig &obexConfig)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (pceService_ == nullptr) {
        PBAP_PCE_LOG_ERROR("pceService_ == nullptr");
        return;
    }
    this->pceObexClient_ = std::make_unique<PbapPceObexClient>(obexConfig, *pceService_);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}
}  // namespace bluetooth
}  // namespace OHOS

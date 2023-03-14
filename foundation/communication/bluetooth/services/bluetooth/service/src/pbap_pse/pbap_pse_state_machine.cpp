/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "pbap_pse_state_machine.h"
#include <cstring>
#include <numeric>
#include "../obex/obex_utils.h"
#include "interface_profile_pbap_pse.h"
#include "log.h"
#include "log_util.h"
#include "pbap_pse_app_params.h"
#include "pbap_pse_def.h"
#include "pbap_pse_obex_server.h"
#include "pbap_pse_service.h"
#include "pbap_pse_vcard_manager.h"
#include "power_manager.h"
#include "raw_address.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
PseWaitingForConnect::PseWaitingForConnect(const std::string &name, PbapPseStateMachine &stm)
    : State(name, stm), stm_(stm)
{}

void PseWaitingForConnect::Entry()
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
}

void PseWaitingForConnect::Exit()
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
}

bool PseWaitingForConnect::Dispatch(const utility::Message &msg)
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = true;
    switch (msg.what_) {
        case PSE_DEVICE_CONNECT_INCOMING:
            Transition(PbapPseStateMachine::PSE_CONNECTING_STATE);
            stm_.ProcessMessage(msg);
            break;
        default:
            PBAP_PSE_LOG_ERROR("NOT SUPPORT MSGID %{public}d", msg.what_);
            ret = false;
            break;
    }
    PBAP_PSE_LOG_INFO("%{public}s end, ret = %{public}d", __PRETTY_FUNCTION__, ret);
    return ret;
}

PseConnecting::PseConnecting(const std::string &name, PbapPseStateMachine &stm,
    BaseObserverList<IPbapPseObserver> &observerMgrList, std::function<uint32_t()> &getNextConnectIdFun)
    : State(name, stm), stm_(stm), observerMgrList_(observerMgrList), getNextConnectIdFun_(getNextConnectIdFun)
{}

void PseConnecting::Entry()
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    observerMgrList_.ForEach([device](IPbapPseObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::CONNECTING));
    });
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PseConnecting::Exit()
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
    stm_.StopTimer();
}

bool PseConnecting::Dispatch(const utility::Message &msg)
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = true;
    switch (msg.what_) {
        case PSE_DEVICE_CONNECT_INCOMING:
            // No need to delete
            incomeConnect_ = static_cast<ObexIncomingConnect *>(msg.arg2_);
            break;
        case PSE_INCOMING_ACCEPT:
            AcceptConnection();
            break;
        case PSE_INCOMING_REJECT:
            RejectConnection();
            break;
        case PSE_WAITING_CONNECTED_TO_DISCONNECT:
            waitingDisconnect_ = true;
            break;
        case PSE_REQ_OBEX_CONNECT:
            // No need to delete
            CheckConnectReq(*static_cast<PbapPseObexMessage *>(msg.arg2_));
            break;
        case PSE_PASSWORD_INPUT:
            // No need to delete
            PasswordInput(*static_cast<PbapPsePasswordInputMsg *>(msg.arg2_));
            break;
        case PSE_DEVICE_DISCONNECTED:  // OnIncomingDisconnected
            Transition(PbapPseStateMachine::PSE_DISCONNECTING_STATE);
            Transition(PbapPseStateMachine::PSE_DISCONNECTED_STATE);
            break;
        default:
            PBAP_PSE_LOG_ERROR("NOT SUPPORT MSGID %{public}d", msg.what_);
            ret = false;
            break;
    }
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void PseConnecting::AcceptConnection()
{
    if (incomeConnect_ != nullptr) {
        PBAP_PSE_LOG_INFO("%{public}s AcceptConnection", __PRETTY_FUNCTION__);
        stm_.StopTimer();
        incomeConnect_->AcceptConnection();
        stm_.GetStmInfo().accepted_ = true;
        incomeConnect_ = nullptr;
    } else {
        HILOGI("%{public}s incomeConnect is null, maybe accepted or rejected.",
            GetEncryptAddr(stm_.GetDevice().GetAddress()).c_str());
    }
}

void PseConnecting::RejectConnection()
{
    if (incomeConnect_ != nullptr) {
        HILOGI("%{public}s RejectConnection", GetEncryptAddr(stm_.GetDevice().GetAddress()).c_str());
        stm_.StopTimer();
        Transition(PbapPseStateMachine::PSE_DISCONNECTING_STATE);
        incomeConnect_->RejectConnection();
        incomeConnect_ = nullptr;
        Transition(PbapPseStateMachine::PSE_DISCONNECTED_STATE);
    } else {
        HILOGI("%{public}s incomeConnect is null, maybe accepted or rejected.",
            GetEncryptAddr(stm_.GetDevice().GetAddress()).c_str());
    }
}

bool PseConnecting::CheckConnectHeader(const ObexHeader &req) const
{
    auto target = req.GetItemTarget();
    if (target == nullptr) {
        PBAP_PSE_LOG_ERROR("Target doesn't exist");
        return false;
    }
    if (target->GetHeaderDataSize() != PBAP_SERVICE_UUID_LEN) {
        PBAP_PSE_LOG_ERROR("Wrong Target size %u", target->GetHeaderDataSize());
        return false;
    }
    if (memcmp(target->GetBytes().get(), PBAP_SERVICE_UUID, PBAP_SERVICE_UUID_LEN) != 0) {
        PBAP_PSE_LOG_ERROR(
            "Wrong TargetId:%{public}s", ObexUtils::ToDebugString(target->GetBytes().get(), PBAP_SERVICE_UUID_LEN).c_str());
        return false;
    }
    return true;
}

void PseConnecting::ToDisconnect(ObexRspCode rspCode)
{
    auto obexSession = stm_.GetStmInfo().obexSession_;
    if (obexSession != nullptr) {
        Transition(PbapPseStateMachine::PSE_DISCONNECTING_STATE);
        auto header = ObexHeader::CreateResponse(rspCode, true);
        obexSession->SendResponse(*header);
        obexSession->Disconnect();
    } else {
        Transition(PbapPseStateMachine::PSE_DISCONNECTED_STATE);
    }
}

void PseConnecting::SendSuccessResponse(const std::string &userId, const std::string &pwd)
{
    auto header = ObexHeader::CreateResponse(ObexRspCode::SUCCESS, true);
    auto &stmInfo = stm_.GetStmInfo();
    stmInfo.connectId_ = getNextConnectIdFun_();

    // set connection id
    header->AppendItemConnectionId(stmInfo.connectId_);
    // set who
    header->AppendItemWho(PBAP_SERVICE_UUID, PBAP_SERVICE_UUID_LEN);

    if (authChallenge_) {
        // Authenticate Response
        // Carries the digest-response string. This is the response to the challenge from the Client.
        ObexDigestResponse response;
        std::vector<uint8_t> requestDigest = ObexUtils::MakeRequestDigest(authNonce_.data(), authNonce_.size(), pwd);
        response.AppendRequestDigest(requestDigest.data(), requestDigest.size());
        if (authNeedUser_ && userId.size() > 0) {
            response.AppendUserId(reinterpret_cast<const uint8_t *>(userId.c_str()), userId.size());
        }
        response.AppendNonce(authNonce_.data(), authNonce_.size());
        header->AppendItemAuthResponse(response);
    }
    // send response to client
    stmInfo.obexSession_->SendResponse(*header);
    Transition(PbapPseStateMachine::PSE_CONNECTED_STATE);
}

void PseConnecting::CheckConnectReq(const PbapPseObexMessage &obexMsg)
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    const ObexHeader &req = obexMsg.GetObexHeader();
    stm_.GetStmInfo().obexSession_ = &obexMsg.GetObexSession();
    if (waitingDisconnect_) {
        ToDisconnect(ObexRspCode::SERVICE_UNAVAILABLE);
        return;
    }
    if (!CheckConnectHeader(req)) {
        ToDisconnect();
        return;
    }

    auto appParamsTlv = req.GetItemAppParams();
    if (appParamsTlv != nullptr && appParamsTlv->GetTlvParamters()) {
        PbapPseAppParams pbapAppParams;
        pbapAppParams.Init(*appParamsTlv->GetTlvParamters());
        if (pbapAppParams.GetPbapSupportedFeatures()) {
            stm_.GetStmInfo().pceFeatures_ = *pbapAppParams.GetPbapSupportedFeatures();
            PBAP_PSE_LOG_INFO("Set PbapPceSupportedFeatures to %x", *pbapAppParams.GetPbapSupportedFeatures());
        }
    }

    auto authChallenges = req.GetItemAuthChallenges();
    if (authChallenges == nullptr) {
        SendSuccessResponse();
        return;
    }

    // support for legacy PBAP devices
    CheckAuthChallenges(req, *authChallenges);
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PseConnecting::CheckAuthChallenges(const ObexHeader &req, const ObexOptionalTlvHeader &authChallenges)
{
    // support for legacy PBAP devices
    // PBAP_v1.2.3-6.3 Implementations conforming to this specification shall not send the first OBEX
    // authentication request. It is solely supported for backwards compatibility with legacy PBAP devices.
    // create auth Response use challenge
    // note: ignore AuthResponse from client. because our server don't auth the client.
    auto digestChallenge = static_cast<ObexDigestChallenge *>(authChallenges.GetTlvParamters().get());
    auto nonceTlv = digestChallenge->GetNonce();
    if (!nonceTlv) {  // nonce is require, but no find
        ToDisconnect();
        return;
    }
    authChallenge_ = true;
    authNonce_.clear();
    authDescription_.clear();
    authFullAccess_ = true;
    authNeedUser_ = false;
    auto targetHr = req.GetItemTarget();
    auto targetBytes = targetHr->GetBytes();
    authNonce_.insert(authNonce_.end(), nonceTlv->GetVal(), nonceTlv->GetVal() + nonceTlv->GetLen());
    auto optionsTlv = digestChallenge->GetOptions();
    if (optionsTlv) {
        uint8_t option = optionsTlv->GetVal()[0];
        // When set, the User Id must be sent in the authenticate response.
        if ((option & 0x01) != 0) {
            authNeedUser_ = true;
        }
        // Access mode: Read Only when set, otherwise Full access is permitted.
        if ((option & 0x02) != 0) {
            authFullAccess_ = false;
        }
    }
    auto realmTlv = digestChallenge->GetRealm();
    if (realmTlv) {
        if (realmTlv->GetLen() > 0) {
            authUserCharset_ = realmTlv->GetVal()[0];
            if (realmTlv->GetLen() > 1) {
                authDescription_.insert(
                    authDescription_.end(), realmTlv->GetVal() + 1, realmTlv->GetVal() + realmTlv->GetLen());
            }
        }
    }
    auto &device = stm_.GetDevice();
    auto &authDescription = authDescription_;
    auto &authUserCharset = authUserCharset_;
    auto &authFullAccess = authFullAccess_;
    observerMgrList_.ForEach([device, authDescription, authUserCharset, authFullAccess](IPbapPseObserver &observer) {
        observer.OnServicePasswordRequired(device, authDescription, authUserCharset, authFullAccess);
    });
}

void PseConnecting::PasswordInput(const PbapPsePasswordInputMsg &pwdInputMsg)
{
    if (waitingDisconnect_) {
        ToDisconnect(ObexRspCode::SERVICE_UNAVAILABLE);
        return;
    }
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (!authChallenge_) {
        return;
    }
    if (pwdInputMsg.GetPassword().size() == 0) {
        return;
    }

    SendSuccessResponse(pwdInputMsg.GetUserId(), pwdInputMsg.GetPassword());
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

PseConnected::PseConnected(
    const std::string &name, PbapPseStateMachine &stm, BaseObserverList<IPbapPseObserver> &observerMgrList)
    : State(name, stm), stm_(stm), observerMgrList_(observerMgrList)
{}

void PseConnected::Entry()
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON, PROFILE_NAME_PBAP_PSE, device);
    observerMgrList_.ForEach([device](IPbapPseObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::CONNECTED));
    });
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PseConnected::Exit()
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
}

bool PseConnected::Dispatch(const utility::Message &msg)
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = true;
    switch (msg.what_) {
        case PSE_REQ_OBEX_DISCONNECT:
            Transition(PbapPseStateMachine::PSE_DISCONNECTING_STATE);
            // No need to delete
            DisconnectObex(*static_cast<PbapPseObexMessage *>(msg.arg2_));
            break;
        case PSE_DEVICE_DISCONNECTED:
            Transition(PbapPseStateMachine::PSE_DISCONNECTING_STATE);
            Transition(PbapPseStateMachine::PSE_DISCONNECTED_STATE);
            break;
        case PSE_API_DISCONNECT:
            if (stm_.GetStmInfo().obexSession_ != nullptr) {
                Transition(PbapPseStateMachine::PSE_DISCONNECTING_STATE);
                int retVal = stm_.GetStmInfo().obexSession_->Disconnect();
                if (retVal != 0) {
                    PBAP_PSE_LOG_ERROR("Obex Disconnect fail ret=%{public}d", retVal);
                }
            }
            break;
        case PSE_REQ_OBEX_GET:
            // No need to delete
            stm_.SetBusy(true);
            HandleGetRequest(*static_cast<PbapPseObexMessage *>(msg.arg2_));
            stm_.SetBusy(false);
            break;
        case PSE_REQ_OBEX_SETPATH:
            // No need to delete
            stm_.SetBusy(true);
            HandleSetPathRequest(*static_cast<PbapPseObexMessage *>(msg.arg2_));
            stm_.SetBusy(false);
            break;
        default:
            PBAP_PSE_LOG_ERROR("NOT SUPPORT MSGID %{public}d", msg.what_);
            ret = false;
            break;
    }
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

void PseConnected::HandleGetRequest(const PbapPseObexMessage &obexMsg)
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
    const ObexHeader &req = obexMsg.GetObexHeader();
    ObexServerSession &obexSession = obexMsg.GetObexSession();
    auto typeItem = req.GetItemType();
    if (typeItem == nullptr) {
        PBAP_PSE_LOG_ERROR("PullPhoneBook: type can't be null!");
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }
    std::string type = typeItem->GetString();
    if (PBAP_PSE_TYPE_PB == type) {  // type for PullPhoneBook function
        PullPhoneBook(obexSession, req);
    } else if (PBAP_PSE_TYPE_LISTING == type) {  // type for PullvCardListing function
        PullvCardListing(obexSession, req);
    } else if (PBAP_PSE_TYPE_VCARD == type) {  // type for PullvCardEntry Function
        PullvCardEntry(obexSession, req);
    } else {
        PBAP_PSE_LOG_ERROR("PullPhoneBook: type [%{public}s] is unknow.", type.c_str());
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
    }
}

void PseConnected::HandleSetPathRequest(const PbapPseObexMessage &obexMsg)
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
    const ObexHeader &req = obexMsg.GetObexHeader();
    const ObexServerSession &obexSession = obexMsg.GetObexSession();
    uint8_t flags = *req.GetFieldFlags();
    PBAP_PSE_LOG_DEBUG("SetPath flags is %x", flags);
    bool backup = false;
    bool create = true;
    if ((flags & OBEX_SETPATH_BACKUP) != 0) {
        backup = true;
    }
    if ((flags & OBEX_SETPATH_NOCREATE) != 0) {
        create = false;
    }
    if (create) {
        PBAP_PSE_LOG_ERROR("path create is forbidden!");
        obexSession.SendSimpleResponse(ObexRspCode::FORBIDDEN);
        return;
    }
    std::vector<std::u16string> tmpPaths = currentPath_;
    if (backup) {
        if (currentPath_.size() > 0) {
            tmpPaths.pop_back();
        }
    } else {
        std::u16string name = u"";
        auto nameItem = req.GetItemName();
        if (nameItem != nullptr) {
            name = nameItem->GetUnicodeText();
        }
        if (name.size() == 0) {
            tmpPaths.clear();
        } else {
            tmpPaths.push_back(name);
        }
    }
    std::u16string tmpFullPath = GetFullPath(tmpPaths);
    if (!PbapPseVcardManager::CheckPhoneBookPath(tmpFullPath)) {
        PBAP_PSE_LOG_ERROR("path isn't allowed! %{public}s", ObexUtils::UnicodeToUtf8(tmpFullPath).c_str());
        obexSession.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::NOT_FOUND));
        return;
    }
    currentPath_ = tmpPaths;
    std::u16string path16 = GetCurrentPath();
    std::string path = ObexUtils::UnicodeToUtf8(path16);
    PBAP_PSE_LOG_DEBUG("CurrentPath is change to %{public}s", path.c_str());
    obexSession.SendResponse(*ObexHeader::CreateResponse(ObexRspCode::SUCCESS));
}

void PseConnected::SendPhoneBookResponse(ObexServerSession &obexSession, const ObexHeader &req,
    const PbapPseVcardManager::PhoneBookResult &pbResult, const PbapPsePhoneBookResOpt &opt) const
{
    PbapPseAppParams respAppParams;
    if (opt.phonebookSize_) {
        uint16_t phoneBookSize = pbResult.phoneBookSize_;
        respAppParams.SetPhonebookSize(phoneBookSize);
    }
    if (opt.newMissedCall_) {
        respAppParams.SetNewMissedCalls(pbResult.newMissedCallsSize_);
    }
    // the “Folder Version Counters” PSE and PCE feature bits are set
    if (opt.folderVer_) {
        respAppParams.SetPrimaryFolderVersion(pbResult.primaryFolderVersion_);
        respAppParams.SetSecondaryFolderVersion(pbResult.secondaryFolderVersion_);
    }
    // the “Database Version” PSE and PCE feature bits are set
    if (opt.dbIdentifier_) {
        respAppParams.SetDatabaseIdentifier(pbResult.databaseIdentifier_);
    }
    auto resp = ObexHeader::CreateResponse(ObexRspCode::SUCCESS);
    respAppParams.AddToObexHeader(*resp);
    std::shared_ptr<ObexArrayBodyObject> bodyObject = nullptr;
    // ADD BODY
    if (!pbResult.phoneBookSizeOnly_ && pbResult.result_.size() > 0) {
            PBAP_PSE_LOG_DEBUG("pbResult.result_.size() = %{public}zu", pbResult.result_.size());
            bodyObject = std::make_shared<ObexArrayBodyObject>(pbResult.result_.data(), pbResult.result_.size());
    }
    obexSession.SendGetResponse(req, *resp, bodyObject);
}

void PseConnected::PullPhoneBook(ObexServerSession &obexSession, const ObexHeader &req) const
{
    PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
    auto nameItem = req.GetItemName();
    if (nameItem == nullptr) {
        PBAP_PSE_LOG_ERROR("PullPhoneBook: name can't be null!");
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }
    std::u16string nameWithFolder = nameItem->GetUnicodeText();
    if (!IsVCardName(nameWithFolder)) {
        PBAP_PSE_LOG_ERROR("PullPhoneBook: name must endwith [.vcf]");
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }

    auto appParamsItem = req.GetItemAppParams();
    PbapPseAppParams pbapAppParams;
    if (appParamsItem != nullptr && appParamsItem->GetTlvParamters()) {
        bool result = pbapAppParams.Init(*appParamsItem->GetTlvParamters());
        if (!result) {
            obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
            return;
        }
    }
    PbapPseVcardManager::PhoneBookResult pbResult;
    PbapPseVcardManager::PullPhoneBook(nameWithFolder, pbapAppParams, GetSupportedFeatures(), pbResult);
    if (pbResult.rspCode_ != ObexRspCode::SUCCESS) {
        obexSession.SendSimpleResponse(pbResult.rspCode_);
        return;
    }

    PbapPsePhoneBookResOpt opts;
    (void)memset_s(&opts, sizeof(PbapPsePhoneBookResOpt), 0, sizeof(PbapPsePhoneBookResOpt));
    opts.phonebookSize_ = pbResult.phoneBookSizeOnly_;
    opts.newMissedCall_ = pbResult.newMissedCalls_;
    opts.dbIdentifier_ = PbapPseVcardManager::IsSupportedDbVer(GetSupportedFeatures());
    opts.folderVer_ = PbapPseVcardManager::IsSupportedFolderVer(GetSupportedFeatures());

    SendPhoneBookResponse(obexSession, req, pbResult, opts);
}

void PseConnected::PullvCardListing(ObexServerSession &obexSession, const ObexHeader &req) const
{
    PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
    auto nameItem = req.GetItemName();
    if (nameItem == nullptr) {
        PBAP_PSE_LOG_ERROR("PullvCardListing: name can't be null!");
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }
    std::u16string folderName = nameItem->GetUnicodeText();
    if (folderName.find(u"/") != std::u16string::npos || folderName.find(u".") != std::u16string::npos) {
        PBAP_PSE_LOG_ERROR("PullvCardListing: The name shall not include any path/file information!");
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }
    auto appParamsItem = req.GetItemAppParams();
    PbapPseAppParams pbapAppParams;
    if (appParamsItem != nullptr && appParamsItem->GetTlvParamters()) {
        bool result = pbapAppParams.Init(*appParamsItem->GetTlvParamters());
        if (!result) {
            obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
            return;
        }
    }
    PbapPseVcardManager::PhoneBookResult pbResult;
    PbapPseVcardManager::PullvCardListing(GetCurrentPath(), folderName, pbapAppParams, GetSupportedFeatures(), pbResult);
    if (pbResult.rspCode_ != ObexRspCode::SUCCESS) {
        obexSession.SendResponse(*ObexHeader::CreateResponse(pbResult.rspCode_));
        return;
    }

    PbapPsePhoneBookResOpt opts;
    (void)memset_s(&opts, sizeof(PbapPsePhoneBookResOpt), 0, sizeof(PbapPsePhoneBookResOpt));
    opts.phonebookSize_ = pbResult.phoneBookSizeOnly_;
    opts.newMissedCall_ = pbResult.newMissedCalls_;
    opts.dbIdentifier_ = PbapPseVcardManager::IsSupportedDbVer(GetSupportedFeatures());
    opts.folderVer_ = PbapPseVcardManager::IsSupportedFolderVer(GetSupportedFeatures());

    SendPhoneBookResponse(obexSession, req, pbResult, opts);
}

bool PseConnected::CheckvCardEntryId(std::u16string &entryId) const
{
    if (entryId.find(u"/") != std::u16string::npos) {
        PBAP_PSE_LOG_ERROR("PullvCardEntry: The name shall not include any path information!");
        return false;
    }
    if (!IsVCardName(entryId) && entryId.find(PBAP_PSE_X_BT_UID_PREFIX) != 0) {
        PBAP_PSE_LOG_ERROR("PullvCardEntry: name must endwith [.vcf] or startWith [X-BT-UID:]");
        return false;
    }

    if (entryId.find(PBAP_PSE_X_BT_UID_PREFIX) == 0) {
        if (entryId.size() == PBAP_PSE_X_BT_UID_PREFIX.size()) {
            PBAP_PSE_LOG_ERROR("PullvCardEntry: X-BT-UID is empty!");
            return false;
        }
    } else {
        if (entryId.size() == PBAP_PSE_VCARD_SUFFIX.size() ||
            entryId.size() > (PBAP_PSE_VCARD_SUFFIX.size() + PBAP_PSE_HANDLE_MAX_LENGTH)) {
            PBAP_PSE_LOG_ERROR("PullvCardEntry: vcf size is invalid!");
            return false;
        }
        std::u16string hdlId = entryId.substr(0, entryId.size() - PBAP_PSE_VCARD_SUFFIX.size());
        if (!PbapPseVcardManager::CheckVcardHandleId(hdlId)) {
            PBAP_PSE_LOG_ERROR("PullvCardEntry: vcf is invalid! %{public}s", ObexUtils::UnicodeToUtf8(entryId).c_str());
            return false;
        }
    }
    return true;
}

void PseConnected::PullvCardEntry(ObexServerSession &obexSession, const ObexHeader &req) const
{
    PBAP_PSE_LOG_INFO("%{public}s ", __PRETTY_FUNCTION__);
    auto nameItem = req.GetItemName();
    if (nameItem == nullptr) {
        PBAP_PSE_LOG_ERROR("PullvCardEntry: name can't be null!");
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }
    std::u16string entryId = nameItem->GetUnicodeText();
    if (!CheckvCardEntryId(entryId)) {
        obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
        return;
    }
    auto appParamsItem = req.GetItemAppParams();
    PbapPseAppParams pbapAppParams;
    if (appParamsItem != nullptr && appParamsItem->GetTlvParamters()) {
        bool result = pbapAppParams.Init(*appParamsItem->GetTlvParamters());
        if (!result) {
            obexSession.SendSimpleResponse(ObexRspCode::NOT_ACCEPTABLE);
            return;
        }
    }
    PbapPseVcardManager::PhoneBookResult pbResult;
    PbapPseVcardManager::PullvCardEntry(GetCurrentPath(), entryId, pbapAppParams, GetSupportedFeatures(), pbResult);
    if (pbResult.rspCode_ != ObexRspCode::SUCCESS) {
        obexSession.SendSimpleResponse(pbResult.rspCode_);
        return;
    }

    PbapPsePhoneBookResOpt opts;
    (void)memset_s(&opts, sizeof(PbapPsePhoneBookResOpt), 0, sizeof(PbapPsePhoneBookResOpt));
    opts.phonebookSize_ = false;
    opts.newMissedCall_ = false;
    opts.dbIdentifier_ = PbapPseVcardManager::IsSupportedDbVer(GetSupportedFeatures());
    opts.folderVer_ = false;

    SendPhoneBookResponse(obexSession, req, pbResult, opts);
}

bool PseConnected::IsVCardName(std::u16string &name) const
{
    return name.size() >= PBAP_PSE_VCARD_SUFFIX.size() &&
           name.find(PBAP_PSE_VCARD_SUFFIX, name.size() - PBAP_PSE_VCARD_SUFFIX.size()) != std::u16string::npos;
}

std::u16string PseConnected::GetCurrentPath() const
{
    return GetFullPath(currentPath_);
}

std::u16string PseConnected::GetFullPath(const std::vector<std::u16string> &paths) const
{
    std::u16string currentPath = std::accumulate(paths.begin(), paths.end(), std::u16string(u""),
        [](std::u16string strRet, std::u16string str)->std::u16string {
            return strRet + u"/" + str;
        });

    return currentPath;
}

uint32_t PseConnected::GetSupportedFeatures() const
{
    return stm_.GetStmInfo().pceFeatures_ & PBAP_PSE_SUPPORTED_FEATURES;
}

void PseConnected::DisconnectObex(const PbapPseObexMessage &obexMsg) const
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    ObexServerSession &obexSession = obexMsg.GetObexSession();
    obexSession.SendSimpleResponse(ObexRspCode::SUCCESS);
    obexSession.Disconnect();
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

PseDisConnected::PseDisConnected(
    const std::string &name, PbapPseStateMachine &stm, BaseObserverList<IPbapPseObserver> &observerMgrList)
    : State(name, stm), stm_(stm), observerMgrList_(observerMgrList)
{}

void PseDisConnected::Entry()
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_OFF, PROFILE_NAME_PBAP_PSE, device);
    observerMgrList_.ForEach([device](IPbapPseObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::DISCONNECTED));
    });
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PseDisConnected::Exit()
{}

bool PseDisConnected::Dispatch(const utility::Message &msg)
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PBAP_PSE_LOG_ERROR("NOT SUPPORT MSGID %{public}d", msg.what_);
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return false;
}

PseDisConnecting::PseDisConnecting(
    const std::string &name, PbapPseStateMachine &stm, BaseObserverList<IPbapPseObserver> &observerMgrList)
    : State(name, stm), stm_(stm), observerMgrList_(observerMgrList)
{}

void PseDisConnecting::Entry()
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    auto &device = stm_.GetDevice();
    observerMgrList_.ForEach([device](IPbapPseObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::DISCONNECTING));
    });
    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PseDisConnecting::Exit()
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
}

bool PseDisConnecting::Dispatch(const utility::Message &msg)
{
    PBAP_PSE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    bool ret = true;
    switch (msg.what_) {
        case PSE_DEVICE_DISCONNECTED:
            Transition(PbapPseStateMachine::PSE_DISCONNECTED_STATE);
            break;
        default:
            PBAP_PSE_LOG_ERROR("NOT SUPPORT MSGID %{public}d", msg.what_);
            ret = false;
            break;
    }

    PBAP_PSE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

PbapPseStateMachine::PbapPseStateMachine(const RawAddress &device, BaseObserverList<IPbapPseObserver> &observerMgrList,
    std::function<uint32_t()> getNextConnectIdFun)
    : device_(device), observerMgrList_(observerMgrList), getNextConnectIdFun_(std::move(getNextConnectIdFun))
{
}

PbapPseStateMachine::~PbapPseStateMachine()
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
}

const std::string PbapPseStateMachine::PSE_WAITING_FOR_CONNECT_STATE = "PseWaitingForConnect";
const std::string PbapPseStateMachine::PSE_CONNECTED_STATE = "PseConnected";
const std::string PbapPseStateMachine::PSE_CONNECTING_STATE = "PseConnecting";
const std::string PbapPseStateMachine::PSE_DISCONNECTED_STATE = "PseDisConnected";
const std::string PbapPseStateMachine::PSE_DISCONNECTING_STATE = "PseDisConnecting";

void PbapPseStateMachine::Init()
{
    std::unique_ptr<State> disconnected =
        std::make_unique<PseDisConnected>(PSE_DISCONNECTED_STATE, *this, observerMgrList_);
    std::unique_ptr<State> disconnecting =
        std::make_unique<PseDisConnecting>(PSE_DISCONNECTING_STATE, *this, observerMgrList_);
    std::unique_ptr<State> connecting =
        std::make_unique<PseConnecting>(PSE_CONNECTING_STATE, *this, observerMgrList_, getNextConnectIdFun_);
    std::unique_ptr<State> waitingForCnt = std::make_unique<PseWaitingForConnect>(PSE_WAITING_FOR_CONNECT_STATE, *this);
    std::unique_ptr<State> connected = std::make_unique<PseConnected>(PSE_CONNECTED_STATE, *this, observerMgrList_);

    Move(disconnected);
    Move(disconnecting);
    Move(connecting);
    Move(waitingForCnt);
    Move(connected);

    InitState(PSE_WAITING_FOR_CONNECT_STATE);
}

bool PbapPseStateMachine::StartTimer(std::function<void()> callback, int ms)
{
    PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
    timer_ = std::make_unique<utility::Timer>(callback);
    return timer_->Start(ms, false);
}

void PbapPseStateMachine::StopTimer()
{
    if (timer_) {
        PBAP_PSE_LOG_INFO("%{public}s", __PRETTY_FUNCTION__);
        timer_->Stop();
        timer_ = nullptr;
    }
}

BTConnectState PbapPseStateMachine::GetConnectState() const
{
    std::string name = GetState()->Name();
    if (name == PSE_CONNECTING_STATE) {
        return BTConnectState::CONNECTING;
    }
    if (name == PSE_CONNECTED_STATE) {
        return BTConnectState::CONNECTED;
    }
    if (name == PSE_DISCONNECTING_STATE) {
        return BTConnectState::DISCONNECTING;
    }
    return BTConnectState::DISCONNECTED;
}

/**
 * @brief get remote address
 * @details get remote address
 * @return RawAddress&
 */
const RawAddress &PbapPseStateMachine::GetDevice() const
{
    return device_;
}

/**
 * @brief Is Connected
 * @details Is Connected
 * @return bool
 */
bool PbapPseStateMachine::IsConnected() const
{
    return GetState()->Name() == PSE_CONNECTED_STATE;
}

bool PbapPseStateMachine::IsAccepted() const
{
    return stmInfo_.accepted_;
}

void PbapPseStateMachine::SetBusy(bool isBusy)
{
    if (stmInfo_.isBusy_ == isBusy) {
        return;
    }
    stmInfo_.isBusy_ = isBusy;
    if (isBusy) {
        HILOGI("[%{public}s] PowerStatusUpdate -> BUSY", GetEncryptAddr(device_.GetAddress()).c_str());
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::BUSY, PROFILE_NAME_PBAP_PSE, device_);
    } else {
        HILOGI("[%{public}s] PowerStatusUpdate -> IDLE", GetEncryptAddr(device_.GetAddress()).c_str());
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::IDLE, PROFILE_NAME_PBAP_PSE, device_);
    }
}

bool PbapPseStateMachine::IsBusy() const
{
    return stmInfo_.isBusy_;
}

PbapPseStateMachineInfo &PbapPseStateMachine::GetStmInfo()
{
    return stmInfo_;
}
}  // namespace bluetooth
}  // namespace OHOS
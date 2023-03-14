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

#include "pbap_pce_connecting_state.h"
#include <cstring>
#include <vector>
#include "../obex/obex_utils.h"
#include "pbap_pce_service.h"
#include "power_manager.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
PceConnectingState::PceConnectingState(
    const std::string &name, PbapPceStateMachine &stm, BaseObserverList<IPbapPceObserver> &observerMgrList)
    : PceBaseState(name, stm, observerMgrList),
      authDescription_(),
      authNonce_(),
      authUserCharset_(0),
      authFullAccess_(false),
      authNeedUser_(false),
      authChallenge_(false),
      header_(nullptr)
{}

void PceConnectingState::Entry()
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);

    PBAP_PCE_LOG_INFO("observerMgrList_ -> CONNECTING");
    auto device = stm_.GetDevice();
    observerMgrList_.ForEach([device](IPbapPceObserver &observer) {
        observer.OnServiceConnectionStateChanged(device, static_cast<int>(BTConnectState::CONNECTING));
    });
    int retVal = stm_.GetSdp().SdpSearch(stm_.GetDevice());
    if (retVal != BT_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("%{public}s end, pce client Call SDP_ServiceSearchAttribute Error", __PRETTY_FUNCTION__);
        Transition(PCE_DISCONNECTING_STATE);
        Transition(PCE_DISCONNECTED_STATE);
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PceConnectingState::Exit()
{}

void PceConnectingState::RegGap(const PbapPceHeaderSdpMsg &sdpMsg) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);

    const auto &obexClientConfig = sdpMsg.GetObexClientConfig();
    bool isGoepL2capPSM = obexClientConfig.isGoepL2capPSM_;
    GapSecChannel gapSecChannel;
    if (isGoepL2capPSM) {
        gapSecChannel.l2capPsm = obexClientConfig.scn_;
    } else {
        gapSecChannel.rfcommChannel = obexClientConfig.scn_;
    }
    auto gap = std::make_unique<PbapPceGap>(stm_.GetPceService(), stm_.GetDevice(), gapSecChannel, isGoepL2capPSM);
    int retVal = gap->Register();
    if (retVal != BT_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("%{public}s end, GAP_RegisterServiceSecurity() error", __PRETTY_FUNCTION__);
        return;
    }

    stm_.SetGap(*(gap.release()));

    stm_.SetGapSecChannel(gapSecChannel);
    stm_.SetGoepL2capPSM(isGoepL2capPSM);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

int PceConnectingState::ObexConnect(const PbapPceHeaderSdpMsg &sdpMsg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    // regisger gap
    RegGap(sdpMsg);

    // set version, repository, feature to stm
    stm_.SetVersionNumber(sdpMsg.GetVersionNumber());
    stm_.SetSupportedRes(sdpMsg.GetSupportedRes());
    stm_.SetSupportedFeature(sdpMsg.GetSupportedFeature());
    stm_.SetFeatureFlag(sdpMsg.GetFeatureFlag());

    obexConfig_ = sdpMsg.GetObexClientConfig();
    SetObexClientConfigDetail(obexConfig_);
    stm_.CreatePceObexClient(obexConfig_);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return stm_.GetGap()->RequestSecurity();
}

void PceConnectingState::InitAuth()
{
    authDescription_.clear();
    authChallenge_ = true;
    authFullAccess_ = true;
    authNeedUser_ = false;
    authNonce_.clear();
}

int PceConnectingState::SaveObexDigestChallenge(const ObexDigestChallenge &digestChallenge)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    auto nonceTlv = digestChallenge.GetNonce();
    if (nonceTlv == nullptr) {  // nonce is required, but not found
        PBAP_PCE_LOG_ERROR("%{public}s end, header is not contained nonce", __PRETTY_FUNCTION__);
        return RET_NO_SUPPORT;
    }

    authNonce_.insert(authNonce_.end(), nonceTlv->GetVal(), nonceTlv->GetVal() + nonceTlv->GetLen());
    auto optionsTlv = digestChallenge.GetOptions();
    if (optionsTlv != nullptr) {
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

    auto realmTlv = digestChallenge.GetRealm();
    if (realmTlv != nullptr) {
        if (realmTlv->GetLen() > 0) {
            authUserCharset_ = realmTlv->GetVal()[0];
            if (realmTlv->GetLen() > 1) {
                authDescription_.insert(
                    authDescription_.end(), realmTlv->GetVal() + 1, realmTlv->GetVal() + realmTlv->GetLen());
            }
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return RET_NO_ERROR;
}

int PceConnectingState::ObexReconnect(const PbapPceObexMessage &obexMsg)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int ret = RET_BAD_STATUS;
    const ObexHeader *header = obexMsg.GetObexHeader();
    ObexClient *client = obexMsg.GetObexClient();
    header_ = std::make_unique<ObexHeader>(*header);
    if (header->GetFieldCode() != static_cast<uint8_t>(ObexRspCode::UNAUTHORIZED)) {
        if (client != nullptr) {
            client->Disconnect(false);
            PBAP_PCE_LOG_ERROR("%{public}s end, response is not UNAUTHORIZED Disconnect(not withObexReq)", __PRETTY_FUNCTION__);
        }
        return ret;
    }
    auto tvlHeader = header->GetItemAuthChallenges();
    if (tvlHeader == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, returned header from pse not contain challenge parameter", __PRETTY_FUNCTION__);
        return RET_NO_SUPPORT;
    }

    InitAuth();

    auto authChallenges = header->GetItemAuthChallenges();
    auto digestChallenge = static_cast<ObexDigestChallenge *>(authChallenges->GetTlvParamters().get());
    ret = SaveObexDigestChallenge(*digestChallenge);
    if (ret != RET_NO_ERROR) {
        PBAP_PCE_LOG_ERROR("%{public}s end, SaveObexDigestChallenge error, ret=%{public}d!", __PRETTY_FUNCTION__, ret);
        return ret;
    }

    auto &device = stm_.GetDevice();
    auto &authDescription = authDescription_;
    auto &authUserCharset = authUserCharset_;
    auto &authFullAccess = authFullAccess_;
    observerMgrList_.ForEach([device, authDescription, authUserCharset, authFullAccess](IPbapPceObserver &observer) {
        observer.OnServicePasswordRequired(device, authDescription, authUserCharset, authFullAccess);
    });

    return RET_NO_ERROR;
}

void PceConnectingState::SetObexClientConfigDetail(ObexClientConfig &obexConfig) const
{
    obexConfig.isSupportReliableSession_ = false;
    if (obexConfig.isGoepL2capPSM_) {
        obexConfig.mtu_ = stm_.GetPceService().GetPceConfig().l2capMtu_;
    } else {
        obexConfig.mtu_ = stm_.GetPceService().GetPceConfig().rfcommMtu_;
    }
    const int len = PBAP_PCE_SERVICE_UUID_LEN;
    (void)memcpy_s(&obexConfig.serviceUUID_.uuid128, len, PBAP_PCE_SERVICE_UUID, len);
}

void PceConnectingState::ProcessObexConnected(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    auto obexMsg = static_cast<PbapPceObexMessage *>(msg.arg2_);
    if ((obexMsg != nullptr) && (obexMsg->GetObexHeader() != nullptr)) {
        auto connectId = obexMsg->GetObexHeader()->GetItemConnectionId();
        if (connectId != nullptr) {
            stm_.SetConnectId(connectId->GetWord());
        }
    }
    Transition(PCE_CONNECTED_STATE);
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
}

void PceConnectingState::ProcessSdpFinish(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    std::unique_ptr<PbapPceHeaderSdpMsg> sdpMsg(static_cast<PbapPceHeaderSdpMsg *>(msg.arg2_));
    if (sdpMsg != nullptr) {
        if (ObexConnect(*sdpMsg) != RET_NO_ERROR) {
            Transition(PCE_DISCONNECTING_STATE);
            Transition(PCE_DISCONNECTED_STATE);
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
}

void PceConnectingState::ProcessGapFinish(const utility::Message &msg) const
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    stm_.GetObexClient().Connect(stm_.GetFeatureFlag());
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
}

void PceConnectingState::ProcessObexConnectFailed(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    auto obexMsg = static_cast<PbapPceObexMessage *>(msg.arg2_);
    if ((obexMsg != nullptr) && ObexReconnect(*obexMsg) != BT_NO_ERROR) {
        Transition(PCE_DISCONNECTING_STATE);
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
}

void PceConnectingState::ProcessObexTransportFailed(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    if (msg.what_ == PCE_TRANSPORT_FAILED) {
        if (msg.arg1_ == CONNECT_COLLISION) {
            // re gap request security and reconnect
            stm_.GetGap()->RequestSecurity();
        } else {
            Transition(PCE_DISCONNECTING_STATE);
            Transition(PCE_DISCONNECTED_STATE);
        }
    }
    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
}

void PceConnectingState::ProcessPasswordInput(const utility::Message &msg) const
{
    std::unique_ptr<PbapPcePasswordInputMsg> pwdInputMsg(static_cast<PbapPcePasswordInputMsg *>(msg.arg2_));
    PasswordInput(*pwdInputMsg);
}

bool PceConnectingState::Dispatch(const utility::Message &msg)
{
    PBAP_PCE_LOG_INFO("%{public}s start, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    switch (msg.what_) {
        case PCE_SDP_FINISH:
            ProcessSdpFinish(msg);
            break;
        case PCE_GAP_FINISH:
            ProcessGapFinish(msg);
            break;
        case PCE_REQ_DISCONNECTED:
            Transition(PCE_DISCONNECTED_STATE);
            break;
        case PCE_OBEX_CONNECTED:
            ProcessObexConnected(msg);
            break;
        case PCE_SDP_FAILED:
        case PCE_GAP_FAILED:
            Transition(PCE_DISCONNECTING_STATE);
            Transition(PCE_DISCONNECTED_STATE);
            break;
        case PCE_OBEX_CONNECT_FAILED:
            ProcessObexConnectFailed(msg);
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
        case PCE_TRANSPORT_FAILED:
            ProcessObexTransportFailed(msg);
            break;
        case PCE_PASSWORD_INPUT:
            ProcessPasswordInput(msg);
            break;
        default:
            return false;
    }

    PBAP_PCE_LOG_INFO("%{public}s end, msg.what_=[%{public}d]", __PRETTY_FUNCTION__, msg.what_);
    return true;
}

void PceConnectingState::PasswordInput(const PbapPcePasswordInputMsg &pwdInputMsg) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (!authChallenge_) {
        return;
    }
    if (pwdInputMsg.GetPassword() == "") {
        return;
    }
    stm_.SetPassword(pwdInputMsg.GetPassword());
    stm_.SetUserId(pwdInputMsg.GetUserId());

    SendRequest();
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PceConnectingState::SendRequest() const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (header_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, header_ is nullptr", __PRETTY_FUNCTION__);
        return;
    }
    // get challenge
    auto tvlHeader = header_->GetItemAuthChallenges();
    auto &tvlParam = tvlHeader->GetTlvParamters();
    auto challenge = static_cast<ObexDigestChallenge *>(tvlParam.get());

    // Authenticate Response
    // Carries the digest-response string. This is the response to the challenge from the Client.
    ObexDigestResponse response;
    std::vector<uint8_t> requestDigest =
        ObexUtils::MakeRequestDigest(authNonce_.data(), authNonce_.size(), stm_.GetPassword());
    response.AppendRequestDigest(requestDigest.data(), requestDigest.size());
    if (authNeedUser_ && stm_.GetUserId().size() > 0) {
        const uint8_t *userId = reinterpret_cast<const uint8_t *>(stm_.GetUserId().c_str());
        response.AppendUserId(userId, stm_.GetUserId().size());
    }
    response.AppendNonce(authNonce_.data(), authNonce_.size());

    stm_.GetObexClient().Reconnect(*challenge, response);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}
}  // namespace bluetooth
}  // namespace OHOS
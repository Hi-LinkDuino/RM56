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

#include "a2dp_state_machine.h"
#include "a2dp_codec_thread.h"
#include "a2dp_service.h"
#include "a2dp_sink.h"
#include "compat.h"
#include "log.h"
#include "power_manager.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::recursive_mutex g_stateMutex {};
void A2dpStateIdle::Entry()
{
    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    if (codecThread->GetInitStatus()) {
        codecThread->StopA2dpCodecThread();
        delete codecThread;
    }
}

bool A2dpStateIdle::Dispatch(const utility::Message &msg)
{
    if (msg.arg2_ == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s input error parameter\n", __func__);
        return false;
    }
    uint8_t label = 0;
    A2dpAvdtMsgData msgData = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->a2dpMsg;
    uint8_t role = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->role;
    A2dpAvdtp avdtp(role);
    switch (msg.what_) {
        case EVT_CONNECT_REQ:
            avdtp.ConnectReq(msgData.connectInfo.addr);
            break;
        case EVT_DISCOVER_REQ:
            ProcessDiscoverReq(msgData.connectInfo.addr, role, msgData.connectInfo.errCode);
            break;
        case EVT_GET_ALLCAP_REQ:
            avdtp.GetAllCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_GET_CAP_REQ:
            avdtp.GetCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_SETCONFIG_REQ:
            ProcessSetConfigReq(msgData, role);
            break;
        case EVT_DISCONNECT_REQ:
            ProcessDisconnectReq(msgData.connectInfo.addr, role);
            break;
        case EVT_SDP_CFM:
            ProcessSDPFailure(msgData.stream.addr, role);
            break;
        case EVT_SETCONFIG_IND:
            ProcessSetConfigInd(msgData, role);
            break;
        case EVT_DISCONNECT_IND:
            ProcessDisconnectInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_DISCONNECT_CFM:
            ProcessDisconnectCfm(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_TIME_OUT:
            ProcessTimeout(msgData.stream.addr, role);
            break;
        default:
            break;
    }
    return true;
}

void A2dpStateIdle::ProcessDisconnectCfm(BtAddr addr, uint16_t handle, uint8_t role) const
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_INT, true, handle};
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }

    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateIdle::SetStateName(std::string state)
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    Transition(state);
}

void A2dpStateIdle::ProcessDisconnectReq(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
}

void A2dpStateIdle::ProcessTimeout(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
}

void A2dpStateIdle::ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_ACP, false, handle};
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateIdle::ProcessSetConfigReq(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);
    uint8_t label = 0;

    SetStateName(A2DP_PROFILE_CONFIG);
    avdtp.SetConfigureReq(msgData.configStream.addr,
        msgData.configStream.intSeid,
        msgData.configStream.acpSeid,
        msgData.configStream.cfg,
        label);
}

void A2dpStateIdle::ProcessSetConfigInd(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);
    A2dpProfile *profile = GetProfileInstance(role);
    CallbackParameter param = {A2DP_ROLE_ACP, false, 0};
    A2dpAvdtp avdtp(role);
    A2dpProfilePeer *profilePeer = nullptr;
    uint8_t resultConfig[A2DP_CODEC_SIZE];
    (void)memset_s(resultConfig, A2DP_CODEC_SIZE, 0, A2DP_CODEC_SIZE);
    uint8_t *pResultConfig = &resultConfig[0];
    A2dpCodecCapability userConfig = {};
    bool reconfig = false;
    bool ret = false;
    if (profile == nullptr) {
        return;
    }
    profilePeer = profile->FindPeerByAddress(msgData.configRsp.addr);
    if (profilePeer == nullptr) {
        return;
    }
    A2dpCodecFactory *codecFactory = profilePeer->GetCodecConfigure();
    if (codecFactory == nullptr) {
        return;
    }
    uint8_t *pCodecInfo = msgData.configRsp.codecInfo;
    SetStateName(A2DP_PROFILE_CONFIG);
    avdtp.SetConfigureRsp(msgData.configRsp.handle, msgData.configRsp.label, msgData.configRsp.category);
    profile->ConnectStateChangedNotify(msgData.configRsp.addr, STREAM_CONNECTING, static_cast<void *>(&param));
    if (role == A2DP_ROLE_SOURCE) {
        ret = codecFactory->SetCodecConfig(pCodecInfo, pResultConfig);
        LOG_INFO("[A2dpStateIdle]%{public}s SetConfig result(%{public}d)\n", __func__, ret);
    } else {
        if (codecFactory->FindSourceCodec(pCodecInfo) == nullptr ||
            codecFactory->GetIndexedSinkCodecs().front() == nullptr) {
            return;
        }
        if (codecFactory->GetIndexedSinkCodecs().front()->GetCodecIndex() ==
            (codecFactory->FindSourceCodec(pCodecInfo)->GetCodecIndex() + A2DP_SINK_CODEC_INDEX_MIN -
            A2DP_SOURCE_CODEC_INDEX_SBC)) {
            ret = codecFactory->SetSinkCodecConfig(pCodecInfo, pResultConfig);
            LOG_INFO("[A2dpStateIdle]%{public}s SetSinkCodecConfig result(%{public}d)\n", __func__, ret);
        } else {
            userConfig.codecIndex_ = codecFactory->FindSourceCodec(pCodecInfo)->GetCodecIndex();
            userConfig.codecPriority_ = A2DP_CODEC_PRIORITY_HIGHEST;
            ret = codecFactory->SetCodecUserConfig(userConfig, pCodecInfo, pResultConfig, &reconfig);
            LOG_INFO("[A2dpStateIdle]%{public}s SetuserConfig result(%{public}d)\n", __func__, ret);
        }
    }
}

void A2dpStateIdle::ProcessDiscoverReq(BtAddr addr, uint8_t role, uint8_t errCode)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    CallbackParameter param = {A2DP_ROLE_INT, true, 0};
    A2dpAvdtp avdtp(role);
    uint8_t label = 0;
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    A2dpProfilePeer *peer = profile->FindPeerByAddress(addr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profilePeer instance\n", __func__);
        return;
    }
    if (errCode != AVDT_SUCCESS) {
        profile->ConnectStateChangedNotify(addr, STREAM_CONNECT_FAILED, (void *)&param);
    } else if (peer->GetDisconnectIndication()) {
        ProcessDisconnectReq(addr, role);
        peer->SetDisconnectIndication(false);
    } else {
        avdtp.DiscoverReq(addr, AVDT_NUM_SEPS, label);
    }
}

void A2dpStateIdle::ProcessSDPFailure(BtAddr addr, uint8_t role) const
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    CallbackParameter param = {A2DP_ROLE_INT, true, 0};
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    profile->DeletePeer(addr);
    profile->ConnectStateChangedNotify(addr, STREAM_CONNECT_FAILED, (void *)&param);
}

bool A2dpStateConfigure::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpStateConfigure]%{public}s\n", __func__);
    if (msg.arg2_ == nullptr) {
        LOG_ERROR("[A2dpStateConfigure]%{public}s input error parameter\n", __func__);
        return false;
    }

    uint8_t label = 0;
    A2dpAvdtMsgData msgData = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->a2dpMsg;
    uint8_t role = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->role;
    A2dpAvdtp avdtp(role);

    switch (msg.what_) {
        case EVT_GET_ALLCAP_REQ:
            avdtp.GetAllCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_GET_CAP_REQ:
            avdtp.GetCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_OPEN_REQ:
            ProcessOpenReq(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_DISCONNECT_REQ:
            ProcessDisconnectReq(msgData.connectInfo.addr, role);
            break;
        case EVT_SETCONFIG_IND:
            ProcessSetConfigInd(msgData, role);
            break;
        case EVT_OPEN_IND:
            ProcessOpenInd(msgData, role);
            break;
        case EVT_DISCONNECT_IND:
            ProcessDisconnectInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_TIME_OUT:
            avdtp.DisconnectReq(msgData.stream.addr);
            SetStateName(A2DP_PROFILE_CLOSING);
            break;
        case EVT_DELAY_IND:
            ProcessDelayReportInd(msgData, role);
            break;
        default:
            break;
    }
    return true;
}

void A2dpStateConfigure::SetStateName(std::string state)
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    Transition(state);
}

void A2dpStateConfigure::ProcessOpenReq(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateConfigure]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);
    uint8_t label = 0;

    SetStateName(A2DP_PROFILE_OPENING);
    avdtp.OpenReq(handle, label);
}

void A2dpStateConfigure::ProcessDisconnectReq(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateConfigure]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
}

void A2dpStateConfigure::ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateIdle]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_ACP, false, handle};
    A2dpProfile *profile = GetProfileInstance(role);

    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }

    SetStateName(A2DP_PROFILE_IDLE);
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateConfigure::ProcessOpenInd(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateConfigure]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_ACP, false, 0};
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    SetStateName(A2DP_PROFILE_OPEN);
    param.handle = msgData.stream.handle;
    profile->CodecChangedNotify(msgData.stream.addr, nullptr);
    LOG_INFO("[A2dpStateOpen]%{public}s open handle[%u]\n", __func__, param.handle);
    profile->ConnectStateChangedNotify(msgData.stream.addr, STREAM_CONNECT, static_cast<void *>(&param));

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON,
            PROFILE_NAME_A2DP_SRC,
            bluetooth::RawAddress::ConvertToString(msgData.stream.addr.addr));
    }
}

void A2dpStateConfigure::ProcessSetConfigInd(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateConfigure]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    A2dpAvdtp avdtp(role);
    A2dpProfilePeer *profilePeer = nullptr;
    uint8_t resultConfig[A2DP_CODEC_SIZE];
    uint8_t *pResultConfig = &resultConfig[0];
    A2dpCodecCapability userConfig = {};
    bool reconfig = false;

    (void)memset_s(resultConfig, A2DP_CODEC_SIZE, 0, A2DP_CODEC_SIZE);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }
    profilePeer = profile->FindPeerByAddress(msgData.configRsp.addr);
    if (profilePeer == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profilePeer instance\n", __func__);
        return;
    }

    A2dpCodecFactory *codecFactory = profilePeer->GetCodecConfigure();
    uint8_t *pCodecInfo = msgData.configRsp.codecInfo;

    if (role == A2DP_ROLE_SOURCE) {
        codecFactory->SetCodecConfig(pCodecInfo, pResultConfig);
    } else {
        if (codecFactory->GetIndexedSinkCodecs().front() == nullptr ||
            codecFactory->FindSourceCodec(pCodecInfo) == nullptr) {
            return;
        }
        if (codecFactory->GetIndexedSinkCodecs().front()->GetCodecIndex() ==
            (codecFactory->FindSourceCodec(pCodecInfo)->GetCodecIndex() + A2DP_SINK_CODEC_INDEX_MIN -
            A2DP_SOURCE_CODEC_INDEX_SBC)) {
            codecFactory->SetSinkCodecConfig(pCodecInfo, pResultConfig);
        } else {
            userConfig.codecIndex_ = codecFactory->FindSourceCodec(pCodecInfo)->GetCodecIndex();
            userConfig.codecPriority_ = A2DP_CODEC_PRIORITY_HIGHEST;
            codecFactory->SetCodecUserConfig(userConfig, pCodecInfo, pResultConfig, &reconfig);
        }
    }

    avdtp.SetConfigureRsp(msgData.configRsp.handle, msgData.configRsp.label, msgData.configRsp.category);
}

void A2dpStateConfigure::ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const
{
    LOG_INFO("[A2dpStateConfigure]%{public}s\n", __func__);
    if (role == A2DP_ROLE_SINK) {
        LOG_INFO("[A2dpStateConfigure]%{public}s sink role does not handle delay report ind return.", __func__);
        return;
    }

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateConfigure]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    BtAddr addr =  msgData.delayReportInfo.addr;
    uint16_t value = msgData.delayReportInfo.delayValue;
    profile->DelayReportNotify(addr, value);
}

bool A2dpStateOpening::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpStateOpening]%{public}s\n", __func__);
    if (msg.arg2_ == nullptr) {
        LOG_ERROR("[A2dpStateOpening]%{public}s input error parameter\n", __func__);
        return false;
    }

    A2dpAvdtMsgData msgData = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->a2dpMsg;
    uint8_t role = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->role;
    A2dpAvdtp avdtp(role);
    uint8_t label = 0;

    switch (msg.what_) {
        case EVT_GET_ALLCAP_REQ:
            avdtp.GetAllCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_GET_CAP_REQ:
            avdtp.GetCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_DISCONNECT_REQ:
            ProcessDisconnectReq(msgData.connectInfo.addr, role);
            break;
        case EVT_DISCONNECT_IND:
            ProcessDisconnectInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_OPEN_CFM:
            ProcessOpenCfm(msgData, role);
            break;
        case EVT_TIME_OUT:
            avdtp.DisconnectReq(msgData.stream.addr);
            SetStateName(A2DP_PROFILE_CLOSING);
            break;
        case EVT_DELAY_IND:
            ProcessDelayReportInd(msgData, role);
            break;
        default:
            break;
    }
    return true;
}

void A2dpStateOpening::SetStateName(std::string state)
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    Transition(state);
}

void A2dpStateOpening::ProcessOpenCfm(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateOpening]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_INT, true, msgData.stream.handle};
    A2dpAvdtp avdtp(role);
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpening]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (msgData.stream.errCode != AVDT_SUCCESS) {
        LOG_ERROR("[A2dpStateOpening]%{public}s [errcode: %u]", __func__, msgData.stream.errCode);
        profile->ConnectStateChangedNotify(msgData.stream.addr, STREAM_CONNECT_FAILED, static_cast<void *>(&param));
    } else {
        SetStateName(A2DP_PROFILE_OPEN);
        profile->CodecChangedNotify(msgData.stream.addr, nullptr);
        LOG_INFO("[A2dpStateOpen]%{public}s open handle[%u]\n", __func__, param.handle);
        if (role == A2DP_ROLE_SOURCE) {
            IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON,
                PROFILE_NAME_A2DP_SRC,
                bluetooth::RawAddress::ConvertToString(msgData.stream.addr.addr));
        }
        profile->ConnectStateChangedNotify(msgData.stream.addr, STREAM_CONNECT, static_cast<void *>(&param));
    }
}

void A2dpStateOpening::ProcessDisconnectReq(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateOpening]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
}

void A2dpStateOpening::ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateOpening]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_ACP, false, handle};
    A2dpProfile *profile = GetProfileInstance(role);

    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpening]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }
    SetStateName(A2DP_PROFILE_IDLE);
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateOpening::ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const
{
    LOG_INFO("[A2dpStateOpening]%{public}s\n", __func__);
    if (role == A2DP_ROLE_SINK) {
        LOG_INFO("[A2dpStateOpening]%{public}s sink role does not handle delay report ind return.", __func__);
        return;
    }

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpening]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    BtAddr addr =  msgData.delayReportInfo.addr;
    uint16_t value = msgData.delayReportInfo.delayValue;
    profile->DelayReportNotify(addr, value);
}

bool A2dpStateOpen::Dispatch(const utility::Message &msg)
{
    if (msg.arg2_ == nullptr) {
        return false;
    }
    A2dpAvdtMsgData msgData = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->a2dpMsg;
    uint8_t role = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->role;
    A2dpAvdtp avdtp(role);

    switch (msg.what_) {
        case EVT_START_IND:
            ProcessStartInd(msgData, role);
            break;
        case EVT_CLOSE_IND:
            ProcessCloseInd(msgData, role);
            break;
        case EVT_DISCONNECT_IND:
            ProcessDisconnectInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_RECONFIG_IND:
            avdtp.ReconfigureRsp(msgData.configRsp.handle, msgData.configRsp.label, msgData.configRsp.category);
            break;
        case EVT_CLOSE_REQ:
            avdtp.CloseReq(msgData.stream.handle);
            break;
        case EVT_DISCONNECT_REQ:
            ProcessDisconnectReq(msgData.connectInfo.addr, role);
            break;
        case EVT_START_REQ:
            avdtp.StartReq(msgData.stream.handle, label_);
            break;
        case EVT_RECONFIG_REQ:
            avdtp.ReconfigureReq(msgData.configStream.intSeid, msgData.configStream.cfg, label_);
            break;
        case EVT_RECONFIG_CFM:
            ProcessReconfigCfm(msgData.stream.addr, role, msgData.stream.handle);
            break;
        case EVT_CLOSE_CFM:
            ProcessCloseCfm(msgData.stream.addr, role);
            break;
        case EVT_START_CFM:
            ProcessStartCfm(msgData.stream.addr, role);
            break;
        case EVT_DELAY_IND:
            ProcessDelayReportInd(msgData, role);
            break;
        default:
            ProcessSubOpenState(msgData, role, msg.what_);
            break;
    }
    return true;
}

void A2dpStateOpen::SetStateName(std::string state)
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    Transition(state);
}

void A2dpStateOpen::ProcessSubOpenState(A2dpAvdtMsgData msgData, uint8_t role, int cmd)
{
    LOG_INFO("[A2dpStateOpen]%{public}s cmd(%{public}d)\n", __func__, cmd);

    A2dpAvdtp avdtp(role);
    uint8_t label = 0;

    switch (cmd) {
        case EVT_GET_ALLCAP_REQ:
            avdtp.GetAllCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_GET_CAP_REQ:
            avdtp.GetCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_TIME_OUT:
            ProcessTimeout(msgData.stream.addr, role);
            break;
        default:
            break;
    }
}

void A2dpStateOpen::ProcessCloseInd(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.CloseRsp(msgData.stream.handle, msgData.stream.label, 0);
}

void A2dpStateOpen::ProcessStartInd(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    uint8_t gavdpRole = A2DP_ROLE_ACP;
    A2dpAvdtp avdtp(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateIdle]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    SetStateName(A2DP_PROFILE_STREAMING);
    if (avdtp.StartRsp(msgData.stream.handle, msgData.stream.label, 0, 0) == AVDT_SUCCESS) {
        profile->AudioStateChangedNotify(msgData.stream.addr, A2DP_IS_PLAYING, (void *)&gavdpRole);
        if (role == A2DP_ROLE_SOURCE) {
            profile->NotifyEncoder(msgData.stream.addr);
        } else {
            profile->NotifyDecoder(msgData.stream.addr);
        }
    } else {
        SetStateName(A2DP_PROFILE_OPEN);
    }
}

void A2dpStateOpen::ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_ACP, false, handle};
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpen]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }

    SetStateName(A2DP_PROFILE_IDLE);
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateOpen::ProcessDisconnectReq(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
}

void A2dpStateOpen::ProcessReconfigCfm(BtAddr addr, uint8_t role, uint16_t handle)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    A2dpAvdtp avdtp(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpen]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    profile->FindPeerByAddress(addr)->SetRestart(false);
    if (Compat::CompatCheck(CompatType::COMPAT_REJECT_AVDTP_RECONFIGURE,
        bluetooth::RawAddress::ConvertToString(addr.addr).GetAddress())) {
        LOG_ERROR("[A2dpStateOpen]%{public}s The remote device is disable to reconfigure\n", __func__);
        avdtp.CloseReq(handle);
        return;
    }
    profile->CodecChangedNotify(addr, nullptr);

    if (profile->IsActiveDevice(addr)) {
        uint8_t label = 0;
        avdtp.StartReq(handle, label);
    }
}

void A2dpStateOpen::ProcessCloseCfm(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpen]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    SetStateName(A2DP_PROFILE_IDLE);
    if (!profile->HasOpen() && profile->GetDisalbeTag()) {
        profile->Disable();
    }
    if (profile->FindPeerByAddress(addr)->GetRestart()) {
        profile->FindPeerByAddress(addr)->UpdateConfigure();
    }
}

void A2dpStateOpen::ProcessStartCfm(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    uint8_t gavdpRole = A2DP_ROLE_INT;
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpen]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    SetStateName(A2DP_PROFILE_STREAMING);
    profile->AudioStateChangedNotify(addr, A2DP_IS_PLAYING, (void *)&gavdpRole);

    if (role == A2DP_ROLE_SOURCE) {
        profile->NotifyEncoder(addr);
    } else {
        profile->NotifyDecoder(addr);
    }
}

void A2dpStateOpen::ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);
    if (role == A2DP_ROLE_SINK) {
        LOG_INFO("[A2dpStateOpen]%{public}s sink role does not handle delay report ind return.", __func__);
        return;
    }

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpen]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    BtAddr addr =  msgData.delayReportInfo.addr;
    uint16_t value = msgData.delayReportInfo.delayValue;
    profile->DelayReportNotify(addr, value);
}

void A2dpStateOpen::ProcessTimeout(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateOpen]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
}

void A2dpStateStreaming::Entry()
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);
    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    if (codecThread == nullptr) {
        return;
    }
    if (codecThread->GetInitStatus()) {
        codecThread->StartTimer();
    } else {
        if (codecThread != nullptr) {
            codecThread->StartA2dpCodecThread();
        }
    }
}

void A2dpStateStreaming::Exit()
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);
    A2dpCodecThread *codecThread = A2dpCodecThread::GetInstance();
    if (codecThread->GetInitStatus()) {
        codecThread->StopTimer();
    }
}

bool A2dpStateStreaming::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);
    if (msg.arg2_ == nullptr) {
        LOG_ERROR("[A2dpStateStreaming]%{public}s input error parameter\n", __func__);
        return false;
    }
    A2dpAvdtMsgData msgData = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->a2dpMsg;
    uint8_t role = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->role;
    A2dpAvdtp avdtp(role);
    uint8_t label = 0;

    switch (msg.what_) {
        case EVT_GET_ALLCAP_REQ:
            avdtp.GetAllCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_GET_CAP_REQ:
            avdtp.GetCapabilityReq(msgData.stream.addr, msgData.stream.acpSeid, label);
            break;
        case EVT_SUSPEND_IND:
            ProcessSuspendInd(msgData, role);
            break;
        case EVT_CLOSE_IND:
            SetStateName(A2DP_PROFILE_IDLE);
            avdtp.CloseRsp(msgData.stream.handle, msgData.stream.label, 0);
            break;
        case EVT_DISCONNECT_IND:
            ProcessDisconnectInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_DISCONNECT_REQ:
            ProcessDisconnectReq(msgData.connectInfo.addr, role);
            break;
        case EVT_CLOSE_REQ:
            avdtp.CloseReq(msgData.stream.handle);
            break;
        case EVT_SUSPEND_REQ:
            ProcessSuspendReq(msgData.stream.handle, role);
            break;
        case EVT_CLOSE_CFM:
            ProcessCloseCfm(msgData.stream.addr, role);
            break;
        case EVT_SUSPEND_CFM:
            ProcessSuspendCfm(msgData, role);
            break;
        case EVT_WRITE_CFM:
            ProcessWriteCfm(msgData, role);
            break;
        case EVT_DELAY_IND:
            ProcessDelayReportInd(msgData, role);
            break;
        default:
            break;
    }
    return true;
}

void A2dpStateStreaming::SetStateName(std::string state)
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    Transition(state);
}

void A2dpStateStreaming::ProcessSuspendInd(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    uint8_t gavdpRole = A2DP_ROLE_ACP;
    A2dpAvdtp avdtp(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateStreaming]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    /// check the stream status
    if (avdtp.SuspendRsp(msgData.stream.handle, msgData.stream.label, 0, 0) == AVDT_SUCCESS) {
        SetStateName(A2DP_PROFILE_OPEN);
        profile->AudioStateChangedNotify(msgData.stream.addr, A2DP_NOT_PLAYING, (void *)&gavdpRole);
    }
}

void A2dpStateStreaming::ProcessSuspendCfm(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    uint8_t gavdpRole = A2DP_ROLE_INT;
    A2dpAvdtp avdtp(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateStreaming]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(RequestStatus::CONNECT_ON,
            PROFILE_NAME_A2DP_SRC,
            bluetooth::RawAddress::ConvertToString(msgData.stream.addr.addr));
    }
    SetStateName(A2DP_PROFILE_OPEN);
    profile->AudioStateChangedNotify(msgData.stream.addr, A2DP_NOT_PLAYING, (void *)&gavdpRole);
    A2dpService *service = GetServiceInstance(role);
    if (service != nullptr) {
        std::string addr = service->GetActiveSinkDevice().GetAddress();
        if (strcmp(addr.c_str(), RawAddress::ConvertToString(msgData.stream.addr.addr).GetAddress().c_str()) != 0) {
            service->ActiveDevice();
        }
    }
    if (profile->GetDisalbeTag()) {
        profile->CloseAll();
    }
    if (profile->FindPeerByAddress(msgData.stream.addr)->GetRestart()) {
        avdtp.ReconfigureReq(
            msgData.stream.handle, profile->FindPeerByAddress(msgData.stream.addr)->GetReconfig(), label_);
    }
}

void A2dpStateStreaming::ProcessWriteCfm(A2dpAvdtMsgData msgData, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);
    A2dpProfile *profile = GetProfileInstance(A2DP_ROLE_SOURCE);
    profile->DequeuePacket();
}

void A2dpStateStreaming::ProcessDisconnectReq(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);

    A2dpAvdtp avdtp(role);

    SetStateName(A2DP_PROFILE_CLOSING);
    avdtp.DisconnectReq(addr);
    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::IDLE, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }
}

void A2dpStateStreaming::ProcessSuspendReq(uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s role(%u)\n", __func__, role);

    A2dpAvdtp avdtp(role);
    avdtp.SuspendReq(handle, label_);
}

void A2dpStateStreaming::ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_ACP, false, handle};
    A2dpProfile *profile = GetProfileInstance(role);
    uint8_t gavdpRole = A2DP_ROLE_ACP;
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateStreaming]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }
    SetStateName(A2DP_PROFILE_IDLE);
    profile->AudioStateChangedNotify(addr, A2DP_NOT_PLAYING, (void *)&gavdpRole);
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateStreaming::ProcessCloseCfm(BtAddr addr, uint8_t role)
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateOpen]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::IDLE, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }

    SetStateName(A2DP_PROFILE_IDLE);
    if (profile->FindPeerByAddress(addr)->GetRestart()) {
        profile->FindPeerByAddress(addr)->UpdateConfigure();
    }
}

void A2dpStateStreaming::ProcessDelayReportInd(A2dpAvdtMsgData msgData, uint8_t role) const
{
    LOG_INFO("[A2dpStateStreaming]%{public}s\n", __func__);
    if (role == A2DP_ROLE_SINK) {
        LOG_INFO("[A2dpStateStreaming]%{public}s sink role does not handle delay report ind return.", __func__);
        return;
    }

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateStreaming]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    BtAddr addr =  msgData.delayReportInfo.addr;
    uint16_t value = msgData.delayReportInfo.delayValue;
    profile->DelayReportNotify(addr, value);
}

bool A2dpStateClosing::Dispatch(const utility::Message &msg)
{
    LOG_INFO("[A2dpStateClosing]%{public}s\n", __func__);

    if (msg.arg2_ == nullptr) {
        LOG_ERROR("[A2dpStateClosing]%{public}s input error parameter\n", __func__);
        return false;
    }

    A2dpAvdtMsgData msgData = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->a2dpMsg;
    uint8_t role = (static_cast<A2dpAvdtMsg*>(msg.arg2_))->role;

    switch (msg.what_) {
        case EVT_DISCONNECT_IND:
            ProcessDisconnectInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_CLOSE_TRANS_IND:
            ProcessCloseStreamInd(msgData.stream.addr, msgData.stream.handle, role);
            break;
        case EVT_DISCONNECT_CFM:
            ProcessDisconnectCfm(msgData.stream.addr, msgData.stream.handle, role);
            break;
        default:
            break;
    }
    return true;
}

void A2dpStateClosing::SetStateName(std::string state)
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    Transition(state);
}

void A2dpStateClosing::ProcessCloseStreamInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateClosing]%{public}s\n", __func__);
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateClosing]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    SetStateName(A2DP_PROFILE_IDLE);
    A2dpProfilePeer *peer = profile->FindOrCreatePeer(addr, role);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpStateClosing]%{public}s Failed to get profile Peer\n", __func__);
        return;
    }
    peer->SetCurrentCmd(EVT_DISCONNECT_IND);
    peer->SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
}

void A2dpStateClosing::ProcessDisconnectInd(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateClosing]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_INT, false, handle};
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateClosing]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }

    SetStateName(A2DP_PROFILE_IDLE);
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

void A2dpStateClosing::ProcessDisconnectCfm(BtAddr addr, uint16_t handle, uint8_t role)
{
    LOG_INFO("[A2dpStateClosing]%{public}s\n", __func__);

    CallbackParameter param = {A2DP_ROLE_INT, true, handle};
    uint8_t gavdpRole = A2DP_ROLE_INT;
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpStateClosing]%{public}s Failed to get profile instance\n", __func__);
        return;
    }

    if (role == A2DP_ROLE_SOURCE) {
        IPowerManager::GetInstance().StatusUpdate(
            RequestStatus::CONNECT_OFF, PROFILE_NAME_A2DP_SRC, bluetooth::RawAddress::ConvertToString(addr.addr));
    }

    SetStateName(A2DP_PROFILE_IDLE);
    profile->AudioStateChangedNotify(addr, A2DP_NOT_PLAYING, (void *)&gavdpRole);
    profile->ConnectStateChangedNotify(addr, STREAM_DISCONNECT, (void *)&param);
}

const std::string A2dpStateMachine::GetStateName(void) const
{
    std::lock_guard<std::recursive_mutex> lock(g_stateMutex);
    return GetState()->Name();
}
}  // namespace bluetooth
}  // namespace OHOS
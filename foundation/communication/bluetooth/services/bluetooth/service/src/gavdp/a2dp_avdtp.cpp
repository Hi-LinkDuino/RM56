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

#include "a2dp_avdtp.h"
#include <cstring>
#include "a2dp_service.h"
#include "a2dp_sink.h"
#include "a2dp_source.h"
#include "log.h"
#include "raw_address.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
const int MEDIA_CATEGORY = 7;
const int CATEGORY_NOT_SUPPORTED = 0x29;
const int CATEGORY_NO_MEANING = 0x00;
A2dpAvdtp::A2dpAvdtp(const uint8_t role)
{
    peerRole_ = role;
}

uint16_t A2dpAvdtp::RegisterAvdtp(const AvdtRegisterParam &rcb)
{
    const_cast<AvdtRegisterParam &>(rcb).ctrlCallback = ProcAvdtpEvent;
    AVDT_Register(&const_cast<AvdtRegisterParam &>(rcb));

    return A2DP_SUCCESS;
}

uint16_t A2dpAvdtp::CreateStream(const BtAddr &addr, const uint8_t codecIndex, uint16_t &handle)
{
    uint16_t ret = AVDT_CreateStream(&const_cast<BtAddr &>(addr), &handle, codecIndex);
    return ret;
}

uint16_t A2dpAvdtp::RegisterLocalSEP(AvdtStreamConfig cfg[6], const uint8_t number) const
{
    LOG_INFO("[A2dpAvdtp] %{public}s number(%u)\n", __func__, number);

    uint16_t ret = AVDT_RegisterLocalSEP(cfg, const_cast<uint8_t &>(number));
    return ret;
}

uint16_t A2dpAvdtp::ConnectReq(const BtAddr &addr) const
{
    uint16_t ret = AVDT_ConnectReq(&const_cast<BtAddr &>(addr), peerRole_);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_ConnectReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to call AVDT_ConnectReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::DisconnectReq(const BtAddr &addr)
{
    uint16_t ret = AVDT_DisconnectReq(&const_cast<BtAddr &>(addr));
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_DisconnectReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to AVDT_DisconnectReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::DiscoverReq(const BtAddr &addr, const uint8_t maxNum, uint8_t &label)
{
    uint16_t ret = AVDT_DiscoverReq(&const_cast<BtAddr &>(addr), maxNum, &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_DiscoverReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to AVDT_DiscoverReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::GetCapabilityReq(const BtAddr &addr, const uint8_t seid, uint8_t &label)
{
    LOG_INFO("[A2dpAvdtp] %{public}s  acpseid(%u)\n", __func__, seid);

    uint16_t ret = AVDT_GetCapReq(&const_cast<BtAddr &>(addr), seid, &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_GetCapReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to GetCapabilityReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::GetAllCapabilityReq(const BtAddr &addr, const uint8_t seid, uint8_t &label)
{
    LOG_INFO("[A2dpAvdtp] %{public}s  acpseid(%u)\n", __func__, seid);

    uint16_t ret = AVDT_GetAllCapReq(&const_cast<BtAddr &>(addr), seid, &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_GetAllCapReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to GetAllCapabilityReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::SetConfigureReq(
    const BtAddr &addr, const uint16_t handle, const uint8_t seid, const AvdtSepConfig &cfg, uint8_t &label)
{
    LOG_INFO("[A2dpAvdtp] %{public}s\n", __func__);
    for (int i = 0; i < A2DP_CODEC_SIZE; i++) {
        LOG_INFO("[A2dpAvdtp]%{public}s Data[0x:%x]\n", __func__, cfg.codecInfo[i]);
    }
    uint16_t ret = AVDT_SetConfigReq(handle, seid, &const_cast<AvdtSepConfig &>(cfg), &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_SetConfigReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to SetConfigureReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::SetConfigureRsp(const uint16_t handle, const uint8_t label, const AvdtCatetory category)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle[%u] label[%u]\n", __func__, handle, label);

    uint16_t ret = AVDT_SetConfigRsp(handle, label, category);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_SetConfigRsp() \n", __func__);
    } else {
        LOG_INFO("[A2dpAvdtp] %{public}s Failed to AVDT_SetConfigRsp() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::ReconfigureRsp(const uint16_t handle, const uint8_t label, const AvdtCatetory category)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle[%u] label[%u]\n", __func__, handle, label);

    uint16_t ret = AVDT_ReconfigRsp(handle, label, category);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_ReconfigRsp() \n", __func__);
    } else {
        LOG_INFO("[A2dpAvdtp] %{public}s Failed to AVDT_ReconfigRsp() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::WriteStream(
    uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t payloadType, uint16_t marker)
{
    uint16_t ret = AVDT_WriteReq(handle, pkt, timeStamp, payloadType, marker);
    return ret;
}

uint16_t A2dpAvdtp::OpenReq(const uint16_t handle, uint8_t &label)
{
    uint16_t ret = AVDT_OpenReq(handle, &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_OpenReq() \n", __func__);
    } else {
        LOG_INFO("[A2dpAvdtp] %{public}s Failed to AVDT_OpenReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::DelayReq(const uint16_t handle, uint8_t &label, uint16_t delayValue)
{
    uint16_t ret = AVDT_DelayReq(handle, &label, delayValue);
    return ret;
}

uint16_t A2dpAvdtp::CloseReq(const uint16_t handle)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle(%u)\n", __func__, handle);

    uint16_t ret = AVDT_CloseReq(handle);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_CloseReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to CloseReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::CloseRsp(const uint16_t handle, const uint8_t label, const uint8_t errCode)
{
    uint16_t ret = AVDT_CloseRsp(handle, label, errCode);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_CloseRsp() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to CloseRsp() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::AbortReq(const uint16_t handle, uint8_t &label)
{
    uint16_t ret = AVDT_AbortReq(handle, &label);
    return ret;
}

uint16_t A2dpAvdtp::SuspendReq(const uint16_t handle, uint8_t &label)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle(%u)\n", __func__, handle);

    uint16_t handles[] = {handle};
    uint16_t ret = AVDT_SuspendReq(handles, A2DP_STREAM_HANDLES_NUMBER, &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_SuspendReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to AVDT_SuspendReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::StartReq(const uint16_t handle, uint8_t &label)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle[%u]\n", __func__, handle);

    uint16_t handles[] = {handle};
    uint16_t ret = AVDT_StartReq(handles, A2DP_STREAM_HANDLES_NUMBER, &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to call AVDT_StartReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to AVDT_StartReq() \n", __func__);
    }

    return ret;
}

uint16_t A2dpAvdtp::SuspendRsp(
    const uint16_t handle, const uint8_t label, const uint16_t errHandle, const uint8_t errCode)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle[%u]\n", __func__, handle);

    uint16_t ret = AVDT_SuspendRsp(handle, label, errHandle, errCode);
    return ret;
}

uint16_t A2dpAvdtp::StartRsp(
    const uint16_t handle, const uint8_t label, const uint16_t errHandle, const uint8_t errCode)
{
    LOG_INFO("[A2dpAvdtp] %{public}s handle[%u]\n", __func__, handle);

    uint16_t ret = AVDT_StartRsp(handle, label, errHandle, errCode);
    return ret;
}

uint16_t A2dpAvdtp::ReconfigureReq(const uint16_t handle, const AvdtSepConfig &cfg, uint8_t &label)
{
    uint16_t ret = AVDT_ReconfigReq(handle, &const_cast<AvdtSepConfig &>(cfg), &label);
    if (ret == A2DP_SUCCESS) {
        LOG_DEBUG("[A2dpAvdtp] %{public}s Success to AVDT_ReconfigReq() \n", __func__);
    } else {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to AVDT_ReconfigReq() \n", __func__);
    }

    return ret;
}

void A2dpAvdtp::ProcAvdtpEvent(
    uint16_t handle, const BtAddr *bdAddr, uint8_t event, const AvdtCtrlData *data, uint8_t role)
{
    LOG_INFO("[A2dpAvdtp]%{public}s role[%u], event[%u] handle[%u] ", __func__, role, event, handle);

    std::unique_ptr<A2dpAvdtMsg> msg = std::make_unique<A2dpAvdtMsg>();
    msg->handle = handle;
    msg->event = event;
    msg->role = role;
    (void)memcpy_s(&msg->a2dpMsg.msg, sizeof(AvdtCtrlData), data, sizeof(AvdtCtrlData));

    utility::Message msgData(A2DP_AVDTP_EVT, (int)A2DP_SUCCESS, msg.release());
    SwitchThreadToA2dpService(role, *bdAddr, msgData);
}

void A2dpAvdtp::SwitchThreadToA2dpService(uint8_t role, BtAddr bdAddr, utility::Message msgData)
{
    A2dpService *service = GetServiceInstance(role);
    RawAddress peerAddr = RawAddress::ConvertToString(bdAddr.addr);

    if (service != nullptr) {
        service->PostEvent(msgData, peerAddr);
    }
}

uint8_t A2dpAvdtp::ParseAvdtpDisconnectInd(const uint16_t handle,
    const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    peer->StopSignalingTimer();

    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;

    return EVT_DISCONNECT_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpConfigureInd(
    const uint16_t handle, const BtAddr bdAddr, const AvdtCtrlData &data, const uint8_t role, A2dpAvdtMsg &msg)
{
    LOG_INFO("[A2dpAvdtp] %{public}s role(%u)\n", __func__, role);

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    if (peer->GetInitSide()) {
        LOG_INFO("[A2dpAvdtp] %{public}s it's initSide, reject the AVDT_CONFIG_IND_EVT \n", __func__);
        AvdtCatetory category = {.errCode = AVDT_ERR_BAD_STATE, .category = CATEGORY_NO_MEANING};
        SetConfigureRsp(handle, data.configInd.hdr.label, category);
        return EVT_CONNECT_IND;
    }

    msg.a2dpMsg.configRsp.addr = bdAddr;
    msg.a2dpMsg.configRsp.handle = static_cast<uint8_t>(handle);
    msg.a2dpMsg.configRsp.label = data.configInd.hdr.label;
    (void)memcpy_s(msg.a2dpMsg.configRsp.codecInfo, A2DP_CODEC_SIZE, data.configInd.cfg.codecInfo, A2DP_CODEC_SIZE);
    msg.a2dpMsg.configRsp.role = role;

    peer->SetCurrentCmd(EVT_SETCONFIG_IND);
    peer->SetStreamHandle(handle);
    peer->UpdatePeerMtu(data.configInd.hdr.mtu);
    if (role == A2DP_ROLE_SOURCE) {
        peer->SetInitSide(false);
        peer->SetIntSeid(handle);
        peer->SetAcpSeid(data.configInd.acpSeid);
        peer->SetPeerCapInfo(handle, peer->GetAcpSeid(), data.configInd.cfg, role);
    }

    return EVT_SETCONFIG_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpReconfigureInd(
    const uint16_t handle, const BtAddr bdAddr, const AvdtCtrlData &data, const uint8_t role, A2dpAvdtMsg &msg)
{
    bool ret = false;
    uint8_t resultConfig[A2DP_CODEC_SIZE];
    (void)memset_s(resultConfig, A2DP_CODEC_SIZE, 0, A2DP_CODEC_SIZE);
    uint8_t *pResultConfig = &resultConfig[0];
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.msg = data;
    msg.a2dpMsg.configRsp.addr = bdAddr;
    msg.a2dpMsg.configRsp.handle = static_cast<uint8_t>(handle);
    msg.a2dpMsg.configRsp.label = data.reconfigInd.hdr.label;
    uint8_t *pCodecInfo = const_cast<uint8_t *>(data.reconfigInd.cfg.codecInfo);

    if (role == AVDT_ROLE_SNK) {
        if (peer->GetCodecConfigure()->SetPeerSourceCodecCapabilities(pCodecInfo)) {
            ret = peer->GetCodecConfigure()->SetSinkCodecConfig(pCodecInfo, pResultConfig);
        } else {
            LOG_ERROR("[A2dpAvdtp] %{public}s Failed to SetPeerSourceCodecCapabilities()\n", __func__);
        }
    } else {
        if (peer->GetCodecConfigure()->SetPeerSinkCodecCapabilities(pCodecInfo)) {
            ret = peer->GetCodecConfigure()->SetCodecConfig(pCodecInfo, pResultConfig);
        } else {
            LOG_ERROR("[A2dpAvdtp] %{public}s Failed to SetPeerSinkCodecCapabilities()\n", __func__);
        }
    }
    LOG_INFO("[A2dpAvdtp] %{public}s SetCodecConfig[%{public}d]\n", __func__, ret);

    if (ret) {
        msg.a2dpMsg.configRsp.category.errCode = AVDT_SUCCESS;
    } else {
        msg.a2dpMsg.configRsp.category.category = MEDIA_CATEGORY;  // Media Codec 7
        msg.a2dpMsg.configRsp.category.errCode = CATEGORY_NOT_SUPPORTED;    // not supported
    }
    peer->SetCurrentCmd(EVT_RECONFIG_IND);

    return EVT_RECONFIG_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpOpenInd(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    peer->SetStreamHandle(handle);
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;

    return EVT_OPEN_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpCloseInd(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *profilePeer = profile->FindPeerByAddress(bdAddr);
    if (profilePeer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;
    msg.a2dpMsg.stream.label = data.closeInd.label;

    profilePeer->SetStreamHandle(handle);
    profilePeer->StopSignalingTimer();
    profilePeer->SetCurrentCmd(EVT_CLOSE_IND);
    profilePeer->SetSignalingTimer(A2DP_ACCEPT_CLOSING_SIGNALLING_TIMEOUT_MS, false);

    return EVT_CLOSE_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpCloseChannelInd(
    const BtAddr bdAddr, const uint16_t handle, const uint8_t role, A2dpAvdtMsg &msg)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.stream.addr = bdAddr;

    return EVT_CLOSE_TRANS_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpStartInd(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;
    msg.a2dpMsg.stream.label = data.startInd.label;

    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    peer->UpdatePeerEdr((uint8_t)data.startInd.mtu);
    return EVT_START_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpSuspendInd(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    LOG_INFO("[A2dpAvdtp] %{public}s role(%u)\n", __func__, role);

    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;
    msg.a2dpMsg.stream.label = data.suspendInd.label;

    return EVT_SUSPEND_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpDelayReportInd(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    LOG_INFO("[A2dpAvdtp] %{public}s role(%{public}u), delay(%{public}d)\n", __func__, role, data.delayRptInd.delay);

    msg.a2dpMsg.delayReportInfo.addr = bdAddr;
    msg.a2dpMsg.delayReportInfo.handle = handle;
    msg.a2dpMsg.delayReportInfo.delayValue = data.delayRptInd.delay;

    return EVT_DELAY_IND;
}

uint8_t A2dpAvdtp::ParseAvdtpConnectCFM(
    const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    LOG_INFO("[A2dpAvdtp] %{public}s errcode(%u)\n", __func__, data.connectCfm.errCode);

    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindOrCreatePeer(bdAddr, role);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    if (data.connectCfm.errCode) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Peer reject the connect req!!\n", __func__);
        peer->SetInitSide(false);
        return EVT_DISCONNECT_CFM;
    }
    peer->SetCurrentCmd(EVT_DISCOVER_REQ);
    peer->SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
    peer->UpdatePeerMtu(data.connectCfm.mtu);

    msg.a2dpMsg.connectInfo.addr = bdAddr;
    msg.a2dpMsg.connectInfo.errCode = data.connectCfm.errCode;

    return EVT_DISCOVER_REQ;
}

uint8_t A2dpAvdtp::ParseAvdtpDisconnectCFM(const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.stream.addr = bdAddr;

    return EVT_DISCONNECT_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpDiscoverCFM(
    const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    uint8_t cmd = EVT_CONNECT_IND;
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return cmd;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return cmd;
    }

    peer->StopSignalingTimer();
    peer->SetPeerSepInfo(data.discoverCfm);
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.acpSeid = peer->GetPeerSEPInformation().seid;
    peer->UpdateSepIndex();
    peer->SetAcpSeid(msg.a2dpMsg.stream.acpSeid);

    if (peer->GetAvdtpVersion()) {
        cmd = EVT_GET_ALLCAP_REQ;
        peer->SetCurrentCmd(EVT_GET_ALLCAP_REQ);
    } else {
        cmd = EVT_GET_CAP_REQ;
        peer->SetCurrentCmd(EVT_GET_CAP_REQ);
    }
    peer->SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);

    return cmd;
}

uint8_t A2dpAvdtp::ParseAvdtpGetCapabilityCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    uint8_t evt = EVT_GET_CAP_REQ;
    A2dpProfile *profile = GetProfileInstance(role);
    bool capabilityComplete = false;
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    peer->SetPeerCapInfo(handle, peer->GetAcpSeid(), data.getCapCfm.cfg, role);

    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.acpSeid = peer->GetPeerSEPInformation().seid;
    LOG_INFO("[A2dpAvdtp] %{public}s Get acp seid(%u) \n", __func__, msg.a2dpMsg.stream.acpSeid);
    if (peer->GetPeerCapComplete(role)) {
        capabilityComplete = true;
    }
    if (msg.a2dpMsg.stream.acpSeid != 0 && !capabilityComplete) {
        peer->UpdateSepIndex();
        peer->SetAcpSeid(msg.a2dpMsg.stream.acpSeid);
        peer->SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
    } else {
        if (peer->JudgeCapabilityMatched(role) && peer->GetInitSide()) {
            peer->SetConfigure();
            evt = EVT_SETCONFIG_REQ;
            (void)memcpy_s(&msg.a2dpMsg.configStream, sizeof(ConfigureStream),
                (&peer->GetConfigure()), sizeof(ConfigureStream));
            peer->SetCurrentCmd(EVT_SETCONFIG_REQ);
            peer->SetSignalingTimer(A2DP_ACCEPT_SIGNALLING_TIMEOUT_MS, false);
        } else if (!peer->GetInitSide()) {
            evt = EVT_CONNECT_IND;
        }
    }
    return evt;
}

uint8_t A2dpAvdtp::ParseAvdtpGetALLCapCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    uint8_t evt = EVT_CONNECT_IND;
    A2dpProfile *profile = GetProfileInstance(role);
    bool capabilityComplete = false;
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    peer->SetPeerCapInfo(handle, peer->GetAcpSeid(), data.getCapCfm.cfg, role);

    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.acpSeid = peer->GetPeerSEPInformation().seid;

    if (peer->GetPeerCapComplete(role)) {
        capabilityComplete = true;
    }

    if (msg.a2dpMsg.stream.acpSeid != 0 && !capabilityComplete) {
        peer->UpdateSepIndex();
        peer->SetAcpSeid(msg.a2dpMsg.stream.acpSeid);
        evt = EVT_GET_ALLCAP_REQ;
    } else {
        if (peer->JudgeCapabilityMatched(role) && peer->GetInitSide()) {
            peer->SetConfigure();
            evt = EVT_SETCONFIG_REQ;
            (void)memcpy_s(&msg.a2dpMsg.configStream, sizeof(ConfigureStream), (&peer->GetConfigure()),
                sizeof(ConfigureStream));
            peer->SetCurrentCmd(EVT_SETCONFIG_REQ);
        } else if (!peer->GetInitSide()) {
            evt = EVT_CONNECT_IND;
        }
    }

    return evt;
}

uint8_t A2dpAvdtp::ParseAvdtpConfigureCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    if (role == A2DP_ROLE_SINK) {
        LOG_INFO("[A2dpAvdtp] %{public}s role(%{public}u)  to send delay reporting\n", __func__, role);
        uint8_t label = 0;
        DelayReq(handle, label, 0);
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.stream.handle = handle;
    msg.a2dpMsg.stream.addr = bdAddr;
    peer->SetIntSeid(static_cast<uint8_t>(handle));
    peer->SetStreamHandle(handle);
    peer->SetCurrentCmd(EVT_OPEN_REQ);
    peer->SetSignalingTimer(A2DP_SIGNALLING_TIMEOUT_MS, false);

    return EVT_OPEN_REQ;
}

uint8_t A2dpAvdtp::ParseAvdtpGetConfigureCFM(const uint16_t handle, const BtAddr bdAddr,
    const uint8_t role, const A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();

    return EVT_GETCONFIG_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpReconfigureCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    if (data.reconfigCfm.hdr.errCode) {
        peer->SetRestart(false);
        if (peer->GetReconfigTag()) {
            peer->SetReconfigTag(false);
            return EVT_CONNECT_IND;
        }
        peer->SetReconfigTag(true);
        A2dpService *service = GetServiceInstance(role);
        if (service != nullptr) {
            A2dpSrcCodecStatus codecStatus = service->GetCodecStatus(RawAddress::ConvertToString(bdAddr.addr));
            profile->SetUserCodecConfigure(bdAddr, codecStatus.codecInfo);
        }
        LOG_ERROR("[A2dpAvdtp] %{public}s errCode(0x%x)\n", __func__, data.reconfigCfm.hdr.errCode);
        return EVT_CONNECT_IND;
    }
    if (peer->GetReconfigTag()) {
        peer->SetReconfigTag(false);
    }
    peer->SetIntSeid(static_cast<uint8_t>(handle));
    peer->SetStreamHandle(handle);
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;

    return EVT_RECONFIG_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpOpenCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    peer->UpdatePeerMtu(data.connectCfm.mtu);
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;
    msg.a2dpMsg.stream.errCode = data.connectCfm.errCode;

    return EVT_OPEN_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpStartCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;
    peer->UpdatePeerEdr(static_cast<uint8_t>(data.startCfm.mtu));
    return EVT_START_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpSuspendCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;

    return EVT_SUSPEND_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpStreamCloseCFM(
    const uint16_t handle, const BtAddr bdAddr, const uint8_t role, A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();
    msg.a2dpMsg.stream.addr = bdAddr;
    msg.a2dpMsg.stream.handle = handle;

    return EVT_CLOSE_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpAbortCFM(const uint16_t handle, const BtAddr bdAddr,
    const uint8_t role, const A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }

    peer->StopSignalingTimer();

    return EVT_ABORT_CFM;
}

uint8_t A2dpAvdtp::ParseAvdtpWriteCFM(const uint16_t handle, const BtAddr bdAddr,
    const uint8_t role, const A2dpAvdtMsg &msg, const AvdtCtrlData &data)
{
    LOG_INFO("[A2dpAvdtp] %{public}s role(%u)\n", __func__, role);
    A2dpProfile *profile = GetProfileInstance(role);
    if (profile == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get profile instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    A2dpProfilePeer *peer = profile->FindPeerByAddress(bdAddr);
    if (peer == nullptr) {
        LOG_ERROR("[A2dpAvdtp] %{public}s Failed to get peer instance \n", __func__);
        return EVT_CONNECT_IND;
    }
    return EVT_WRITE_CFM;
}

A2dpAvdtMsg *A2dpAvdtp::ParseAvdtpCallbackContent(const uint16_t handle, const BtAddr bdAddr,
    const uint8_t event, const AvdtCtrlData &data, const uint8_t role) const
{
    LOG_INFO("[A2dpAvdtp] %{public}s event(%u) role(%u)\n", __func__, event, role);
    std::unique_ptr<A2dpAvdtMsg> msg = std::make_unique<A2dpAvdtMsg>();
    uint8_t avdtEvent = EVT_CONNECT_IND;

    switch (event) {
        case AVDT_CONNECT_IND_EVT:
            break;
        case AVDT_DISCONNECT_IND_EVT:
            avdtEvent = ParseAvdtpDisconnectInd(handle, bdAddr, role, *msg.get());
            break;
        case AVDT_CONFIG_IND_EVT:
            avdtEvent = ParseAvdtpConfigureInd(handle, bdAddr, data, role, *msg.get());
            break;
        case AVDT_OPEN_IND_EVT:
            avdtEvent = ParseAvdtpOpenInd(handle, bdAddr, role, *msg.get());
            break;
        case AVDT_START_IND_EVT:
            avdtEvent = ParseAvdtpStartInd(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_SUSPEND_IND_EVT:
            avdtEvent = ParseAvdtpSuspendInd(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_RECONFIG_IND_EVT:
            avdtEvent = ParseAvdtpReconfigureInd(handle, bdAddr, data, role, *msg.get());
            break;
        case AVDT_CLOSE_IND_EVT:
            avdtEvent = ParseAvdtpCloseInd(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_CLOSE_TRANS_IND_EVT:
            avdtEvent = ParseAvdtpCloseChannelInd(bdAddr, handle, role, *msg.get());
            break;
        case AVDT_DELAY_REPORT_IND_EVT:
            avdtEvent = ParseAvdtpDelayReportInd(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_CONNECT_CFM_EVT:
            avdtEvent = ParseAvdtpConnectCFM(bdAddr, role, *msg.get(), data);
            break;
        case AVDT_DISCONNECT_CFM_EVT:
            avdtEvent = ParseAvdtpDisconnectCFM(bdAddr, role, *msg.get());
            break;
        case AVDT_DISCOVER_CFM_EVT:
            avdtEvent = ParseAvdtpDiscoverCFM(bdAddr, role, *msg.get(), data);
            break;
        case AVDT_GETCAP_CFM_EVT:
            avdtEvent = ParseAvdtpGetCapabilityCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_GET_ALLCAP_CFM_EVT:
            avdtEvent = ParseAvdtpGetALLCapCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_CONFIG_CFM_EVT:
            avdtEvent = ParseAvdtpConfigureCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_OPEN_CFM_EVT:
            avdtEvent = ParseAvdtpOpenCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_START_CFM_EVT:
            avdtEvent = ParseAvdtpStartCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_SUSPEND_CFM_EVT:
            avdtEvent = ParseAvdtpSuspendCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_CLOSE_CFM_EVT:
            avdtEvent = ParseAvdtpStreamCloseCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_GETCONFIG_CFM_EVT:
            avdtEvent = ParseAvdtpGetConfigureCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_RECONFIG_CFM_EVT:
            avdtEvent = ParseAvdtpReconfigureCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_ABORT_CFM_EVT:
            avdtEvent = ParseAvdtpAbortCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        case AVDT_WRITE_CFM_EVT:
            avdtEvent = ParseAvdtpWriteCFM(handle, bdAddr, role, *msg.get(), data);
            break;
        default:
            break;
    }
    msg->event = avdtEvent;
    if (avdtEvent == EVT_CONNECT_IND) {
        return nullptr;
    } else {
        return msg.release();
    }
}
}  // namespace bluetooth
}  // namespace OHOS

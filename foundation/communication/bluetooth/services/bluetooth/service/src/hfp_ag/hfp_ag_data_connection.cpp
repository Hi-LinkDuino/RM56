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

#include "hfp_ag_data_connection.h"

#include "btm.h"
#include "hfp_ag_profile_event_sender.h"
#include "log_util.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
uint32_t HfpAgDataConnection::g_localFeatures {HFP_AG_FEATURES_DEFAULT};

void HfpAgDataConnection::DataConnectionCallback(uint16_t handle, uint32_t eventId,
                                                 const void *eventData, void *context)
{
    LOG_INFO("[HFP AG]%{public}s():", __PRETTY_FUNCTION__);
    HfpAgProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpAgDataConnection::ProcessDataConnectionCallback, handle, eventId));
    return;
}

void HfpAgDataConnection::ProcessDataConnectionCallback(uint16_t handle, uint32_t eventId)
{
    auto addr = HfpAgRfcommConnection::GetRemoteAddressByHandle(handle);
    HILOGI("Event from rfcomm device: %{public}s, handle: %{public}hu, eventId: %{public}u",
        GetEncryptAddr(addr).c_str(), handle, eventId);
    int event = HFP_AG_INVALID_EVT;
    switch (eventId) {
        case RFCOMM_CHANNEL_EV_CONNECT_SUCCESS:
            event = HFP_AG_CONNECTED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_CONNECT_FAIL:
            event = HFP_AG_CONNECT_FAILED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECTED:
        case RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS:
            HfpAgRfcommConnection::RemoveConnectionDevice(handle);
            event = HFP_AG_DISCONNECTED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECT_FAIL:
            event = HFP_AG_DISCONNECT_FAILED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_REV_DATA:
            event = HFP_AG_DATA_AVAILABLE_EVT;
            break;
        default:
            break;
    }

    HfpAgProfileEventSender::GetInstance().UpdateConnectState(addr, event);
    return;
}

int HfpAgDataConnection::Init()
{
    g_localFeatures = HFP_AG_FEATURES_DEFAULT;
    g_localSupportCodecs = HFP_AG_CODEC_NONE;
    HfpAgRfcommConnection::Init();

    BtmLocalSupportedCodecs *pCodecs = nullptr;
    int ret = BTM_GetLocalSupportedCodecs(&pCodecs);
    HFP_AG_RETURN_IF_FAIL(ret);

    if (pCodecs != nullptr) {
        for (size_t i = 0; i < pCodecs->numberOfSupportedCodecs; i++) {
            if (pCodecs->supportedCodecs[i] == ANUM_MSBC) {
                g_localFeatures |= HFP_AG_FEATURES_CODEC_NEGOTIATION;
                g_localSupportCodecs |= HFP_AG_CODEC_MSBC;
                LOG_INFO("[HFP AG]%{public}s(): Support MSBC", __FUNCTION__);
            } else if (pCodecs->supportedCodecs[i] == ANUM_CVSD) {
                g_localSupportCodecs |= HFP_AG_CODEC_CVSD;
                LOG_INFO("[HFP AG]%{public}s(): Support CVSD", __FUNCTION__);
            } else {
                LOG_INFO("[HFP AG]%{public}s(): Support other codecId[%hhu]",
                    __FUNCTION__, pCodecs->supportedCodecs[i]);
            }
        }
    } else {
        LOG_INFO("[HFP AG]%{public}s(): Get local support codecs failed!", __FUNCTION__);
    }

    if (BTM_IsControllerSupportEsco()) {
        g_localFeatures |= HFP_AG_FEATURES_SUPPORT_ESCO;
        LOG_INFO("[HFP AG]%{public}s(): Support ESCO", __FUNCTION__);
    }
    return ret;
}

void HfpAgDataConnection::CleanUp()
{
    g_localFeatures = HFP_AG_FEATURES_DEFAULT;
    HfpAgRfcommConnection::CleanUp();
}

uint32_t HfpAgDataConnection::GetLocalFeatures()
{
    return g_localFeatures;
}

bool HfpAgDataConnection::IsCodecNegotiationSupport() const
{
    return ((g_localFeatures & HFP_AG_FEATURES_CODEC_NEGOTIATION) &&
            (remoteFeatures_ & HFP_AG_HF_FEATURES_CODEC_NEGOTIATION));
}

bool HfpAgDataConnection::IsEscoSupport()
{
    return BTM_IsControllerSupportEsco();
}

bool HfpAgDataConnection::IsEscoS4Support() const
{
    return ((g_localFeatures & HFP_AG_FEATURES_SUPPORT_ESCO) && (remoteFeatures_ & HFP_AG_HF_FEATURES_ESCO));
}

int HfpAgDataConnection::Connect()
{
    return rfcommConnection_.Connect();
}

int HfpAgDataConnection::Disconnect() const
{
    return rfcommConnection_.Disconnect();
}

int HfpAgDataConnection::ReadData(Packet **pkt) const
{
    return rfcommConnection_.ReadData(pkt);
}

int HfpAgDataConnection::WriteData(Packet &pkt) const
{
    return rfcommConnection_.WriteData(pkt);
}

void HfpAgDataConnection::SetRemoteAddr(const std::string &addr)
{
    remoteAddr_ = addr;
    rfcommConnection_.SetRemoteAddr(addr);
}

void HfpAgDataConnection::SetRole(int role)
{
    role_ = role;
}

int HfpAgDataConnection::GetRole() const
{
    return role_;
}

void HfpAgDataConnection::SetConnectionHandle(uint16_t handle)
{
    rfcommConnection_.SetConnectionHandle(handle);
}

uint16_t HfpAgDataConnection::GetConnectionHandle() const
{
    return rfcommConnection_.GetConnectionHandle();
}

void HfpAgDataConnection::SetSdpInfo(HfpAgRemoteSdpInfo sdpInfo)
{
    sdpInfo_ = sdpInfo;
    rfcommConnection_.SetRemoteScn(sdpInfo.remoteServerChannelNumber);
}

void HfpAgDataConnection::SetSlcConnectState(bool state)
{
    HILOGI("[HFP AG]:state = %{public}d", state);
    slcConnected_ = state;
}

uint8_t HfpAgDataConnection::GetRemoteScn() const
{
    return rfcommConnection_.GetRemoteScn();
}
}  // namespace bluetooth
}  // namespace OHOS
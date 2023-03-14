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

#include "hfp_hf_data_connection.h"

#include "btm.h"
#include "hfp_hf_profile_event_sender.h"
#include "log_util.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
uint32_t HfpHfDataConnection::g_localFeatures {HFP_HF_FEATURES_DEFAULT};

void HfpHfDataConnection::ProcessDataConnectionCallback(uint16_t handle, uint32_t eventId)
{
    auto addr = HfpHfRfcommConnection::GetRemoteAddressByHandle(handle);
    HILOGI("Event from rfcomm device: %{public}s, handle: %{public}hu, eventId: %{public}u",
        GetEncryptAddr(addr).c_str(), handle, eventId);
    int event = HFP_HF_INVALID_EVT;
    switch (eventId) {
        case RFCOMM_CHANNEL_EV_CONNECT_SUCCESS:
            event = HFP_HF_CONNECTED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_CONNECT_FAIL:
            event = HFP_HF_CONNECT_FAILED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECTED:
        case RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS:
            HfpHfRfcommConnection::RemoveConnectionDevice(handle);
            event = HFP_HF_DISCONNECTED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_DISCONNECT_FAIL:
            event = HFP_HF_DISCONNECT_FAILED_EVT;
            break;
        case RFCOMM_CHANNEL_EV_REV_DATA:
            event = HFP_HF_DATA_AVAILABLE_EVT;
            break;
        default:
            break;
    }

    HfpHfProfileEventSender::GetInstance().UpdateConnectState(addr, event);
    return;
}

void HfpHfDataConnection::DataConnectionCallback(uint16_t handle, uint32_t eventId,
                                                 const void *eventData, void *context)
{
    LOG_DEBUG("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);
    HfpHfProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpHfDataConnection::ProcessDataConnectionCallback, handle, eventId));

    return;
}

int HfpHfDataConnection::Init()
{
    g_localSupportCodecs = HFP_HF_CODEC_NONE;
    g_localFeatures = HFP_HF_FEATURES_DEFAULT;
    HfpHfRfcommConnection::Init();

    BtmLocalSupportedCodecs *pCodecs = nullptr;
    int ret = BTM_GetLocalSupportedCodecs(&pCodecs);
    HFP_HF_RETURN_IF_FAIL(ret);

    if (pCodecs != nullptr) {
        for (size_t i = 0; i < pCodecs->numberOfSupportedCodecs; i++) {
            if (pCodecs->supportedCodecs[i] == ANUM_MSBC) {
                g_localFeatures |= HFP_HF_FEATURES_CODEC_NEGOTIATION;
                g_localSupportCodecs |= HFP_HF_CODEC_MSBC;
                LOG_DEBUG("[HFP HF]%{public}s(): Support MSBC", __FUNCTION__);
            } else if (pCodecs->supportedCodecs[i] == ANUM_CVSD) {
                g_localSupportCodecs |= HFP_HF_CODEC_CVSD;
                LOG_DEBUG("[HFP HF]%{public}s(): Support CVSD", __FUNCTION__);
            } else {
                LOG_DEBUG("[HFP HF]%{public}s(): Support other codecId[%hhu]", __FUNCTION__, pCodecs->supportedCodecs[i]);
            }
        }
    } else {
        LOG_DEBUG("[HFP HF]%{public}s(): Get local support codecs failed!", __FUNCTION__);
    }

    if (BTM_IsControllerSupportEsco()) {
        g_localFeatures |= HFP_HF_FEATURES_ESCO;
        LOG_DEBUG("[HFP HF]%{public}s(): Support ESCO", __FUNCTION__);
    }
    return ret;
}

void HfpHfDataConnection::CleanUp()
{
    HfpHfRfcommConnection::CleanUp();
    g_localFeatures = HFP_HF_FEATURES_DEFAULT;
}

uint32_t HfpHfDataConnection::GetLocalFeatures()
{
    return g_localFeatures;
}

int HfpHfDataConnection::Connect()
{
    return rfcommConnection_.Connect();
}

int HfpHfDataConnection::Disconnect() const
{
    return rfcommConnection_.Disconnect();
}

int HfpHfDataConnection::ReadData(Packet **pkt) const
{
    return rfcommConnection_.ReadData(pkt);
}

int HfpHfDataConnection::WriteData(Packet &pkt) const
{
    return rfcommConnection_.WriteData(pkt);
}

void HfpHfDataConnection::SetRemoteAddr(const std::string &addr)
{
    remoteAddr_ = addr;
    rfcommConnection_.SetRemoteAddr(addr);
}

void HfpHfDataConnection::SetRole(int role)
{
    role_ = role;
}

int HfpHfDataConnection::GetRole() const
{
    return role_;
}

void HfpHfDataConnection::SetConnectionHandle(uint16_t handle)
{
    rfcommConnection_.SetConnectionHandle(handle);
}

uint16_t HfpHfDataConnection::GetConnectionHandle() const
{
    return rfcommConnection_.GetConnectionHandle();
}

void HfpHfDataConnection::SetSdpInfo(HfpHfRemoteSdpInfo sdpInfo)
{
    sdpInfo_ = sdpInfo;
    rfcommConnection_.SetRemoteScn(sdpInfo.remoteServerChannelNumber);
}

void HfpHfDataConnection::SetSlcConnectState(bool state)
{
    slcConnected_ = state;
}

bool HfpHfDataConnection::IsCodecNegotiationSupport() const
{
    return ((g_localFeatures & HFP_HF_FEATURES_CODEC_NEGOTIATION) &&
            (remoteFeatures_ & HFP_HF_AG_FEATURES_CODEC_NEGOTIATION));
}

bool HfpHfDataConnection::IsEscoSupport()
{
    return BTM_IsControllerSupportEsco();
}

bool HfpHfDataConnection::IsEscoS4Support() const
{
    return ((g_localFeatures & HFP_HF_FEATURES_ESCO) && (remoteFeatures_ & HFP_HF_AG_FEATURES_SUPPORT_ESCO));
}

uint8_t HfpHfDataConnection::GetRemoteScn() const
{
    return rfcommConnection_.GetRemoteScn();
}
}  // namespace bluetooth
}  // namespace OHOS
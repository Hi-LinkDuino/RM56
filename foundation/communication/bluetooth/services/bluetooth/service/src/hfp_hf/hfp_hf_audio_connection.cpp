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

#include "hfp_hf_audio_connection.h"

#include "btstack.h"
#include "hfp_hf_profile_event_sender.h"
#include "log_util.h"
#include "raw_address.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
std::vector<HfpHfAudioConnection::AudioDevice> HfpHfAudioConnection::g_audioDevices;

BtmScoCallbacks HfpHfAudioConnection::g_cbs = {
    &HfpHfAudioConnection::OnConnectCompleted,
    &HfpHfAudioConnection::OnConnectionChanged,
    &HfpHfAudioConnection::OnDisconnectCompleted,
    &HfpHfAudioConnection::OnConnectRequest,
    &HfpHfAudioConnection::OnWriteVoiceSettingCompleted,
};

void HfpHfAudioConnection::SetRemoteAddr(const std::string &addr)
{
    remoteAddr_ = addr;
}

BtAddr HfpHfAudioConnection::ConvertToBtAddr(const std::string &address)
{
    RawAddress rawAddr(address);
    BtAddr btAddr;
    rawAddr.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    return btAddr;
}

int HfpHfAudioConnection::Register()
{
    LOG_INFO("[HFP HF]%{public}s(): Audio Register start", __FUNCTION__);
    std::vector<HfpHfAudioConnection::AudioDevice>().swap(g_audioDevices);
    int ret = BTM_RegisterScoCallbacks(&g_cbs, nullptr);
    HFP_HF_RETURN_IF_FAIL(ret);
    return ret;
}

int HfpHfAudioConnection::Deregister()
{
    LOG_INFO("[HFP HF]%{public}s(): Audio Deregister start", __FUNCTION__);
    std::vector<HfpHfAudioConnection::AudioDevice>().swap(g_audioDevices);
    int ret = BTM_DeregisterScoCallbacks(&g_cbs);
    HFP_HF_RETURN_IF_FAIL(ret);
    return ret;
}

bool HfpHfAudioConnection::IsAudioConnected()
{
    std::vector<HfpHfAudioConnection::AudioDevice>::iterator it;
    for (it = g_audioDevices.begin(); it != g_audioDevices.end(); ++it) {
        if (it->lastConnectResult == CONNECT_SUCCESS) {
            return true;
        }
    }
    return false;
}

void HfpHfAudioConnection::SetSupportFeatures(bool escoSupport, bool escoS4Support, int inUseCodec)
{
    escoSupport_ = escoSupport;
    escoS4Support_ = escoS4Support;
    inUseCodec_ = inUseCodec;
    LOG_INFO("[HFP HF]%{public}s(): escoSupport_[%{public}d], escoS4Support_[%{public}d], inUseCodec_[%{public}d]",
        __FUNCTION__,
        escoSupport_,
        escoS4Support_,
        inUseCodec_);
}

std::vector<HfpHfAudioConnection::AudioDevice>::iterator HfpHfAudioConnection::GetDeviceByAddr(const std::string &addr)
{
    std::vector<HfpHfAudioConnection::AudioDevice>::iterator it;
    for (it = g_audioDevices.begin(); it != g_audioDevices.end(); ++it) {
        if (it->addr == addr) {
            break;
        }
    }
    return it;
}

std::vector<HfpHfAudioConnection::AudioDevice>::iterator HfpHfAudioConnection::GetDeviceByHandle(uint16_t handle)
{
    std::vector<HfpHfAudioConnection::AudioDevice>::iterator it;
    for (it = g_audioDevices.begin(); it != g_audioDevices.end(); ++it) {
        if (it->handle == handle) {
            break;
        }
    }
    return it;
}

int HfpHfAudioConnection::ConnectByCvsd(AudioDevice &dev, BtAddr btAddr, bool cvsdEscoFailed) const
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_CVSD);
    HFP_HF_RETURN_IF_FAIL(ret);

    if (escoSupport_ && !cvsdEscoFailed) {
        dev.linkType = LINK_TYPE_ESCO;
        if (escoS4Support_) {
            LOG_INFO("[HFP HF]%{public}s():Try connect by CVSD ESCO S4.", __FUNCTION__);
            dev.lastParam = CVSD_ESCO_S4;
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S4_PARAM;
            param.addr = btAddr;
            ret = BTM_CreateEscoConnection(&param);
            HFP_HF_RETURN_IF_FAIL(ret);
        } else {
            LOG_INFO("[HFP HF]%{public}s():Try connect by CVSD ESCO S1.", __FUNCTION__);
            dev.lastParam = CVSD_ESCO_S1;
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S1_PARAM;
            param.addr = btAddr;
            ret = BTM_CreateEscoConnection(&param);
            HFP_HF_RETURN_IF_FAIL(ret);
        }
    } else {
        if (!BTM_IsSecureConnection(&btAddr)) {
            dev.linkType = LINK_TYPE_SCO;
            LOG_INFO("[HFP HF]%{public}s():Try connect by CVSD SCO.", __FUNCTION__);
            dev.lastParam = CVSD_SCO;
            BtmCreateScoConnectionParam param = CVSD_SCO_PARAM;
            param.addr = btAddr;
            ret = BTM_CreateScoConnection(&param);
            HFP_HF_RETURN_IF_FAIL(ret);
        } else {
            HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_HF_AUDIO_CONNECT_FAILED_EVT);
            return ret;
        }
    }

    HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_HF_AUDIO_CONNECTING_EVT);
    return ret;
}

int HfpHfAudioConnection::ConnectAudio() const
{
    LOG_INFO("[HFP HF]%{public}s():Connect SCO to %{public}s", __FUNCTION__, remoteAddr_.c_str());

    auto dev = GetDeviceByAddr(remoteAddr_);
    if (dev == g_audioDevices.end()) {
        HfpHfAudioConnection::AudioDevice audioDev;
        audioDev.role = ROLE_INITIATOR;
        audioDev.addr = remoteAddr_;
        g_audioDevices.push_back(audioDev);
        dev = GetDeviceByAddr(remoteAddr_);
        LOG_INFO("[HFP HF]%{public}s():Create Audio device for [%{public}s]", __FUNCTION__, remoteAddr_.c_str());
    } else {
        LOG_INFO("[HFP HF]%{public}s():Audio device [%{public}s] already in device list ", __FUNCTION__,
            remoteAddr_.c_str());
    }

    bool cvsdEscoFailed = false;
    if (dev->lastConnectResult == CONNECT_FAIL) {
        if (dev->lastParam == CVSD_ESCO_S4 || dev->lastParam == CVSD_ESCO_S1) {
            cvsdEscoFailed = true;
        } else {
            LOG_INFO(
                "[HFP HF]%{public}s():Audio device [%{public}s], lastParam[%{public}d]",
                __FUNCTION__,
                remoteAddr_.c_str(),
                dev->lastParam);
        }
    }

    BtAddr btAddr = ConvertToBtAddr(remoteAddr_);
    return ConnectByCvsd(*dev, btAddr, cvsdEscoFailed);
}

int HfpHfAudioConnection::DisconnectAudio() const
{
    LOG_INFO("[HFP HF]%{public}s():Disconnect SCO from %{public}s", __FUNCTION__, remoteAddr_.c_str());

    int ret;
    auto dev = GetDeviceByAddr(remoteAddr_);
    if (dev != g_audioDevices.end()) {
        ret = BTM_DisconnectScoConnection(dev->handle, REMOTE_USER_TERMINATED_CONNECTION);
        HFP_HF_RETURN_IF_FAIL(ret);
        HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_HF_AUDIO_DISCONNECTING_EVT);
    } else {
        LOG_WARN("[HFP HF]%{public}s():Invalid Address", remoteAddr_.c_str());
        ret = BT_DEVICE_ERROR;
    }
    return ret;
}

int HfpHfAudioConnection::AcceptByMsbc(BtAddr btAddr)
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_TRANS);
    HFP_HF_RETURN_IF_FAIL(ret);

    if (BTM_IsSecureConnection(&btAddr)) {
        LOG_INFO("[HFP HF]%{public}s():Accept by MSBC T2.", __FUNCTION__);
        BtmCreateEscoConnectionParam param = MSBC_T2_PARAM;
        param.addr = btAddr;
        ret = BTM_AcceptEscoConnectionRequest(&param);
        HFP_HF_RETURN_IF_FAIL(ret);
    } else {
        LOG_INFO("[HFP HF]%{public}s():Accept by MSBC T1.", __FUNCTION__);
        BtmCreateEscoConnectionParam param = MSBC_T1_PARAM;
        param.addr = btAddr;
        ret = BTM_AcceptEscoConnectionRequest(&param);
        HFP_HF_RETURN_IF_FAIL(ret);
    }
    return ret;
}

int HfpHfAudioConnection::AcceptByCvsd(AudioDevice &dev, BtAddr btAddr) const
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_CVSD);
    HFP_HF_RETURN_IF_FAIL(ret);

    if (dev.linkType == LINK_TYPE_ESCO && escoSupport_) {
        if (escoS4Support_) {
            LOG_INFO("[HFP HF]%{public}s():Accept by CVSD ESCO S4.", __FUNCTION__);
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S4_PARAM;
            param.addr = btAddr;
            ret = BTM_AcceptEscoConnectionRequest(&param);
            HFP_HF_RETURN_IF_FAIL(ret);
        } else {
            LOG_INFO("[HFP HF]%{public}s():Accept by CVSD ESCO S1.", __FUNCTION__);
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S1_PARAM;
            param.addr = btAddr;
            ret = BTM_AcceptEscoConnectionRequest(&param);
            HFP_HF_RETURN_IF_FAIL(ret);
        }
    } else if (dev.linkType == LINK_TYPE_SCO) {
        LOG_INFO("[HFP HF]%{public}s():Accept by CVSD SCO.", __FUNCTION__);
        BtmCreateScoConnectionParam param = CVSD_SCO_PARAM;
        param.addr = btAddr;
        ret = BTM_AcceptScoConnectionRequest(&param);
        HFP_HF_RETURN_IF_FAIL(ret);
    } else {
        LOG_INFO(
            "[HFP HF]%{public}s():Can not cvsd e(sco) connection, "
            "because linktype[%{public}d] and escoSupport[%{public}d] are not matched!",
            __FUNCTION__,
            dev.linkType,
            escoSupport_);
        return BT_BAD_PARAM;
    }
    return ret;
}

int HfpHfAudioConnection::AcceptAudioConnection() const
{
    int ret = BT_NO_ERROR;
    BtAddr btAddr = ConvertToBtAddr(remoteAddr_);

    // Only accpet CVSD sco from remote device.
    auto dev = GetDeviceByAddr(remoteAddr_);
    if (dev != g_audioDevices.end()) {
        if (inUseCodec_ == HFP_HF_CODEC_MSBC) {
            if (dev->linkType == LINK_TYPE_ESCO && escoSupport_) {
                return AcceptByMsbc(btAddr);
            } else {
                LOG_INFO("[HFP HF]%{public}s():Accpet MSBC ESCO connection failed, "
                    "linktype[%hhu] and escoSupport[%{public}d] are not matched!",
                    __FUNCTION__,
                    dev->linkType,
                    escoSupport_);
                return BT_BAD_PARAM;
            }
        } else if (inUseCodec_ == HFP_HF_CODEC_CVSD) {
            return AcceptByCvsd(*dev, btAddr);
        } else {
            LOG_INFO("[HFP HF]%{public}s():Invalid Codec[%{public}d]!", __FUNCTION__, inUseCodec_);
            return BT_BAD_PARAM;
        }
    } else {
        LOG_WARN("[HFP HF]%{public}s:Invalid Address", remoteAddr_.c_str());
        return BT_BAD_PARAM;
    }

    return ret;
}

int HfpHfAudioConnection::RejectAudioConnection() const
{
    LOG_INFO("[HFP HF]%{public}s():Reject sco connect request from %{public}s", __FUNCTION__, remoteAddr_.c_str());
    BtAddr btAddr = ConvertToBtAddr(remoteAddr_);
    BtmRejectScoConnectionRequestParam param = {btAddr, REJECT_DUE_TO_LIMITED_RESOURCES};

    int ret = BTM_RejectScoConnectionRequest(&param);
    HFP_HF_RETURN_IF_FAIL(ret);

    return ret;
}

void HfpHfAudioConnection::OnConnectRequest(const BtmScoConnectionRequestParam *param, void *context)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);
    HfpScoConnectionRequestParam parameters;
    parameters.linkType = param->linkType;
    (void)memcpy_s(&parameters.addr, sizeof(BtAddr), param->addr, sizeof(BtAddr));
    HfpHfProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpHfAudioConnection::ProcessOnConnectRequest, parameters));
}

void HfpHfAudioConnection::ProcessOnConnectRequest(HfpScoConnectionRequestParam parameters)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);

    RawAddress btAddr = RawAddress::ConvertToString(parameters.addr.addr);
    std::string address = btAddr.GetAddress();
    auto dev = GetDeviceByAddr(address);
    if (dev == g_audioDevices.end()) {
        HfpHfAudioConnection::AudioDevice audioDev;
        audioDev.role = ROLE_ACCEPTOR;
        audioDev.addr = address;
        audioDev.linkType = parameters.linkType;
        g_audioDevices.push_back(audioDev);
        LOG_INFO("[HFP HF]%{public}s():Create Audio device for [%{public}s]", __FUNCTION__,
            address.c_str());
    } else {
        dev->linkType = parameters.linkType;
        LOG_INFO("[HFP HF]%{public}s():Audio device [%{public}s] already in device list ", __FUNCTION__,
            address.c_str());
    }
    HfpHfProfileEventSender::GetInstance().ScoConnectRequest(
        btAddr.GetAddress(), HFP_HF_AUDIO_CONNECT_REQUEST_EVT, parameters.linkType);
}

void HfpHfAudioConnection::OnConnectCompleted(const BtmScoConnectionCompleteParam *param, void *context)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);
    HfpScoConnectionCompleteParam parameters;
    parameters.status = param->status;
    parameters.connectionHandle = param->connectionHandle;
    (void)memcpy_s(&parameters.addr, sizeof(BtAddr), param->addr, sizeof(BtAddr));
    HfpHfProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpHfAudioConnection::ProcessOnConnectCompleted, parameters));
}

void HfpHfAudioConnection::ProcessOnConnectCompleted(HfpScoConnectionCompleteParam parameters)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);

    RawAddress btAddr = RawAddress::ConvertToString(parameters.addr.addr);
    std::string address = btAddr.GetAddress();
    auto dev = GetDeviceByAddr(address);
    if (dev != g_audioDevices.end()) {
        dev->addr = address;
        dev->handle = parameters.connectionHandle;
        if (!parameters.status) {
            LOG_INFO("[HFP HF]%{public}s(): SCO connect successfully!", __FUNCTION__);
            dev->lastConnectResult = CONNECT_SUCCESS;
            HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_HF_AUDIO_CONNECTED_EVT);
        } else {
            ProcessOnConnectCompletedFail(dev, address);
        }
    } else {
        LOG_INFO("[HFP HF]%{public}s:Invalid audio device", address.c_str());
    }
}

void HfpHfAudioConnection::ProcessOnConnectCompletedFail(
    std::vector<HfpHfAudioConnection::AudioDevice>::iterator dev, const std::string &address)
{
    dev->lastConnectResult = CONNECT_FAIL;
    if (dev->role == ROLE_INITIATOR) {
        if (dev->lastParam == CVSD_ESCO_S4 || dev->lastParam == CVSD_ESCO_S1) {
            LOG_INFO("[HFP HF]%{public}s():CVSD ESCO failed, try CVSD SCO", __FUNCTION__);
            HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_HF_RETRY_CONNECT_AUDIO_EVT);
        } else if (dev->lastParam == CVSD_SCO) {
            LOG_INFO("[HFP HF]%{public}s():CVSD SCO failed, report fail event to service", __FUNCTION__);
            HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_HF_AUDIO_CONNECT_FAILED_EVT);
        } else {
            LOG_INFO(
                "[HFP HF]%{public}s():address[%{public}s] lastParam[%{public}d] is invalid",
                __FUNCTION__,
                address.c_str(),
                dev->lastParam);
        }
    } else {
        // As acceptor, report connect failed event to service directly.
        HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_HF_AUDIO_CONNECT_FAILED_EVT);
        LOG_INFO("[HFP HF]%{public}s(): Accept SCO from address[%{public}s]failed", __FUNCTION__, address.c_str());
    }
}

void HfpHfAudioConnection::OnDisconnectCompleted(const BtmScoDisconnectionCompleteParam *param, void *context)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);
    HfpScoDisconnectionCompleteParam parameters;
    parameters.connectionHandle = param->connectionHandle;
    parameters.reason = param->reason;
    parameters.status = param->status;
    HfpHfProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpHfAudioConnection::ProcessOnDisconnectCompleted, parameters));
}

void HfpHfAudioConnection::ProcessOnDisconnectCompleted(HfpScoDisconnectionCompleteParam parameters)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);

    auto it = GetDeviceByHandle(parameters.connectionHandle);
    if (it != g_audioDevices.end()) {
        if (!parameters.status) {
            HILOGI("Disconnect SCO from address: %{public}s successfully.", GetEncryptAddr(it->addr).c_str());
            HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(it->addr, HFP_HF_AUDIO_DISCONNECTED_EVT);
            g_audioDevices.erase(it);
        } else {
            HILOGI("Disconnect SCO from address: %{public}s failed.", GetEncryptAddr(it->addr).c_str());
            HfpHfProfileEventSender::GetInstance().UpdateScoConnectState(
                it->addr, HFP_HF_AUDIO_DISCONNECT_FAILED_EVT);
        }
    } else {
        LOG_DEBUG("[HFP HF]%{public}s():Invalid audio device", __FUNCTION__);
    }
}

void HfpHfAudioConnection::OnConnectionChanged(const BtmScoConnectionChangedParam *param, void *context)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);
    LOG_INFO("[HFP HF]%{public}s():connectionHandle[%hu]", __FUNCTION__, param->connectionHandle);
}

void HfpHfAudioConnection::OnWriteVoiceSettingCompleted(uint8_t status, void *context)
{
    LOG_INFO("[HFP HF]%{public}s():", __PRETTY_FUNCTION__);
    LOG_INFO("[HFP HF]%{public}s(): status[%hhu]", __FUNCTION__, status);
}
}  // namespace bluetooth
}  // namespace OHOS
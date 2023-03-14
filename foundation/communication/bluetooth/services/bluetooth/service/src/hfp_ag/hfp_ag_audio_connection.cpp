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

#include <cstring>
#include "btstack.h"
#include "hfp_ag_profile_event_sender.h"
#include "log_util.h"
#include "raw_address.h"
#include "securec.h"
#include "hfp_ag_audio_connection.h"

namespace OHOS {
namespace bluetooth {
std::string HfpAgAudioConnection::g_activeAddr {NULL_ADDRESS};
std::vector<HfpAgAudioConnection::AudioDevice> HfpAgAudioConnection::g_audioDevices {};

BtmScoCallbacks HfpAgAudioConnection::g_cbs = {
    &HfpAgAudioConnection::OnConnectCompleted,
    &HfpAgAudioConnection::OnConnectionChanged,
    &HfpAgAudioConnection::OnDisconnectCompleted,
    &HfpAgAudioConnection::OnConnectRequest,
    &HfpAgAudioConnection::OnWriteVoiceSettingCompleted,
};

void HfpAgAudioConnection::SetRemoteAddr(const std::string &addr)
{
    remoteAddr_ = addr;
}

void HfpAgAudioConnection::SetActiveDevice(const std::string &address)
{
    g_activeAddr = address;
}

bool HfpAgAudioConnection::IsAudioConnected(const std::string &address)
{
    auto dev = GetDeviceByAddr(address);
    if (dev != g_audioDevices.end()) {
        if (dev->lastConnectResult == CONNECT_SUCCESS) {
            return true;
        }
    }

    return false;
}

std::string HfpAgAudioConnection::GetActiveDevice()
{
    return g_activeAddr;
}

BtAddr HfpAgAudioConnection::ConvertToBtAddr(std::string address)
{
    RawAddress rawAddr(address);
    BtAddr btAddr;
    rawAddr.ConvertToUint8(btAddr.addr);
    btAddr.type = BT_PUBLIC_DEVICE_ADDRESS;
    return btAddr;
}

int HfpAgAudioConnection::Register()
{
    HILOGI("enter");
    g_activeAddr = NULL_ADDRESS;
    std::vector<HfpAgAudioConnection::AudioDevice>().swap(g_audioDevices);
    int ret = BTM_RegisterScoCallbacks(&g_cbs, nullptr);
    HFP_AG_RETURN_IF_FAIL(ret);
    return ret;
}

int HfpAgAudioConnection::Deregister()
{
    HILOGI("enter");
    g_activeAddr = NULL_ADDRESS;
    std::vector<HfpAgAudioConnection::AudioDevice>().swap(g_audioDevices);
    int ret = BTM_DeregisterScoCallbacks(&g_cbs);
    HFP_AG_RETURN_IF_FAIL(ret);
    return ret;
}

void HfpAgAudioConnection::SetSupportFeatures(bool escoSupport, bool escoS4Support, int inUseCodec)
{
    escoSupport_ = escoSupport;
    escoS4Support_ = escoS4Support;
    inUseCodec_ = inUseCodec;
    HILOGI("escoSupport_: %{public}d, escoS4Support_: %{public}d, inUseCodec_: %{public}d",
        escoSupport_,
        escoS4Support_,
        inUseCodec_);
}

std::vector<HfpAgAudioConnection::AudioDevice>::iterator HfpAgAudioConnection::GetDeviceByAddr(const std::string &addr)
{
    std::vector<HfpAgAudioConnection::AudioDevice>::iterator it;
    for (it = g_audioDevices.begin(); it != g_audioDevices.end(); ++it) {
        if (it->addr == addr) {
            break;
        }
    }
    return it;
}

std::vector<HfpAgAudioConnection::AudioDevice>::iterator HfpAgAudioConnection::GetDeviceByHandle(uint16_t handle)
{
    std::vector<HfpAgAudioConnection::AudioDevice>::iterator it;
    for (it = g_audioDevices.begin(); it != g_audioDevices.end(); ++it) {
        if (it->handle == handle) {
            break;
        }
    }
    return it;
}

int HfpAgAudioConnection::ConnectByMsbc(AudioDevice &dev, BtAddr btAddr) const
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_TRANS);
    HFP_AG_RETURN_IF_FAIL(ret);

    dev.linkType = LINK_TYPE_ESCO;

    if (BTM_IsSecureConnection(&btAddr)) {
        HILOGI("Try connect by MSBC T2.");
        BtmCreateEscoConnectionParam param = MSBC_T2_PARAM;
        param.addr = btAddr;
        ret = BTM_CreateEscoConnection(&param);
        HFP_AG_RETURN_IF_FAIL(ret);
        dev.lastParam = MSBC_ESCO_T2;
    } else {
        HILOGI("Try connect by MSBC T1.");
        BtmCreateEscoConnectionParam param = MSBC_T1_PARAM;
        param.addr = btAddr;
        ret = BTM_CreateEscoConnection(&param);
        HFP_AG_RETURN_IF_FAIL(ret);
        dev.lastParam = MSBC_ESCO_T1;
    }
    HFP_AG_RETURN_IF_FAIL(ret);

    HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_AG_AUDIO_CONNECTING_EVT);
    return ret;
}

int HfpAgAudioConnection::ConnectByCvsd(AudioDevice &dev, BtAddr btAddr, bool cvsdEscoFailed) const
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_CVSD);
    HFP_AG_RETURN_IF_FAIL(ret);

    if (escoSupport_ && !cvsdEscoFailed) {
        dev.linkType = LINK_TYPE_ESCO;
        if (escoS4Support_) {
            HILOGI("Try connect by CVSD ESCO S4.");
            dev.lastParam = CVSD_ESCO_S4;
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S4_PARAM;
            param.addr = btAddr;
            ret = BTM_CreateEscoConnection(&param);
            HFP_AG_RETURN_IF_FAIL(ret);
        } else {
            HILOGI("Try connect by CVSD ESCO S1.");
            dev.lastParam = CVSD_ESCO_S1;
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S1_PARAM;
            param.addr = btAddr;
            ret = BTM_CreateEscoConnection(&param);
            HFP_AG_RETURN_IF_FAIL(ret);
        }
    } else {
        if (!BTM_IsSecureConnection(&btAddr)) {
            dev.linkType = LINK_TYPE_SCO;
            HILOGI("Try connect by CVSD SCO.");
            dev.lastParam = CVSD_SCO;
            BtmCreateScoConnectionParam param = CVSD_SCO_PARAM;
            param.addr = btAddr;
            ret = BTM_CreateScoConnection(&param);
            HFP_AG_RETURN_IF_FAIL(ret);
        } else {
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_AG_AUDIO_CONNECT_FAILED_EVT);
            return ret;
        }
    }

    HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_AG_AUDIO_CONNECTING_EVT);
    return ret;
}

int HfpAgAudioConnection::ConnectAudio() const
{
    HILOGI("Connect SCO to %{public}s", GetEncryptAddr(remoteAddr_).c_str());

    if (remoteAddr_ != g_activeAddr) {
        HILOGW("remoteAddr: %{public}s and g_activeAddr: %{public}s match failed!",
            GetEncryptAddr(remoteAddr_).c_str(), GetEncryptAddr(g_activeAddr).c_str());
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_AG_AUDIO_CONNECT_FAILED_EVT);
        return BT_BAD_PARAM;
    }

    auto dev = GetDeviceByAddr(remoteAddr_);
    if (dev == g_audioDevices.end()) {
        HfpAgAudioConnection::AudioDevice audioDev;
        audioDev.role = ROLE_INITIATOR;
        audioDev.addr = remoteAddr_;
        g_audioDevices.push_back(audioDev);
        dev = GetDeviceByAddr(remoteAddr_);
        HILOGI("Create Audio device for %{public}s", GetEncryptAddr(remoteAddr_).c_str());
    } else {
        HILOGI("Audio device: %{public}s already in device list", GetEncryptAddr(remoteAddr_).c_str());
    }

    bool msbcEscoFailed = false;
    bool cvsdEscoFailed = false;
    if (dev->lastConnectResult == CONNECT_FAIL) {
        if (dev->lastParam == MSBC_ESCO_T2 || dev->lastParam == MSBC_ESCO_T1) {
            msbcEscoFailed = true;
        } else if (dev->lastParam == CVSD_ESCO_S4 || dev->lastParam == CVSD_ESCO_S1) {
            cvsdEscoFailed = true;
        } else {
            HILOGI("Audio device: %{public}s, lastParam: %{public}d",
                GetEncryptAddr(remoteAddr_).c_str(), dev->lastParam);
        }
    }

    BtAddr btAddr = ConvertToBtAddr(remoteAddr_);
    if (inUseCodec_ == HFP_AG_CODEC_MSBC) {
        if (!msbcEscoFailed && escoSupport_) {
            return ConnectByMsbc(*dev, btAddr);
        } else {
            HILOGW("Need re-negotiate codec.");
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_AG_SETUP_CODEC_CVSD);
            return BT_BAD_PARAM;
        }
    } else if (inUseCodec_ == HFP_AG_CODEC_CVSD) {
        return ConnectByCvsd(*dev, btAddr, cvsdEscoFailed);
    } else {
        HILOGI("RemoteAddr: %{public}s, invalid codec: %{public}d",
            GetEncryptAddr(remoteAddr_).c_str(), inUseCodec_);
        return BT_BAD_PARAM;
    }

    return BT_NO_ERROR;
}

int HfpAgAudioConnection::DisconnectAudio() const
{
    HILOGI("Disconnect SCO from %{public}s", GetEncryptAddr(remoteAddr_).c_str());

    int ret;
    auto dev = GetDeviceByAddr(remoteAddr_);
    if (dev != g_audioDevices.end()) {
        ret = BTM_DisconnectScoConnection(dev->handle, REMOTE_USER_TERMINATED_CONNECTION);
        HFP_AG_RETURN_IF_FAIL(ret);
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(remoteAddr_, HFP_AG_AUDIO_DISCONNECTING_EVT);
    } else {
        HILOGW("%{public}s: Invalid Address", GetEncryptAddr(remoteAddr_).c_str());
        ret = BT_DEVICE_ERROR;
    }
    return ret;
}

int HfpAgAudioConnection::AcceptByMsbc(BtAddr btAddr)
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_TRANS);
    HFP_AG_RETURN_IF_FAIL(ret);

    if (BTM_IsSecureConnection(&btAddr)) {
        HILOGI("Accept by MSBC T2.");
        BtmCreateEscoConnectionParam param = MSBC_T2_PARAM;
        param.addr = btAddr;
        ret = BTM_AcceptEscoConnectionRequest(&param);
        HFP_AG_RETURN_IF_FAIL(ret);
    } else {
        HILOGI("Accept by MSBC T1.");
        BtmCreateEscoConnectionParam param = MSBC_T1_PARAM;
        param.addr = btAddr;
        ret = BTM_AcceptEscoConnectionRequest(&param);
        HFP_AG_RETURN_IF_FAIL(ret);
    }

    return ret;
}

int HfpAgAudioConnection::AcceptByCvsd(const AudioDevice &dev, BtAddr btAddr) const
{
    int ret = BTM_WriteVoiceSetting(BTM_VOICE_SETTING_CVSD);
    HFP_AG_RETURN_IF_FAIL(ret);

    if (dev.linkType == LINK_TYPE_ESCO && escoSupport_) {
        if (escoS4Support_) {
            HILOGI("Accept by CVSD ESCO S4.");
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S4_PARAM;
            param.addr = btAddr;
            ret = BTM_AcceptEscoConnectionRequest(&param);
            HFP_AG_RETURN_IF_FAIL(ret);
        } else {
            HILOGI("Accept by CVSD ESCO S1.");
            BtmCreateEscoConnectionParam param = CVSD_ESCO_S1_PARAM;
            param.addr = btAddr;
            ret = BTM_AcceptEscoConnectionRequest(&param);
            HFP_AG_RETURN_IF_FAIL(ret);
        }
    } else if (dev.linkType == LINK_TYPE_SCO) {
        HILOGI("Accept by CVSD SCO.");
        BtmCreateScoConnectionParam param = CVSD_SCO_PARAM;
        param.addr = btAddr;
        ret = BTM_AcceptScoConnectionRequest(&param);
        HFP_AG_RETURN_IF_FAIL(ret);
    } else {
        HILOGI("CVSD ESCO connection fail, "
            "linktype: %{public}hhu and escoSupport: %{public}d are not matched!",
            dev.linkType,
            escoSupport_);
        return BT_BAD_PARAM;
    }

    return ret;
}

int HfpAgAudioConnection::AcceptAudioConnection() const
{
    int ret = BT_NO_ERROR;
    BtAddr btAddr = ConvertToBtAddr(remoteAddr_);

    // Only accpet CVSD sco from remote device.
    auto dev = GetDeviceByAddr(remoteAddr_);
    if (dev != g_audioDevices.end()) {
        if (inUseCodec_ == HFP_AG_CODEC_MSBC) {
            if (dev->linkType == LINK_TYPE_ESCO && escoSupport_) {
                return AcceptByMsbc(btAddr);
            } else {
                HILOGI("MSBC ESCO connection fail, "
                    "linktype: %{public}hhu and escoSupport: %{public}d are not matched!",
                    dev->linkType,
                    escoSupport_);
                return BT_BAD_PARAM;
            }
        } else if (inUseCodec_ == HFP_AG_CODEC_CVSD) {
            return AcceptByCvsd(*dev, btAddr);
        } else {
            HILOGI("Invalid Codec: %{public}d", inUseCodec_);
            return BT_BAD_PARAM;
        }
    } else {
        HILOGW("%{public}s: Invalid Address", GetEncryptAddr(remoteAddr_).c_str());
        return BT_BAD_PARAM;
    }

    return ret;
}

int HfpAgAudioConnection::RejectAudioConnection() const
{
    HILOGI("Reject sco connect request from %{public}s", GetEncryptAddr(remoteAddr_).c_str());
    BtAddr btAddr = ConvertToBtAddr(remoteAddr_);
    BtmRejectScoConnectionRequestParam param = {btAddr, REJECT_DUE_TO_LIMITED_RESOURCES};

    int ret = BTM_RejectScoConnectionRequest(&param);
    HFP_AG_RETURN_IF_FAIL(ret);

    return ret;
}

void HfpAgAudioConnection::OnConnectRequest(const BtmScoConnectionRequestParam *param, void *context)
{
    HILOGI("enter");
    HfpScoConnectionRequestParam parameters;
    parameters.linkType = param->linkType;
    (void)memcpy_s(&parameters.addr, sizeof(BtAddr), param->addr, sizeof(BtAddr));
    HfpAgProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpAgAudioConnection::ProcessOnConnectRequest, parameters));
}

void HfpAgAudioConnection::ProcessOnConnectRequest(HfpScoConnectionRequestParam parameters)
{
    HILOGI("enter");

    RawAddress btAddr = RawAddress::ConvertToString(parameters.addr.addr);
    std::string address = btAddr.GetAddress();

    auto dev = GetDeviceByAddr(address);
    if (dev == g_audioDevices.end()) {
        HfpAgAudioConnection::AudioDevice audioDev;
        audioDev.role = ROLE_ACCEPTOR;
        audioDev.addr = address;
        audioDev.linkType = parameters.linkType;
        g_audioDevices.push_back(audioDev);
        HILOGI("Create Audio device for %{public}s", GetEncryptAddr(address).c_str());
    } else {
        dev->linkType = parameters.linkType;
        HILOGI("Audio device %{public}s already in device list",
            GetEncryptAddr(address).c_str());
    }
    HfpAgProfileEventSender::GetInstance().ScoConnectRequest(
        btAddr.GetAddress(), HFP_AG_AUDIO_CONNECT_REQUEST_EVT, parameters.linkType);
}

void HfpAgAudioConnection::OnConnectCompleted(const BtmScoConnectionCompleteParam *param, void *context)
{
    HILOGI("enter");
    HfpScoConnectionCompleteParam parameters;
    parameters.status = param->status;
    parameters.connectionHandle = param->connectionHandle;
    (void)memcpy_s(&parameters.addr, sizeof(BtAddr), param->addr, sizeof(BtAddr));
    HfpAgProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpAgAudioConnection::ProcessOnConnectCompleted, parameters));
}

void HfpAgAudioConnection::ProcessOnConnectCompletedFail(
    std::vector<HfpAgAudioConnection::AudioDevice>::iterator dev, const std::string &address)
{
    dev->lastConnectResult = CONNECT_FAIL;
    if (dev->role == ROLE_INITIATOR) {
        if (dev->lastParam == MSBC_ESCO_T2 || dev->lastParam == MSBC_ESCO_T1) {
            HILOGI("MSBC ESCO failed, try CVSD ESCO.");
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_AG_SETUP_CODEC_CVSD);
        } else if (dev->lastParam == CVSD_ESCO_S4 || dev->lastParam == CVSD_ESCO_S1) {
            HILOGI("CVSD ESCO failed, try CVSD SCO");
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_AG_RETRY_CONNECT_AUDIO_EVT);
        } else if (dev->lastParam == CVSD_SCO) {
            HILOGI("CVSD SCO failed, report fail event to service");
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_AG_AUDIO_CONNECT_FAILED_EVT);
        } else {
            HILOGI("Invalidaddress: %{public}s, lastParam: %{public}d",
                GetEncryptAddr(address).c_str(), dev->lastParam);
        }
    } else {
        // As acceptor, report connect failed event to service directly.
        HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_AG_AUDIO_CONNECT_FAILED_EVT);
        HILOGI("Accept SCO from address: %{public}s failed", GetEncryptAddr(address).c_str());
    }
}

void HfpAgAudioConnection::ProcessOnConnectCompleted(HfpScoConnectionCompleteParam parameters)
{
    HILOGI("enter");

    RawAddress btAddr = RawAddress::ConvertToString(parameters.addr.addr);
    std::string address = btAddr.GetAddress();
    auto dev = GetDeviceByAddr(address);
    if (dev != g_audioDevices.end()) {
        dev->addr = address;
        dev->handle = parameters.connectionHandle;
        if (!parameters.status) {
            HILOGI("SCO connect successfully!");
            dev->lastConnectResult = CONNECT_SUCCESS;
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(dev->addr, HFP_AG_AUDIO_CONNECTED_EVT);
        } else {
            ProcessOnConnectCompletedFail(dev, address);
        }
    } else {
        HILOGI("%{public}s: Invalid audio device", GetEncryptAddr(address).c_str());
    }
}

void HfpAgAudioConnection::OnDisconnectCompleted(const BtmScoDisconnectionCompleteParam *param, void *context)
{
    HILOGI("enter");
    HfpScoDisconnectionCompleteParam parameters;
    parameters.connectionHandle = param->connectionHandle;
    parameters.reason = param->reason;
    parameters.status = param->status;
    HfpAgProfileEventSender::GetInstance().GetDispatchter()->PostTask(
        std::bind(&HfpAgAudioConnection::ProcessOnDisconnectCompleted, parameters));
}

void HfpAgAudioConnection::ProcessOnDisconnectCompleted(HfpScoDisconnectionCompleteParam parameters)
{
    HILOGI("enter");

    auto it = GetDeviceByHandle(parameters.connectionHandle);
    if (it != g_audioDevices.end()) {
        if (!parameters.status) {
            HILOGI("Disconnect SCO from address: %{public}s successfully.", GetEncryptAddr(it->addr).c_str());
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(it->addr, HFP_AG_AUDIO_DISCONNECTED_EVT);
            g_audioDevices.erase(it);
        } else {
            HILOGI("Disconnect SCO from address: %{public}s failed.", GetEncryptAddr(it->addr).c_str());
            HfpAgProfileEventSender::GetInstance().UpdateScoConnectState(
                it->addr, HFP_AG_AUDIO_DISCONNECT_FAILED_EVT);
        }
    } else {
        HILOGI("Invalid audio device");
    }
}

void HfpAgAudioConnection::OnConnectionChanged(const BtmScoConnectionChangedParam *param, void *context)
{
    HILOGI("enter, connectionHandle: %{public}hu", param->connectionHandle);
}

void HfpAgAudioConnection::OnWriteVoiceSettingCompleted(uint8_t status, void *context)
{
    HILOGI("enter, status: %{public}hhu", status);
}
}  // namespace bluetooth
}  // namespace OHOS
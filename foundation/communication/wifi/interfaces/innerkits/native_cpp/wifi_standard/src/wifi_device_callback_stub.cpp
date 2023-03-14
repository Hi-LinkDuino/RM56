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
#include "wifi_device_callback_stub.h"
#include "define.h"
#include "wifi_hisysevent.h"
#include "wifi_logger.h"
#include "wifi_msg.h"
#include "wifi_errcode.h"

DEFINE_WIFILOG_LABEL("WifiDeviceCallBackStub");
namespace OHOS {
namespace Wifi {
WifiDeviceCallBackStub::WifiDeviceCallBackStub() : callback_(nullptr), mRemoteDied(false)
{}

WifiDeviceCallBackStub::~WifiDeviceCallBackStub()
{}

int WifiDeviceCallBackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("WifiDeviceCallBackStub::OnRemoteRequest!");
    if (mRemoteDied) {
        WIFI_LOGD("Failed to `%{public}s`,Remote service is died!", __func__);
        return -1;
    }

    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WIFI_LOGE("Sta callback stub token verification error");
        return WIFI_OPT_FAILED;
    }

    int exception = data.ReadInt32();
    if (exception) {
        WIFI_LOGE("WifiDeviceCallBackStub::OnRemoteRequest, got exception: %{public}d!", exception);
        return WIFI_OPT_FAILED;
    }
    int ret = -1;
    switch (code) {
        case WIFI_CBK_CMD_STATE_CHANGE: {
            ret = RemoteOnWifiStateChanged(code, data, reply);
            break;
        }
        case WIFI_CBK_CMD_CONNECTION_CHANGE: {
            ret = RemoteOnWifiConnectionChanged(code, data, reply);
            break;
        }
        case WIFI_CBK_CMD_RSSI_CHANGE: {
            ret = RemoteOnWifiRssiChanged(code, data, reply);
            break;
        }
        case WIFI_CBK_CMD_WPS_STATE_CHANGE: {
            ret = RemoteOnWifiWpsStateChanged(code, data, reply);
            break;
        }
        case WIFI_CBK_CMD_STREAM_DIRECTION: {
            ret = RemoteOnStreamChanged(code, data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

void WifiDeviceCallBackStub::RegisterUserCallBack(const sptr<IWifiDeviceCallBack> &callBack)
{
    if (callBack == nullptr) {
        WIFI_LOGD("RegisterUserCallBack:callBack is nullptr!");
        return;
    }
    callback_ = callBack;
}

bool WifiDeviceCallBackStub::IsRemoteDied() const
{
    return mRemoteDied;
}

void WifiDeviceCallBackStub::SetRemoteDied(bool val)
{
    mRemoteDied = val;
}

void WifiDeviceCallBackStub::OnWifiStateChanged(int state)
{
    WIFI_LOGD("WifiDeviceCallBackStub::OnWifiStateChanged");

    if (callback_) {
        callback_->OnWifiStateChanged(state);
    }
    WriteWifiEventReceivedHiSysEvent(HISYS_STA_POWER_STATE_CHANGE, state);
}

void WifiDeviceCallBackStub::OnWifiConnectionChanged(int state, const WifiLinkedInfo &info)
{
    WIFI_LOGD("WifiDeviceCallBackStub::OnWifiConnectionChanged");
    if (callback_) {
        callback_->OnWifiConnectionChanged(state, info);
    }
    WriteWifiEventReceivedHiSysEvent(HISYS_STA_CONN_STATE_CHANGE, state);
}

void WifiDeviceCallBackStub::OnWifiRssiChanged(int rssi)
{
    WIFI_LOGD("WifiDeviceCallBackStub::OnWifiRssiChanged");
    if (callback_) {
        callback_->OnWifiRssiChanged(rssi);
    }
    WriteWifiEventReceivedHiSysEvent(HISYS_STA_RSSI_STATE_CHANGE, rssi);
}

void WifiDeviceCallBackStub::OnWifiWpsStateChanged(int state, const std::string &pinCode)
{
    WIFI_LOGD("WifiDeviceCallBackStub::OnWifiWpsStateChanged");
    if (callback_) {
        callback_->OnWifiWpsStateChanged(state, pinCode);
    }
}

void WifiDeviceCallBackStub::OnStreamChanged(int direction)
{
    WIFI_LOGD("WifiDeviceCallBackStub::OnStreamChanged");
    if (callback_) {
        callback_->OnStreamChanged(direction);
    }
}

int WifiDeviceCallBackStub::RemoteOnWifiStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int state = data.ReadInt32();
    OnWifiStateChanged(state);
    reply.WriteInt32(0); /* Reply 0 to indicate that no exception occurs. */
    return 0;
}

int WifiDeviceCallBackStub::RemoteOnWifiConnectionChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int state = data.ReadInt32();
    WifiLinkedInfo info;
    info.networkId = data.ReadInt32();
    info.ssid = data.ReadCString();
    info.bssid = data.ReadCString();
    info.rssi = data.ReadInt32();
    info.band = data.ReadInt32();
    info.frequency = data.ReadInt32();
    info.linkSpeed = data.ReadInt32();
    info.macAddress = data.ReadCString();
    info.ipAddress = data.ReadInt32();
    int tmpConnState = data.ReadInt32();
    if (tmpConnState >= 0 && tmpConnState <= int(ConnState::UNKNOWN)) {
        info.connState = ConnState(tmpConnState);
    } else {
        info.connState = ConnState::UNKNOWN;
    }
    info.ifHiddenSSID = data.ReadBool();
    info.rxLinkSpeed = data.ReadInt32();
    info.txLinkSpeed = data.ReadInt32();
    info.chload = data.ReadInt32();
    info.snr = data.ReadInt32();
    info.isDataRestricted = data.ReadInt32();
    info.portalUrl = data.ReadCString();
    int tmpState = data.ReadInt32();
    if (tmpState >= 0 && tmpState <= int(SupplicantState::INVALID)) {
        info.supplicantState = SupplicantState(tmpState);
    } else {
        info.supplicantState = SupplicantState::INVALID;
    }

    int tmpDetailState = data.ReadInt32();
    if (tmpDetailState >= 0 && tmpDetailState <= int(DetailedState::INVALID)) {
        info.detailedState = DetailedState(tmpDetailState);
    } else {
        info.detailedState = DetailedState::INVALID;
    }
    OnWifiConnectionChanged(state, info);
    reply.WriteInt32(0); /* Reply 0 to indicate that no exception occurs. */
    return 0;
}

int WifiDeviceCallBackStub::RemoteOnWifiRssiChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int rssi = data.ReadInt32();
    OnWifiRssiChanged(rssi);
    reply.WriteInt32(0); /* Reply 0 to indicate that no exception occurs. */
    return 0;
}

int WifiDeviceCallBackStub::RemoteOnWifiWpsStateChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int state = data.ReadInt32();
    std::string pinCode = data.ReadCString();
    OnWifiWpsStateChanged(state, pinCode);
    reply.WriteInt32(0); /* Reply 0 to indicate that no exception occurs. */
    return 0;
}

int WifiDeviceCallBackStub::RemoteOnStreamChanged(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int direction = data.ReadInt32();
    OnStreamChanged(direction);
    reply.WriteInt32(0); /* Reply 0 to indicate that no exception occurs. */
    return 0;
}
}  // namespace Wifi
}  // namespace OHOS
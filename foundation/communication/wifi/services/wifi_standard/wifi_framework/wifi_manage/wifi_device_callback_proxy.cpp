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

#include "wifi_device_callback_proxy.h"
#include "wifi_logger.h"
#include "define.h"

DEFINE_WIFILOG_LABEL("WifiDeviceCallBackProxy");

namespace OHOS {
namespace Wifi {
WifiDeviceCallBackProxy::WifiDeviceCallBackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IWifiDeviceCallBack>(impl)
{}

void WifiDeviceCallBackProxy::OnWifiStateChanged(int state)
{
    WIFI_LOGD("WifiDeviceCallBackProxy::OnWifiStateChanged");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(state);
    int error = Remote()->SendRequest(WIFI_CBK_CMD_STATE_CHANGE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_STATE_CHANGE, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi state change failed!");
    }

    return;
}

void WifiDeviceCallBackProxy::OnWifiConnectionChanged(int state, const WifiLinkedInfo &info)
{
    WIFI_LOGD("WifiDeviceCallBackProxy::OnWifiConnectionChanged");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(state);
    data.WriteInt32(info.networkId);
    data.WriteCString(info.ssid.c_str());
    data.WriteCString(info.bssid.c_str());
    data.WriteInt32(info.rssi);
    data.WriteInt32(info.band);
    data.WriteInt32(info.frequency);
    data.WriteInt32(info.linkSpeed);
    data.WriteCString(info.macAddress.c_str());
    data.WriteInt32(info.ipAddress);
    data.WriteInt32((int)info.connState);
    data.WriteBool(info.ifHiddenSSID);
    data.WriteInt32(info.rxLinkSpeed);
    data.WriteInt32(info.txLinkSpeed);
    data.WriteInt32(info.chload);
    data.WriteInt32(info.snr);
    data.WriteInt32(info.isDataRestricted);
    data.WriteCString(info.portalUrl.c_str());
    data.WriteInt32((int)info.supplicantState);
    data.WriteInt32((int)info.detailedState);
    int error = Remote()->SendRequest(WIFI_CBK_CMD_CONNECTION_CHANGE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_CONNECTION_CHANGE, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi connection change failed!");
    }
    return;
}

void WifiDeviceCallBackProxy::OnWifiRssiChanged(int rssi)
{
    WIFI_LOGD("WifiDeviceCallBackProxy::OnWifiRssiChanged");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(rssi);
    int error = Remote()->SendRequest(WIFI_CBK_CMD_RSSI_CHANGE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_RSSI_CHANGE, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi rssi change failed!");
    }
    return;
}

void WifiDeviceCallBackProxy::OnWifiWpsStateChanged(int state, const std::string &pinCode)
{
    WIFI_LOGD("WifiDeviceCallBackProxy::OnWifiWpsStateChanged");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(state);
    data.WriteCString(pinCode.c_str());
    int error = Remote()->SendRequest(WIFI_CBK_CMD_WPS_STATE_CHANGE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_WPS_STATE_CHANGE, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wps pinmode started failed!");
    }
    return;
}

void WifiDeviceCallBackProxy::OnStreamChanged(int direction)
{
    WIFI_LOGD("WifiDeviceCallBackProxy::OnStreamChanged");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(direction);
    int error = Remote()->SendRequest(WIFI_CBK_CMD_STREAM_DIRECTION, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_STREAM_DIRECTION, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi stream direction failed!");
    }
    return;
}
}  // namespace Wifi
}  // namespace OHOS
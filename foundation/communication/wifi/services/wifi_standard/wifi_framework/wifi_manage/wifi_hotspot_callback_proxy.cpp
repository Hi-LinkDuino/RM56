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

#include "wifi_hotspot_callback_proxy.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspotCallbackProxy");

namespace OHOS {
namespace Wifi {
WifiHotspotCallbackProxy::WifiHotspotCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IWifiHotspotCallback>(impl)
{}

void WifiHotspotCallbackProxy::OnHotspotStateChanged(int state)
{
    WIFI_LOGD("WifiHotspotCallbackProxy::OnHotspotStateChanged state %{public}d", state);
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(state);
    int error = Remote()->SendRequest(WIFI_CBK_CMD_HOTSPOT_STATE_CHANGE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_HOTSPOT_STATE_CHANGE, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi hotspot state change failed!");
    }

    return;
}

void WifiHotspotCallbackProxy::OnHotspotStaJoin(const StationInfo &info)
{
    WIFI_LOGD("WifiHotspotCallbackProxy::OnHotspotStaJoin");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteCString(info.deviceName.c_str());
    data.WriteCString(info.bssid.c_str());
    data.WriteCString(info.ipAddr.c_str());
    int error = Remote()->SendRequest(WIFI_CBK_CMD_HOTSPOT_STATE_JOIN, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_HOTSPOT_STATE_JOIN, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi hotspot join failed!");
    }
    return;
}

void WifiHotspotCallbackProxy::OnHotspotStaLeave(const StationInfo &info)
{
    WIFI_LOGD("WifiHotspotCallbackProxy::OnHotspotStaLeave");
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteCString(info.deviceName.c_str());
    data.WriteCString(info.bssid.c_str());
    data.WriteCString(info.ipAddr.c_str());
    int error = Remote()->SendRequest(WIFI_CBK_CMD_HOTSPOT_STATE_LEAVE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_CBK_CMD_HOTSPOT_STATE_LEAVE, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        WIFI_LOGE("notify wifi hotspot leave failed!");
    }
    return;
}
}  // namespace Wifi
}  // namespace OHOS
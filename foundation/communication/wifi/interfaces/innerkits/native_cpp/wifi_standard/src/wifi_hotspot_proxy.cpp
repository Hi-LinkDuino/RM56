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
#include "wifi_hotspot_proxy.h"
#include "string_ex.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "wifi_logger.h"
#include "wifi_hotspot_callback_stub.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspotProxy");
namespace OHOS {
namespace Wifi {
static WifiHotspotCallbackStub *g_wifiHotspotCallbackStub = new WifiHotspotCallbackStub;
WifiHotspotProxy::WifiHotspotProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IWifiHotspot>(impl), mRemoteDied(false)
{
    if (impl) {
        if ((impl->IsProxyObject()) && (!impl->AddDeathRecipient(this))) {
            WIFI_LOGD("AddDeathRecipient!");
        } else {
            WIFI_LOGW("no recipient!");
        }
    }
}

WifiHotspotProxy::~WifiHotspotProxy()
{}

ErrCode WifiHotspotProxy::IsHotspotActive(bool &bActive)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_IS_HOTSPOT_ACTIVE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_IS_HOTSPOT_ACTIVE);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ErrCode(ret) != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }
    bActive = ((reply.ReadInt32() == 1) ? true : false);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::GetHotspotState(int &state)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GETAPSTATE_WIFI, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GETAPSTATE_WIFI);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ErrCode(ret) != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }
    state = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::GetHotspotConfig(HotspotConfig &result)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_HOTSPOT_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_HOTSPOT_CONFIG);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ErrCode(ret) != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    result.SetSsid(reply.ReadCString());
    result.SetSecurityType(static_cast<KeyMgmt>(reply.ReadInt32()));
    result.SetBand(static_cast<BandType>(reply.ReadInt32()));
    result.SetChannel(reply.ReadInt32());
    result.SetPreSharedKey(reply.ReadCString());
    result.SetMaxConn(reply.ReadInt32());

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::SetHotspotConfig(const HotspotConfig &config)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteCString(config.GetSsid().c_str());
    data.WriteInt32(static_cast<int>(config.GetSecurityType()));
    data.WriteInt32(static_cast<int>(config.GetBand()));
    data.WriteInt32(config.GetChannel());
    data.WriteCString(config.GetPreSharedKey().c_str());
    data.WriteInt32(config.GetMaxConn());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_SETAPCONFIG_WIFI, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_SETAPCONFIG_WIFI);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiHotspotProxy::GetStationList(std::vector<StationInfo> &result)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_STATION_LIST, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_STATION_LIST);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ErrCode(ret) != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }
    constexpr int MAX_SIZE = 512;
    int size = reply.ReadInt32();
    if (size > MAX_SIZE) {
        WIFI_LOGE("Station list size error: %{public}d", size);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < size; i++) {
        StationInfo info;
        info.deviceName = reply.ReadCString();
        info.bssid = reply.ReadCString();
        info.ipAddr = reply.ReadCString();
        result.emplace_back(info);
    }

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::DisassociateSta(const StationInfo &info)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteCString(info.deviceName.c_str());
    data.WriteCString(info.bssid.c_str());
    data.WriteCString(info.ipAddr.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_DISCONNECT_STA, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_DISCONNECT_STA);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return ErrCode(exception);
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiHotspotProxy::EnableHotspot(void)
{
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_ENABLE_WIFI_AP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_ENABLE_WIFI_AP);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiHotspotProxy::DisableHotspot(void)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_DISABLE_WIFI_AP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_DISABLE_WIFI_AP);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiHotspotProxy::GetBlockLists(std::vector<StationInfo> &infos)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_BLOCK_LISTS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_BLOCK_LISTS);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int err = reply.ReadInt32();
    if (err != WIFI_OPT_SUCCESS) {
        return ErrCode(err);
    }

    constexpr int MAX_SIZE = 512;
    int size = reply.ReadInt32();
    if (size > MAX_SIZE) {
        WIFI_LOGE("Get block size error: %{public}d", size);
        return WIFI_OPT_FAILED;
    }

    for (int i = 0; i < size; i++) {
        StationInfo info;
        info.deviceName = reply.ReadCString();
        info.bssid = reply.ReadCString();
        info.ipAddr = reply.ReadCString();
        infos.push_back(info);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::AddBlockList(const StationInfo &info)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteCString(info.deviceName.c_str());
    data.WriteCString(info.bssid.c_str());
    data.WriteCString(info.ipAddr.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_ADD_BLOCK_LIST, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_ADD_BLOCK_LIST);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiHotspotProxy::DelBlockList(const StationInfo &info)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteCString(info.deviceName.c_str());
    data.WriteCString(info.bssid.c_str());
    data.WriteCString(info.ipAddr.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_DEL_BLOCK_LIST, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_DEL_BLOCK_LIST);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiHotspotProxy::GetValidBands(std::vector<BandType> &bands)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_VALID_BANDS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_VALID_BANDS);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int err = reply.ReadInt32();
    if (err != WIFI_OPT_SUCCESS) {
        return ErrCode(err);
    }

    constexpr int MAX_BAND_SIZE = 512;
    int count = reply.ReadInt32();
    if (count > MAX_BAND_SIZE) {
        WIFI_LOGE("Band size error: %{public}d", count);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < count; i++) {
        int val = reply.ReadInt32();
        bands.push_back(BandType(val));
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::GetValidChannels(BandType band, std::vector<int32_t> &validchannels)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32((int)band);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_VALID_CHANNELS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_VALID_CHANNELS);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int err = reply.ReadInt32();
    if (err != WIFI_OPT_SUCCESS) {
        return ErrCode(err);
    }

    constexpr int MAX_CHANNELS_SIZE = 512;
    int count = reply.ReadInt32();
    if (count > MAX_CHANNELS_SIZE) {
        WIFI_LOGE("Channel size error: %{public}d", count);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < count; i++) {
        int val = reply.ReadInt32();
        validchannels.push_back(val);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiHotspotProxy::RegisterCallBack(const sptr<IWifiHotspotCallback> &callback)
{
    WIFI_LOGD("WifiHotspotProxy::RegisterCallBack!");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    g_wifiHotspotCallbackStub->RegisterCallBack(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_wifiHotspotCallbackStub->AsObject())) {
        WIFI_LOGE("WifiHotspotProxy::RegisterCallBack WriteDate fail, write callback.");
        return WIFI_OPT_FAILED;
    }

    int error = Remote()->SendRequest(WIFI_SVR_CMD_REGISTER_HOTSPOT_CALLBACK, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("WifiHotspotProxy::RegisterCallBack failed, error code is %{public}d ", error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    return ErrCode(ret);
}

ErrCode WifiHotspotProxy::GetSupportedFeatures(long &features)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_SUPPORTED_FEATURES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_SUPPORTED_FEATURES, error);
        return ErrCode(error);
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    features = reply.ReadInt64();
    return WIFI_OPT_SUCCESS;
}

void WifiHotspotProxy::OnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    WIFI_LOGD("Remote service is died!");
    mRemoteDied = true;
    if (g_wifiHotspotCallbackStub != nullptr) {
        g_wifiHotspotCallbackStub->SetRemoteDied(true);
    }
}
}  // namespace Wifi
}  // namespace OHOS
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

#include "wifi_hotspot_stub.h"
#include "wifi_hotspot_callback_proxy.h"
#include "wifi_logger.h"
#include "string_ex.h"
#include "wifi_errcode.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_hotspot_death_recipient.h"
DEFINE_WIFILOG_HOTSPOT_LABEL("WifiHotspotStub");

namespace OHOS {
namespace Wifi {
WifiHotspotStub::WifiHotspotStub():mSingleCallback(false)
{
    InitHandleMap();
}

WifiHotspotStub::~WifiHotspotStub()
{}

void WifiHotspotStub::InitHandleMap()
{
    handleFuncMap[WIFI_SVR_CMD_IS_HOTSPOT_ACTIVE] = &WifiHotspotStub::OnIsHotspotActive;
    handleFuncMap[WIFI_SVR_CMD_GETAPSTATE_WIFI] = &WifiHotspotStub::OnGetApStateWifi;
    handleFuncMap[WIFI_SVR_CMD_GET_HOTSPOT_CONFIG] = &WifiHotspotStub::OnGetHotspotConfig;
    handleFuncMap[WIFI_SVR_CMD_SETAPCONFIG_WIFI] = &WifiHotspotStub::OnSetApConfigWifi;
    handleFuncMap[WIFI_SVR_CMD_GET_STATION_LIST] = &WifiHotspotStub::OnGetStationList;
    handleFuncMap[WIFI_SVR_CMD_ENABLE_WIFI_AP] = &WifiHotspotStub::OnEnableWifiAp;
    handleFuncMap[WIFI_SVR_CMD_DISABLE_WIFI_AP] = &WifiHotspotStub::OnDisableWifiAp;
    handleFuncMap[WIFI_SVR_CMD_ADD_BLOCK_LIST] = &WifiHotspotStub::OnAddBlockList;
    handleFuncMap[WIFI_SVR_CMD_DEL_BLOCK_LIST] = &WifiHotspotStub::OnDelBlockList;
    handleFuncMap[WIFI_SVR_CMD_GET_BLOCK_LISTS] = &WifiHotspotStub::OnGetBlockLists;
    handleFuncMap[WIFI_SVR_CMD_DISCONNECT_STA] = &WifiHotspotStub::OnDisassociateSta;
    handleFuncMap[WIFI_SVR_CMD_GET_VALID_BANDS] = &WifiHotspotStub::OnGetValidBands;
    handleFuncMap[WIFI_SVR_CMD_GET_VALID_CHANNELS] = &WifiHotspotStub::OnGetValidChannels;
    handleFuncMap[WIFI_SVR_CMD_REGISTER_HOTSPOT_CALLBACK] = &WifiHotspotStub::OnRegisterCallBack;
    handleFuncMap[WIFI_SVR_CMD_GET_SUPPORTED_FEATURES] = &WifiHotspotStub::OnGetSupportedFeatures;
    return;
}

int WifiHotspotStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WIFI_LOGE("Hotspot stub token verification error");
        return WIFI_OPT_FAILED;
    }

    int exception = data.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }

    HandleFuncMap::iterator iter = handleFuncMap.find(code);
    if (iter == handleFuncMap.end()) {
        WIFI_LOGD("not find function to deal, code %{public}u", code);
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_NOT_SUPPORTED);
    } else {
        (this->*(iter->second))(code, data, reply, option);
    }
    return 0;
}

void WifiHotspotStub::OnIsHotspotActive(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    bool bActive = false;
    ErrCode ret = IsHotspotActive(bActive);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(bActive ? 1 : 0);
    }
    return;
}

void WifiHotspotStub::OnGetApStateWifi(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int state = 0;
    ErrCode ret = GetHotspotState(state);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(state);
    }
    return;
}

void WifiHotspotStub::OnGetHotspotConfig(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    HotspotConfig hotspotConfig;

    ErrCode ret = GetHotspotConfig(hotspotConfig);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteCString(hotspotConfig.GetSsid().c_str());
        reply.WriteInt32(static_cast<int>(hotspotConfig.GetSecurityType()));
        reply.WriteInt32(static_cast<int>(hotspotConfig.GetBand()));
        reply.WriteInt32(hotspotConfig.GetChannel());
        reply.WriteCString(hotspotConfig.GetPreSharedKey().c_str());
        reply.WriteInt32(hotspotConfig.GetMaxConn());
    }

    return;
}

void WifiHotspotStub::OnSetApConfigWifi(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    HotspotConfig config;
    config.SetSsid(data.ReadCString());
    config.SetSecurityType(static_cast<KeyMgmt>(data.ReadInt32()));
    config.SetBand(static_cast<BandType>(data.ReadInt32()));
    config.SetChannel(data.ReadInt32());
    config.SetPreSharedKey(data.ReadCString());
    config.SetMaxConn(data.ReadInt32());
    ErrCode ret = SetHotspotConfig(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return;
}

void WifiHotspotStub::OnGetStationList(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<StationInfo> result;
    ErrCode ret = GetStationList(result);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        int size = result.size();
        reply.WriteInt32(size);
        for (int i = 0; i < size; i++) {
            reply.WriteCString(result[i].deviceName.c_str());
            reply.WriteCString(result[i].bssid.c_str());
            reply.WriteCString(result[i].ipAddr.c_str());
        }
    }

    return;
}

void WifiHotspotStub::OnDisassociateSta(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    StationInfo info;
    info.deviceName = data.ReadCString();
    info.bssid = data.ReadCString();
    info.ipAddr = data.ReadCString();
    ErrCode ret = DisassociateSta(info);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiHotspotStub::OnGetValidBands(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<BandType> bands;
    ErrCode ret = GetValidBands(bands);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        int count = bands.size();
        reply.WriteInt32(count);
        for (int i = 0; i < count; i++) {
            reply.WriteInt32((int)bands[i]);
        }
    }
    return;
}

void WifiHotspotStub::OnGetValidChannels(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<int32_t> channels;
    int32_t band = data.ReadInt32();
    ErrCode ret = GetValidChannels(static_cast<BandType>(band), channels);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        int count = channels.size();
        reply.WriteInt32(count);
        for (int i = 0; i < count; i++) {
            reply.WriteInt32(channels[i]);
        }
    }
    return;
}

void WifiHotspotStub::OnEnableWifiAp(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = EnableHotspot();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return;
}

void WifiHotspotStub::OnDisableWifiAp(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = DisableHotspot();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return;
}

void WifiHotspotStub::OnAddBlockList(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    StationInfo info;
    info.deviceName = data.ReadCString();
    info.bssid = data.ReadCString();
    info.ipAddr = data.ReadCString();
    ErrCode ret = AddBlockList(info);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return;
}

void WifiHotspotStub::OnDelBlockList(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    StationInfo info;
    info.deviceName = data.ReadCString();
    info.bssid = data.ReadCString();
    info.ipAddr = data.ReadCString();
    ErrCode ret = DelBlockList(info);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return;
}

void WifiHotspotStub::OnGetBlockLists(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<StationInfo> infos;
    ErrCode ret = GetBlockLists(infos);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        int size = infos.size();
        reply.WriteInt32(size);
        for (int i = 0; i < size; i++) {
            reply.WriteCString(infos[i].deviceName.c_str());
            reply.WriteCString(infos[i].bssid.c_str());
            reply.WriteCString(infos[i].ipAddr.c_str());
        }
    }

    return;
}

void WifiHotspotStub::OnRegisterCallBack(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = WIFI_OPT_FAILED;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            WIFI_LOGD("Failed to ReadRemoteObject!");
            break;
        }
        sptr<IWifiHotspotCallback> callback_ = iface_cast<IWifiHotspotCallback>(remote);
        if (callback_ == nullptr) {
            callback_ = new (std::nothrow) WifiHotspotCallbackProxy(remote);
            WIFI_LOGD("create new WifiHotspotCallbackProxy!");
        }

        if (mSingleCallback) {
            ret = RegisterCallBack(callback_);
        } else {
            if (deathRecipient_ == nullptr) {
                deathRecipient_ = new (std::nothrow) WifiHotspotDeathRecipient();
            }
            if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
                WIFI_LOGD("AddDeathRecipient!");
            }
            if (callback_ != nullptr) {
                WifiInternalEventDispatcher::GetInstance().AddHotspotCallback(remote, callback_);
            }
        }
    } while (0);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiHotspotStub::OnGetSupportedFeatures(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    long features = 0;
    int ret = GetSupportedFeatures(features);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt64(features);
    }

    return;
}
}  // namespace Wifi
}  // namespace OHOS

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
#include "wifi_scan_proxy.h"
#include "define.h"
#include "wifi_logger.h"
#include "wifi_scan_callback_stub.h"

namespace OHOS {
DEFINE_WIFILOG_SCAN_LABEL("WifiScanProxy");
namespace Wifi {
static WifiScanCallbackStub g_wifiScanCallbackStub;

WifiScanProxy::WifiScanProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<IWifiScan>(remote), mRemoteDied(false)
{
    if (remote) {
        if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(this))) {
            WIFI_LOGD("AddDeathRecipient!");
        } else {
            WIFI_LOGW("no recipient!");
        }
    }
}

WifiScanProxy::~WifiScanProxy()
{}

ErrCode WifiScanProxy::SetScanControlInfo(const ScanControlInfo &info)
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
    data.WriteInt32(info.scanForbidList.size());
    for (auto iter = info.scanForbidList.begin(); iter != info.scanForbidList.end(); iter++) {
        data.WriteInt32(iter->scanScene);
        data.WriteInt32(static_cast<int>(iter->scanMode));
        data.WriteInt32(iter->forbidTime);
        data.WriteInt32(iter->forbidCount);
    }

    data.WriteInt32(info.scanIntervalList.size());
    for (auto iter2 = info.scanIntervalList.begin(); iter2 != info.scanIntervalList.end(); iter2++) {
        data.WriteInt32(iter2->scanScene);
        data.WriteInt32(static_cast<int>(iter2->scanMode));
        data.WriteBool(iter2->isSingle);
        data.WriteInt32(static_cast<int>(iter2->intervalMode));
        data.WriteInt32(iter2->interval);
        data.WriteInt32(iter2->count);
    }

    int error = Remote()->SendRequest(WIFI_SVR_CMD_SET_SCAN_CONTROL_INFO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_SET_SCAN_CONTROL_INFO);
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

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiScanProxy::Scan()
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_FULL_SCAN, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_FULL_SCAN);
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

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiScanProxy::AdvanceScan(const WifiScanParams &params)
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
    data.WriteCString(params.ssid.c_str());
    data.WriteCString(params.bssid.c_str());
    data.WriteInt32(params.freqs.size());
    for (std::size_t i = 0; i < params.freqs.size(); i++) {
        data.WriteInt32(params.freqs[i]);
    }
    data.WriteInt32(params.band);

    int error = Remote()->SendRequest(WIFI_SVR_CMD_SPECIFIED_PARAMS_SCAN, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_SPECIFIED_PARAMS_SCAN);
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

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiScanProxy::IsWifiClosedScan(bool &bOpen)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_IS_SCAN_ALWAYS_ACTIVE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_IS_SCAN_ALWAYS_ACTIVE);
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
    bOpen = reply.ReadBool();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiScanProxy::GetScanInfoList(std::vector<WifiScanInfo> &result)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_SCAN_INFO_LIST, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_SCAN_INFO_LIST);
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

    constexpr int MAX_SIZE = 4096;
    int tmpsize = reply.ReadInt32();
    if (tmpsize > MAX_SIZE) {
        WIFI_LOGE("Scan info size exceeds maximum allowed size: %{public}d", tmpsize);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < tmpsize; ++i) {
        WifiScanInfo info;
        info.bssid = reply.ReadCString();
        info.ssid = reply.ReadCString();
        info.capabilities = reply.ReadCString();
        info.frequency = reply.ReadInt32();
        info.rssi = reply.ReadInt32();
        info.timestamp = reply.ReadInt64();
        info.band = reply.ReadInt32();
        info.securityType = static_cast<WifiSecurity>(reply.ReadInt32());
        info.channelWidth = static_cast<WifiChannelWidth>(reply.ReadInt32());
        info.centerFrequency0 = reply.ReadInt32();
        info.centerFrequency1 = reply.ReadInt32();
        info.features = reply.ReadInt64();

        constexpr int IE_SIZE_MAX = 256;
        int ieSize = reply.ReadInt32();
        if (ieSize > IE_SIZE_MAX) {
            WIFI_LOGE("ie size error: %{public}d", ieSize);
            return WIFI_OPT_FAILED;
        }
        for (int m = 0; m < ieSize; ++m) {
            WifiInfoElem tempWifiInfoElem;
            tempWifiInfoElem.id = reply.ReadInt32();
            int contentSize = reply.ReadInt32();
            for (int n = 0; n < contentSize; n++) {
                char tempChar = static_cast<char>(reply.ReadInt8());
                tempWifiInfoElem.content.emplace_back(tempChar);
            }
            info.infoElems.emplace_back(tempWifiInfoElem);
        }
        result.emplace_back(info);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiScanProxy::RegisterCallBack(const sptr<IWifiScanCallback> &callback)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    WIFI_LOGD("RegisterCallBack start!");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    g_wifiScanCallbackStub.RegisterCallBack(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_wifiScanCallbackStub.AsObject())) {
        WIFI_LOGE("RegisterCallBack WriteRemoteObject failed!");
        return WIFI_OPT_FAILED;
    }

    int error = Remote()->SendRequest(WIFI_SVR_CMD_REGISTER_SCAN_CALLBACK, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("RegisterCallBack failed, error code is %{public}d", error);
        return WIFI_OPT_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    WIFI_LOGD("RegisterCallBack is finished: result=%{public}d", ret);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiScanProxy::GetSupportedFeatures(long &features)
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

void WifiScanProxy::OnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    WIFI_LOGD("Remote service is died!");
    mRemoteDied = true;
    g_wifiScanCallbackStub.SetRemoteDied(true);
}
}  // namespace Wifi
}  // namespace OHOS

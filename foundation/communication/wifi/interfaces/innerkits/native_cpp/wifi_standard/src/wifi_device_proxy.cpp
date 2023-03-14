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
#include "wifi_device_proxy.h"
#include "wifi_device_callback_stub.h"
#include "wifi_logger.h"
#include "define.h"

DEFINE_WIFILOG_LABEL("WifiDeviceProxy");

namespace OHOS {
namespace Wifi {
static WifiDeviceCallBackStub* g_deviceCallBackStub = new WifiDeviceCallBackStub;
WifiDeviceProxy::WifiDeviceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IWifiDevice>(impl), mRemoteDied(false)
{
    if (impl) {
        if ((impl->IsProxyObject()) && (!impl->AddDeathRecipient(this))) {
            WIFI_LOGD("AddDeathRecipient!");
        } else {
            WIFI_LOGW("no recipient!");
        }
    }
}

WifiDeviceProxy::~WifiDeviceProxy()
{}

ErrCode WifiDeviceProxy::EnableWifi()
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_ENABLE_WIFI, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_ENABLE_WIFI, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::DisableWifi()
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_DISABLE_WIFI, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_DISABLE_WIFI, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::InitWifiProtect(const WifiProtectType &protectType, const std::string &protectName)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32((int)protectType);
    data.WriteCString(protectName.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_INIT_WIFI_PROTECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_INIT_WIFI_PROTECT, error);
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

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::GetWifiProtectRef(const WifiProtectMode &protectMode, const std::string &protectName)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32((int)protectMode);
    data.WriteCString(protectName.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_WIFI_PROTECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_WIFI_PROTECT, error);
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

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::PutWifiProtectRef(const std::string &protectName)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteCString(protectName.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_PUT_WIFI_PROTECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_PUT_WIFI_PROTECT, error);
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

    return WIFI_OPT_SUCCESS;
}

void WifiDeviceProxy::WriteIpAddress(MessageParcel &data, const WifiIpAddress &address)
{
    data.WriteInt32(address.family);
    data.WriteInt32(address.addressIpv4);
    int size = address.addressIpv6.size();
    data.WriteInt32(size);
    for (int i = 0; i < size; i++) {
        data.WriteInt8(address.addressIpv6[i]);
    }
    return;
}

void WifiDeviceProxy::WriteDeviceConfig(const WifiDeviceConfig &config, MessageParcel &data)
{
    data.WriteInt32(config.networkId);
    data.WriteInt32(config.status);
    data.WriteCString(config.bssid.c_str());
    data.WriteCString(config.ssid.c_str());
    data.WriteInt32(config.band);
    data.WriteInt32(config.channel);
    data.WriteInt32(config.frequency);
    data.WriteInt32(config.level);
    data.WriteBool(config.isPasspoint);
    data.WriteBool(config.isEphemeral);
    data.WriteCString(config.preSharedKey.c_str());
    data.WriteCString(config.keyMgmt.c_str());
    for (int i = 0; i < WEPKEYS_SIZE; i++) {
        data.WriteCString(config.wepKeys[i].c_str());
    }
    data.WriteInt32(config.wepTxKeyIndex);
    data.WriteInt32(config.priority);
    data.WriteBool(config.hiddenSSID);
    data.WriteInt32((int)config.wifiIpConfig.assignMethod);
    WriteIpAddress(data, config.wifiIpConfig.staticIpAddress.ipAddress.address);
    data.WriteInt32(config.wifiIpConfig.staticIpAddress.ipAddress.prefixLength);
    data.WriteInt32(config.wifiIpConfig.staticIpAddress.ipAddress.flags);
    data.WriteInt32(config.wifiIpConfig.staticIpAddress.ipAddress.scope);
    WriteIpAddress(data, config.wifiIpConfig.staticIpAddress.gateway);
    WriteIpAddress(data, config.wifiIpConfig.staticIpAddress.dnsServer1);
    WriteIpAddress(data, config.wifiIpConfig.staticIpAddress.dnsServer2);
    data.WriteCString(config.wifiIpConfig.staticIpAddress.domains.c_str());
    data.WriteCString(config.wifiEapConfig.eap.c_str());
    data.WriteCString(config.wifiEapConfig.identity.c_str());
    data.WriteCString(config.wifiEapConfig.password.c_str());
    data.WriteInt32((int)config.wifiProxyconfig.configureMethod);
    data.WriteCString(config.wifiProxyconfig.autoProxyConfig.pacWebAddress.c_str());
    data.WriteCString(config.wifiProxyconfig.manualProxyConfig.serverHostName.c_str());
    data.WriteInt32(config.wifiProxyconfig.manualProxyConfig.serverPort);
    data.WriteCString(config.wifiProxyconfig.manualProxyConfig.exclusionObjectList.c_str());
    data.WriteInt32((int)config.wifiPrivacySetting);
}

ErrCode WifiDeviceProxy::AddDeviceConfig(const WifiDeviceConfig &config, int &result)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    WriteDeviceConfig(config, data);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_ADD_DEVICE_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_ADD_DEVICE_CONFIG, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }
    result = reply.ReadInt32();

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::UpdateDeviceConfig(const WifiDeviceConfig &config, int &result)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }

    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    WriteDeviceConfig(config, data);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_UPDATE_DEVICE_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_UPDATE_DEVICE_CONFIG, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }
    result = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::RemoveDevice(int networkId)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32(networkId);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_REMOVE_DEVICE_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_REMOVE_DEVICE_CONFIG, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::RemoveAllDevice()
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_REMOVE_ALL_DEVICE_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_REMOVE_ALL_DEVICE_CONFIG, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

void WifiDeviceProxy::ReadIpAddress(MessageParcel &reply, WifiIpAddress &address)
{
    constexpr int MAX_SIZE = 256;
    address.family = reply.ReadInt32();
    address.addressIpv4 = reply.ReadInt32();
    int size = reply.ReadInt32();
    if (size > MAX_SIZE) {
        WIFI_LOGE("Read IP address size error: %{public}d", size);
        return;
    }
    for (int i = 0; i < size; i++) {
        address.addressIpv6.push_back(reply.ReadInt8());
    }
    return;
}

void WifiDeviceProxy::ParseDeviceConfigs(MessageParcel &reply, std::vector<WifiDeviceConfig> &result)
{
    constexpr int MAX_DEVICE_CONFIG_SIZE = 1024;
    int retSize = reply.ReadInt32();
    if (retSize > MAX_DEVICE_CONFIG_SIZE) {
        WIFI_LOGE("Parse device config size error: %{public}d", retSize);
        return;
    }
    for (int i = 0; i < retSize; ++i) {
        WifiDeviceConfig config;
        config.networkId = reply.ReadInt32();
        config.status = reply.ReadInt32();
        config.bssid = reply.ReadCString();
        config.ssid = reply.ReadCString();
        config.band = reply.ReadInt32();
        config.channel = reply.ReadInt32();
        config.frequency = reply.ReadInt32();
        config.level = reply.ReadInt32();
        config.isPasspoint = reply.ReadBool();
        config.isEphemeral = reply.ReadBool();
        config.preSharedKey = reply.ReadCString();
        config.keyMgmt = reply.ReadCString();
        for (int j = 0; j < WEPKEYS_SIZE; j++) {
            config.wepKeys[j] = reply.ReadCString();
        }
        config.wepTxKeyIndex = reply.ReadInt32();
        config.priority = reply.ReadInt32();
        config.hiddenSSID = reply.ReadBool();
        config.wifiIpConfig.assignMethod = AssignIpMethod(reply.ReadInt32());
        ReadIpAddress(reply, config.wifiIpConfig.staticIpAddress.ipAddress.address);
        config.wifiIpConfig.staticIpAddress.ipAddress.prefixLength = reply.ReadInt32();
        config.wifiIpConfig.staticIpAddress.ipAddress.flags = reply.ReadInt32();
        config.wifiIpConfig.staticIpAddress.ipAddress.scope = reply.ReadInt32();
        ReadIpAddress(reply, config.wifiIpConfig.staticIpAddress.gateway);
        ReadIpAddress(reply, config.wifiIpConfig.staticIpAddress.dnsServer1);
        ReadIpAddress(reply, config.wifiIpConfig.staticIpAddress.dnsServer2);
        config.wifiIpConfig.staticIpAddress.domains = reply.ReadCString();
        config.wifiEapConfig.eap = reply.ReadCString();
        config.wifiEapConfig.identity = reply.ReadCString();
        config.wifiEapConfig.password = reply.ReadCString();
        config.wifiProxyconfig.configureMethod = ConfigureProxyMethod(reply.ReadInt32());
        config.wifiProxyconfig.autoProxyConfig.pacWebAddress = reply.ReadCString();
        config.wifiProxyconfig.manualProxyConfig.serverHostName = reply.ReadCString();
        config.wifiProxyconfig.manualProxyConfig.serverPort = reply.ReadInt32();
        config.wifiProxyconfig.manualProxyConfig.exclusionObjectList = reply.ReadCString();
        config.wifiPrivacySetting = WifiPrivacyConfig(reply.ReadInt32());

        result.emplace_back(config);
    }
}

ErrCode WifiDeviceProxy::GetDeviceConfigs(std::vector<WifiDeviceConfig> &result)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_DEVICE_CONFIGS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_DEVICE_CONFIGS, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    ParseDeviceConfigs(reply, result);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::EnableDeviceConfig(int networkId, bool attemptEnable)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32(networkId);
    data.WriteInt32(attemptEnable);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_ENABLE_DEVICE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_ENABLE_DEVICE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::DisableDeviceConfig(int networkId)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32(networkId);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_DISABLE_DEVICE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_DISABLE_DEVICE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::ConnectToNetwork(int networkId)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32(networkId);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_CONNECT_TO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_CONNECT_TO, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::ConnectToDevice(const WifiDeviceConfig &config)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    WriteDeviceConfig(config, data);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_CONNECT2_TO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_CONNECT2_TO, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

bool WifiDeviceProxy::IsConnected()
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_IS_WIFI_CONNECTED, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_IS_WIFI_CONNECTED, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return false;
    }
    return reply.ReadBool();
}

ErrCode WifiDeviceProxy::ReConnect()
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_RECONNECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_RECONNECT, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::ReAssociate(void)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_REASSOCIATE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_REASSOCIATE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::Disconnect(void)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_DISCONNECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_DISCONNECT, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::StartWps(const WpsConfig &config)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32(static_cast<int>(config.setup));
    data.WriteCString(config.pin.c_str());
    data.WriteCString(config.bssid.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_START_WPS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_START_WPS, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::CancelWps(void)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_CANCEL_WPS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_CANCEL_WPS, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::IsWifiActive(bool &bActive)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_IS_WIFI_ACTIVE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_IS_WIFI_ACTIVE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    bActive = reply.ReadBool();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::GetWifiState(int &state)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_WIFI_STATE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_WIFI_STATE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    state = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

void WifiDeviceProxy::ReadLinkedInfo(MessageParcel &reply, WifiLinkedInfo &info)
{
    info.networkId = reply.ReadInt32();
    info.ssid = reply.ReadCString();
    info.bssid = reply.ReadCString();
    info.rssi = reply.ReadInt32();
    info.band = reply.ReadInt32();
    info.frequency = reply.ReadInt32();
    info.linkSpeed = reply.ReadInt32();
    info.macAddress = reply.ReadCString();
    info.ipAddress = reply.ReadInt32();
    int tmpConnState = reply.ReadInt32();
    if ((tmpConnState >= 0) && (tmpConnState <= (int)ConnState::UNKNOWN)) {
        info.connState = ConnState(tmpConnState);
    } else {
        info.connState = ConnState::UNKNOWN;
    }
    info.ifHiddenSSID = reply.ReadBool();
    info.rxLinkSpeed = reply.ReadInt32();
    info.txLinkSpeed = reply.ReadInt32();
    info.chload = reply.ReadInt32();
    info.snr = reply.ReadInt32();
    info.isDataRestricted = reply.ReadInt32();
    info.portalUrl = reply.ReadCString();

    int tmpState = reply.ReadInt32();
    if ((tmpState >= 0) && (tmpState <= (int)SupplicantState::INVALID)) {
        info.supplicantState = (SupplicantState)tmpState;
    } else {
        info.supplicantState = SupplicantState::INVALID;
    }

    int tmpDetailState = reply.ReadInt32();
    if ((tmpDetailState >= 0) && (tmpDetailState <= (int)DetailedState::INVALID)) {
        info.detailedState = (DetailedState)tmpDetailState;
    } else {
        info.detailedState = DetailedState::INVALID;
    }
}

ErrCode WifiDeviceProxy::GetLinkedInfo(WifiLinkedInfo &info)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_LINKED_INFO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_LINKED_INFO, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    ReadLinkedInfo(reply, info);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::GetIpInfo(IpInfo &info)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_DHCP_INFO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_DHCP_INFO, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    info.ipAddress = reply.ReadInt32();
    info.gateway = reply.ReadInt32();
    info.netmask = reply.ReadInt32();
    info.primaryDns = reply.ReadInt32();
    info.secondDns = reply.ReadInt32();
    info.serverIp = reply.ReadInt32();
    info.leaseDuration = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::SetCountryCode(const std::string &countryCode)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteCString(countryCode.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_SET_COUNTRY_CODE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_SET_COUNTRY_CODE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::GetCountryCode(std::string &countryCode)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_COUNTRY_CODE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_COUNTRY_CODE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    countryCode = reply.ReadCString();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::RegisterCallBack(const sptr<IWifiDeviceCallBack> &callback)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);

    g_deviceCallBackStub->RegisterUserCallBack(callback);

    if (!data.WriteRemoteObject(g_deviceCallBackStub->AsObject())) {
        WIFI_LOGE("WifiDeviceProxy::RegisterCallBack WriteRemoteObject failed!");
        return WIFI_OPT_FAILED;
    }

    int error = Remote()->SendRequest(WIFI_SVR_CMD_REGISTER_CALLBACK_CLIENT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed, code is %{public}d", WIFI_SVR_CMD_REGISTER_CALLBACK_CLIENT, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiDeviceProxy::GetSignalLevel(const int &rssi, const int &band, int &level)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data, reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    data.WriteInt32(rssi);
    data.WriteInt32(band);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_SIGNAL_LEVEL, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_GET_SIGNAL_LEVEL, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ret != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }

    level = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceProxy::GetSupportedFeatures(long &features)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
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
        return WIFI_OPT_FAILED;
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

ErrCode WifiDeviceProxy::GetDeviceMacAddress(std::string &result)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_DERVICE_MAC_ADD, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_DERVICE_MAC_ADD);
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
    result = reply.ReadCString();
    return WIFI_OPT_SUCCESS;
}

bool WifiDeviceProxy::SetLowLatencyMode(bool enabled)
{
    if (mRemoteDied) {
        WIFI_LOGE("failed to `%{public}s`,remote service is died!", __func__);
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
    data.WriteBool(enabled);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_SET_LOW_LATENCY_MODE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_SET_LOW_LATENCY_MODE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return reply.ReadBool();
}

void WifiDeviceProxy::OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
{
    WIFI_LOGD("Remote service is died!");
    mRemoteDied = true;
    if (g_deviceCallBackStub != nullptr) {
        g_deviceCallBackStub->SetRemoteDied(true);
    }
}
}  // namespace Wifi
}  // namespace OHOS
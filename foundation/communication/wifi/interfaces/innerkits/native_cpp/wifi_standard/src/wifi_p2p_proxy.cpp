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
#include "wifi_p2p_proxy.h"
#include "define.h"
#include "wifi_logger.h"
#include "wifi_p2p_callback_stub.h"

namespace OHOS {
namespace Wifi {
DEFINE_WIFILOG_P2P_LABEL("WifiP2pProxy");

static WifiP2pCallbackStub g_wifiP2pCallbackStub;
WifiP2pProxy::WifiP2pProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IWifiP2p>(impl), mRemoteDied(false)
{
    if (impl) {
        if ((impl->IsProxyObject()) && (!impl->AddDeathRecipient(this))) {
            WIFI_LOGD("AddDeathRecipient!");
        } else {
            WIFI_LOGW("no recipient!");
        }
    }
}

WifiP2pProxy::~WifiP2pProxy()
{}

ErrCode WifiP2pProxy::EnableP2p(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_ENABLE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_ENABLE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::DisableP2p(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_DISABLE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_DISABLE, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::DiscoverDevices(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_DISCOVER_DEVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_DISCOVER_DEVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::StopDiscoverDevices(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_STOP_DISCOVER_DEVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE(
            "Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_STOP_DISCOVER_DEVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::DiscoverServices(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_DISCOVER_SERVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_DISCOVER_SERVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::StopDiscoverServices(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_STOP_DISCOVER_SERVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE(
            "Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_STOP_DISCOVER_SERVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request)
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
    WriteWifiP2pServiceRequest(data, device, request);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_REQUEST_SERVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_REQUEST_SERVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::PutLocalP2pService(const WifiP2pServiceInfo &srvInfo)
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
    WriteWifiP2pServiceInfo(data, srvInfo);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_PUT_LOCAL_SERVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_PUT_LOCAL_SERVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo)
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
    WriteWifiP2pServiceInfo(data, srvInfo);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_DELETE_LOCAL_SERVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE(
            "Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_DELETE_LOCAL_SERVICES, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::StartP2pListen(int period, int interval)
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
    data.WriteInt32(period);
    data.WriteInt32(interval);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_START_LISTEN, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_START_LISTEN, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::StopP2pListen(void)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_STOP_LISTEN, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_STOP_LISTEN, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::CreateGroup(const WifiP2pConfig &config)
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
    WriteWifiP2pConfigData(data, config);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_CREATE_GROUP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_CREATE_GROUP, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::RemoveGroup()
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_REMOVE_GROUP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_REMOVE_GROUP, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::DeleteGroup(const WifiP2pGroupInfo &group)
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
    WriteWifiP2pGroupData(data, group);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_DELETE_GROUP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_DELETE_GROUP, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

void WifiP2pProxy::ReadWifiP2pServiceInfo(MessageParcel &reply, WifiP2pServiceInfo &info) const
{
    info.SetServiceName(reply.ReadCString());
    info.SetDeviceAddress(reply.ReadCString());
    info.SetServicerProtocolType(static_cast<P2pServicerProtocolType>(reply.ReadInt32()));
    std::vector<std::string> queryList;

    constexpr int MAX_SIZE = 512;
    int size = reply.ReadInt32();
    if (size > MAX_SIZE) {
        WIFI_LOGE("P2p service size error: %{public}d", size);
        return;
    }
    for (int i = 0; i < size; i++) {
        std::string str = reply.ReadCString();
        queryList.push_back(str);
    }
    info.SetQueryList(queryList);
    return;
}

void WifiP2pProxy::WriteWifiP2pServiceInfo(MessageParcel &data, const WifiP2pServiceInfo &info) const
{
    data.WriteCString(info.GetServiceName().c_str());
    data.WriteCString(info.GetDeviceAddress().c_str());
    data.WriteInt32(static_cast<int>(info.GetServicerProtocolType()));
    std::vector<std::string> queryList = info.GetQueryList();
    data.WriteInt32(queryList.size());
    for (std::size_t i = 0; i < queryList.size(); i++) {
        data.WriteCString(queryList[i].c_str());
    }
    return;
}

void WifiP2pProxy::WriteWifiP2pServiceRequest(
    MessageParcel &data, const WifiP2pDevice &device, const WifiP2pServiceRequest &request) const
{
    WriteWifiP2pDeviceData(data, device);
    data.WriteInt32(static_cast<int>(request.GetProtocolType()));
    data.WriteInt32(request.GetTransactionId());
    std::vector<unsigned char> query = request.GetQuery();
    data.WriteInt32(query.size());
    for (std::size_t i = 0; i < query.size(); i++) {
        data.WriteInt8(query[i]);
    }
    return;
}

void WifiP2pProxy::WriteWifiP2pDeviceData(MessageParcel &data, const WifiP2pDevice &device) const
{
    data.WriteCString(device.GetDeviceName().c_str());
    data.WriteCString(device.GetDeviceAddress().c_str());
    data.WriteCString(device.GetPrimaryDeviceType().c_str());
    data.WriteCString(device.GetSecondaryDeviceType().c_str());
    data.WriteInt32(static_cast<int>(device.GetP2pDeviceStatus()));
    data.WriteBool(device.GetWfdInfo().GetWfdEnabled());
    data.WriteInt32(device.GetWfdInfo().GetDeviceInfo());
    data.WriteInt32(device.GetWfdInfo().GetCtrlPort());
    data.WriteInt32(device.GetWfdInfo().GetMaxThroughput());
    data.WriteInt32(device.GetWpsConfigMethod());
    data.WriteInt32(device.GetDeviceCapabilitys());
    data.WriteInt32(device.GetGroupCapabilitys());
}

void WifiP2pProxy::ReadWifiP2pDeviceData(MessageParcel &reply, WifiP2pDevice &device) const
{
    device.SetDeviceName(reply.ReadCString());
    device.SetDeviceAddress(reply.ReadCString());
    device.SetPrimaryDeviceType(reply.ReadCString());
    device.SetSecondaryDeviceType(reply.ReadCString());
    device.SetP2pDeviceStatus(static_cast<P2pDeviceStatus>(reply.ReadInt32()));
    WifiP2pWfdInfo wfdInfo;
    wfdInfo.SetWfdEnabled(reply.ReadBool());
    wfdInfo.SetDeviceInfo(reply.ReadInt32());
    wfdInfo.SetCtrlPort(reply.ReadInt32());
    wfdInfo.SetMaxThroughput(reply.ReadInt32());
    device.SetWfdInfo(wfdInfo);
    device.SetWpsConfigMethod(reply.ReadInt32());
    device.SetDeviceCapabilitys(reply.ReadInt32());
    device.SetGroupCapabilitys(reply.ReadInt32());
}

void WifiP2pProxy::WriteWifiP2pGroupData(MessageParcel &data, const WifiP2pGroupInfo &info) const
{
    data.WriteBool(info.IsGroupOwner());
    WriteWifiP2pDeviceData(data, info.GetOwner());
    data.WriteCString(info.GetPassphrase().c_str());
    data.WriteCString(info.GetInterface().c_str());
    data.WriteCString(info.GetGroupName().c_str());
    data.WriteInt32(info.GetFrequency());
    data.WriteBool(info.IsPersistent());
    data.WriteInt32(static_cast<int>(info.GetP2pGroupStatus()));
    data.WriteInt32(info.GetNetworkId());
    data.WriteCString(info.GetGoIpAddress().c_str());
    std::vector<WifiP2pDevice> deviceVec;
    deviceVec = info.GetClientDevices();
    data.WriteInt32(deviceVec.size());
    for (auto it = deviceVec.begin(); it != deviceVec.end(); ++it) {
        WriteWifiP2pDeviceData(data, *it);
    }
}

void WifiP2pProxy::ReadWifiP2pGroupData(MessageParcel &reply, WifiP2pGroupInfo &info) const
{
    info.SetIsGroupOwner(reply.ReadBool());
    WifiP2pDevice device;
    ReadWifiP2pDeviceData(reply, device);
    info.SetOwner(device);
    info.SetPassphrase(reply.ReadCString());
    info.SetInterface(reply.ReadCString());
    info.SetGroupName(reply.ReadCString());
    info.SetFrequency(reply.ReadInt32());
    info.SetIsPersistent(reply.ReadBool());
    info.SetP2pGroupStatus(static_cast<P2pGroupStatus>(reply.ReadInt32()));
    info.SetNetworkId(reply.ReadInt32());
    info.SetGoIpAddress(reply.ReadCString());

    constexpr int MAX_SIZE = 512;
    int size = reply.ReadInt32();
    if (size > MAX_SIZE) {
        WIFI_LOGE("Group info device size error: %{public}d", size);
        return;
    }
    for (auto it = 0; it < size; ++it) {
        WifiP2pDevice cliDev;
        ReadWifiP2pDeviceData(reply, cliDev);
        info.AddClientDevice(cliDev);
    }
}

void WifiP2pProxy::WriteWifiP2pConfigData(MessageParcel &data, const WifiP2pConfig &config) const
{
    data.WriteCString(config.GetDeviceAddress().c_str());
    data.WriteCString(config.GetPassphrase().c_str());
    data.WriteCString(config.GetGroupName().c_str());
    data.WriteInt32(static_cast<int>(config.GetGoBand()));
    data.WriteInt32(config.GetNetId());
    data.WriteInt32(config.GetGroupOwnerIntent());
}

ErrCode WifiP2pProxy::P2pConnect(const WifiP2pConfig &config)
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
    WriteWifiP2pConfigData(data, config);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_CONNECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_CONNECT, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::P2pCancelConnect()
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_CANCEL_CONNECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_CANCEL_CONNECT, error);
        return WIFI_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_QUERY_INFO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_QUERY_INFO, error);
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
    linkedInfo.SetConnectState(static_cast<P2pConnectedState>(reply.ReadInt32()));
    linkedInfo.SetIsGroupOwner(reply.ReadBool());
    linkedInfo.SetIsGroupOwnerAddress(reply.ReadCString());

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::GetCurrentGroup(WifiP2pGroupInfo &group)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_GET_CURRENT_GROUP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_GET_CURRENT_GROUP, error);
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
    ReadWifiP2pGroupData(reply, group);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::GetP2pEnableStatus(int &status)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_GET_ENABLE_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_GET_ENABLE_STATUS, error);
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
    status = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::GetP2pDiscoverStatus(int &status)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_GET_DISCOVER_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_GET_DISCOVER_STATUS, error);
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
    status = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::GetP2pConnectedStatus(int &status)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_GET_CONNECTED_STATUS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_GET_CONNECTED_STATUS, error);
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
    status = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::QueryP2pDevices(std::vector<WifiP2pDevice> &devices)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_QUERY_DEVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_QUERY_DEVICES, error);
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
        WIFI_LOGE("Get p2p devices size error: %{public}d", size);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < size; ++i) {
        WifiP2pDevice config;
        ReadWifiP2pDeviceData(reply, config);
        devices.emplace_back(config);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_QUERY_GROUPS, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_QUERY_GROUPS, error);
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
        WIFI_LOGE("Get p2p group size error: %{public}d", size);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < size; ++i) {
        WifiP2pGroupInfo group;
        ReadWifiP2pGroupData(reply, group);
        groups.emplace_back(group);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::QueryP2pServices(std::vector<WifiP2pServiceInfo> &services)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_QUERY_SERVICES, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_QUERY_SERVICES, error);
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
        WIFI_LOGE("Get p2p service size error: %{public}d", size);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < size; ++i) {
        WifiP2pServiceInfo info;
        ReadWifiP2pServiceInfo(reply, info);
        services.emplace_back(info);
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::SetP2pDeviceName(const std::string &deviceName)
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
    data.WriteCString(deviceName.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_SET_DEVICE_NAME, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_SET_DEVICE_NAME, error);
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
ErrCode WifiP2pProxy::SetP2pWfdInfo(const WifiP2pWfdInfo &wfdInfo)
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
    data.WriteBool(wfdInfo.GetWfdEnabled());
    data.WriteInt32(wfdInfo.GetDeviceInfo());
    data.WriteInt32(wfdInfo.GetCtrlPort());
    data.WriteInt32(wfdInfo.GetMaxThroughput());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_SET_WFD_INFO, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_SET_WFD_INFO, error);
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


ErrCode WifiP2pProxy::RegisterCallBack(const sptr<IWifiP2pCallback> &callback)
{
    if (mRemoteDied) {
        WIFI_LOGD("failed to `%{public}s`,remote service is died!", __func__);
        return WIFI_OPT_FAILED;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    g_wifiP2pCallbackStub.RegisterCallBack(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WIFI_LOGE("Write interface token error: %{public}s", __func__);
        return WIFI_OPT_FAILED;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_wifiP2pCallbackStub.AsObject())) {
        WIFI_LOGE("RegisterCallBack WriteRemoteObject failed!");
        return WIFI_OPT_FAILED;
    }

    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_REGISTER_CALLBACK, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("RegisterCallBack failed, error code is %{public}d", error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    WIFI_LOGD("RegisterCallBack is finished: result=%{public}d", ret);
    return ErrCode(ret);
}

ErrCode WifiP2pProxy::GetSupportedFeatures(long &features)
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

ErrCode WifiP2pProxy::Hid2dRequestGcIp(const std::string& gcMac, std::string& ipAddr)
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
    data.WriteCString(gcMac.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_APPLY_IP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d", WIFI_SVR_CMD_P2P_HID2D_APPLY_IP, error);
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
    ipAddr = reply.ReadCString();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::Hid2dSharedlinkIncrease()
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_INCREASE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_INCREASE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dSharedlinkDecrease()
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_DECREASE, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_DECREASE, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dCreateGroup(const int frequency, FreqType type)
{
    WIFI_LOGI("Request hid2d create group");

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
    data.WriteInt32(frequency);
    data.WriteInt32(static_cast<int>(type));
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_CREATE_GROUP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_CREATE_GROUP, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dRemoveGcGroup(const std::string& gcIfName)
{
    WIFI_LOGI("Request hid2d remove group");

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
    data.WriteCString(gcIfName.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_REMOVE_GC_GROUP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_REMOVE_GC_GROUP, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dConnect(const Hid2dConnectConfig& config)
{
    WIFI_LOGI("Request hid2d connect");

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
    data.WriteCString(config.GetBssid().c_str());
    data.WriteCString(config.GetPreSharedKey().c_str());
    data.WriteInt32(config.GetFrequency());
    data.WriteInt32(static_cast<int>(config.GetDhcpMode()));
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_CONNECT, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_CONNECT, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dConfigIPAddr(const std::string& ifName, const IpAddrInfo& ipInfo)
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
    data.WriteCString(ifName.c_str());
    data.WriteCString(ipInfo.ip.c_str());
    data.WriteCString(ipInfo.gateway.c_str());
    data.WriteCString(ipInfo.netmask.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_CONFIG_IP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_CONFIG_IP, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dReleaseIPAddr(const std::string& ifName)
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
    data.WriteCString(ifName.c_str());
    int error = Remote()->SendRequest(WIFI_SVR_CMD_P2P_HID2D_RELEASE_IP, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_P2P_HID2D_RELEASE_IP, error);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

ErrCode WifiP2pProxy::Hid2dGetRecommendChannel(const RecommendChannelRequest& request,
    RecommendChannelResponse& response)
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
    data.WriteCString(request.remoteIfName.c_str());
    data.WriteInt32(request.remoteIfMode);
    data.WriteCString(request.localIfName.c_str());
    data.WriteInt32(request.localIfMode);
    data.WriteInt32(request.prefBand);
    data.WriteInt32(static_cast<int>(request.prefBandwidth));
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_P2P_RECOMMENDED_CHANNEL, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGE("Set Attr(%{public}d) failed,error code is %{public}d",
            WIFI_SVR_CMD_GET_P2P_RECOMMENDED_CHANNEL, error);
        return ErrCode(error);
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ErrCode(ret) != WIFI_OPT_SUCCESS) {
        return ErrCode(ret);
    }
    response.status = RecommendStatus(reply.ReadInt32());
    response.index = reply.ReadInt32();
    response.centerFreq = reply.ReadInt32();
    response.centerFreq1 = reply.ReadInt32();
    response.centerFreq2 = reply.ReadInt32();
    response.bandwidth = reply.ReadInt32();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::Hid2dGetChannelListFor5G(std::vector<int>& vecChannelList)
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
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_5G_CHANNEL_LIST, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_5G_CHANNEL_LIST);
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
    int listSize = reply.ReadInt32();
    if (listSize > MAX_SIZE) {
        WIFI_LOGE("Get channel list for 5G size error: %{public}d", listSize);
        return WIFI_OPT_FAILED;
    }
    for (int i = 0; i < listSize; ++i) {
        vecChannelList.emplace_back(reply.ReadInt32());
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType,
    char cfgData[CFG_DATA_MAX_BYTES], int* getDatValidLen)
{
    if (getDatValidLen == nullptr) {
        WIFI_LOGD("getDatValidLen is nullptr!");
        return WIFI_OPT_FAILED;
    }

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
    data.WriteInt32(static_cast<int>(cfgType));
    int error = Remote()->SendRequest(WIFI_SVR_CMD_GET_SELF_WIFI_CFG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_GET_SELF_WIFI_CFG);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    int ret = reply.ReadInt32();
    if (ErrCode(ret) != WIFI_OPT_SUCCESS) {
        return WIFI_OPT_FAILED;
    }

    *getDatValidLen = reply.ReadInt32();
    if (*getDatValidLen > 0) {
        const char *dataBuffer = (const char *)reply.ReadBuffer(*getDatValidLen);
        if (dataBuffer == nullptr) {
            WIFI_LOGE("`%{public}s` inner communication error!", __func__);
            return WIFI_OPT_FAILED;
        }
        if (memcpy_s(cfgData, CFG_DATA_MAX_BYTES, dataBuffer, *getDatValidLen) != EOK) {
            WIFI_LOGD("`%{public}s` memcpy_s failed!", __func__);
            return WIFI_OPT_FAILED;
        }
    }
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiP2pProxy::Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType,
    char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen)
{
    if (setDataValidLen <= 0) {
        WIFI_LOGE("`%{public}s` parameter is error!", __func__);
        return WIFI_OPT_INVALID_PARAM;
    }
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
    data.WriteInt32(static_cast<int>(cfgType));
    data.WriteInt32(setDataValidLen);
    data.WriteBuffer(cfgData, setDataValidLen);
    int error = Remote()->SendRequest(WIFI_SVR_CMD_SET_PEER_WIFI_CFG, data, reply, option);
    if (error != ERR_NONE) {
        WIFI_LOGW("Set Attr(%{public}d) failed", WIFI_SVR_CMD_SET_PEER_WIFI_CFG);
        return WIFI_OPT_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return WIFI_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}

void WifiP2pProxy::OnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    WIFI_LOGD("Remote service is died!");
    mRemoteDied = true;
    g_wifiP2pCallbackStub.SetRemoteDied(true);
}
}  // namespace Wifi
}  // namespace OHOS
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
#include "wifi_p2p_stub.h"
#include "wifi_logger.h"
#include "wifi_errcode.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_p2p_callback_proxy.h"
#include "wifi_p2p_death_recipient.h"

DEFINE_WIFILOG_P2P_LABEL("WifiP2pStub");

namespace OHOS {
namespace Wifi {
WifiP2pStub::WifiP2pStub() : callback_(nullptr), mSingleCallback(false)
{
    InitHandleMap();
}

WifiP2pStub::~WifiP2pStub()
{}

void WifiP2pStub::InitHandleMap()
{
    handleFuncMap[WIFI_SVR_CMD_P2P_ENABLE] = &WifiP2pStub::OnEnableP2p;
    handleFuncMap[WIFI_SVR_CMD_P2P_DISABLE] = &WifiP2pStub::OnDisableP2p;
    handleFuncMap[WIFI_SVR_CMD_P2P_DISCOVER_DEVICES] = &WifiP2pStub::OnDiscoverDevices;
    handleFuncMap[WIFI_SVR_CMD_P2P_STOP_DISCOVER_DEVICES] = &WifiP2pStub::OnStopDiscoverDevices;
    handleFuncMap[WIFI_SVR_CMD_P2P_DISCOVER_SERVICES] = &WifiP2pStub::OnDiscoverServices;
    handleFuncMap[WIFI_SVR_CMD_P2P_STOP_DISCOVER_SERVICES] = &WifiP2pStub::OnStopDiscoverServices;
    handleFuncMap[WIFI_SVR_CMD_P2P_REQUEST_SERVICES] = &WifiP2pStub::OnRequestService;
    handleFuncMap[WIFI_SVR_CMD_P2P_PUT_LOCAL_SERVICES] = &WifiP2pStub::OnPutLocalP2pService;
    handleFuncMap[WIFI_SVR_CMD_P2P_DELETE_LOCAL_SERVICES] = &WifiP2pStub::OnDeleteLocalP2pService;
    handleFuncMap[WIFI_SVR_CMD_P2P_START_LISTEN] = &WifiP2pStub::OnStartP2pListen;
    handleFuncMap[WIFI_SVR_CMD_P2P_STOP_LISTEN] = &WifiP2pStub::OnStopP2pListen;
    handleFuncMap[WIFI_SVR_CMD_P2P_CREATE_GROUP] = &WifiP2pStub::OnCreateGroup;
    handleFuncMap[WIFI_SVR_CMD_P2P_REMOVE_GROUP] = &WifiP2pStub::OnRemoveGroup;
    handleFuncMap[WIFI_SVR_CMD_P2P_DELETE_GROUP] = &WifiP2pStub::OnDeleteGroup;
    handleFuncMap[WIFI_SVR_CMD_P2P_CONNECT] = &WifiP2pStub::OnP2pConnect;
    handleFuncMap[WIFI_SVR_CMD_P2P_CANCEL_CONNECT] = &WifiP2pStub::OnP2pCancelConnect;
    handleFuncMap[WIFI_SVR_CMD_P2P_QUERY_INFO] = &WifiP2pStub::OnQueryP2pLinkedInfo;
    handleFuncMap[WIFI_SVR_CMD_P2P_GET_CURRENT_GROUP] = &WifiP2pStub::OnGetCurrentGroup;
    handleFuncMap[WIFI_SVR_CMD_P2P_GET_ENABLE_STATUS] = &WifiP2pStub::OnGetP2pEnableStatus;
    handleFuncMap[WIFI_SVR_CMD_P2P_GET_DISCOVER_STATUS] = &WifiP2pStub::OnGetP2pDiscoverStatus;
    handleFuncMap[WIFI_SVR_CMD_P2P_GET_CONNECTED_STATUS] = &WifiP2pStub::OnGetP2pConnectedStatus;
    handleFuncMap[WIFI_SVR_CMD_P2P_QUERY_DEVICES] = &WifiP2pStub::OnQueryP2pDevices;
    handleFuncMap[WIFI_SVR_CMD_P2P_QUERY_GROUPS] = &WifiP2pStub::OnQueryP2pGroups;
    handleFuncMap[WIFI_SVR_CMD_P2P_QUERY_SERVICES] = &WifiP2pStub::OnQueryP2pServices;
    handleFuncMap[WIFI_SVR_CMD_P2P_REGISTER_CALLBACK] = &WifiP2pStub::OnRegisterCallBack;
    handleFuncMap[WIFI_SVR_CMD_GET_SUPPORTED_FEATURES] = &WifiP2pStub::OnGetSupportedFeatures;
    handleFuncMap[WIFI_SVR_CMD_P2P_SET_DEVICE_NAME] = &WifiP2pStub::OnSetP2pDeviceName;
    handleFuncMap[WIFI_SVR_CMD_P2P_SET_WFD_INFO] = &WifiP2pStub::OnSetP2pWfdInfo;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_APPLY_IP] = &WifiP2pStub::OnHid2dRequestGcIp;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_INCREASE] = &WifiP2pStub::OnHid2dSharedlinkIncrease;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_DECREASE] = &WifiP2pStub::OnHid2dSharedlinkDecrease;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_CREATE_GROUP] = &WifiP2pStub::OnHid2dCreateGroup;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_REMOVE_GC_GROUP] = &WifiP2pStub::OnHid2dRemoveGcGroup;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_CONNECT] = &WifiP2pStub::OnHid2dConnect;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_CONFIG_IP] = &WifiP2pStub::OnHid2dConfigIPAddr;
    handleFuncMap[WIFI_SVR_CMD_P2P_HID2D_RELEASE_IP] = &WifiP2pStub::OnHid2dReleaseIPAddr;
    handleFuncMap[WIFI_SVR_CMD_GET_P2P_RECOMMENDED_CHANNEL] = &WifiP2pStub::OnHid2dGetRecommendChannel;
    handleFuncMap[WIFI_SVR_CMD_GET_5G_CHANNEL_LIST] = &WifiP2pStub::OnHid2dGetChannelListFor5G;
    handleFuncMap[WIFI_SVR_CMD_GET_SELF_WIFI_CFG] = &WifiP2pStub::OnHid2dGetSelfWifiCfgInfo;
    handleFuncMap[WIFI_SVR_CMD_SET_PEER_WIFI_CFG] = &WifiP2pStub::OnHid2dSetPeerWifiCfgInfo;
    return;
}

int WifiP2pStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WIFI_LOGE("P2p stub token verification error");
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

void WifiP2pStub::OnEnableP2p(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = EnableP2p();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnDisableP2p(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = DisableP2p();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnDiscoverDevices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = DiscoverDevices();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnStopDiscoverDevices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = StopDiscoverDevices();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnDiscoverServices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = DiscoverServices();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnStopDiscoverServices(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = StopDiscoverServices();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnRequestService(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pDevice device;
    WifiP2pServiceRequest request;
    if (!ReadWifiP2pServiceRequest(data, device, request)) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return;
    }
    ErrCode ret = RequestService(device, request);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnPutLocalP2pService(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pServiceInfo config;
    if (!ReadWifiP2pServiceInfo(data, config)) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return;
    }
    ErrCode ret = PutLocalP2pService(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnDeleteLocalP2pService(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pServiceInfo config;
    if (!ReadWifiP2pServiceInfo(data, config)) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return;
    }
    ErrCode ret = DeleteLocalP2pService(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnStartP2pListen(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int period = data.ReadInt32();
    int interval = data.ReadInt32();
    ErrCode ret = StartP2pListen(period, interval);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnStopP2pListen(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = StopP2pListen();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnCreateGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pConfig config;
    ReadWifiP2pConfigData(data, config);
    ErrCode ret = CreateGroup(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnRemoveGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = RemoveGroup();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnDeleteGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pGroupInfo config;
    if (!ReadWifiP2pGroupData(data, config)) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return;
    }
    ErrCode ret = DeleteGroup(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnP2pConnect(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pConfig config;
    ReadWifiP2pConfigData(data, config);

    ErrCode ret = P2pConnect(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnP2pCancelConnect(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = P2pCancelConnect();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnQueryP2pLinkedInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pLinkedInfo config;
    ErrCode ret = QueryP2pLinkedInfo(config);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(static_cast<int>(config.GetConnectState()));
        reply.WriteBool(config.IsGroupOwner());
        reply.WriteCString(config.GetGroupOwnerAddress().c_str());
    }
    return;
}

void WifiP2pStub::OnGetCurrentGroup(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pGroupInfo config;
    ErrCode ret = GetCurrentGroup(config);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        WriteWifiP2pGroupData(reply, config);
    }
    return;
}

void WifiP2pStub::OnGetP2pEnableStatus(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int status = 0;
    ErrCode ret = GetP2pEnableStatus(status);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(status);
    }
    return;
}

void WifiP2pStub::OnGetP2pDiscoverStatus(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int status = 0;
    ErrCode ret = GetP2pDiscoverStatus(status);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(status);
    }
    return;
}

void WifiP2pStub::OnGetP2pConnectedStatus(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int status = 0;
    ErrCode ret = GetP2pConnectedStatus(status);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(status);
    }
    return;
}

void WifiP2pStub::OnQueryP2pDevices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<WifiP2pDevice> devives;
    ErrCode ret = QueryP2pDevices(devives);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    if (ret == WIFI_OPT_SUCCESS) {
        int size = devives.size();
        reply.WriteInt32(size);
        for (int i = 0; i < size; ++i) {
            WriteWifiP2pDeviceData(reply, devives[i]);
        }
    }
    return;
}

void WifiP2pStub::OnQueryP2pGroups(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<WifiP2pGroupInfo> groups;
    ErrCode ret = QueryP2pGroups(groups);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    if (ret == WIFI_OPT_SUCCESS) {
        int size = groups.size();
        reply.WriteInt32(size);
        for (int i = 0; i < size; ++i) {
            WriteWifiP2pGroupData(reply, groups[i]);
        }
    }
    return;
}

void WifiP2pStub::OnQueryP2pServices(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::vector<WifiP2pServiceInfo> services;
    ErrCode ret = QueryP2pServices(services);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    if (ret == WIFI_OPT_SUCCESS) {
        int size = services.size();
        reply.WriteInt32(size);
        for (int i = 0; i < size; ++i) {
            WriteWifiP2pServiceInfo(reply, services[i]);
        }
    }
    return;
}

bool WifiP2pStub::ReadWifiP2pServiceInfo(MessageParcel &data, WifiP2pServiceInfo &info)
{
    constexpr int MAX_QUERY_SIZE = 256;
    info.SetServiceName(data.ReadCString());
    info.SetDeviceAddress(data.ReadCString());
    info.SetServicerProtocolType(static_cast<P2pServicerProtocolType>(data.ReadInt32()));
    std::vector<std::string> queryList;
    int size = data.ReadInt32();
    if (size > MAX_QUERY_SIZE) {
        return false;
    }
    for (int i = 0; i < size; i++) {
        std::string str = data.ReadCString();
        queryList.push_back(str);
    }
    info.SetQueryList(queryList);
    return true;
}

bool WifiP2pStub::ReadWifiP2pServiceRequest(MessageParcel &data, WifiP2pDevice &device, WifiP2pServiceRequest &request)
{
    constexpr int MAX_QUERY_SIZE = 256;
    ReadWifiP2pDeviceData(data, device);
    request.SetProtocolType(static_cast<P2pServicerProtocolType>(data.ReadInt32()));
    request.SetTransactionId(data.ReadInt32());
    int size = data.ReadInt32();
    if (size > MAX_QUERY_SIZE) {
        return false;
    }
    std::vector<unsigned char> query;
    for (int i = 0; i < size; i++) {
        unsigned char chr = data.ReadInt8();
        query.push_back(chr);
    }
    request.SetQuery(query);
    return true;
}

void WifiP2pStub::WriteWifiP2pServiceInfo(MessageParcel &reply, const WifiP2pServiceInfo &info)
{
    reply.WriteCString(info.GetServiceName().c_str());
    reply.WriteCString(info.GetDeviceAddress().c_str());
    reply.WriteInt32(static_cast<int>(info.GetServicerProtocolType()));
    reply.WriteInt32(info.GetQueryList().size());
    std::vector<std::string> queryList = info.GetQueryList();
    for (auto it = queryList.begin(); it != queryList.end(); it++) {
        reply.WriteCString((*it).c_str());
    }
    return;
}

void WifiP2pStub::ReadWifiP2pDeviceData(MessageParcel &data, WifiP2pDevice &device)
{
    device.SetDeviceName(data.ReadCString());
    device.SetDeviceAddress(data.ReadCString());
    device.SetPrimaryDeviceType(data.ReadCString());
    device.SetSecondaryDeviceType(data.ReadCString());
    device.SetP2pDeviceStatus(static_cast<P2pDeviceStatus>(data.ReadInt32()));
    WifiP2pWfdInfo wfdInfo;
    wfdInfo.SetWfdEnabled(data.ReadBool());
    wfdInfo.SetDeviceInfo(data.ReadInt32());
    wfdInfo.SetCtrlPort(data.ReadInt32());
    wfdInfo.SetMaxThroughput(data.ReadInt32());
    device.SetWfdInfo(wfdInfo);
    device.SetWpsConfigMethod(data.ReadInt32());
    device.SetDeviceCapabilitys(data.ReadInt32());
    device.SetGroupCapabilitys(data.ReadInt32());
}

void WifiP2pStub::WriteWifiP2pDeviceData(MessageParcel &reply, const WifiP2pDevice &device)
{
    reply.WriteCString(device.GetDeviceName().c_str());
    reply.WriteCString(device.GetDeviceAddress().c_str());
    reply.WriteCString(device.GetPrimaryDeviceType().c_str());
    reply.WriteCString(device.GetSecondaryDeviceType().c_str());
    reply.WriteInt32(static_cast<int>(device.GetP2pDeviceStatus()));
    reply.WriteBool(device.GetWfdInfo().GetWfdEnabled());
    reply.WriteInt32(device.GetWfdInfo().GetDeviceInfo());
    reply.WriteInt32(device.GetWfdInfo().GetCtrlPort());
    reply.WriteInt32(device.GetWfdInfo().GetMaxThroughput());
    reply.WriteInt32(device.GetWpsConfigMethod());
    reply.WriteInt32(device.GetDeviceCapabilitys());
    reply.WriteInt32(device.GetGroupCapabilitys());
}

bool WifiP2pStub::ReadWifiP2pGroupData(MessageParcel &data, WifiP2pGroupInfo &info)
{
    constexpr int MAX_DEV_SIZE = 256;
    info.SetIsGroupOwner(data.ReadBool());
    WifiP2pDevice device;
    ReadWifiP2pDeviceData(data, device);
    info.SetOwner(device);
    info.SetPassphrase(data.ReadCString());
    info.SetInterface(data.ReadCString());
    info.SetGroupName(data.ReadCString());
    info.SetFrequency(data.ReadInt32());
    info.SetIsPersistent(data.ReadBool());
    info.SetP2pGroupStatus(static_cast<P2pGroupStatus>(data.ReadInt32()));
    info.SetNetworkId(data.ReadInt32());
    info.SetGoIpAddress(data.ReadCString());
    int size = data.ReadInt32();
    if (size > MAX_DEV_SIZE) {
        return false;
    }
    for (auto it = 0; it < size; ++it) {
        WifiP2pDevice cliDev;
        ReadWifiP2pDeviceData(data, cliDev);
        info.AddClientDevice(cliDev);
    }
    return true;
}

void WifiP2pStub::WriteWifiP2pGroupData(MessageParcel &reply, const WifiP2pGroupInfo &info)
{
    reply.WriteBool(info.IsGroupOwner());
    WriteWifiP2pDeviceData(reply, info.GetOwner());
    reply.WriteCString(info.GetPassphrase().c_str());
    reply.WriteCString(info.GetInterface().c_str());
    reply.WriteCString(info.GetGroupName().c_str());
    reply.WriteInt32(info.GetFrequency());
    reply.WriteBool(info.IsPersistent());
    reply.WriteInt32(static_cast<int>(info.GetP2pGroupStatus()));
    reply.WriteInt32(info.GetNetworkId());
    reply.WriteCString(info.GetGoIpAddress().c_str());
    std::vector<WifiP2pDevice> deviceVec = info.GetClientDevices();
    reply.WriteInt32(deviceVec.size());
    for (auto it = deviceVec.begin(); it != deviceVec.end(); ++it) {
        WriteWifiP2pDeviceData(reply, *it);
    }
}

void WifiP2pStub::ReadWifiP2pConfigData(MessageParcel &data, WifiP2pConfig &config)
{
    config.SetDeviceAddress(data.ReadCString());
    config.SetPassphrase(data.ReadCString());
    config.SetGroupName(data.ReadCString());
    config.SetGoBand(static_cast<GroupOwnerBand>(data.ReadInt32()));
    config.SetNetId(data.ReadInt32());
    config.SetGroupOwnerIntent(data.ReadInt32());
}

sptr<IWifiP2pCallback> WifiP2pStub::GetCallback() const
{
    return callback_;
}

void WifiP2pStub::OnRegisterCallBack(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = WIFI_OPT_FAILED;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            WIFI_LOGD("Failed to readRemoteObject!");
            break;
        }

        callback_ = iface_cast<IWifiP2pCallback>(remote);
        if (callback_ == nullptr) {
            callback_ = new (std::nothrow) WifiP2pCallbackProxy(remote);
            WIFI_LOGD("create new `WifiP2pCallbackProxy`!");
        }

        if (mSingleCallback) {
            ret = RegisterCallBack(callback_);
        } else {
            if (deathRecipient_ == nullptr) {
                deathRecipient_ = new (std::nothrow) WifiP2pDeathRecipient();
            }
            if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
                WIFI_LOGD("AddDeathRecipient!");
            }
            if (callback_ != nullptr) {
                WifiInternalEventDispatcher::GetInstance().AddP2pCallback(remote, callback_);
            }
            ret = WIFI_OPT_SUCCESS;
        }
    } while (0);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnGetSupportedFeatures(
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

void WifiP2pStub::OnSetP2pDeviceName(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::string deviceName = data.ReadCString();
    int ret = SetP2pDeviceName(deviceName);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return;
}

void WifiP2pStub::OnSetP2pWfdInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    WifiP2pWfdInfo wfdInfo;
    wfdInfo.SetWfdEnabled(data.ReadBool());
    wfdInfo.SetDeviceInfo(data.ReadInt32());
    wfdInfo.SetCtrlPort(data.ReadInt32());
    wfdInfo.SetMaxThroughput(data.ReadInt32());

    int ret = SetP2pWfdInfo(wfdInfo);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return;
}

void WifiP2pStub::OnHid2dRequestGcIp(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    std::string gcMac = data.ReadCString();
    std::string ipAddr;
    int ret = Hid2dRequestGcIp(gcMac, ipAddr);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteCString(ipAddr.c_str());
    }
}

void WifiP2pStub::OnHid2dSharedlinkIncrease(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    int ret = Hid2dSharedlinkIncrease();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dSharedlinkDecrease(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    int ret = Hid2dSharedlinkDecrease();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dCreateGroup(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    int frequency = data.ReadInt32();
    int type = data.ReadInt32();
    int ret = Hid2dCreateGroup(frequency, FreqType(type));
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dRemoveGcGroup(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    std::string gcIfName = data.ReadCString();
    int ret = Hid2dRemoveGcGroup(gcIfName);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dConnect(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    Hid2dConnectConfig config;
    config.SetSsid(data.ReadCString());
    config.SetBssid(data.ReadCString());
    config.SetPreSharedKey(data.ReadCString());
    config.SetFrequency(data.ReadInt32());
    config.SetDhcpMode(DhcpMode(data.ReadInt32()));

    int ret = Hid2dConnect(config);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dConfigIPAddr(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    IpAddrInfo ipInfo;
    std::string ifName = data.ReadCString();
    ipInfo.ip = data.ReadCString();
    ipInfo.gateway = data.ReadCString();
    ipInfo.netmask = data.ReadCString();
    int ret = Hid2dConfigIPAddr(ifName, ipInfo);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dReleaseIPAddr(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    std::string ifName = data.ReadCString();
    int ret = Hid2dReleaseIPAddr(ifName);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

void WifiP2pStub::OnHid2dGetRecommendChannel(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    RecommendChannelRequest request;
    RecommendChannelResponse response;
    request.remoteIfName = data.ReadCString();
    request.remoteIfMode = data.ReadInt32();
    request.localIfName = data.ReadCString();
    request.localIfMode = data.ReadInt32();
    request.prefBand = data.ReadInt32();
    request.prefBandwidth = PreferBandwidth(data.ReadInt32());
    ErrCode ret = Hid2dGetRecommendChannel(request, response);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32(static_cast<int>(response.status));
        reply.WriteInt32(response.index);
        reply.WriteInt32(response.centerFreq);
        reply.WriteInt32(response.centerFreq1);
        reply.WriteInt32(response.centerFreq2);
        reply.WriteInt32(response.bandwidth);
    }
}

void WifiP2pStub::OnHid2dGetChannelListFor5G(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    std::vector<int> vecChannelList;
    ErrCode ret = Hid2dGetChannelListFor5G(vecChannelList);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt32((int)vecChannelList.size());
        for (auto& channel : vecChannelList) {
            reply.WriteInt32(channel);
        }
    }
}

void WifiP2pStub::OnHid2dGetSelfWifiCfgInfo(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    int cfgType = data.ReadInt32();
    int len = 0;
    char cfgData[CFG_DATA_MAX_BYTES];
    if (memset_s(cfgData, CFG_DATA_MAX_BYTES, 0, CFG_DATA_MAX_BYTES) != EOK) {
        WIFI_LOGE("`%{public}s` memset_s failed!", __func__);
    }
    ErrCode ret = Hid2dGetSelfWifiCfgInfo(SelfCfgType(cfgType), cfgData, &len);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    reply.WriteInt32(len);
    if (ret == WIFI_OPT_SUCCESS && len > 0) {
        reply.WriteBuffer(cfgData, len);
    }
}

void WifiP2pStub::OnHid2dSetPeerWifiCfgInfo(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());

    char cfgData[CFG_DATA_MAX_BYTES];
    if (memset_s(cfgData, CFG_DATA_MAX_BYTES, 0, CFG_DATA_MAX_BYTES) != EOK) {
        WIFI_LOGE("`%{public}s` memset_s failed!", __func__);
    }
    int cfgType = data.ReadInt32();
    int len = data.ReadInt32();
    const char *dataBuffer = (const char *)data.ReadBuffer(len);
    if (memcpy_s(cfgData, CFG_DATA_MAX_BYTES, dataBuffer, len) != EOK) {
        WIFI_LOGE("`%{public}s` memcpy_s failed!", __func__);
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_FAILED);
        return;
    }
    ErrCode ret = Hid2dSetPeerWifiCfgInfo(PeerCfgType(cfgType), cfgData, len);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}

bool WifiP2pStub::IsSingleCallback() const
{
    return mSingleCallback;
}

void WifiP2pStub::SetSingleCallback(const bool isSingleCallback)
{
    mSingleCallback = true;
}
}  // namespace Wifi
}  // namespace OHOS
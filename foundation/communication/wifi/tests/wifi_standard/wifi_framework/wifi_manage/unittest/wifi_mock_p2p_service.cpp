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

#include "wifi_mock_p2p_service.h"
#include "wifi_log.h"

namespace OHOS {
namespace Wifi {
ErrCode WifiMockP2pService::EnableP2p()
{
    LOGI("Mock_p2p:EnableP2p");
    bool state = true;
    mCallback.OnP2pStateChangedEvent(state);
    WifiP2pDevice device;
    device.SetDeviceName("DeviceName1");
    device.SetDeviceAddress("DeviceAddress1");
    device.SetPrimaryDeviceType("PrimaryDeviceType1");
    device.SetSecondaryDeviceType("SecondaryDeviceType1");
    device.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);

    WifiP2pWfdInfo wfdInfo;
    wfdInfo.SetWfdEnabled(false);
    wfdInfo.SetDeviceInfo(TEST_ONE_THOUSAND);
    wfdInfo.SetCtrlPort(TEST_ONE_THOUSAND);
    wfdInfo.SetMaxThroughput(TEST_ONE_THOUSAND);
    device.SetWfdInfo(wfdInfo);

    device.SetWpsConfigMethod(TEST_ONE_THOUSAND);
    device.SetDeviceCapabilitys(TEST_ONE_THOUSAND);
    device.SetGroupCapabilitys(TEST_ONE_THOUSAND);
    mCallback.OnP2pThisDeviceChangedEvent(device);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::DisableP2p()
{
    LOGI("Mock_p2p:DisableP2p");
    bool state = false;
    mCallback.OnP2pStateChangedEvent(state);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::DiscoverDevices()
{
    LOGI("Mock_p2p:DiscoverDevices");
    std::vector<WifiP2pDevice> devices;
    WifiP2pDevice device1;
    device1.SetDeviceName("DeviceName1");
    device1.SetDeviceAddress("DeviceAddress1");
    device1.SetPrimaryDeviceType("PrimaryDeviceType1");
    device1.SetSecondaryDeviceType("SecondaryDeviceType1");
    device1.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    WifiP2pWfdInfo wfdInfo1;
    wfdInfo1.SetWfdEnabled(false);
    wfdInfo1.SetDeviceInfo(TEST_ONE_THOUSAND);
    wfdInfo1.SetCtrlPort(TEST_ONE_THOUSAND);
    wfdInfo1.SetMaxThroughput(TEST_ONE_THOUSAND);
    device1.SetWfdInfo(wfdInfo1);
    device1.SetWpsConfigMethod(TEST_ONE_THOUSAND);
    device1.SetDeviceCapabilitys(TEST_ONE_THOUSAND);
    device1.SetGroupCapabilitys(TEST_ONE_THOUSAND);
    devices.push_back(device1);
    WifiP2pDevice device2;
    device2.SetDeviceName("DeviceName2");
    device2.SetDeviceAddress("DeviceAddress2");
    device2.SetPrimaryDeviceType("PrimaryDeviceType2");
    device2.SetSecondaryDeviceType("SecondaryDeviceType2");
    device2.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    WifiP2pWfdInfo wfdInfo2;
    wfdInfo2.SetWfdEnabled(false);
    wfdInfo2.SetDeviceInfo(TEST_TWO_THOUSAND);
    wfdInfo2.SetCtrlPort(TEST_TWO_THOUSAND);
    wfdInfo2.SetMaxThroughput(TEST_TWO_THOUSAND);
    device2.SetWfdInfo(wfdInfo2);
    device2.SetWpsConfigMethod(TEST_TWO_THOUSAND);
    device2.SetDeviceCapabilitys(TEST_TWO_THOUSAND);
    device2.SetGroupCapabilitys(TEST_TWO_THOUSAND);
    devices.push_back(device2);

    mCallback.OnP2pPeersChangedEvent(devices);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::StopDiscoverDevices()
{
    LOGI("Mock_p2p:StopDiscoverDevices");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::DiscoverServices()
{
    LOGI("Mock_p2p:DiscoverServices");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::StopDiscoverServices()
{
    LOGI("Mock_p2p:StopDiscoverServices");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::RequestService(const WifiP2pDevice &device, const WifiP2pServiceRequest &request)
{
    LOGI("Mock_p2p:RequestService");
    LOGI("request.GetProtocolType() = %d", static_cast<int>(request.GetProtocolType()));
    LOGI("request.GetTransactionId() = %d", request.GetTransactionId());
    std::vector<unsigned char> query = request.GetQuery();
    for (auto it = query.begin(); it != query.end(); it++) {
        LOGI("vec_query = %d", *it);
    }
    LOGI("device.GetDeviceName() = %s", device.GetDeviceName().c_str());
    LOGI("device.GetDeviceAddress() = %s", device.GetDeviceAddress().c_str());
    LOGI("device.GetPrimaryDeviceType() = %s", device.GetPrimaryDeviceType().c_str());
    LOGI("device.GetSecondaryDeviceType() = %s", device.GetSecondaryDeviceType().c_str());
    LOGI("device.GetP2pDeviceStatus() = %d", static_cast<int>(device.GetP2pDeviceStatus()));

    LOGI("device.GetWfdInfo().GetWfdEnabled() = %s", device.GetWfdInfo().GetWfdEnabled() ? "true" : "false");
    LOGI("device.GetWfdInfo().GetDeviceInfo() = %d", device.GetWfdInfo().GetDeviceInfo());
    LOGI("device.GetWfdInfo().GetCtrlPort() = %d", device.GetWfdInfo().GetCtrlPort());
    LOGI("device.GetWfdInfo().GetMaxThroughput() = %d", device.GetWfdInfo().GetMaxThroughput());

    LOGI("device.GetWpsConfigMethod() = %d", device.GetWpsConfigMethod());
    LOGI("device.GetDeviceCapabilitys() = %d", device.GetDeviceCapabilitys());
    LOGI("device.GetGroupCapabilitys() = %d", device.GetGroupCapabilitys());
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::PutLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    LOGI("Mock_p2p:PutLocalP2pService");
    std::vector<WifiP2pServiceInfo> services;
    LOGI("srvInfo.GetServiceName() = %s", srvInfo.GetServiceName().c_str());
    LOGI("srvInfo.GetDeviceAddress() = %s", srvInfo.GetDeviceAddress().c_str());
    LOGI("srvInfo.GetServicerProtocolType() = %d", static_cast<int>(srvInfo.GetServicerProtocolType()));
    services.emplace_back(srvInfo);
    mCallback.OnP2pServicesChangedEvent(services);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::DeleteLocalP2pService(const WifiP2pServiceInfo &srvInfo)
{
    LOGI("Mock_p2p:DeleteLocalP2pService");
    std::vector<WifiP2pServiceInfo> services;
    LOGI("srvInfo.GetServiceName() = %s", srvInfo.GetServiceName().c_str());
    LOGI("srvInfo.GetDeviceAddress() = %s", srvInfo.GetDeviceAddress().c_str());
    LOGI("srvInfo.GetServicerProtocolType() = %d", static_cast<int>(srvInfo.GetServicerProtocolType()));
    services.emplace_back(srvInfo);
    mCallback.OnP2pServicesChangedEvent(services);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::StartP2pListen(int period, int interval)
{
    LOGI("Mock_p2p:StartP2pListen");
    LOGI("period = %d interval = %d\n", period, interval);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::StopP2pListen()
{
    LOGI("Mock_p2p:StopP2pListen");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::CreateGroup(const WifiP2pConfig &config)
{
    LOGI("Mock_p2p:CreateGroup");
    LOGI("config.GetDeviceAddress() = %s\n", config.GetDeviceAddress().c_str());
    LOGI("config.GetGoBand() = %d\n", config.GetGoBand());
    LOGI("config.GetNetId() = %d\n", config.GetNetId());
    LOGI("config.GetPassphrase() = %s\n", config.GetPassphrase().c_str());
    LOGI("config.GetGroupOwnerIntent() = %d\n", config.GetGroupOwnerIntent());
    LOGI("config.GetGroupName() = %s\n", config.GetGroupName().c_str());

    LOGI("config.GetWpsInfo().GetWpsMethod() = %d\n", static_cast<int>(config.GetWpsInfo().GetWpsMethod()));
    LOGI("config.GetWpsInfo().GetBssid() = %s\n", config.GetWpsInfo().GetBssid().c_str());
    LOGI("config.GetWpsInfo().GetPin() = %s\n", config.GetWpsInfo().GetPin().c_str());

    mCallback.OnP2pGroupsChangedEvent();
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::RemoveGroup()
{
    LOGI("Mock_p2p:RemoveGroup");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::DeleteGroup(const WifiP2pGroupInfo &group)
{
    LOGI("Mock_p2p:DeleteGroup");
    LOGI("group.IsGroupOwner() = %s\n", group.IsGroupOwner() ? "true" : "false");

    LOGI("group.GetOwner().GetDeviceName() = %s\n", group.GetOwner().GetDeviceName().c_str());
    LOGI("group.GetOwner().GetDeviceAddress() = %s\n", group.GetOwner().GetDeviceAddress().c_str());
    LOGI("group.GetOwner().GetPrimaryDeviceType() = %s\n", group.GetOwner().GetPrimaryDeviceType().c_str());
    LOGI("group.GetOwner().GetSecondaryDeviceType() = %s\n", group.GetOwner().GetSecondaryDeviceType().c_str());
    LOGI("group.GetOwner().GetP2pDeviceStatus() = %d\n", static_cast<int>(group.GetOwner().GetP2pDeviceStatus()));

    LOGI("group.GetOwner().GetWfdInfo().GetWfdEnabled() = %s\n",
        group.GetOwner().GetWfdInfo().GetWfdEnabled() ? "true" : "false");
    LOGI("group.GetOwner().GetWfdInfo().GetDeviceInfo() = %d\n", group.GetOwner().GetWfdInfo().GetDeviceInfo());
    LOGI("group.GetOwner().GetWfdInfo().GetCtrlPort() = %d\n", group.GetOwner().GetWfdInfo().GetCtrlPort());
    LOGI("group.GetOwner().GetWfdInfo().GetMaxThroughput() = %d\n", group.GetOwner().GetWfdInfo().GetMaxThroughput());

    LOGI("group.GetOwner().GetWpsConfigMethod() = %d\n", group.GetOwner().GetWpsConfigMethod());
    LOGI("group.GetOwner().GetDeviceCapabilitys() = %d\n", group.GetOwner().GetDeviceCapabilitys());
    LOGI("group.GetOwner().GetGroupCapabilitys() = %d\n", group.GetOwner().GetGroupCapabilitys());

    LOGI("group.GetPassphrase() = %s\n", group.GetPassphrase().c_str());
    LOGI("group.GetInterface() = %s\n", group.GetInterface().c_str());
    LOGI("group.GetGroupName() = %s\n", group.GetGroupName().c_str());
    LOGI("group.GetNetworkId() = %d\n", group.GetNetworkId());
    LOGI("group.GetFrequency() = %d\n", group.GetFrequency());
    LOGI("group.IsPersistent() = %s\n", group.IsPersistent() ? "true" : "false");
    LOGI("group.GetP2pGroupStatus() = %d\n", static_cast<int>(group.GetP2pGroupStatus()));
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::P2pConnect(const WifiP2pConfig &config)
{
    LOGI("Mock_p2p:P2pConnect");
    LOGI("config.GetDeviceAddress() = %s\n", config.GetDeviceAddress().c_str());
    LOGI("config.GetGoBand() = %d\n", config.GetGoBand());
    LOGI("config.GetNetId() = %d\n", config.GetNetId());
    LOGI("config.GetPassphrase() = %s\n", config.GetPassphrase().c_str());

    LOGI("config.GetWpsInfo().GetWpsMethod() = %d\n", static_cast<int>(config.GetWpsInfo().GetWpsMethod()));
    LOGI("config.GetWpsInfo().GetBssid() = %s\n", config.GetWpsInfo().GetBssid().c_str());
    LOGI("config.GetWpsInfo().GetPin() = %s\n", config.GetWpsInfo().GetPin().c_str());

    LOGI("config.GetGroupOwnerIntent() = %d\n", config.GetGroupOwnerIntent());
    LOGI("config.GetGroupName() = %s\n", config.GetGroupName().c_str());
    WifiP2pLinkedInfo linkedInfo;
    linkedInfo.SetGroupFormed(true);
    linkedInfo.SetIsGroupOwner(true);
    linkedInfo.SetIsGroupOwnerAddress("IsGroupOwnerAddress");
    mCallback.OnP2pConnectionChangedEvent(linkedInfo);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::P2pCancelConnect()
{
    LOGI("Mock_p2p:P2pCancelConnect");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::QueryP2pLinkedInfo(WifiP2pLinkedInfo &linkedInfo)
{
    LOGI("Mock_p2p:QueryP2pLinkedInfo");
    linkedInfo.SetGroupFormed(true);
    linkedInfo.SetIsGroupOwner(true);
    linkedInfo.SetIsGroupOwnerAddress("IsGroupOwnerAddress");
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::GetCurrentGroup(WifiP2pGroupInfo &group)
{
    LOGI("Mock_p2p:GetCurrentGroup");
    group.SetGroupName("GroupName");

    WifiP2pDevice owner;
    owner.SetDeviceName("DeviceName");
    owner.SetDeviceAddress("DeviceAddress");
    owner.SetPrimaryDeviceType("PrimaryDeviceType");
    owner.SetSecondaryDeviceType("SecondaryDeviceType");
    owner.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    WifiP2pWfdInfo wfdInfo;
    wfdInfo.SetWfdEnabled(false);
    wfdInfo.SetDeviceInfo(TEST_ONE_THOUSAND);
    wfdInfo.SetCtrlPort(TEST_ONE_THOUSAND);
    wfdInfo.SetMaxThroughput(TEST_ONE_THOUSAND);
    owner.SetWfdInfo(wfdInfo);
    owner.SetWpsConfigMethod(TEST_ONE_THOUSAND);
    owner.SetDeviceCapabilitys(TEST_ONE_THOUSAND);
    owner.SetGroupCapabilitys(TEST_ONE_THOUSAND);
    group.SetOwner(owner);
    group.SetIsGroupOwner(true);

    group.SetPassphrase("Passphrase");
    group.SetInterface("Interface");
    group.SetGroupName("GroupName");
    group.SetNetworkId(TEST_ONE_THOUSAND);
    group.SetFrequency(TEST_ONE_THOUSAND);
    group.SetIsPersistent(true);
    group.SetP2pGroupStatus(P2pGroupStatus::GS_INVALID);
    std::vector<WifiP2pDevice> clientDevices;
    clientDevices.push_back(owner);
    group.SetClientDevices(clientDevices);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::GetP2pEnableStatus(int &status)
{
    LOGI("Mock_p2p:GetP2pEnableStatus");
    status = static_cast<int>(P2pState::P2P_STATE_STARTING);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::GetP2pDiscoverStatus(int &status)
{
    LOGI("Mock_p2p:GetP2pDiscoverStatus");
    status = static_cast<int>(P2pDiscoverState::P2P_DISCOVER_STARTING);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::GetP2pConnectedStatus(int &status)
{
    LOGI("Mock_p2p:GetP2pConnectedStatus");
    status = static_cast<int>(P2pConnectedState::P2P_CONNECTED_STARTING);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::QueryP2pDevices(std::vector<WifiP2pDevice> &devives)
{
    LOGI("Mock_p2p:QueryP2pDevices");
    WifiP2pDevice device1;
    device1.SetDeviceName("DeviceName1");
    device1.SetDeviceAddress("DeviceAddress1");
    device1.SetPrimaryDeviceType("PrimaryDeviceType1");
    device1.SetSecondaryDeviceType("SecondaryDeviceType1");
    device1.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    WifiP2pWfdInfo wfdInfo1;
    wfdInfo1.SetWfdEnabled(false);
    wfdInfo1.SetDeviceInfo(TEST_ONE_THOUSAND);
    wfdInfo1.SetCtrlPort(TEST_ONE_THOUSAND);
    wfdInfo1.SetMaxThroughput(TEST_ONE_THOUSAND);
    device1.SetWfdInfo(wfdInfo1);
    device1.SetWpsConfigMethod(TEST_ONE_THOUSAND);
    device1.SetDeviceCapabilitys(TEST_ONE_THOUSAND);
    device1.SetGroupCapabilitys(TEST_ONE_THOUSAND);
    devives.push_back(device1);
    WifiP2pDevice device2;
    device2.SetDeviceName("DeviceName2");
    device2.SetDeviceAddress("DeviceAddress2");
    device2.SetPrimaryDeviceType("PrimaryDeviceType2");
    device2.SetSecondaryDeviceType("SecondaryDeviceType2");
    device2.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    WifiP2pWfdInfo wfdInfo2;
    wfdInfo2.SetWfdEnabled(false);
    wfdInfo2.SetDeviceInfo(TEST_TWO_THOUSAND);
    wfdInfo2.SetCtrlPort(TEST_TWO_THOUSAND);
    wfdInfo2.SetMaxThroughput(TEST_TWO_THOUSAND);
    device2.SetWfdInfo(wfdInfo2);
    device2.SetWpsConfigMethod(TEST_TWO_THOUSAND);
    device2.SetDeviceCapabilitys(TEST_TWO_THOUSAND);
    device2.SetGroupCapabilitys(TEST_TWO_THOUSAND);
    devives.push_back(device2);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::QueryP2pGroups(std::vector<WifiP2pGroupInfo> &groups)
{
    LOGI("Mock_p2p:QueryP2pGroups");
    WifiP2pGroupInfo group1;
    group1.SetGroupName("GroupName1");
    WifiP2pDevice owner1;
    owner1.SetDeviceName("DeviceName1");
    owner1.SetDeviceAddress("DeviceAddress1");
    owner1.SetPrimaryDeviceType("PrimaryDeviceType1");
    owner1.SetSecondaryDeviceType("SecondaryDeviceType1");
    owner1.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    WifiP2pWfdInfo wfdInfo1;
    wfdInfo1.SetWfdEnabled(false);
    wfdInfo1.SetDeviceInfo(TEST_ONE_THOUSAND);
    wfdInfo1.SetCtrlPort(TEST_ONE_THOUSAND);
    wfdInfo1.SetMaxThroughput(TEST_ONE_THOUSAND);
    owner1.SetWfdInfo(wfdInfo1);
    owner1.SetWpsConfigMethod(TEST_ONE_THOUSAND);
    owner1.SetDeviceCapabilitys(TEST_ONE_THOUSAND);
    owner1.SetGroupCapabilitys(TEST_ONE_THOUSAND);
    group1.SetOwner(owner1);
    group1.SetIsGroupOwner(true);
    group1.SetPassphrase("Passphrase1");
    group1.SetInterface("Interface1");
    group1.SetGroupName("GroupName1");
    group1.SetNetworkId(TEST_ONE_THOUSAND);
    group1.SetFrequency(TEST_ONE_THOUSAND);
    group1.SetIsPersistent(true);
    group1.SetP2pGroupStatus(P2pGroupStatus::GS_CREATED);
    std::vector<WifiP2pDevice> clientDevices1;
    clientDevices1.push_back(owner1);
    group1.SetClientDevices(clientDevices1);
    groups.push_back(group1);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::QueryP2pServices(std::vector<WifiP2pServiceInfo> &services)
{
    LOGI("Mock_p2p:QueryP2pServices");
    OHOS::WifiP2pServiceInfo service1;
    service1.SetServiceName("ServiceName1");
    service1.SetDeviceAddress("00:xx:xx:xx:xx:00");
    service1.SetServicerProtocolType(P2pServicerProtocolType::SERVICE_TYPE_ALL);
    services.push_back(service1);

    OHOS::WifiP2pServiceInfo service2;
    service2.SetServiceName("ServiceName2");
    service1.SetDeviceAddress("00:xx:xx:xx:xx:00");
    service1.SetServicerProtocolType(P2pServicerProtocolType::SERVICE_TYPE_ALL);
    services.push_back(service2);

    return WIFI_OPT_SUCCESS;
}

ErrCode WifiMockP2pService::RegisterP2pServiceCallbacks(const IP2pServiceCallbacks &callbacks)
{
    mCallback = callbacks;
    return WIFI_OPT_SUCCESS;
}

extern "C" IP2pService *Create(void)
{
    return new (std::nothrow) WifiMockP2pService();
}

extern "C" void Destroy(IP2pService *p)
{
    delete p;
    p = nullptr;
}
}  // namespace Wifi
}  // namespace OHOS
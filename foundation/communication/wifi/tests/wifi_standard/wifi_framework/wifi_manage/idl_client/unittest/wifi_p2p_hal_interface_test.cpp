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
#include "wifi_p2p_hal_interface_test.h"
#include <gtest/gtest.h>
#include "wifi_p2p_hal_interface.h"
#include "wifi_p2p_msg.h"
#include "wifi_log.h"
#include "wifi_global_func.h"
#undef LOG_TAG
#define LOG_TAG "OHWIFI_IDLCLIENT_P2P_TEST"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
void onConnectSupplicant(int status)
{
    LOGD("onConnectSupplicant...status: %d\n", status);
}

void onDeviceFound(const IdlP2pDeviceFound &info)
{
    LOGD("onDeviceFound...deviceCapabilities: %d\n", info.deviceCapabilities);
}

void onDeviceLost(const std::string &address)
{
    LOGD("onDeviceLost...address: %s\n", address.c_str());
}

void onGoNegotiationRequest(const std::string &srcAddr, short passId)
{
    LOGD("onGoNegotiationRequest...srcAddr: %s, passId: %d\n", srcAddr.c_str(), passId);
}

void onGoNegotiationSuccess()
{
    LOGD("onGoNegotiationSuccess...\n");
}

void onGoNegotiationFailure(int status)
{
    LOGD("onGoNegotiationFailure...status: %d\n", status);
}

void onInvitationReceived(const IdlP2pInvitationInfo &info)
{
    LOGD("onInvitationReceived...operatingFrequency: %d\n", info.operatingFrequency);
}

void onInvitationResult(const std::string &bssid, int status)
{
    LOGD("onInvitationResult...bssid: %s, status: %d\n", bssid.c_str(), status);
}

void onGroupFormationSuccess()
{
    LOGD("onGroupFormationSuccess...\n");
}

void onGroupFormationFailure(const std::string &reason)
{
    LOGD("onGroupFormationFailure...reason: %s\n", reason.c_str());
}

void onGroupStarted(const IdlP2pGroupInfo &info)
{
    LOGD("onGroupStarted...frequency: %d\n", info.frequency);
}

void onGroupRemoved(const std::string &groupIfName, bool isGo)
{
    LOGD("onGroupRemoved...groupIfName: %s, isGo: %d\n", groupIfName.c_str(), isGo);
}

void onProvisionDiscoveryPbcRequest(const std::string &address)
{
    LOGD("onProvisionDiscoveryPbcRequest...address: %s\n", address.c_str());
}

void onProvisionDiscoveryPbcResponse(const std::string &address)
{
    LOGD("onProvisionDiscoveryPbcResponse...address: %s\n", address.c_str());
}

void onProvisionDiscoveryEnterPin(const std::string &address)
{
    LOGD("onProvisionDiscoveryEnterPin...address: %s\n", address.c_str());
}

void onProvisionDiscoveryShowPin(const std::string &address, const std::string &pin)
{
    LOGD("onProvisionDiscoveryShowPin...address: %s, pin: %s\n", address.c_str(), pin.c_str());
}

void onProvisionDiscoveryFailure()
{
    LOGD("onProvisionDiscoveryFailure...\n");
}

void onFindStopped()
{
    LOGD("onFindStopped...\n");
}

void onServiceDiscoveryResponse(
    const std::string &srcAddress, short updateIndicator, const std::vector<unsigned char> &tlvs)
{
    LOGD("onServiceDiscoveryResponse...address: %s, updateIndicator: %d, tlvsLength: %zu\n",
        srcAddress.c_str(),
        updateIndicator,
        tlvs.size());
}

void onStaDeauthorized(const std::string &address)
{
    LOGD("onStaDeauthorized...address: %s\n", address.c_str());
}

void onStaAuthorized(const std::string &address)
{
    LOGD("onStaAuthorized...address: %s\n", address.c_str());
}

void connectSupplicantFailed()
{
    LOGD("connectSupplicantFailed...\n");
}

void onP2pServDiscReq(const IdlP2pServDiscReqInfo &info)
{
    LOGD("onP2pServDiscReq    \n");
    LOGD("          service discovery freq: %d\n", info.freq);
    LOGD("          service discovery dialogToken: %d\n", info.dialogToken);
    LOGD("          service discovery updateIndic: %d\n", info.updateIndic);
    LOGD("          service discovery mac: %s\n", info.mac.c_str());
}

void WifiP2pHalInterfaceTest::SetUpTestCase()
{
    P2pHalCallback callbacks;
    callbacks.onConnectSupplicant = onConnectSupplicant;
    callbacks.onDeviceLost = onDeviceLost;
    callbacks.onGoNegotiationRequest = onGoNegotiationRequest;
    callbacks.onGoNegotiationSuccess = onGoNegotiationSuccess;
    callbacks.onGoNegotiationFailure = onGoNegotiationFailure;
    callbacks.onInvitationReceived = onInvitationReceived;
    callbacks.onInvitationResult = onInvitationResult;
    callbacks.onGroupFormationSuccess = onGroupFormationSuccess;
    callbacks.onGroupFormationFailure = onGroupFormationFailure;
    callbacks.onGroupStarted = onGroupStarted;
    callbacks.onGroupRemoved = onGroupRemoved;
    callbacks.onProvisionDiscoveryPbcRequest = onProvisionDiscoveryPbcRequest;
    callbacks.onProvisionDiscoveryPbcResponse = onProvisionDiscoveryPbcResponse;
    callbacks.onProvisionDiscoveryEnterPin = onProvisionDiscoveryEnterPin;
    callbacks.onProvisionDiscoveryShowPin = onProvisionDiscoveryShowPin;
    callbacks.onProvisionDiscoveryFailure = onProvisionDiscoveryFailure;
    callbacks.onFindStopped = onFindStopped;
    callbacks.onServiceDiscoveryResponse = onServiceDiscoveryResponse;
    callbacks.onStaDeauthorized = onStaDeauthorized;
    callbacks.onStaAuthorized = onStaAuthorized;
    callbacks.connectSupplicantFailed = connectSupplicantFailed;
    callbacks.onP2pServDiscReq = onP2pServDiscReq;
    WifiP2PHalInterface::GetInstance().RegisterP2pCallback(callbacks);
}

void WifiP2pHalInterfaceTest::TearDownTestCase()
{
    P2pHalCallback callbacks;
    WifiP2PHalInterface::GetInstance().RegisterP2pCallback(callbacks);
}

HWTEST_F(WifiP2pHalInterfaceTest, P2pStart_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().StartP2p());
}

HWTEST_F(WifiP2pHalInterfaceTest, SetupWpsPbc_SUCCESS, TestSize.Level1)
{
    std::string groupInterface = "groupInterface";
    std::string bssid = "bssid";

    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().StartWpsPbc(groupInterface, bssid));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetupWpsPin_SUCCESS, TestSize.Level1)
{
    std::string groupInterface = "groupInterface";
    std::string bssid;
    std::string pin = "adc123456";
    std::string result;

    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().StartWpsPin(groupInterface, bssid, pin, result));
    pin.clear();
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK,
        WifiP2PHalInterface::GetInstance().StartWpsPin(groupInterface, bssid, pin, result));
}

HWTEST_F(WifiP2pHalInterfaceTest, RemoveNetwork_SUCCESS, TestSize.Level1)
{
    int networkId = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().RemoveNetwork(networkId));
}

HWTEST_F(WifiP2pHalInterfaceTest, ListNetworksTest, TestSize.Level1)
{
    std::map<int, WifiP2pGroupInfo> mapGroups;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().ListNetworks(mapGroups));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetWpsDeviceName_SUCCESS, TestSize.Level1)
{
    std::string name = "testName";
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetP2pDeviceName(name));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetWpsDeviceType_SUCCESS, TestSize.Level1)
{
    std::string type = "testType";
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetP2pDeviceType(type));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetWpsConfigMethods_SUCCESS, TestSize.Level1)
{
    std::string config = "testConfig";
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetP2pConfigMethods(config));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetSsidPostfixName_SUCCESS, TestSize.Level1)
{
    std::string postfixName = "testPostfixName";
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetP2pSsidPostfix(postfixName));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetGroupMaxIdle_SUCCESS, TestSize.Level1)
{
    std::string groupInterface = "testGroupName";
    size_t time = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetP2pGroupIdle(groupInterface, time));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetPowerSave_SUCCESS, TestSize.Level1)
{
    std::string groupInterface;
    bool enable = false;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetP2pPowerSave(groupInterface, enable));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetWfdEnable_SUCCESS, TestSize.Level1)
{
    bool enable = false;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetWfdEnable(enable));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetWfdDeviceConfig_SUCCESS, TestSize.Level1)
{
    std::string config;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetWfdDeviceConfig(config));
}

HWTEST_F(WifiP2pHalInterfaceTest, StartP2pFind_SUCCESS, TestSize.Level1)
{
    size_t timeout = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pFind(timeout));
}

HWTEST_F(WifiP2pHalInterfaceTest, StopP2pFind_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pStopFind());
}

HWTEST_F(WifiP2pHalInterfaceTest, SetExtListen_SUCCESS, TestSize.Level1)
{
    bool enable = false;
    size_t period = 0;
    size_t interval = 0;
    EXPECT_EQ(
        WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pConfigureListen(enable, period, interval));
    enable = true;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().P2pConfigureListen(enable, period, interval));
    period = 65536;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().P2pConfigureListen(enable, period, interval));
    period = 10;
    interval = 65536;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().P2pConfigureListen(enable, period, interval));
    interval = 60;
    EXPECT_EQ(
        WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pConfigureListen(enable, period, interval));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetListenChannel_SUCCESS, TestSize.Level1)
{
    size_t channel = 0;
    unsigned char regClass = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetListenChannel(channel, regClass));
}

HWTEST_F(WifiP2pHalInterfaceTest, P2pFlush_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pFlush());
}

HWTEST_F(WifiP2pHalInterfaceTest, Connect_SUCCESS, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    bool isJoinExistingGroup = false;
    std::string pin;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().Connect(config, isJoinExistingGroup, pin));
    config.SetDeviceAddress("00:00:00:00:00:00");
    WpsInfo wpsInfo;
    wpsInfo.SetPin("12345678");
    wpsInfo.SetWpsMethod(WpsMethod::WPS_METHOD_PBC);
    config.SetWpsInfo(wpsInfo);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().Connect(config, isJoinExistingGroup, pin));
    wpsInfo.SetPin("");
    config.SetWpsInfo(wpsInfo);
    EXPECT_EQ(
        WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().Connect(config, isJoinExistingGroup, pin));
    isJoinExistingGroup = true;
    EXPECT_EQ(
        WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().Connect(config, isJoinExistingGroup, pin));
}

HWTEST_F(WifiP2pHalInterfaceTest, CancelConnect_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().CancelConnect());
}

HWTEST_F(WifiP2pHalInterfaceTest, ProvisionDiscovery_SUCCESS, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().ProvisionDiscovery(config));
}

HWTEST_F(WifiP2pHalInterfaceTest, AddToGroup_SUCCESS, TestSize.Level1)
{
    bool isPersistent = false;
    int networkId = 0;
    int freq = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().GroupAdd(isPersistent, networkId, freq));
}

HWTEST_F(WifiP2pHalInterfaceTest, RemoveGroup_SUCCESS, TestSize.Level1)
{
    std::string groupInterface;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().GroupRemove(groupInterface));
}

HWTEST_F(WifiP2pHalInterfaceTest, Invite_SUCCESS, TestSize.Level1)
{
    WifiP2pGroupInfo group;
    std::string deviceAddr;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().Invite(group, deviceAddr));
}

HWTEST_F(WifiP2pHalInterfaceTest, Reinvoke_SUCCESS, TestSize.Level1)
{
    int networkId = 13;
    std::string deviceAddr;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().Reinvoke(networkId, deviceAddr));
}

HWTEST_F(WifiP2pHalInterfaceTest, GetDeviceAddress_SUCCESS, TestSize.Level1)
{
    std::string deviceAddress;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().GetDeviceAddress(deviceAddress));
}

HWTEST_F(WifiP2pHalInterfaceTest, GetGroupCapability_SUCCESS, TestSize.Level1)
{
    std::string deviceAddress;
    uint32_t capacity = 0;
    EXPECT_EQ(
        WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().GetGroupCapability(deviceAddress, capacity));
}

HWTEST_F(WifiP2pHalInterfaceTest, P2pServiceAddTest, TestSize.Level1)
{
    WifiP2pServiceInfo info;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pServiceAdd(info));
    std::vector<std::string> vec;
    vec.push_back("upnp 0");
    info.SetQueryList(vec);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_FAILED, WifiP2PHalInterface::GetInstance().P2pServiceAdd(info));
    vec.clear();
    vec.push_back("upnpp 1 letusplaygame");
    info.SetQueryList(vec);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_FAILED, WifiP2PHalInterface::GetInstance().P2pServiceAdd(info));
    vec.clear();
    vec.push_back("upnp 0 letusplaygames");
    vec.push_back("bonjour from china");
    info.SetQueryList(vec);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pServiceAdd(info));
}

HWTEST_F(WifiP2pHalInterfaceTest, P2pServiceRemoveTest, TestSize.Level1)
{
    WifiP2pServiceInfo info;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pServiceRemove(info));
    std::vector<std::string> vec;
    vec.push_back("upnp 0");
    info.SetQueryList(vec);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_FAILED, WifiP2PHalInterface::GetInstance().P2pServiceRemove(info));
    vec.clear();
    vec.push_back("upnpp 1 letusplaygame");
    info.SetQueryList(vec);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_FAILED, WifiP2PHalInterface::GetInstance().P2pServiceRemove(info));
    vec.clear();
    vec.push_back("upnp 0 letusplaygames");
    vec.push_back("bonjour from china");
    info.SetQueryList(vec);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().P2pServiceRemove(info));
}

HWTEST_F(WifiP2pHalInterfaceTest, FlushService_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().FlushService());
}

HWTEST_F(WifiP2pHalInterfaceTest, ReqServiceDiscovery_SUCCESS, TestSize.Level1)
{
    std::string macAddr;
    std::vector<unsigned char> tlvs;
    std::string reqID;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().ReqServiceDiscovery(macAddr, tlvs, reqID));
    macAddr = "00:00:00:00:00:00";
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().ReqServiceDiscovery(macAddr, tlvs, reqID));
    tlvs.push_back('a');
    EXPECT_EQ(
        WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().ReqServiceDiscovery(macAddr, tlvs, reqID));
}

HWTEST_F(WifiP2pHalInterfaceTest, CancalReqServiceDiscovery_SUCCESS, TestSize.Level1)
{
    std::string id;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().CancelReqServiceDiscovery(id));
}

HWTEST_F(WifiP2pHalInterfaceTest, StoreConfig_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SaveConfig());
}

HWTEST_F(WifiP2pHalInterfaceTest, SetRandomMacAddr_SUCCESS, TestSize.Level1)
{
    bool enable = false;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetRandomMacAddr(enable));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetMiracastType_SUCCESS, TestSize.Level1)
{
    int type = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetMiracastMode(type));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetPersistentReconnectTest, TestSize.Level1)
{
    int mode = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetPersistentReconnect(mode));
}

HWTEST_F(WifiP2pHalInterfaceTest, RespServiceDiscoveryTest, TestSize.Level1)
{
    WifiP2pDevice device;
    int frequency = 0;
    int dialogToken = 0;
    std::vector<unsigned char> tlvs;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_INVALID_PARAM,
        WifiP2PHalInterface::GetInstance().RespServiceDiscovery(device, frequency, dialogToken, tlvs));
    tlvs.push_back(0x01);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK,
        WifiP2PHalInterface::GetInstance().RespServiceDiscovery(device, frequency, dialogToken, tlvs));
}

HWTEST_F(WifiP2pHalInterfaceTest, SetServiceDiscoveryExternalTest, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().SetServiceDiscoveryExternal(false));
}

HWTEST_F(WifiP2pHalInterfaceTest, P2pStop_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, WifiP2PHalInterface::GetInstance().StopP2p());
}
}  // namespace Wifi
}  // namespace OHOS
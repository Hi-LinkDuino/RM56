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
#include "wifi_hal_wpa_p2p_test.h"
#include "securec.h"
#include "mock_wpa_ctrl.h"
#include "wifi_hal_crpc_server.h"
#include "wifi_hal_common_func.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "WifiHalWpaP2pTest"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
const std::string g_rpcSockPath = "./unix_sock_test.sock";
WifiWpaInterface *WifiHalWpaP2pTest::mGlobalInterface = nullptr;
WifiWpaP2pInterface *WifiHalWpaP2pTest::mInterface = nullptr;
RpcServer *WifiHalWpaP2pTest::mServer = nullptr;

void WifiHalWpaP2pTest::SetUpTestCase()
{
    if (access(g_rpcSockPath.c_str(), 0) == 0) {
        unlink(g_rpcSockPath.c_str());
    }
    mServer = CreateRpcServer(g_rpcSockPath.c_str());
    if (mServer == nullptr) {
        printf("Init rpc server failed!");
        exit(-1);
    }
    InitCallbackMsg();
    SetRpcServerInited(mServer);
    MockInitGlobalCmd();
    MockInitP2pSupportedCmd();
    mGlobalInterface = GetWifiWapGlobalInterface();
    mGlobalInterface->wpaCliConnect(mGlobalInterface);
    mInterface = GetWifiWapP2pInterface();
}

void WifiHalWpaP2pTest::TearDownTestCase()
{
    sleep(1);
    RelesaeWpaP2pInterface();
    if (mServer != nullptr) {
        ReleaseRpcServer(mServer);
        mServer = nullptr;
    }
    SetRpcServerInited(NULL);
    ReleaseCallbackMsg();
}

HWTEST_F(WifiHalWpaP2pTest, GetWifiWpaP2pInterfaceTest, TestSize.Level1)
{
    WifiWpaP2pInterface *p = GetWifiWapP2pInterface();
    EXPECT_TRUE(p == mInterface);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetWpsNameTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsName(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SET");
    char name[] = "p2p_device";
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsName(mInterface, name) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsName(mInterface, name) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsName(mInterface, name) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetWpsDeviceTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsName(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SET");
    char type[] = "p2p_device_type";
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsDeviceType(mInterface, type) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsDeviceType(mInterface, type) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsDeviceType(mInterface, type) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetWpsConfigMethodsTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsConfigMethods(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SET");
    char methods[] = "p2p_config_method";
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsConfigMethods(mInterface, methods) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsConfigMethods(mInterface, methods) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWpsConfigMethods(mInterface, methods) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetSsidPostfixNameTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetSsidPostfixName(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_SET");
    char postfix[] = "p2p_postfix_name";
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetSsidPostfixName(mInterface, postfix) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetSsidPostfixName(mInterface, postfix) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetSsidPostfixName(mInterface, postfix) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdGetDeviceAddressTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGetDeviceAddress(nullptr, nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("STATUS");
    char address[18] = {0};
    int size = 18;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGetDeviceAddress(mInterface, address, size) != P2P_SUP_ERRCODE_SUCCESS);
    char buf1[] = "wpa_state=SCANNING\n"
                  "ip_address=192.168.xxx.xxx\n"
                  "freq=1024\n"
                  "key_mgmt=WPA-PSK\n";
    MockSetWpaExpectCmdResponse("STATUS", buf1);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGetDeviceAddress(mInterface, address, size) != P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(strlen(address) == 0);
    char buf2[] = "wpa_state=SCANNING\n"
                  "ip_address=192.168.xxx.xxx\n"
                  "freq=1024\n"
                  "key_mgmt=WPA-PSK\n"
                  "p2p_device_address=00:00:00:00:00:00\n";
    MockSetWpaExpectCmdResponse("STATUS", buf2);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGetDeviceAddress(mInterface, address, size) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(strcmp(address, "00:00:00:00:00:00") == 0);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdFlushTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlush(nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_FLUSH");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlush(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_FLUSH", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlush(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_FLUSH", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlush(mInterface) == P2P_SUP_ERRCODE_SUCCESS);
    char replyDeviceLost[] = "P2P-DEVICE-LOST p2p_dev_addr=00:00:00:00:00:00\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDeviceLost);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdFlushServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlushService(nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_SERVICE_FLUSH");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlushService(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERVICE_FLUSH", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlushService(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERVICE_FLUSH", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdFlushService(mInterface) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdP2pStopFindTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pStopFind(nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_STOP_FIND");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pStopFind(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_STOP_FIND", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pStopFind(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_STOP_FIND", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pStopFind(mInterface) == P2P_SUP_ERRCODE_SUCCESS);
    char replyFindStop[] = "P2P-FIND-STOPPED \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyFindStop);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdP2pRemoveGroupTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pRemoveGroup(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    char groupname[] = "p2p-dev-p2p0";
    MockEraseSupportedCmd("P2P_GROUP_REMOVE");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pRemoveGroup(mInterface, groupname) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_GROUP_REMOVE", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pRemoveGroup(mInterface, groupname) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_GROUP_REMOVE", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pRemoveGroup(mInterface, groupname) == P2P_SUP_ERRCODE_SUCCESS);
    char replyGroupRemove[] = "P2P-GROUP-REMOVED p2p-dev-p2p0 GO reason=REQUESTED\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGroupRemove);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdP2pFoundTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pFound(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_FIND");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pFound(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_FIND", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pFound(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_FIND", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pFound(mInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pFound(mInterface, -1) == P2P_SUP_ERRCODE_SUCCESS);

    char replyP2pFound[] =
        "<3>P2P-DEVICE-FOUND ff:ff:ff:ff:ff:ff p2p_dev_addr=00:00:00:00:00:00 pri_dev_type=1-0050f204-1 "
        "name='Wireless Client' config_methods=0xa4 dev_capab=0x2C group_capab=0xdD "
        "wfd_dev_info=0x00111c440032 new=1\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyP2pFound);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdRemoveNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRemoveNetwork(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("REMOVE_NETWORK");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRemoveNetwork(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("REMOVE_NETWORK", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRemoveNetwork(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("REMOVE_NETWORK", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRemoveNetwork(mInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRemoveNetwork(mInterface, -1) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRemoveNetwork(mInterface, -2) != P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetWfdEnableTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdEnable(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SET");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdEnable(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdEnable(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdEnable(mInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetWfdDeviceInfoTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdDeviceInfo(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("WFD_SUBELEM_SET");
    char conf[] = "xxxxxxxx";
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdDeviceInfo(mInterface, conf) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WFD_SUBELEM_SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdDeviceInfo(mInterface, conf) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WFD_SUBELEM_SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetWfdDeviceInfo(mInterface, conf) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdExtListenTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdExtListen(nullptr, 0, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_EXT_LISTEN");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdExtListen(mInterface, 0, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_EXT_LISTEN", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdExtListen(mInterface, 0, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_EXT_LISTEN", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdExtListen(mInterface, 0, 0, 0) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdExtListen(mInterface, 1, 120, 30) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetListenChannelTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetListenChannel(nullptr, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_SET");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetListenChannel(mInterface, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetListenChannel(mInterface, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetListenChannel(mInterface, 0, 0) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetListenChannel(mInterface, 0, 1) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdCancelConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdCancelConnect(nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_CANCEL");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdCancelConnect(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_CANCEL", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdCancelConnect(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_CANCEL", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdCancelConnect(mInterface) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdInviteTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pHalInviteArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(argv.ifname, sizeof(argv.ifname), "p2p-dev-p2p0");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(argv.gobssid, sizeof(argv.gobssid), "00:00:00:00:00:00");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(argv.peerbssid, sizeof(argv.peerbssid), "ff:ff:ff:ff:ff:ff");
    MockEraseSupportedCmd("P2P_INVITE");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_INVITE", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_INVITE", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdInvite(mInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    char replyInviteResult1[] = "P2P-INVITATION-RESULT status=0 00:00:00:00:00:00\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyInviteResult1);
    char replyInviteResult2[] = "P2P-INVITATION-RESULT status=0 \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyInviteResult2);
    char replyInviteReceive[] = "P2P-INVITATION-RECEIVED sa=00:00:00:00:00:00 persistent=4 freq=2467 "
                                "go_dev_addr=ff:ff:ff:ff:ff:ff bssid=0F:0F:0F:0F:0F:0F\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyInviteReceive);
    char replyInviateAccept[] = "P2P-INVITATION-ACCEPTED sa=00:00:00:00:00:00 persistent=0 freq=5180\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyInviateAccept);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdReInviteTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdReInvite(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pHalReInviteArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdReInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(argv.peerbssid, sizeof(argv.peerbssid), "00:00:00:00:00:00");
    MockEraseSupportedCmd("P2P_INVITE");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdReInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_INVITE", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdReInvite(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_INVITE", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdReInvite(mInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdServiceAddTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pServiceInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(info.name, sizeof(info.name), "random_string");
    MockEraseSupportedCmd("P2P_SERVICE_ADD");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERVICE_ADD", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERVICE_ADD", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    info.mode = 1;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(info.query, sizeof(info.query), "random_hex_type_query_message");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(info.resp, sizeof(info.resp), "random_hex_type_resp_message");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceAdd(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdServiceDelTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pServiceInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(info.name, sizeof(info.name), "random_string");
    MockEraseSupportedCmd("P2P_SERVICE_DEL");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERVICE_DEL", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERVICE_DEL", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    info.mode = 1;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(info.query, sizeof(info.query), "random_hex_type_query_message");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServiceDel(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdServDiscReqTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServDiscReq(nullptr, nullptr, nullptr, nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    char peerBssid[] = "00:00:00:00:00:00";
    char tlvs[256] = {0};
    char retSeq[64] = {0};
    unsigned size = sizeof(retSeq);
    EXPECT_TRUE(
        mInterface->wpaP2pCliCmdServDiscReq(mInterface, peerBssid, tlvs, retSeq, size) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(tlvs, sizeof(tlvs), "hex_type_tlv_string");
    MockEraseSupportedCmd("P2P_SERV_DISC_REQ");
    EXPECT_TRUE(
        mInterface->wpaP2pCliCmdServDiscReq(mInterface, peerBssid, tlvs, retSeq, size) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_REQ", "FAIL\n");
    EXPECT_TRUE(
        mInterface->wpaP2pCliCmdServDiscReq(mInterface, peerBssid, tlvs, retSeq, size) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_REQ", "OK\n");
    EXPECT_TRUE(
        mInterface->wpaP2pCliCmdServDiscReq(mInterface, peerBssid, tlvs, retSeq, size) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(strcmp(retSeq, "OK\n") == 0);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdServDiscCancelReqTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServDiscCancelReq(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    char val[] = "hex_string";
    MockEraseSupportedCmd("P2P_SERV_DISC_CANCEL_REQ");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServDiscCancelReq(mInterface, val) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_CANCEL_REQ", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServDiscCancelReq(mInterface, val) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_CANCEL_REQ", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdServDiscCancelReq(mInterface, val) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdProvisionDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pProvisionDiscoveryArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = HAL_WPS_METHOD_PBC;
    StrSafeCopy(argv.peerbssid, sizeof(argv.peerbssid), "00:00:00:00:00:00");
    MockEraseSupportedCmd("P2P_PROV_DISC");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_PROV_DISC", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_PROV_DISC", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = HAL_WPS_METHOD_DISPLAY;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = HAL_WPS_METHOD_KEYPAD;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = HAL_WPS_METHOD_LABEL;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = HAL_WPS_METHOD_INVALID;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdProvisionDiscovery(mInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);

    char replyDiscPbcReq1[] = "P2P-PROV-DISC-PBC-REQ 00:00:00:00:00:00 \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscPbcReq1);
    char replyDiscPbcReq2[] = "P2P-PROV-DISC-PBC-REQ invalid mac \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscPbcReq2);
    char replyDiscEnterPin1[] = "P2P-PROV-DISC-ENTER-PIN 00:00:00:00:00:00 \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscEnterPin1);
    char replyDiscEnterPin2[] = "P2P-PROV-DISC-ENTER-PIN invalid mac \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscEnterPin2);
    char replyDiscShowPin1[] = "P2P-PROV-DISC-SHOW-PIN 00:00:00:00:00:00 12345678\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscShowPin1);
    char replyDiscShowPin2[] = "P2P-PROV-DISC-SHOW-PIN invalid mac \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscShowPin2);
    char replyServiceDiscReq[] = "P2P-SERV-DISC-REQ 2462 00:00:00:00:00:00 0 0 0200010102000202\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyServiceDiscReq);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdGroupAddTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGroupAdd(nullptr, 0, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_GROUP_ADD");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGroupAdd(mInterface, 0, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_GROUP_ADD", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGroupAdd(mInterface, 0, 0, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_GROUP_ADD", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGroupAdd(mInterface, 0, 0, 0) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGroupAdd(mInterface, 1, -1, 0) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdGroupAdd(mInterface, 1, 0, 0) == P2P_SUP_ERRCODE_SUCCESS);

    char replyGroupStart[] = "P2P-GROUP-STARTED p2p-wlan0-0 GO "
                             "ssid=\"OHOS p2p ssid\" "
                             "freq=5180 "
                             "psk=pskmsg "
                             "passphrase=\"UotFDHj3\" "
                             "go_dev_addr=00:00:00:00:00:00\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGroupStart);
    char replyGroupFailure[] = "P2P-GROUP-FORMATION-FAILURE reason=FREQ_CONFLICT\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGroupFailure);
    char replyGroupSuccess[] = "P2P-GROUP-FORMATION-SUCCESS \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGroupSuccess);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdStoreConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdStoreConfig(nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SAVE_CONFIG");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdStoreConfig(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SAVE_CONFIG", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdStoreConfig(mInterface) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SAVE_CONFIG", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdStoreConfig(mInterface) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdNetworkListTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdNetworkList(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pNetworkList infoList;
    ASSERT_TRUE(memset_s(&infoList, sizeof(infoList), 0, sizeof(infoList)) == EOK);
    MockEraseSupportedCmd("LIST_NETWORKS");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdNetworkList(mInterface, &infoList) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("LIST_NETWORKS", "OK");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdNetworkList(mInterface, &infoList) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("LIST_NETWORKS", "network id / ssid / bssid / flags\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdNetworkList(mInterface, &infoList) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(infoList.infoNum == 0);
    char buf[] = "network id / ssid / bssid / flags\n"
                 "0\tp2p_ssid_1\t00:00:00:00:00:00\t[DISABLED][P2P-PERSISTENT]\n"
                 "1\tp2p_ssid_2\tff:ff:ff:ff:ff:ff\t[DISABLED]\n";
    MockSetWpaExpectCmdResponse("LIST_NETWORKS", buf);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdNetworkList(mInterface, &infoList) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(infoList.infoNum == 2);
    EXPECT_TRUE(infoList.infos[0].id == 0);
    EXPECT_TRUE(strcmp(infoList.infos[0].ssid, "p2p_ssid_1") == 0);
    EXPECT_TRUE(strcmp(infoList.infos[0].bssid, "00:00:00:00:00:00") == 0);
    EXPECT_TRUE(strcmp(infoList.infos[0].flags, "[DISABLED][P2P-PERSISTENT]") == 0);
    EXPECT_TRUE(infoList.infos[1].id == 1);
    EXPECT_TRUE(strcmp(infoList.infos[1].ssid, "p2p_ssid_2") == 0);
    EXPECT_TRUE(strcmp(infoList.infos[1].bssid, "ff:ff:ff:ff:ff:ff") == 0);
    EXPECT_TRUE(strcmp(infoList.infos[1].flags, "[DISABLED]") == 0);
    free(infoList.infos);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pConnectInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    info.mode = 0;
    info.goIntent = 0;
    info.provdisc = HAL_WPS_METHOD_DISPLAY;
    StrSafeCopy(info.peerDevAddr, sizeof(info.peerDevAddr), "00:00:00:00:00:00");
    MockEraseSupportedCmd("P2P_CONNECT");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_CONNECT", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_CONNECT", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    info.mode = 1;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    info.provdisc = HAL_WPS_METHOD_KEYPAD;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    info.provdisc = HAL_WPS_METHOD_PBC;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    info.provdisc = HAL_WPS_METHOD_LABEL;
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    info.provdisc = HAL_WPS_METHOD_DISPLAY;
    StrSafeCopy(info.pin, sizeof(info.pin), "pin");
    MockSetWpaExpectCmdResponse("P2P_CONNECT", "12345678");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdConnect(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(strcmp(info.pin, "12345678") == 0);
    char replyGoNegRequest1[] = "P2P-GO-NEG-REQUEST 00:00:00:00:00:00 dev_passwd_id=1 go_intent=7\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGoNegRequest1);
    char replyGoNegRequest2[] = "P2P-GO-NEG-REQUEST 00:00:00:00:00:00 \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGoNegRequest2);
    char replyGoNegSuccess[] = "P2P-GO-NEG-SUCCESS role=GO freq=5180 ht40=0 peer_dev=00:00:00:00:00:00 "
                               "peer_iface=00:00:00:00:00:00 wps_method=Display\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGoNegSuccess);
    char replyGoNegFailure1[] = "P2P-GO-NEG-FAILURE status=1\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGoNegFailure1);
    char replyGoNegFailure2[] = "P2P-GO-NEG-FAILURE \n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyGoNegFailure2);
    char replyConnect[] = "AP-STA-CONNECTED 00:00:00:00:00:00 p2p_dev_addr=ff:ff:ff:ff:ff:ff\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyConnect);
    char replyDisconnect[] = "AP-STA-DISCONNECTED 00:00:00:00:00:00 p2p_dev_addr=ff:ff:ff:ff:ff:ff\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDisconnect);
    char replyDisconnect2[] = "AP-STA-DISCONNECTED 00:00:00:00:00:00\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDisconnect2);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdP2pGetPeerTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pGetPeer(nullptr, nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    const char *bssid = "00:00:00:00:00:00";
    P2pDeviceInfo info;
    MockEraseSupportedCmd("P2P_PEER");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pGetPeer(mInterface, nullptr, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_PEER", "group_capab=0x0\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdP2pGetPeer(mInterface, bssid, &info) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetPersistentReconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetPersistentReconnect(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetPersistentReconnect(mInterface, -1) != P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetPersistentReconnect(mInterface, 2) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SET");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetPersistentReconnect(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetPersistentReconnect(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetPersistentReconnect(mInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdRespServerDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRespServerDiscovery(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pServDiscReqInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    info.freq = 2412;
    info.dialogToken = 0;
    StrSafeCopy(info.mac, sizeof(info.mac), "00:00:00:00:00:00");
    info.tlvs = (char *)calloc(256, sizeof(char));
    ASSERT_TRUE(info.tlvs != NULL);
    StrSafeCopy(info.tlvs, 256, "08000104112233445566");
    MockEraseSupportedCmd("P2P_SERV_DISC_RESP");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRespServerDiscovery(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_RESP", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRespServerDiscovery(mInterface, &info) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_RESP", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdRespServerDiscovery(mInterface, &info) == P2P_SUP_ERRCODE_SUCCESS);
    free(info.tlvs);

    char replyDiscPbc[] = "P2P-PROV-DISC-PBC-RESP 00:00:00:00:00:00\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscPbc);
    char replyDiscPbc2[] = "P2P-PROV-DISC-PBC-RESP invalid mac\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscPbc2);
    char replyServiceDisc[] = "P2P-SERV-DISC-RESP 00:00:00:00:00:00 0 03000101010300020201\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyServiceDisc);
    char replyDiscFailure[] = "P2P-PROV-DISC-FAILURE p2p_dev_addr=00:00:00:00:00:00 status=0\n";
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, replyDiscFailure);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetServDiscExternalTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetServDiscExternal(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_SERV_DISC_EXTERNAL");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetServDiscExternal(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_EXTERNAL", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetServDiscExternal(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SERV_DISC_EXTERNAL", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetServDiscExternal(mInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetRandomMacTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetRandomMac(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("DRIVER_FLAGS");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetRandomMac(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("DRIVER_FLAGS", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetRandomMac(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("DRIVER_FLAGS", "DEDICATED_P2P_DEVICE\n");
    MockEraseSupportedCmd("SET");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetRandomMac(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetRandomMac(mInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(mInterface->wpaP2pCliCmdSetRandomMac(mInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdWpsPbcTest, TestSize.Level1)
{
    WifiWpaP2pGroupInterface *groupInterface = GetWifiWpaP2pGroupInterface("p2p-dev-wlan0");
    ASSERT_TRUE(groupInterface != nullptr);
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPbc(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    const char *bssid = "00:00:00:00:00:00";
    MockEraseSupportedCmd("WPS_PBC");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPbc(groupInterface, bssid) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WPS_PBC", "FAIL\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPbc(groupInterface, bssid) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WPS_PBC", "FAIL-PBC-OVERLAP\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPbc(groupInterface, bssid) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WPS_PBC", "OK\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPbc(groupInterface, bssid) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdWpsPinTest, TestSize.Level1)
{
    WifiWpaP2pGroupInterface *groupInterface = GetWifiWpaP2pGroupInterface("p2p-dev-wlan0");
    ASSERT_TRUE(groupInterface != nullptr);
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(nullptr, nullptr) != P2P_SUP_ERRCODE_SUCCESS);
    P2pWpsPinDisplayArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = P2P_PIN_KEYPAD;
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    StrSafeCopy(argv.pinCode, sizeof(argv.pinCode), "12345678");
    MockEraseSupportedCmd("WPS_PIN");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WPS_PIN", "FAIL\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("WPS_PIN", "OK\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    argv.mode = P2P_PIN_DISPLAY;
    MockSetWpaExpectCmdResponse("WPS_PIN", "987654321");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(strcmp(argv.pinCode, "98765432") == 0);
    StrSafeCopy(argv.bssid, sizeof(argv.bssid), "00:00:00:00:00:00");
    MockSetWpaExpectCmdResponse("WPS_PIN", "00123456");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdWpsPin(groupInterface, &argv) == P2P_SUP_ERRCODE_SUCCESS);
    EXPECT_TRUE(strcmp(argv.pinCode, "00123456") == 0);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetPowerSaveTest, TestSize.Level1)
{
    WifiWpaP2pGroupInterface *groupInterface = GetWifiWpaP2pGroupInterface("p2p-dev-wlan0");
    ASSERT_TRUE(groupInterface != nullptr);
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetPowerSave(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("P2P_SET");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetPowerSave(groupInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SET", "FAIL\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetPowerSave(groupInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("P2P_SET", "OK\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetPowerSave(groupInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, WpaP2pCliCmdSetGroupIdleTest, TestSize.Level1)
{
    WifiWpaP2pGroupInterface *groupInterface = GetWifiWpaP2pGroupInterface("p2p-dev-wlan0");
    ASSERT_TRUE(groupInterface != nullptr);
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetGroupIdle(nullptr, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockEraseSupportedCmd("SET");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetGroupIdle(groupInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetGroupIdle(groupInterface, 0) != P2P_SUP_ERRCODE_SUCCESS);
    MockSetWpaExpectCmdResponse("SET", "OK\n");
    EXPECT_TRUE(groupInterface->wpaP2pCliCmdSetGroupIdle(groupInterface, 0) == P2P_SUP_ERRCODE_SUCCESS);
}

HWTEST_F(WifiHalWpaP2pTest, ConvertP2pErrCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(ConvertP2pErrCode(P2P_SUP_ERRCODE_SUCCESS) == WIFI_HAL_SUCCESS);
    EXPECT_TRUE(ConvertP2pErrCode(P2P_SUP_ERRCODE_PBC_OVERLAP) == WIFI_HAL_PBC_OVERLAP);
    EXPECT_TRUE(ConvertP2pErrCode(P2P_SUP_ERRCODE_INPUT_ERROR) == WIFI_HAL_INVALID_PARAM);
    EXPECT_TRUE(ConvertP2pErrCode(P2P_SUP_ERRCODE_FAILED) == WIFI_HAL_FAILED);
    EXPECT_TRUE(ConvertP2pErrCode(P2P_SUP_ERRCODE_TIMEOUT) == WIFI_HAL_FAILED);
}

HWTEST_F(WifiHalWpaP2pTest, GetWifiWpaP2pGroupInterfaceTest, TestSize.Level1)
{
    WifiWpaP2pGroupInterface *groupInterface = GetWifiWpaP2pGroupInterface("p2p-dev-wlan0");
    ASSERT_TRUE(groupInterface != nullptr);
    WifiWpaP2pGroupInterface *groupInterface1 = GetWifiWpaP2pGroupInterface("p2p-dev-wlan0");
    ASSERT_TRUE(groupInterface1 != nullptr);
    EXPECT_TRUE(groupInterface == groupInterface1);
    ReleaseWpaP2pGroupInterface("p2p-dev-unknown");
    ReleaseWpaP2pGroupInterface("p2p-dev-wlan0");
    ReleaseWpaP2pGroupInterface("p2p-dev-wlan0");
}
}  // namespace Wifi
}  // namespace OHOS
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
#include "wifi_hal_crpc_server_test.h"
#include "wifi_log.h"
#include "wifi_hal_crpc_server.h"
#include "wifi_hal_crpc_base.h"
#include "wifi_hal_crpc_chip.h"
#include "wifi_hal_crpc_supplicant.h"
#include "wifi_hal_crpc_sta.h"
#include "wifi_hal_crpc_ap.h"
#include "wifi_hal_crpc_common.h"
#include "wifi_hal_crpc_p2p.h"
#include "wifi_hal_ap_interface.h"
#include "wifi_hal_sta_interface.h"
#include "mock_wpa_ctrl.h"
#include "wifi_hal_common_func.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
static std::string g_rpcSockPath = "./unix_sock_test.sock";
RpcServer *WifiHalCRpcServerTest::mServer = nullptr;
Context *WifiHalCRpcServerTest::mContext = nullptr;

static int StrcmpMathRight(const char *left, const char *right)
{
    return strncmp(left, right, strlen(right));
}

void WifiHalCRpcServerTest::SetUpTestCase()
{
    if (access(g_rpcSockPath.c_str(), 0) == 0) {
        unlink(g_rpcSockPath.c_str());
    }
    mServer = CreateRpcServer(g_rpcSockPath.c_str());
    mContext = CreateContext(CONTEXT_BUFFER_MIN_SIZE);
    if (mServer == nullptr || mContext == nullptr) {
        printf("Init rpc server failed or create context failed!");
        exit(-1);
    }
    SetRpcServerInited(mServer);
    MockInitGlobalCmd();
    MockInitStaSupportedCmd();
    MockInitApSupportedCmd();
}

void WifiHalCRpcServerTest::TearDownTestCase()
{
    if (mServer != nullptr) {
        ReleaseRpcServer(mServer);
        mServer = nullptr;
    }
    SetRpcServerInited(NULL);
    if (mContext != nullptr) {
        ReleaseContext(mContext);
        mContext = nullptr;
    }
}

void WifiHalCRpcServerTest::SetUp()
{
    InitRpcFunc();
    if (mContext != nullptr) {
        mContext->wBegin = mContext->wEnd = 0;
    }
}

void WifiHalCRpcServerTest::TearDown()
{
    ReleaseRpcFunc();
    if (mContext != nullptr) {
        mContext->wBegin = mContext->wEnd = 0;
    }
}

HWTEST_F(WifiHalCRpcServerTest, GetRpcFuncTest, TestSize.Level1)
{
    EXPECT_TRUE(GetRpcFunc("GetName") != nullptr);
    EXPECT_TRUE(GetRpcFunc("GetNameTest") == nullptr);
}

HWTEST_F(WifiHalCRpcServerTest, OnTransactTest, TestSize.Level1)
{
    char buff[] = "N|IncorrectTypeInputMessage";
    mContext->oneProcess = buff;
    mContext->nPos = 2;
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(OnTransact(mServer, mContext) < 0);
    char buff2[] = "N|UnsupportedCmd|";
    mContext->oneProcess = buff2;
    mContext->nPos = 2;
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(OnTransact(mServer, mContext) == 0);
    EXPECT_TRUE(strstr(mContext->szWrite, "unsupport function") != nullptr);
    char buff3[] = "N|SetCountryCode|";
    mContext->oneProcess = buff3;
    mContext->nPos = 2;
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(OnTransact(mServer, mContext) == 0);
    EXPECT_TRUE(strstr(mContext->szWrite, "server deal failed!") != nullptr);
}

HWTEST_F(WifiHalCRpcServerTest, PushPopCallbackMsgTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *msg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    ASSERT_TRUE(msg != nullptr);
    msg->msg.scanStatus = 100;
    int event = WIFI_FAILURE_EVENT - 1;
    EXPECT_TRUE(PushBackCallbackMsg(event, msg) < 0);
    event = WIFI_HAL_MAX_EVENT;
    EXPECT_TRUE(PushBackCallbackMsg(event, msg) < 0);
    event = WIFI_FAILURE_EVENT;
    EXPECT_TRUE(PushBackCallbackMsg(event, msg) == 0);
    WifiHalEventCallbackMsg *msg1 = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    ASSERT_TRUE(msg1 != nullptr);
    msg1->msg.scanStatus = 101;
    EXPECT_TRUE(PushBackCallbackMsg(event, msg1) == 0);
    WifiHalEventCallbackMsg *msg2 = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    ASSERT_TRUE(msg2 != nullptr);
    msg2->msg.scanStatus = 102;
    EXPECT_TRUE(PushBackCallbackMsg(event, msg2) == 0);
    EXPECT_TRUE(PopBackCallbackMsg(WIFI_HAL_MAX_EVENT) < 0);
    EXPECT_TRUE(PopBackCallbackMsg(event) == 0);
    EXPECT_TRUE(FrontCallbackMsg(WIFI_HAL_MAX_EVENT) == nullptr);
    WifiHalEventCallbackMsg *p = FrontCallbackMsg(event);
    EXPECT_TRUE(p->msg.scanStatus == 100);
    EXPECT_TRUE(PopFrontCallbackMsg(WIFI_HAL_MAX_EVENT) < 0);
    EXPECT_TRUE(PopFrontCallbackMsg(event) == 0);
    p = FrontCallbackMsg(event);
    EXPECT_TRUE(p->msg.scanStatus == 101);
    EXPECT_TRUE(PopFrontCallbackMsg(event) == 0);
    p = FrontCallbackMsg(event);
    EXPECT_TRUE(p == nullptr);
    free(msg2);
}

HWTEST_F(WifiHalCRpcServerTest, OnCallbackTransactTest, TestSize.Level1)
{
    EXPECT_TRUE(OnCallbackTransact(nullptr, 0, nullptr) < 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, WIFI_FAILURE_EVENT, mContext) == 0);
    EXPECT_TRUE(EndCallbackTransact(nullptr, 0) < 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, WIFI_FAILURE_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealCommonCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    cbmsg->msg.scanStatus = 100;
    EXPECT_TRUE(PushBackCallbackMsg(WIFI_SCAN_INFO_NOTIFY_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, WIFI_SCAN_INFO_NOTIFY_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|107|100|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, WIFI_SCAN_INFO_NOTIFY_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealIfaceCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    cbmsg->msg.ifMsg.type = 100;
    StrSafeCopy(cbmsg->msg.ifMsg.ifname, sizeof(cbmsg->msg.ifMsg.ifname), "wlan0");
    EXPECT_TRUE(PushBackCallbackMsg(WIFI_ADD_IFACE_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, WIFI_ADD_IFACE_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|103|100|wlan0|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, WIFI_ADD_IFACE_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealConnectionChangedCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    cbmsg->msg.connMsg.status = 100;
    cbmsg->msg.connMsg.networkId = 1;
    StrSafeCopy(cbmsg->msg.connMsg.bssid, sizeof(cbmsg->msg.connMsg.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(WIFI_CONNECT_CHANGED_NOTIFY_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, WIFI_CONNECT_CHANGED_NOTIFY_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|108|100|1|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, WIFI_CONNECT_CHANGED_NOTIFY_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pDeviceFoundCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.deviceInfo.srcAddress, sizeof(cbmsg->msg.deviceInfo.srcAddress), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_DEVICE_FOUND_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_DEVICE_FOUND_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|117|0|0|0|0|00:00:00:00:00:00|||||$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_DEVICE_FOUND_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pNegoriationCbkLostTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.connMsg.bssid, sizeof(cbmsg->msg.connMsg.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_DEVICE_LOST_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_DEVICE_LOST_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|118|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_DEVICE_LOST_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pNegoriationCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.connMsg.bssid, sizeof(cbmsg->msg.connMsg.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_GO_NEGOTIATION_REQUEST_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_GO_NEGOTIATION_REQUEST_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|119|0|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_GO_NEGOTIATION_REQUEST_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pInviationCbkReceiveTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.invitaInfo.srcAddress, sizeof(cbmsg->msg.invitaInfo.srcAddress), "00:00:00:00:00:00");
    StrSafeCopy(cbmsg->msg.invitaInfo.bssid, sizeof(cbmsg->msg.invitaInfo.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_INVITATION_RECEIVED_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_INVITATION_RECEIVED_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|122|0|0|0|00:00:00:00:00:00||00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_INVITATION_RECEIVED_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pInviationCbkResultTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.invitaInfo.bssid, sizeof(cbmsg->msg.invitaInfo.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_INVITATION_RESULT_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_INVITATION_RESULT_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|123|0|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_INVITATION_RESULT_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pInviationCbkFailureTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.invitaInfo.bssid, sizeof(cbmsg->msg.invitaInfo.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_GROUP_FORMATION_FAILURE_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_GROUP_FORMATION_FAILURE_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|125|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_GROUP_FORMATION_FAILURE_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pGroupInfoCbkStartTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.groupInfo.groupIfName, sizeof(cbmsg->msg.groupInfo.groupIfName), "p2p-dev-wlan0");
    StrSafeCopy(cbmsg->msg.groupInfo.ssid, sizeof(cbmsg->msg.groupInfo.ssid), "test_p2p");
    StrSafeCopy(
        cbmsg->msg.groupInfo.goDeviceAddress, sizeof(cbmsg->msg.groupInfo.goDeviceAddress), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_GROUP_STARTED_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_GROUP_STARTED_EVENT, mContext) == 0);
    EXPECT_TRUE(
        StrcmpMathRight(mContext->szWrite, "C|126|0|0|0|p2p-dev-wlan0|test_p2p|||00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_GROUP_STARTED_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pGroupInfoCbkRemoveTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.groupInfo.groupIfName, sizeof(cbmsg->msg.groupInfo.groupIfName), "p2p-dev-wlan0");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_GROUP_REMOVED_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_GROUP_REMOVED_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|127|0|p2p-dev-wlan0|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_GROUP_REMOVED_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pDeviceInfoCbkPbcTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.deviceInfo.srcAddress, sizeof(cbmsg->msg.deviceInfo.srcAddress), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_PROV_DISC_PBC_REQ_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_PROV_DISC_PBC_REQ_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|128|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_PROV_DISC_PBC_REQ_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pDeviceInfoCbkPinTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.deviceInfo.srcAddress, sizeof(cbmsg->msg.deviceInfo.srcAddress), "00:00:00:00:00:00");
    StrSafeCopy(cbmsg->msg.deviceInfo.deviceName, sizeof(cbmsg->msg.deviceInfo.deviceName), "test_p2p");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_PROV_DISC_SHOW_PIN_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_PROV_DISC_SHOW_PIN_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|131|00:00:00:00:00:00|test_p2p|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_PROV_DISC_SHOW_PIN_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pDeviceInfoCbkConnectionTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(
        cbmsg->msg.deviceInfo.p2pDeviceAddress, sizeof(cbmsg->msg.deviceInfo.p2pDeviceAddress), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(AP_STA_DISCONNECTED_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, AP_STA_DISCONNECTED_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|135|00:00:00:00:00:00|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, AP_STA_DISCONNECTED_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pServerInfoCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.serverInfo.srcAddress, sizeof(cbmsg->msg.serverInfo.srcAddress), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_SERV_DISC_RESP_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_SERV_DISC_RESP_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|133|0|00:00:00:00:00:00|0|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_SERV_DISC_RESP_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, DealP2pServerDiscReqCbkTest, TestSize.Level1)
{
    WifiHalEventCallbackMsg *cbmsg = (WifiHalEventCallbackMsg *)calloc(1, sizeof(WifiHalEventCallbackMsg));
    StrSafeCopy(cbmsg->msg.serDiscReqInfo.mac, sizeof(cbmsg->msg.serDiscReqInfo.mac), "00:00:00:00:00:00");
    EXPECT_TRUE(PushBackCallbackMsg(P2P_SERV_DISC_REQ_EVENT, cbmsg) == 0);
    EXPECT_TRUE(OnCallbackTransact(mServer, P2P_SERV_DISC_REQ_EVENT, mContext) == 0);
    EXPECT_TRUE(StrcmpMathRight(mContext->szWrite, "C|137|0|0|0|00:00:00:00:00:00|0|$$$$$$") == 0);
    EXPECT_TRUE(EndCallbackTransact(mServer, P2P_SERV_DISC_REQ_EVENT) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetNameTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetName(nullptr, nullptr) < 0);
    char buff[] = "N|GetName|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetName|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetName(mServer, mContext) < 0);
    char buff2[] = "N|GetName|128|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|GetName|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcGetName(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetType(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetType(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcRegisterEventCallbackTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcRegisterEventCallback(nullptr, nullptr) < 0);
    char buff[] = "N|RegisterEventCallback|asdgfd|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|RegisterEventCallback|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcRegisterEventCallback(mServer, mContext) < 0);
    char buff2[] = "N|RegisterEventCallback|2|101|asdf|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|RegisterEventCallback|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcRegisterEventCallback(mServer, mContext) < 0);
    char buff3[] = "N|RegisterEventCallback|2|101|108|";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|RegisterEventCallback|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcRegisterEventCallback(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcUnRegisterEventCallbackTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcUnRegisterEventCallback(nullptr, nullptr) < 0);
    char buff[] = "N|UnRegisterEventCallback|asdgfd|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|UnRegisterEventCallback|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcUnRegisterEventCallback(mServer, mContext) < 0);
    char buff2[] = "N|UnRegisterEventCallback|2|101|asdf|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|UnRegisterEventCallback|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcUnRegisterEventCallback(mServer, mContext) < 0);
    char buff3[] = "N|UnRegisterEventCallback|2|101|108|";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|UnRegisterEventCallback|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcUnRegisterEventCallback(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcNotifyClearTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcNotifyClear(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcNotifyClear(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetWifiChipTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetWifiChip(nullptr, nullptr) < 0);
    char buff[] = "N|GetWifiChip|adsgfsd|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetWifiChip|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetWifiChip(mServer, mContext) < 0);
    char buff1[] = "N|GetWifiChip|8|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetWifiChip|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetWifiChip(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetWifiChipIdsTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetWifiChipIds(nullptr, nullptr) < 0);
    char buff[] = "N|GetWifiChipIds|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetWifiChipIds|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetWifiChipIds(mServer, mContext) < 0);
    char buff1[] = "N|GetWifiChipIds|8|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetWifiChipIds|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetWifiChipIds(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetChipIdTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetChipId(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetChipId(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcCreateIfaceTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcCreateIface(nullptr, nullptr) < 0);
    char buff[] = "N|CreateIface|fdshajkdsghk|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|CreateIface|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcCreateIface(mServer, mContext) < 0);
    char buff1[] = "N|CreateIface|8|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|CreateIface|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcCreateIface(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetIfaceTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetIface(nullptr, nullptr) < 0);
    char buff[] = "N|GetIface|wlan0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetIface|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetIface(mServer, mContext) < 0);
    char buff1[] = "N|GetIface|wlan0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetIface|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetIface(mServer, mContext) == 0);
    char buff2[] = "N|GetIface|01234567890123456789012345678901|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|GetIface|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcGetIface(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetIfaceNamesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetIfaceNames(nullptr, nullptr) < 0);
    char buff[] = "N|GetIfaceNames|asdgf|asdgf|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetIfaceNames|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetIfaceNames(mServer, mContext) < 0);
    char buff1[] = "N|GetIfaceNames|12|asdgf|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetIfaceNames|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetIfaceNames(mServer, mContext) < 0);
    char buff2[] = "N|GetIfaceNames|12|128|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|GetIfaceNames|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcGetIfaceNames(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcRemoveIfaceTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcRemoveIface(nullptr, nullptr) < 0);
    char buff[] = "N|RemoveIface|wlan0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|RemoveIface|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcRemoveIface(mServer, mContext) < 0);
    char buff1[] = "N|RemoveIface|wlan0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|RemoveIface|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcRemoveIface(mServer, mContext) == 0);
    char buff2[] = "N|RemoveIface|01234567890123456789012345678901|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|RemoveIface|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcRemoveIface(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetCapabilitiesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetCapabilities(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetCapabilities(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetSupportedComboModesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetSupportedComboModes(nullptr, nullptr) < 0);
    char buff[] = "N|GetSupportedComboModes|asdgds|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetSupportedComboModes|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetSupportedComboModes(mServer, mContext) < 0);
    char buff1[] = "N|GetSupportedComboModes|134|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetSupportedComboModes|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetSupportedComboModes(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcConfigComboModesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcConfigComboModes(nullptr, nullptr) < 0);
    char buff[] = "N|ConfigComboModes|asdgds|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|ConfigComboModes|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcConfigComboModes(mServer, mContext) < 0);
    char buff1[] = "N|ConfigComboModes|134|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|ConfigComboModes|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcConfigComboModes(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetComboModesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetComboModes(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetComboModes(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcRequestFirmwareDebugDumpTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcRequestFirmwareDebugDump(nullptr, nullptr) < 0);
    char buff[] = "N|RequestFirmwareDebugDump|asdgds|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|RequestFirmwareDebugDump|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcRequestFirmwareDebugDump(mServer, mContext) < 0);
    char buff1[] = "N|RequestFirmwareDebugDump|134|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|RequestFirmwareDebugDump|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcRequestFirmwareDebugDump(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetPowerModeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetPowerMode(nullptr, nullptr) < 0);
    char buff[] = "N|SetPowerMode|asdgds|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetPowerMode|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetPowerMode(mServer, mContext) < 0);
    char buff1[] = "N|SetPowerMode|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetPowerMode|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetPowerMode(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStart(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStart(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStartSupplicant(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStartSupplicant(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStopSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStopSupplicant(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStopSupplicant(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStopTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStop(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStop(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcConnectSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcConnectSupplicant(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcConnectSupplicant(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcDisconnectSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcDisconnectSupplicant(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcDisconnectSupplicant(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcRequestToSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcRequestToSupplicant(nullptr, nullptr) < 0);
    char buff[] = "N|RequestToSupplicant|asdf|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|RequestToSupplicant|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcRequestToSupplicant(mServer, mContext) < 0);
    char buff1[] = "N|RequestToSupplicant|4|8c677c8d5a|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|RequestToSupplicant|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcRequestToSupplicant(mServer, mContext) < 0);
    char buff2[] = "N|RequestToSupplicant|4|8c677c8a|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|RequestToSupplicant|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcRequestToSupplicant(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetPowerSaveTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetPowerSave(nullptr, nullptr) < 0);
    char buff[] = "N|SetPowerSave|fds|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetPowerSave|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetPowerSave(mServer, mContext) < 0);
    char buff1[] = "N|SetPowerSave|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetPowerSave|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetPowerSave(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcWpaSetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcWpaSetCountryCode(nullptr, nullptr) < 0);
    char buff[] = "N|WpaSetCountryCode|CHINA|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|WpaSetCountryCode|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcWpaSetCountryCode(mServer, mContext) < 0);
    char buff1[] = "N|WpaSetCountryCode|CN|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|WpaSetCountryCode|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcWpaSetCountryCode(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcWpaGetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcWpaGetCountryCode(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcWpaGetCountryCode(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartScanTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStartScan(nullptr, nullptr) < 0);
    char buff[] = "N|StartScan|x|10|scan_ssid1|10|scan_ssid2|2|2427|2442|2|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|StartScan|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcStartScan(mServer, mContext) < 0);
    char buff1[] = "N|StartScan|2|10|scan_ssid1|10|scan_ssid2";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|StartScan|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcStartScan(mServer, mContext) < 0);
    char buff2[] = "N|StartScan|2|10|scan_ssid1|10|scan_ssid2|x|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|StartScan|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcStartScan(mServer, mContext) < 0);
    char buff3[] = "N|StartScan|2|10|scan_ssid1|10|scan_ssid2|2|2427|x|x|";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|StartScan|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcStartScan(mServer, mContext) < 0);
    char buff4[] = "N|StartScan|2|10|scan_ssid1|10|scan_ssid2|2|2427|2442|x|";
    mContext->oneProcess = buff4;
    mContext->nPos = strlen("N|StartScan|");
    mContext->nSize = strlen(buff4);
    EXPECT_TRUE(RpcStartScan(mServer, mContext) < 0);
    char buff5[] = "N|StartScan|2|10|scan_ssid1|10|scan_ssid2|2|2427|2442|2|";
    mContext->oneProcess = buff5;
    mContext->nPos = strlen("N|StartScan|");
    mContext->nSize = strlen(buff5);
    EXPECT_TRUE(RpcStartScan(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetScanInfosTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetScanInfos(nullptr, nullptr) < 0);
    char buff[] = "N|GetScanInfos|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetScanInfos|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetScanInfos(mServer, mContext) < 0);
    char buff1[] = "N|GetScanInfos|12|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetScanInfos|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetScanInfos(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetNetworkListTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetNetworkList(nullptr, nullptr) < 0);
    char buff[] = "N|GetNetworkList|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetNetworkList|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetNetworkList(mServer, mContext) < 0);
    char buff1[] = "N|GetNetworkList|12|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetNetworkList|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetNetworkList(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartPnoScanTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStartPnoScan(nullptr, nullptr) < 0);
    char buff[] = "N|StartPnoScan|1|x|1|2|3|asd|4|asdf|1|5|asdfg|2|5040|5080|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) < 0);
    char buff1[] = "N|StartPnoScan|1|2|1|2|10|scan_ssid1|10|scan_ssid2";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) < 0);
    char buff2[] = "N|StartPnoScan|1|2|1|2|10|scan_ssid1|10|scan_ssid2|x|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) < 0);
    char buff3[] = "N|StartPnoScan|1|2|1|2|10|scan_ssid1|10|scan_ssid2|2|10|save_ssid1|10|save_ssid2";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) < 0);
    char buff4[] = "N|StartPnoScan|1|2|1|2|10|scan_ssid1|10|scan_ssid2|2|10|save_ssid1|10|save_ssid2|x|";
    mContext->oneProcess = buff4;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff4);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) < 0);
    char buff5[] = "N|StartPnoScan|1|2|1|2|10|scan_ssid1|10|scan_ssid2|2|10|save_ssid1|10|save_ssid2|2|5040|x|";
    mContext->oneProcess = buff5;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff5);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) < 0);
    char buff6[] = "N|StartPnoScan|1|2|1|2|10|scan_ssid1|10|scan_ssid2|2|10|save_ssid1|10|save_ssid2|2|5040|5080|";
    mContext->oneProcess = buff6;
    mContext->nPos = strlen("N|StartPnoScan|");
    mContext->nSize = strlen(buff6);
    EXPECT_TRUE(RpcStartPnoScan(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStopPnoScanTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStopPnoScan(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStopPnoScan(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcConnect(nullptr, nullptr) < 0);
    char buff[] = "N|Connect|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|Connect|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcConnect(mServer, mContext) < 0);
    char buff1[] = "N|Connect|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|Connect|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcConnect(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcWpaAutoConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcWpaAutoConnect(nullptr, nullptr) < 0);
    char buff[] = "N|WpaAutoConnect|0ad|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|WpaAutoConnect|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcWpaAutoConnect(mServer, mContext) < 0);
    char buff1[] = "N|WpaAutoConnect|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|WpaAutoConnect|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcWpaAutoConnect(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcReconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcReconnect(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcReconnect(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcReassociateTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcReassociate(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcReassociate(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcDisconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcDisconnect(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcDisconnect(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetStaCapabilitiesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetStaCapabilities(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetStaCapabilities(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetDeviceMacAddressTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetDeviceMacAddress(nullptr, nullptr) < 0);
    char buff[] = "N|GetDeviceMacAddress|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetDeviceMacAddress|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetDeviceMacAddress(mServer, mContext) < 0);
    char buff1[] = "N|GetDeviceMacAddress|17|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetDeviceMacAddress|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetDeviceMacAddress(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetFrequenciesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetFrequencies(nullptr, nullptr) < 0);
    char buff[] = "N|GetFrequencies|1|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetFrequencies|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetFrequencies(mServer, mContext) < 0);
    char buff1[] = "N|GetFrequencies|1|128|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetFrequencies|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetFrequencies(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetAssocMacAddrTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetAssocMacAddr(nullptr, nullptr) < 0);
    char buff[] = "N|SetAssocMacAddr|x|7d9c039dfeba46|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetAssocMacAddr|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetAssocMacAddr(mServer, mContext) < 0);
    char buff1[] = "N|SetAssocMacAddr|6|7d9c039dfeba46|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetAssocMacAddr|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetAssocMacAddr(mServer, mContext) < 0);
    char buff2[] = "N|SetAssocMacAddr|7|7d9c039dfeba46|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|SetAssocMacAddr|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcSetAssocMacAddr(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetScanningMacAddressTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetScanningMacAddress(nullptr, nullptr) < 0);
    char buff[] = "N|SetScanningMacAddress|x|7d9c039dfeba46|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetScanningMacAddress|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetScanningMacAddress(mServer, mContext) < 0);
    char buff1[] = "N|SetScanningMacAddress|6|7d9c039dfeba46|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetScanningMacAddress|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetScanningMacAddress(mServer, mContext) < 0);
    char buff2[] = "N|SetScanningMacAddress|7|7d9c039dfeba46|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|SetScanningMacAddress|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcSetScanningMacAddress(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcDeauthLastRoamingBssidTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcDeauthLastRoamingBssid(nullptr, nullptr) < 0);
    char buff[] = "N|DeauthLastRoamingBssid|x|7d9c039dfeba46|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|DeauthLastRoamingBssid|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcDeauthLastRoamingBssid(mServer, mContext) < 0);
    char buff1[] = "N|DeauthLastRoamingBssid|6|7d9c039dfeba46|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|DeauthLastRoamingBssid|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcDeauthLastRoamingBssid(mServer, mContext) < 0);
    char buff2[] = "N|DeauthLastRoamingBssid|7|7d9c039dfeba46|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|DeauthLastRoamingBssid|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcDeauthLastRoamingBssid(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetSupportFeatureTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetSupportFeature(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetSupportFeature(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcRunCmdTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcRunCmd(nullptr, nullptr) < 0);
    char buff[] = "N|RunCmd|wlan0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|RunCmd|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcRunCmd(mServer, mContext) < 0);
    char buff1[] = "N|RunCmd|wlan0|x|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|RunCmd|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcRunCmd(mServer, mContext) < 0);
    char buff2[] = "N|RunCmd|wlan0|1|x|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|RunCmd|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcRunCmd(mServer, mContext) < 0);
    char buff3[] = "N|RunCmd|wlan0|1|6|7d9c039dfeba46|";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|RunCmd|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcRunCmd(mServer, mContext) < 0);
    char buff4[] = "N|RunCmd|wlan0|1|7|7d9c039dfeba46|";
    mContext->oneProcess = buff4;
    mContext->nPos = strlen("N|RunCmd|");
    mContext->nSize = strlen(buff4);
    EXPECT_TRUE(RpcRunCmd(mServer, mContext) == 0);
    char buff5[] = "N|RunCmd|0123456789012345678901|1|7|7d9c039dfeba46|";
    mContext->oneProcess = buff5;
    mContext->nPos = strlen("N|RunCmd|");
    mContext->nSize = strlen(buff5);
    EXPECT_TRUE(RpcRunCmd(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetWifiTxPowerTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetWifiTxPower(nullptr, nullptr) < 0);
    char buff[] = "N|SetWifiTxPower|12";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetWifiTxPower|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetWifiTxPower(mServer, mContext) < 0);
    char buff1[] = "N|SetWifiTxPower|12|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetWifiTxPower|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetWifiTxPower(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcRemoveNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcRemoveNetwork(nullptr, nullptr) < 0);
    char buff[] = "N|RemoveNetwork|12";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|RemoveNetwork|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcRemoveNetwork(mServer, mContext) < 0);
    char buff1[] = "N|RemoveNetwork|12|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|RemoveNetwork|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcRemoveNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcAddNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcAddNetwork(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcAddNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcEnableNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcEnableNetwork(nullptr, nullptr) < 0);
    char buff[] = "N|EnableNetwork|12";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|EnableNetwork|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcEnableNetwork(mServer, mContext) < 0);
    char buff1[] = "N|EnableNetwork|12|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|EnableNetwork|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcEnableNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcDisableNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcDisableNetwork(nullptr, nullptr) < 0);
    char buff[] = "N|DisableNetwork|12";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|DisableNetwork|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcDisableNetwork(mServer, mContext) < 0);
    char buff1[] = "N|DisableNetwork|12|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|DisableNetwork|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcDisableNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetNetwork(nullptr, nullptr) < 0);
    char buff[] = "N|SetNetwork|0|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetNetwork|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetNetwork(mServer, mContext) < 0);
    char buff1[] = "N|SetNetwork|0|1|12|afsdgljsd";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetNetwork|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetNetwork(mServer, mContext) == 0);
    char buff2[] = "N|SetNetwork|0|1|12|afsdgljsd|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|SetNetwork|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcSetNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcWpaGetNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcWpaGetNetwork(nullptr, nullptr) < 0);
    char buff[] = "N|WpaGetNetwork|2|ssid";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|WpaGetNetwork|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcWpaGetNetwork(mServer, mContext) < 0);
    char buff1[] = "N|WpaGetNetwork|2|ssid|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|WpaGetNetwork|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcWpaGetNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSaveNetworkConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSaveNetworkConfig(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcSaveNetworkConfig(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartWpsPbcModeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStartWpsPbcMode(nullptr, nullptr) < 0);
    char buff[] = "N|StartWpsPbcMode|1|2|adsgfkdsj";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|StartWpsPbcMode|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcStartWpsPbcMode(mServer, mContext) < 0);
    char buff1[] = "N|StartWpsPbcMode|1|2|adsgfkdsj|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|StartWpsPbcMode|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcStartWpsPbcMode(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartWpsPinModeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStartWpsPinMode(nullptr, nullptr) < 0);
    char buff[] = "N|StartWpsPinMode|1|2|adsgfkdsj";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|StartWpsPinMode|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcStartWpsPinMode(mServer, mContext) < 0);
    char buff1[] = "N|StartWpsPinMode|1|2|adsgfkdsj|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|StartWpsPinMode|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcStartWpsPinMode(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStopWpsTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStopWps(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStopWps(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcWpaBlocklistClearTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcWpaBlocklistClear(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcWpaBlocklistClear(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetRoamingCapabilitiesTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetRoamingCapabilities(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcGetRoamingCapabilities(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetRoamConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetRoamConfig(nullptr, nullptr) < 0);
    char buff[] = "N|SetRoamConfig|x|fdsagdsa|safdgfds|1|vcxzcbvx|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetRoamConfig|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetRoamConfig(mServer, mContext) < 0);
    char buff1[] = "N|SetRoamConfig|2|fdsagdsa|safdgfds";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetRoamConfig|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetRoamConfig(mServer, mContext) < 0);
    char buff2[] = "N|SetRoamConfig|2|fdsagdsa|safdgfds|x|vcxzcbvx|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|SetRoamConfig|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcSetRoamConfig(mServer, mContext) < 0);
    char buff3[] = "N|SetRoamConfig|2|fdsagdsa|safdgfds|1|vcxzcbvx";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|SetRoamConfig|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcSetRoamConfig(mServer, mContext) < 0);
    char buff4[] = "N|SetRoamConfig|2|fdsagdsa|safdgfds|1|vcxzcbvx|";
    mContext->oneProcess = buff4;
    mContext->nPos = strlen("N|SetRoamConfig|");
    mContext->nSize = strlen(buff4);
    EXPECT_TRUE(RpcSetRoamConfig(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetConnectSignalInfoTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetConnectSignalInfo(nullptr, nullptr) < 0);
    char buff[] = "N|GetConnectSignalInfo|ssid";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetConnectSignalInfo|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetConnectSignalInfo(mServer, mContext) < 0);
    char buff1[] = "N|GetConnectSignalInfo|00:00:00:00:00:00|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetConnectSignalInfo|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetConnectSignalInfo(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStartSoftApTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStartSoftAp(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStartSoftAp(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcStopSoftApTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcStopSoftAp(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcStopSoftAp(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetHostapdConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetHostapdConfig(nullptr, nullptr) < 0);
    char buff[] = "N|SetHostapdConfig|tests|5|adc123456|9|1|0|6|20|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetHostapdConfig|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetHostapdConfig(mServer, mContext) == 0);
    char buff1[] = "N|SetHostapdConfig|tests|5|adc123456|9|1|0|6|20";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetHostapdConfig|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetHostapdConfig(mServer, mContext) < 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetStaInfosTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetStaInfos(nullptr, nullptr) < 0);
    char buff[] = "N|GetStaInfos|128";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetStaInfos|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetStaInfos(mServer, mContext) < 0);
    char buff1[] = "N|GetStaInfos|128|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetStaInfos|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetStaInfos(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetCountryCode(nullptr, nullptr) < 0);
    char buff[] = "N|SetCountryCode|CN";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetCountryCode|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetCountryCode(mServer, mContext) < 0);
    char buff1[] = "N|SetCountryCode|CN|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetCountryCode|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetCountryCode(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcSetMacFilterTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcSetMacFilter(nullptr, nullptr) < 0);
    char buff[] = "N|SetMacFilter|x|345697dbf921d3|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|SetMacFilter|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcSetMacFilter(mServer, mContext) < 0);
    char buff1[] = "N|SetMacFilter|6|345697dbf921d3|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|SetMacFilter|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcSetMacFilter(mServer, mContext) < 0);
    char buff2[] = "N|SetMacFilter|7|345697dbf921d3|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|SetMacFilter|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcSetMacFilter(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcDelMacFilterTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcDelMacFilter(nullptr, nullptr) < 0);
    char buff[] = "N|DelMacFilter|x|345697dbf921d3|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|DelMacFilter|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcDelMacFilter(mServer, mContext) < 0);
    char buff1[] = "N|DelMacFilter|6|345697dbf921d3|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|DelMacFilter|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcDelMacFilter(mServer, mContext) < 0);
    char buff2[] = "N|DelMacFilter|7|345697dbf921d3|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|DelMacFilter|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcDelMacFilter(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcDisassociateStaTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcDisassociateSta(nullptr, nullptr) < 0);
    char buff[] = "N|DisassociateSta|x|345697dbf921d3|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|DisassociateSta|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcDisassociateSta(mServer, mContext) < 0);
    char buff1[] = "N|DisassociateSta|6|345697dbf921d3|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|DisassociateSta|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcDisassociateSta(mServer, mContext) < 0);
    char buff2[] = "N|DisassociateSta|7|345697dbf921d3|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|DisassociateSta|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcDisassociateSta(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerTest, RpcGetValidFrequenciesForBandTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcGetValidFrequenciesForBand(nullptr, nullptr) < 0);
    char buff[] = "N|GetValidFrequenciesForBand|1|128";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|GetValidFrequenciesForBand|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcGetValidFrequenciesForBand(mServer, mContext) < 0);
    char buff1[] = "N|GetValidFrequenciesForBand|1|128|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|GetValidFrequenciesForBand|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcGetValidFrequenciesForBand(mServer, mContext) == 0);
}
}  // namespace Wifi
}  // namespace OHOS
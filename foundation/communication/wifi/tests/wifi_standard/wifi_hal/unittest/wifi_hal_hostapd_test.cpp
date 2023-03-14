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
#include "wifi_hal_hostapd_test.h"
#include "securec.h"
#include "wifi_log.h"
#include "mock_wpa_ctrl.h"
#include "wifi_hal_crpc_server.h"
#include "wifi_hal_common_func.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
static std::string g_rpcSockPath = "./unix_sock_test.sock";
WifiHostapdHalDevice *WifiHalHostapdTest::mInterface = nullptr;
RpcServer *WifiHalHostapdTest::mServer = nullptr;

void WifiHalHostapdTest::SetUpTestCase()
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
    MockInitApSupportedCmd();
    mInterface = GetWifiHostapdDev();
}

void WifiHalHostapdTest::TearDownTestCase()
{
    sleep(1);
    ReleaseHostapdDev();
    if (mServer != nullptr) {
        ReleaseRpcServer(mServer);
        mServer = nullptr;
    }
    SetRpcServerInited(NULL);
    ReleaseCallbackMsg();
}

HWTEST_F(WifiHalHostapdTest, EnableApTest, TestSize.Level1)
{
    MockEraseSupportedCmd("ENABLE");
    int ret = mInterface->enableAp();
    EXPECT_TRUE(ret != 0);
    MockSetWpaExpectCmdResponse("ENABLE", "OK");
    ret = mInterface->enableAp();
    EXPECT_TRUE(ret == 0);
    MockWpaCallback(mInterface->ctrlConn, "<3>AP-ENABLED \n");
}

HWTEST_F(WifiHalHostapdTest, SetApInfoTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->setApInfo(nullptr) < 0);
    HostapdConfig conf;
    ASSERT_TRUE(memset_s(&conf, sizeof(conf), 0, sizeof(conf)) == EOK);
    conf.securityType = WPA_PSK;
    EXPECT_TRUE(mInterface->setApInfo(&conf) < 0);
    StrSafeCopy(conf.preSharedKey, sizeof(conf.preSharedKey), "1234567");
    EXPECT_TRUE(mInterface->setApInfo(&conf) < 0);
    StrSafeCopy(conf.preSharedKey, sizeof(conf.preSharedKey), "12345678");
    conf.preSharedKeyLen = strlen(conf.preSharedKey);
    EXPECT_TRUE(mInterface->setApInfo(&conf) == 0);
    conf.securityType = WPA2_PSK;
    EXPECT_TRUE(mInterface->setApInfo(&conf) == 0);
    conf.securityType = NONE;
    EXPECT_TRUE(mInterface->setApInfo(&conf) == 0);
    conf.securityType = IEEE8021X;
    EXPECT_TRUE(mInterface->setApInfo(&conf) < 0);
    conf.securityType = NONE;
    conf.band = AP_2GHZ_BAND;
    EXPECT_TRUE(mInterface->setApInfo(&conf) == 0);
    conf.band = AP_5GHZ_BAND;
    EXPECT_TRUE(mInterface->setApInfo(&conf) == 0);
    conf.band = -1;
    EXPECT_TRUE(mInterface->setApInfo(&conf) < 0);
    conf.band = AP_DFS_BAND;
    EXPECT_TRUE(mInterface->setApInfo(&conf) < 0);
}

HWTEST_F(WifiHalHostapdTest, DisableApTest, TestSize.Level1)
{
    MockEraseSupportedCmd("DISABLE");
    int ret = mInterface->disableAp();
    EXPECT_TRUE(ret != 0);
    MockSetWpaExpectCmdResponse("DISABLE", "OK");
    ret = mInterface->disableAp();
    EXPECT_TRUE(ret == 0);
    MockWpaCallback(mInterface->ctrlConn, "<3>AP-DISABLED \n");
}

HWTEST_F(WifiHalHostapdTest, AddBlockListTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->addBlocklist(nullptr) < 0);
    EXPECT_TRUE(mInterface->addBlocklist("00:00:00:00:00:00") == 0);
    MockSetWpaExpectCmdResponse("DENY_ACL", "UNKNOWN COMMAND");
    EXPECT_TRUE(mInterface->addBlocklist("00:00:00:00:00:00") == 0);
    MockSetWpaExpectCmdResponse("DENY_ACL", "OK");
}

HWTEST_F(WifiHalHostapdTest, DelBlockListTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->delBlocklist(nullptr) < 0);
    EXPECT_TRUE(mInterface->delBlocklist("00:00:00:00:00:00") == 0);
    MockSetWpaExpectCmdResponse("DENY_ACL", "UNKNOWN COMMAND");
    EXPECT_TRUE(mInterface->delBlocklist("00:00:00:00:00:00") == 0);
    MockSetWpaExpectCmdResponse("DENY_ACL", "OK");
}

HWTEST_F(WifiHalHostapdTest, GetApStatusTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->status(nullptr) < 0);
    StatusInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(mInterface->status(&info) == 0);
    char buf[BUFSIZE_REQUEST] = "test1=adsgdjks\nteset2=gflsdfis\ntest3="
                                "gfdklse\nstate=disable\ngfdsd=gdhjs\n";
    MockSetWpaExpectCmdResponse("STATUS", buf);
    EXPECT_TRUE(mInterface->status(&info) == 0);
    EXPECT_TRUE(strcmp(info.state, "disable") == 0);
}

HWTEST_F(WifiHalHostapdTest, HostapdCliCmdListStaTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->showConnectedDevList(nullptr, 0) < 0);
    char buff[256] = {0};
    int size = 256;
    MockEraseSupportedCmd("STA-FIRST");
    EXPECT_TRUE(mInterface->showConnectedDevList(buff, size) < 0);
    MockSetWpaExpectCmdResponse("STA-FIRST", "00:00:00:00:00:00");
    MockSetWpaExpectCmdResponse("STA-NEXT", "FAIL");
    EXPECT_TRUE(mInterface->showConnectedDevList(buff, size) == 0);
    EXPECT_TRUE(strcmp(buff, ",00:00:00:00:00:00") == 0);
}

HWTEST_F(WifiHalHostapdTest, ReloadApConfigInfoTest, TestSize.Level1)
{
    MockEraseSupportedCmd("RELOAD");
    int ret = mInterface->reloadApConfigInfo();
    EXPECT_TRUE(ret != 0);
    MockSetWpaExpectCmdResponse("RELOAD", "OK");
    ret = mInterface->reloadApConfigInfo();
    EXPECT_TRUE(ret == 0);
}

HWTEST_F(WifiHalHostapdTest, DisConnectedDevTest, TestSize.Level1)
{
    MockWpaCallback(mInterface->ctrlConn, "<3>AP-STA-CONNECTED 00:00:00:00:00:00\n");
    EXPECT_TRUE(mInterface->disConnectedDev(nullptr) < 0);
    MockEraseSupportedCmd("DISASSOCIATE");
    int ret = mInterface->disConnectedDev("00:00:00:00:00:00");
    EXPECT_TRUE(ret != 0);
    MockSetWpaExpectCmdResponse("DISASSOCIATE", "OK");
    ret = mInterface->disConnectedDev("00:00:00:00:00:00");
    EXPECT_TRUE(ret == 0);
    MockWpaCallback(mInterface->ctrlConn, "<3>AP-STA-DISCONNECTED 00:00:00:00:00:00\n");
}

HWTEST_F(WifiHalHostapdTest, SetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->setCountryCode(nullptr) < 0);
    MockEraseSupportedCmd("SET");
    int ret = mInterface->setCountryCode("CN");
    EXPECT_TRUE(ret != 0);
    MockSetWpaExpectCmdResponse("SET", "OK");
    ret = mInterface->setCountryCode("CN");
    EXPECT_TRUE(ret == 0);
}
}  // namespace Wifi
}  // namespace OHOS
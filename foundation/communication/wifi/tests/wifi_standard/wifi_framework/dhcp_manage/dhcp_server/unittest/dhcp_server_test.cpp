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

#include <gtest/gtest.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include "dhcp_server.h"
#include "address_utils.h"
#include "dhcp_config.h"
#include "dhcp_option.h"
#include "dhcp_logger.h"
#include "system_func_mock.h"
#include "dhcp_message_sim.h"
#include "dhcp_ipv4.h"
#include "common_util.h"
#include "securec.h"

using namespace testing::ext;
using namespace std;
using namespace OHOS::Wifi;

#undef LOG_TAG
#define LOG_TAG "DhcpServerTest"


static const int SERVER_RUNING_TIME = 10; // the value is in units of seconds.

struct ServerContext {
    int broadCastFlagEnable;
    DhcpAddressPool addressPool;
    DhcpServerCallback callback;
    DhcpConfig config;
    int serverFd;
    int looperState;
    int initialized;
};

class DhcpServerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {}
    virtual void TearDown()
    {
        SystemFuncMock::GetInstance().SetMockFlag(false);
    }

    int InitServerConfig(DhcpConfig *config);
    int FreeServerConfig(DhcpConfig *config);
    int TestDhcpRequest(uint32_t testIp, uint32_t srvId);
    int TestDhcpRequestByMac(uint32_t testIp, uint32_t srvId, uint8_t *macAddr);
    int TestDhcpRelease(uint32_t testIp, uint32_t srvId);
    int InitDhcpRequests();
    int InitDhcpErrorRequests();
    bool InitBindingRecodersTest();
    bool FixedOptionsTest();
    int InitDhcpClient();
    bool ServerRun(void);
    bool StartServerTest();
    void DelayStopServer();

private:
    DhcpServerContext *m_pServerCtx = nullptr;
    DhcpConfig m_serverConfg;
    thread delayStopTh;

    DhcpClientContext *m_pMockClient = nullptr;
    DhcpClientConfig m_clientConfg;
};

int DhcpServerTest::InitServerConfig(DhcpConfig *config)
{
    if (!config) {
        return RET_FAILED;
    }
    const char* testIfaceName = "test_if0";
    uint32_t serverId = ParseIpAddr("192.168.189.254");
    uint32_t netmask = ParseIpAddr("255.255.255.0");
    uint32_t beginIp = ParseIpAddr("192.168.189.100");
    uint32_t endIp = ParseIpAddr("192.168.189.200");
    if (serverId == 0 || netmask == 0 || beginIp == 0 || endIp == 0) {
        printf("failed to parse address.\n");
        return RET_FAILED;
    }
    if (memset_s(config, sizeof(DhcpConfig), 0, sizeof(DhcpConfig)) != EOK) {
        return RET_FAILED;
    }
    if (memset_s(config->ifname, sizeof(config->ifname), '\0', sizeof(config->ifname)) != EOK) {
        return RET_FAILED;
    }
    if (strncpy_s(config->ifname, sizeof(config->ifname), testIfaceName, strlen(testIfaceName)) != EOK) {
        return RET_FAILED;
    }
    config->serverId = serverId;
    config->netmask = netmask;
    config->pool.beginAddress = beginIp;
    config->pool.endAddress = endIp;
    config->broadcast = 1;
    if (InitOptionList(&config->options) != RET_SUCCESS) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

extern "C" void InitBindingRecoders(DhcpAddressPool *pool);
bool DhcpServerTest::InitBindingRecodersTest()
{
    if (!m_pServerCtx) {
        return false;
    }
    ServerContext *srvIns = (ServerContext *)m_pServerCtx->instance;
    
    AddressBinding bind = {0};
    bind.bindingMode = BIND_MODE_DYNAMIC;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.189.101");
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    if (AddLease(&srvIns->addressPool, &bind) != RET_SUCCESS) {
        LOGD("failed to add lease recoder.");
        return false;
    }

    uint8_t testMac5[DHCP_HWADDR_LENGTH] =  {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0e, 0};
    uint32_t testIp5 = ParseIpAddr("192.168.189.130");
    bind.ipAddress = testIp5;
    bind.bindingMode = BIND_MODE_DYNAMIC;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac5[i];
    }
    bind.expireIn = Tmspsec();
    bind.bindingTime = bind.expireIn  - DHCP_LEASE_TIME;
    if (AddLease(&srvIns->addressPool, &bind) != RET_SUCCESS) {
        LOGD("failed to add lease recoder.");
        return false;
    }

    uint8_t testMac6[DHCP_HWADDR_LENGTH] =  {0x00, 0xae, 0x3c, 0x65, 0x3a, 0x0e, 0};
    uint32_t testIp6 = ParseIpAddr("192.168.189.118");
    bind.ipAddress = testIp6;
    bind.bindingMode = BIND_MODE_DYNAMIC;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac6[i];
    }
    bind.expireIn = Tmspsec();
    bind.bindingTime = bind.expireIn  - DHCP_LEASE_TIME;
    if (AddLease(&srvIns->addressPool, &bind) != RET_SUCCESS) {
        LOGD("failed to add lease recoder.");
        return false;
    }
    srvIns->addressPool.leaseTime = DHCP_LEASE_TIME;
    srvIns->addressPool.renewalTime = DHCP_RENEWAL_TIME;
    InitBindingRecoders(nullptr);
    DhcpAddressPool pool;
    if (memset_s(&pool, sizeof(DhcpAddressPool), 0, sizeof(DhcpAddressPool)) != EOK) {
        return false;
    }
    InitBindingRecoders(&pool);
    InitBindingRecoders(&srvIns->addressPool);
    return true;
}


bool DhcpServerTest::FixedOptionsTest()
{
    const char *serverVendorId = "ohos-dhcp-server";
    DhcpOption optVendorId = {VENDOR_CLASS_IDENTIFIER_OPTION, 0, {0}};
    if (FillOption(&optVendorId, serverVendorId, strlen(serverVendorId)) != RET_SUCCESS) {
        return false;
    }
    if (PushBackOption(&m_serverConfg.options, &optVendorId) != RET_SUCCESS) {
        return false;
    }
    return true;
}

static int TestServerCallback(int state, int code, const char *ifname)
{
    int ret = 0;
    switch (state) {
        case ST_STARTING: {
            if (code == 0) {
                LOGD(" callback[%s] ==> server starting ...", ifname);
            } else if (code == 1) {
                LOGD(" callback[%s] ==> server started.", ifname);
            } else if (code == NUM_TWO) {
                LOGD(" callback[%s] ==> server start failed.", ifname);
            }
            break;
        }
        case ST_RELOADNG: {
            LOGD(" callback[%s] ==> reloading ...", ifname);
            break;
        }
        case ST_STOPED: {
            LOGD(" callback[%s] ==> server stoped.", ifname);
            break;
        }
        default:
            break;
    }
    return ret;
}

bool DhcpServerTest::ServerRun(void)
{
    LOGD("begin test start dhcp server.");
    int retval = true;
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_CALL(SystemFuncMock::GetInstance(), socket(_, _, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(SystemFuncMock::GetInstance(), setsockopt(_, _, _, _, _))
        .WillOnce(Return(-1))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), select(_, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), bind(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), sendto(_, _, _, _, _, _)).WillRepeatedly(Return(sizeof(DhcpMessage)));
    EXPECT_CALL(SystemFuncMock::GetInstance(), recvfrom(_, _, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), close(_)).WillRepeatedly(Return(0));
    
    m_pServerCtx = InitializeServer(&m_serverConfg);
    if (!m_pServerCtx) {
        LOGE("failed to initialized dhcp server context.");
        retval = false;
    }
    if (!InitBindingRecodersTest()) {
        LOGE("failed to initialize binding recoders.");
        retval = false;
    }
    RegisterDhcpCallback(m_pServerCtx, TestServerCallback);
    if (StartDhcpServer(nullptr) != RET_FAILED) {
        printf("failed to start dhcp server. \n");
        retval = false;
    }
    DhcpServerContext tempCtx;
    memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext));
    memset_s(tempCtx.ifname, sizeof(tempCtx.ifname), '\0', sizeof(tempCtx.ifname));
    if (StartDhcpServer(&tempCtx) != RET_FAILED) {
        printf("failed to start dhcp server. \n");
        retval = false;
    }
    strcpy_s(tempCtx.ifname, sizeof(tempCtx.ifname), "test_if1");
    if (StartDhcpServer(&tempCtx) != RET_FAILED) {
        printf("failed to start dhcp server. \n");
        retval = false;
    }
    if (m_pServerCtx && StartDhcpServer(m_pServerCtx) != RET_SUCCESS) {
        printf("failed to start dhcp server. \n");
        retval = false;
    }
    if (SaveLease(m_pServerCtx) != RET_SUCCESS) {
        retval = false;
    }
    if (m_pServerCtx) {
        FreeServerContext(m_pServerCtx);
    }
    SystemFuncMock::GetInstance().SetMockFlag(false);
    return retval;
}

bool DhcpServerTest::StartServerTest()
{
    bool retval = true;
    if (InitServerConfig(&m_serverConfg) != RET_SUCCESS) {
        LOGD("failed to initialized dhcp server config.");
        retval = false;
    }
    if (!FixedOptionsTest()) {
        LOGE("failed to initialized fixed options.");
        retval = false;
    }
    delayStopTh = std::thread(std::bind(&DhcpServerTest::DelayStopServer, this));
    delayStopTh.detach();
    if (InitDhcpClient() != RET_SUCCESS) {
        retval = false;
    }
    LOGD("wait for test complated...");
    retval = ServerRun();
    return retval;
}

void DhcpServerTest::DelayStopServer()
{
    const int SLEEP_TIME = 3;
    const int SLEEP_TIME1 = 1;
    const int SLEEP_TIME2 = 1;
    LOGD("wait for dhcp server stoped...");
    LOGD("wait %d seconds...\n", SERVER_RUNING_TIME);
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_CALL(SystemFuncMock::GetInstance(), close(_)).WillRepeatedly(Return(0));
    std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));
    if (m_pServerCtx && m_pServerCtx->instance) {
        ServerContext *srvIns = (ServerContext *)m_pServerCtx->instance;
        srvIns->looperState = SLEEP_TIME;
    }
    std::this_thread::sleep_for(std::chrono::seconds(SERVER_RUNING_TIME));
    if (StopDhcpServer(m_pServerCtx) != RET_SUCCESS) {
        LOGD("failed to stop dhcp server.");
        return;
    }
    int waitSesc = 0;
    while (waitSesc < SERVER_RUNING_TIME) {
        if (GetServerStatus(m_pServerCtx) == ST_STOPED) {
            LOGD("dhcp server stoped.");
            break;
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME1));
            waitSesc++;
        }
    }
    SystemFuncMock::GetInstance().SetMockFlag(false);
    std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME2));
}

int DhcpServerTest::TestDhcpRequest(uint32_t testIp, uint32_t srvId)
{
    return TestDhcpRequestByMac(testIp, srvId, nullptr);
}

int DhcpServerTest::TestDhcpRequestByMac(uint32_t testIp, uint32_t srvId, uint8_t *macAddr)
{
    DhcpMsgInfo msgInfo = {{0}, 0, {0}};
    InitOptionList(&msgInfo.options);
    if (!InitMessage(m_pMockClient, &msgInfo, DHCPREQUEST)) {
        LOGD("failed to init dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }
    RemoveOption(&msgInfo.options, END_OPTION);
    if (srvId != 0) {
        DhcpOption optSrvId = {SERVER_IDENTIFIER_OPTION, 0, {0}};
        AppendAddressOption(&optSrvId, srvId);
        PushFrontOption(&msgInfo.options, &optSrvId);
    }
    DhcpOption optReqIp = {REQUESTED_IP_ADDRESS_OPTION, 0, {0}};
    AppendAddressOption(&optReqIp, testIp);
    PushBackOption(&msgInfo.options, &optReqIp);
    DhcpOption endOpt = {END_OPTION, 0, {0}};
    PushBackOption(&msgInfo.options, &endOpt);
    msgInfo.packet.ciaddr = testIp;
    if (macAddr != nullptr) {
        if (!FillHwAddr(msgInfo.packet.chaddr, DHCP_HWADDR_LENGTH, macAddr, MAC_ADDR_LENGTH)) {
            return DHCP_FALSE;
        }
    }
    if (SendDhcpMessage(m_pMockClient, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }
    FreeOptionList(&msgInfo.options);
    return RET_SUCCESS;
}

int DhcpServerTest::TestDhcpRelease(uint32_t testIp, uint32_t srvId)
{
    DhcpMsgInfo msgInfo = {{0}, 0, {0}};
    InitOptionList(&msgInfo.options);
    if (!InitMessage(m_pMockClient, &msgInfo, DHCPRELEASE)) {
        LOGD("failed to init dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }
    RemoveOption(&msgInfo.options, END_OPTION);
    if (srvId != 0) {
        DhcpOption optSrvId = {SERVER_IDENTIFIER_OPTION, 0, {0}};
        AppendAddressOption(&optSrvId, srvId);
        PushBackOption(&msgInfo.options, &optSrvId);
    }
    DhcpOption optReqIp = {REQUESTED_IP_ADDRESS_OPTION, 0, {0}};
    AppendAddressOption(&optReqIp, testIp);
    PushBackOption(&msgInfo.options, &optReqIp);
    msgInfo.packet.ciaddr = testIp;

    DhcpOption endOpt = {END_OPTION, 0, {0}};
    PushBackOption(&msgInfo.options, &endOpt);
    if (SendDhcpMessage(m_pMockClient, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }
    FreeOptionList(&msgInfo.options);
    return RET_SUCCESS;
}

int DhcpServerTest::InitDhcpRequests()
{
    uint32_t testIp = ParseIpAddr("192.168.189.101");
    uint32_t srvId = ParseIpAddr("192.168.189.254");
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (DhcpRequest(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(0);
    if (TestDhcpRequest(testIp, srvId) != RET_SUCCESS) {
        return RET_FAILED;
    }
    testIp = ParseIpAddr("192.168.189.102");
    if (TestDhcpRequest(testIp, 0) != RET_SUCCESS) {
        return RET_FAILED;
    }
    testIp = ParseIpAddr("192.168.189.120");
    uint8_t testMac1[DHCP_HWADDR_LENGTH] =  {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] =  {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0b, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] =  {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0c, 0};
    if (TestDhcpRequestByMac(testIp, srvId, testMac1) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (TestDhcpRequest(testIp, srvId) != RET_SUCCESS) {
        return RET_FAILED;
    }
    testIp = ParseIpAddr("192.168.189.101");
    if (TestDhcpRequestByMac(testIp, srvId, testMac2) != RET_SUCCESS) {
        return RET_FAILED;
    }
    testIp = ParseIpAddr("192.168.189.120");
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (TestDhcpRequestByMac(testIp, srvId, testMac3) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(0);
    uint8_t testMac4[DHCP_HWADDR_LENGTH] =  {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0d, 0};
    testIp = ParseIpAddr("192.168.190.210");
    if (TestDhcpRequestByMac(testIp, srvId, testMac4) != RET_SUCCESS) {
        return RET_FAILED;
    }
    uint8_t testMac5[DHCP_HWADDR_LENGTH] =  {0x0a, 0x0e, 0x3c, 0x65, 0x3a, 0x0e, 0};
    testIp = ParseIpAddr("192.168.189.130");
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (TestDhcpRequestByMac(testIp, srvId, testMac5) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(0);
    return RET_SUCCESS;
}

int DhcpServerTest::InitDhcpErrorRequests()
{
    uint32_t srvId = ParseIpAddr("192.168.100.254");
    uint32_t testIp = ParseIpAddr("192.168.100.101");
    DhcpMsgInfo msgInfo = {{0}, 0, {0}};
    InitOptionList(&msgInfo.options);
    if (!InitMessage(m_pMockClient, &msgInfo, DHCPREQUEST)) {
        LOGD("failed to init dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }

    DhcpOption optReqIp = {REQUESTED_IP_ADDRESS_OPTION, 0, {0}};
    AppendAddressOption(&optReqIp, testIp);
    PushFrontOption(&msgInfo.options, &optReqIp);
    DhcpOption optSrvId = {SERVER_IDENTIFIER_OPTION, 0, {0}};
    AppendAddressOption(&optSrvId, srvId);
    PushFrontOption(&msgInfo.options, &optSrvId);
    if (SendDhcpMessage(m_pMockClient, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }

    RemoveOption(&msgInfo.options, DHCP_MESSAGE_TYPE_OPTION);
    if (SendDhcpMessage(m_pMockClient, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }

    RemoveOption(&msgInfo.options, END_OPTION);
    RemoveOption(&msgInfo.options, SERVER_IDENTIFIER_OPTION);
    if (SendDhcpMessage(m_pMockClient, &msgInfo) != RET_SUCCESS) {
        LOGD("failed to send dhcp message.");
        FreeOptionList(&msgInfo.options);
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

int DhcpServerTest::InitDhcpClient()
{
    uint8_t testMacAddr[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};

    LOGD("init mock dhcp client.");
    const char* testIfname = "test_if0";

    if (memset_s(&m_clientConfg, sizeof(DhcpClientConfig), 0, sizeof(DhcpClientConfig)) != EOK) {
        return RET_FAILED;
    }
    if (!FillHwAddr(m_clientConfg.chaddr, DHCP_HWADDR_LENGTH, testMacAddr, MAC_ADDR_LENGTH)) {
        return RET_FAILED;
    }
    if (memset_s(m_clientConfg.ifname, IFACE_NAME_SIZE, '\0', IFACE_NAME_SIZE) != EOK) {
        return RET_FAILED;
    }
    if (memcpy_s(m_clientConfg.ifname, IFACE_NAME_SIZE, testIfname, strlen(testIfname)) != EOK) {
        return RET_FAILED;
    }

    m_pMockClient = InitialDhcpClient(&m_clientConfg);
    if (!m_pMockClient) {
        return RET_FAILED;
    }
    if (DhcpDiscover(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    uint32_t testIp = ParseIpAddr("192.168.189.102");
    uint32_t srvId = ParseIpAddr("192.168.189.254");
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (DhcpDiscover(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    InitDhcpRequests();
    InitDhcpErrorRequests();
    if (DhcpInform(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (DhcpInform(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(0);
    if (DhcpDecline(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    testIp = ParseIpAddr("192.168.189.118");
    DhcpMsgManager::GetInstance().SetClientIp(testIp);
    if (DhcpDecline(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    DhcpMsgManager::GetInstance().SetClientIp(0);
    if (DhcpRelease(m_pMockClient) != RET_SUCCESS) {
        return RET_FAILED;
    }
    testIp = ParseIpAddr("192.168.189.102");
    if (TestDhcpRelease(testIp, srvId)) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

int DhcpServerTest::FreeServerConfig(DhcpConfig *config)
{
    if (!config) {
        return RET_FAILED;
    }
    FreeOptionList(&config->options);
    return RET_SUCCESS;
}

HWTEST_F(DhcpServerTest, InitializeServerTest, TestSize.Level1)
{
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_CALL(SystemFuncMock::GetInstance(), socket(_, _, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(SystemFuncMock::GetInstance(), setsockopt(_, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), select(_, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), recvfrom(_, _, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), bind(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), close(_)).WillRepeatedly(Return(0));
    DhcpConfig config;
    PDhcpServerContext ctx = InitializeServer(nullptr);
    EXPECT_TRUE(ctx == nullptr);
    ASSERT_TRUE(memset_s(&config, sizeof(DhcpConfig), 0, sizeof(DhcpConfig)) == EOK);
    ctx = InitializeServer(&config);
    EXPECT_TRUE(ctx == nullptr);
    EXPECT_TRUE(strcpy_s(config.ifname, sizeof(config.ifname), "test_if0") == EOK);
    ctx = InitializeServer(&config);
    EXPECT_TRUE(ctx == nullptr);

    EXPECT_EQ(RET_SUCCESS, InitServerConfig(&config));
    ctx = InitializeServer(&config);
    ASSERT_TRUE(ctx != nullptr);

    EXPECT_EQ(RET_SUCCESS, FreeServerConfig(&config));
    EXPECT_EQ(RET_SUCCESS, FreeServerContext(ctx));
}

extern "C" int InitServer(const char *ifname);

HWTEST_F(DhcpServerTest, InitServerByIfaceTest, TestSize.Level1)
{
    const int SO_BROADCAST = 6;
    const int SO_REUSEADDR = 2;
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_CALL(SystemFuncMock::GetInstance(), socket(_, _, _))
        .WillOnce(Return(-1))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(SystemFuncMock::GetInstance(), setsockopt(_, _, SO_REUSEADDR, _, _))
        .WillOnce(Return(-1))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), setsockopt(_, _, SO_BROADCAST, _, _))
        .WillOnce(Return(-1))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), select(_, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), recvfrom(_, _, _, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), bind(_, _, _))
    .WillOnce(Return(-1))
    .WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), close(_)).WillRepeatedly(Return(0));
    const char* ifaceName = "test_if01";
    EXPECT_EQ(-1, InitServer(ifaceName));
    EXPECT_EQ(-1, InitServer(ifaceName));
    EXPECT_EQ(-1, InitServer(ifaceName));
    EXPECT_EQ(1, InitServer(ifaceName));
}

HWTEST_F(DhcpServerTest, StartServerTest, TestSize.Level1)
{
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_TRUE(StartServerTest());
    SystemFuncMock::GetInstance().SetMockFlag(false);
}

extern "C" int ReceiveDhcpMessage(int sock, PDhcpMsgInfo msgInfo);
HWTEST_F(DhcpServerTest, ReceiveDhcpMessageFailedTest, TestSize.Level1)
{
    SystemFuncMock::GetInstance().SetMockFlag(true);
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_CALL(SystemFuncMock::GetInstance(), select(_, _, _, _, _))
        .WillOnce(Return(-1))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(SystemFuncMock::GetInstance(), recvfrom(_, _, _, _, _, _))
        .WillOnce(Return((int)(sizeof(DhcpMsgInfo) + 1)))
        .WillRepeatedly(Return((int)sizeof(DhcpMsgInfo)));

    DhcpMsgInfo msgInfo = {{0}, 0, {0}};
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    EXPECT_EQ(ERR_SELECT, ReceiveDhcpMessage(1, &msgInfo)); // failed to select isset.
    EXPECT_EQ(RET_FAILED, ReceiveDhcpMessage(1, &msgInfo)); // message length error
    EXPECT_EQ(RET_FAILED, ReceiveDhcpMessage(1, &msgInfo)); // dhcp message type error
    msgInfo.packet.hlen = 128;
    EXPECT_EQ(RET_FAILED, ReceiveDhcpMessage(1, &msgInfo)); // hlen error
    msgInfo.packet.hlen = 16;
    msgInfo.packet.op = BOOTREPLY;
    EXPECT_EQ(RET_FAILED, ReceiveDhcpMessage(1, &msgInfo)); // client op type error!
    msgInfo.packet.op = BOOTREQUEST;
    EXPECT_EQ(RET_FAILED, ReceiveDhcpMessage(1, &msgInfo)); // client hardware address error!
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        msgInfo.packet.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_FAILED, ReceiveDhcpMessage(1, &msgInfo));
}

extern "C" int FillReply(PDhcpServerContext ctx, PDhcpMsgInfo received, PDhcpMsgInfo reply);
HWTEST_F(DhcpServerTest, FillReplyFailedTest, TestSize.Level1)
{
    DhcpServerContext tempCtx;
    ASSERT_TRUE(memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext)) == EOK);
    ASSERT_TRUE(memset_s(tempCtx.ifname, sizeof(tempCtx.ifname), '\0', sizeof(tempCtx.ifname)) == EOK);
    DhcpMsgInfo recv, repl;
    ASSERT_TRUE(memset_s(&recv, sizeof(DhcpMsgInfo), 0, sizeof(DhcpMsgInfo)) == EOK);
    ASSERT_TRUE(memset_s(&repl, sizeof(DhcpMsgInfo), 0, sizeof(DhcpMsgInfo)) == EOK);
    EXPECT_EQ(RET_ERROR, FillReply(&tempCtx, &recv, nullptr));
    EXPECT_EQ(RET_FAILED, FillReply(&tempCtx, &recv, &repl));
    recv.packet.flags = 1;
    ServerContext srvInst;
    ASSERT_TRUE(memset_s(&srvInst, sizeof(ServerContext), 0, sizeof(ServerContext)) == EOK);
    tempCtx.instance = &srvInst;
    recv.packet.siaddr = ParseIpAddr("192.168.189.252");
    srvInst.addressPool.gateway = ParseIpAddr("192.168.189.254");
    recv.packet.giaddr = ParseIpAddr("192.168.189.254");
    EXPECT_EQ(RET_SUCCESS, FillReply(&tempCtx, &recv, &repl));
    recv.packet.giaddr = 0;
    EXPECT_EQ(RET_SUCCESS, FillReply(&tempCtx, &recv, &repl));
}

extern "C" int BindNetInterface(int fd, const char *ifname);
HWTEST_F(DhcpServerTest, BindNetInterfaceFailedTest, TestSize.Level1)
{
    SystemFuncMock::GetInstance().SetMockFlag(true);
    EXPECT_CALL(SystemFuncMock::GetInstance(), setsockopt(_, _, _, _, _))
        .WillOnce(Return(-1))
        .WillOnce(Return(0));
    const char* ifaceName = "test_if01";
    EXPECT_EQ(RET_FAILED, BindNetInterface(1, nullptr));
    EXPECT_EQ(RET_FAILED, BindNetInterface(1, ifaceName));
    EXPECT_EQ(RET_SUCCESS, BindNetInterface(1, ifaceName));
}

extern "C" int AddReplyServerIdOption(PDhcpOptionList options, uint32_t serverId);
HWTEST_F(DhcpServerTest, AddReplyServerIdOptionFailedTest, TestSize.Level1)
{
    uint32_t srvId = ParseIpAddr("192.168.189.254");
    ASSERT_TRUE(srvId != 0);
    DhcpOptionList opts;
    ASSERT_TRUE(memset_s(&opts, sizeof(DhcpOptionList), '\0', sizeof(DhcpOptionList)) == EOK);
    EXPECT_EQ(RET_FAILED, AddReplyServerIdOption(nullptr, 0));
    EXPECT_EQ(RET_FAILED, AddReplyServerIdOption(&opts, 0));

    EXPECT_EQ(RET_SUCCESS, AddReplyServerIdOption(&opts, srvId));
    EXPECT_EQ(RET_SUCCESS, AddReplyServerIdOption(&opts, srvId));
}

extern "C" int InitServerFixedOptions(DhcpConfig *config, DhcpServerContext *ctx);
HWTEST_F(DhcpServerTest, InitServerFixedOptionsFailedTest, TestSize.Level1)
{
    DhcpServerContext tempCtx;
    tempCtx.instance = nullptr;
    ASSERT_TRUE(memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext)) == EOK);
    ASSERT_TRUE(memset_s(tempCtx.ifname, sizeof(tempCtx.ifname), '\0', sizeof(tempCtx.ifname)) == EOK);
    DhcpConfig tempConfig;
    ASSERT_TRUE(memset_s(&tempConfig, sizeof(DhcpConfig), 0, sizeof(DhcpConfig)) == EOK);
    EXPECT_EQ(RET_FAILED, InitServerFixedOptions(nullptr, &tempCtx));
    EXPECT_EQ(RET_FAILED, InitServerFixedOptions(&tempConfig, &tempCtx));

    ServerContext srvInst;
    ASSERT_TRUE(memset_s(&srvInst, sizeof(ServerContext), 0, sizeof(ServerContext)) == EOK);
    tempCtx.instance = &srvInst;
    EXPECT_EQ(RET_FAILED, InitServerFixedOptions(&tempConfig, &tempCtx));
}


extern "C" int AppendReplyTimeOptions(PDhcpServerContext ctx, PDhcpOptionList options);
HWTEST_F(DhcpServerTest, AppendReplyTimeOptionsFailedTest, TestSize.Level1)
{
    DhcpServerContext tempCtx;
    tempCtx.instance = nullptr;
    ASSERT_TRUE(memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext)) == EOK);
    ASSERT_TRUE(memset_s(tempCtx.ifname, sizeof(tempCtx.ifname), '\0', sizeof(tempCtx.ifname)) == EOK);
    EXPECT_EQ(RET_FAILED, AppendReplyTimeOptions(nullptr, nullptr));
    EXPECT_EQ(RET_FAILED, AppendReplyTimeOptions(&tempCtx, nullptr));
}

HWTEST_F(DhcpServerTest, FreeServerContextFailedTest, TestSize.Level1)
{
    DhcpServerContext tempCtx;
    tempCtx.instance = nullptr;
    ASSERT_TRUE(memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext)) == EOK);
    ASSERT_TRUE(memset_s(tempCtx.ifname, sizeof(tempCtx.ifname), '\0', sizeof(tempCtx.ifname)) == EOK);
    EXPECT_EQ(RET_FAILED, FreeServerContext(nullptr));
    EXPECT_EQ(RET_FAILED, FreeServerContext(&tempCtx));
}

extern "C" AddressBinding *GetBinding(DhcpAddressPool *pool, PDhcpMsgInfo received);
HWTEST_F(DhcpServerTest, GetBindingFailedTest, TestSize.Level1)
{
    DhcpAddressPool tempPool;
    DhcpMsgInfo msgInfo;
    ASSERT_TRUE(memset_s(&tempPool, sizeof(DhcpAddressPool), 0, sizeof(DhcpAddressPool)) == EOK);
    ASSERT_TRUE(memset_s(&msgInfo, sizeof(DhcpMsgInfo), 0, sizeof(DhcpMsgInfo)) == EOK);
    EXPECT_TRUE(GetBinding(&tempPool, nullptr) == nullptr);
    EXPECT_TRUE(GetBinding(nullptr, &msgInfo) == nullptr);
    ASSERT_EQ(RET_SUCCESS, InitAddressPool(&tempPool, "test_if01", nullptr));
    EXPECT_TRUE(GetBinding(&tempPool, &msgInfo) != nullptr);
    FreeAddressPool(&tempPool);
}

extern "C" int SendDhcpReply(PDhcpServerContext ctx, int replyType, PDhcpMsgInfo reply);
HWTEST_F(DhcpServerTest, SendDhcpReplyTest, TestSize.Level1)
{
    DhcpServerContext tempCtx;
    tempCtx.instance = nullptr;
    ASSERT_TRUE(memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext)) == EOK);
    EXPECT_EQ(RET_FAILED, SendDhcpReply(&tempCtx, REPLY_NAK, nullptr));
    EXPECT_EQ(RET_FAILED, SendDhcpReply(nullptr, REPLY_NAK, nullptr));
}

HWTEST_F(DhcpServerTest, SaveLeaseFailedTest, TestSize.Level1)
{
    DhcpServerContext tempCtx;
    tempCtx.instance = nullptr;
    ASSERT_TRUE(memset_s(&tempCtx, sizeof(DhcpServerContext), 0, sizeof(DhcpServerContext)) == EOK);
    EXPECT_EQ(RET_FAILED, SaveLease(nullptr));
    EXPECT_EQ(RET_FAILED, SaveLease(&tempCtx));
}

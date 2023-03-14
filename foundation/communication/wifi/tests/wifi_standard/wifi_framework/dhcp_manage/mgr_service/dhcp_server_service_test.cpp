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

#include "dhcp_server_service.h"
#include "dhcp_result_notify.h"
#include "mock_system_func.h"
#include "dhcp_func.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Wifi;

class DhcpServerServiceTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        printf("DhcpServerServiceTest SetUp()...\n");
        pServerService = std::make_unique<DhcpServerService>();
        if (pServerService == nullptr) {
            printf("DhcpServerServiceTest SetUp() make_unique DhcpServerService failed\n");
        }
    }
    virtual void TearDown()
    {
        printf("DhcpServerServiceTest TearDown()...\n");
        if (pServerService != nullptr) {
            pServerService.reset(nullptr);
        }
    }
public:
    std::unique_ptr<DhcpServerService> pServerService;
};

HWTEST_F(DhcpServerServiceTest, DhcpServerService_Test2, TestSize.Level1)
{
    ASSERT_TRUE(pServerService != nullptr);

    MockSystemFunc::SetMockFlag(true);

    EXPECT_CALL(MockSystemFunc::GetInstance(), vfork())
        .WillOnce(Return(-1)).WillOnce(Return(1))
        .WillOnce(Return(-1)).WillOnce(Return(1))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), waitpid(_, _, _))
        .WillOnce(Return(-1)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), kill(_, _))
        .WillOnce(Return(-1)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), socket(_, _, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), ioctl(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), open(_, _)).WillRepeatedly(Return(1));

    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StopDhcpServer("wlan1"));

    std::string tagName;
    DhcpRange putRange;
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->PutDhcpRange(tagName, putRange));
    tagName = "sta";
    putRange.iptype = -1;
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->PutDhcpRange(tagName, putRange));
    putRange.iptype = 0;
    putRange.strStartip = "192.168.0.1";
    putRange.strEndip = "192.168.0.49";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->PutDhcpRange(tagName, putRange));
    putRange.leaseHours = 12;
    putRange.strTagName = tagName;
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->PutDhcpRange(tagName, putRange));
    putRange.strStartip = "192.168.0.50";
    putRange.strEndip = "192.168.0.99";
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->PutDhcpRange(tagName, putRange));
    tagName.clear();
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->RemoveDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->RemoveDhcpRange("ap", putRange));
    tagName = "sta";
    putRange.iptype = -1;
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->RemoveDhcpRange(tagName, putRange));
    putRange.iptype = 0;
    putRange.strStartip = "192.168.0.1";
    putRange.strEndip = "192.168.0.49";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->RemoveDhcpRange(tagName, putRange));
    tagName.clear();
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->RemoveAllDhcpRange(tagName));
    tagName = "sta1";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->RemoveAllDhcpRange(tagName));

    std::string ifname;
    DhcpRange setRange;
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->SetDhcpRange(ifname, setRange));
    ifname = "wlan0";
    setRange.iptype = 0;
    setRange.strStartip = "192.168.0.1";
    setRange.strEndip = "192.168.0.49";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname, setRange));
    setRange.leaseHours = 12;
    setRange.strTagName = ifname;
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname, setRange));

    ifname.clear();
    tagName = "sta";
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->SetDhcpRange(ifname, tagName));
    ifname = "wlan0";
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->SetDhcpRange(ifname, "sta1"));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->PutDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->RemoveDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->SetDhcpRange(ifname, tagName));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->PutDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname, tagName));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname, tagName));

    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->StartDhcpServer(ifname));    //start vfork failed
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname));   //start vfork success
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->StartDhcpServer(ifname));    //restart stop kill failed
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->StartDhcpServer(ifname));    //restart stop waitpid failed
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->StartDhcpServer(ifname));    //restart start vfork failed
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname));   //restart start vfork success

    DhcpResultNotify dhcpResultNotify;
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->GetDhcpSerProExit("", nullptr));
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->GetDhcpSerProExit(ifname, nullptr));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->GetDhcpSerProExit(ifname, &dhcpResultNotify));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->GetDhcpSerProExit(ifname, &dhcpResultNotify));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StopDhcpServer(ifname));

    MockSystemFunc::SetMockFlag(false);
}

HWTEST_F(DhcpServerServiceTest, DhcpServerService_Test3, TestSize.Level1)
{
    ASSERT_TRUE(pServerService != nullptr);

    MockSystemFunc::SetMockFlag(true);

    EXPECT_CALL(MockSystemFunc::GetInstance(), vfork())
        .WillOnce(Return(0)).WillOnce(Return(1))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), waitpid(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), kill(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), socket(_, _, _))
        .WillOnce(Return(1)).WillOnce(Return(-1))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), ioctl(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), open(_, _)).WillRepeatedly(Return(1));

    std::string ifname = "wlan0";
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->StartDhcpServer(ifname));    //start vfork child failed
    DhcpRange setRange;
    setRange.iptype = 0;
    setRange.strStartip = "192.168.0.1";
    setRange.strEndip = "192.168.0.49";
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->SetDhcpRange(ifname, setRange));//socket failed
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname, setRange));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StopDhcpServer(ifname));

    std::string ifname2 = "wlan2";
    std::string tagName = "sta2";
    DhcpRange putRange;
    putRange.iptype = 0;
    putRange.strStartip = "192.168.0.50";
    putRange.strEndip = "192.168.0.99";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->PutDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname2, tagName));
    std::string ipRange;
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->GetUsingIpRange(ifname2, ipRange));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname2));   //start vfork parent success
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname2));   //restart start vfork parent success
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StopDhcpServer(ifname2));

    MockSystemFunc::SetMockFlag(false);
}

HWTEST_F(DhcpServerServiceTest, DhcpServerService_Test4, TestSize.Level1)
{
    ASSERT_TRUE(pServerService != nullptr);

    MockSystemFunc::SetMockFlag(true);

    EXPECT_CALL(MockSystemFunc::GetInstance(), vfork())
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), waitpid(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), kill(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), socket(_, _, _))
        .WillOnce(Return(1)).WillOnce(Return(-1))
        .WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), ioctl(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), close(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), open(_, _)).WillRepeatedly(Return(1));

    std::string ifname = "wlan0";
    DhcpRange setRange;
    setRange.iptype = 0;
    setRange.strStartip = "192.168.0.1";
    setRange.strEndip = "192.168.0.49";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->SetDhcpRange(ifname, setRange));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname));
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->ReloadConfig(ifname));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StartDhcpServer(ifname));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->StopDhcpServer(ifname));

    MockSystemFunc::SetMockFlag(false);
}

HWTEST_F(DhcpServerServiceTest, DhcpServerService_Test5, TestSize.Level1)
{
    ASSERT_TRUE(pServerService != nullptr);

    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->ReloadConfig(""));
    EXPECT_EQ(DHCP_OPT_FAILED, pServerService->ReloadConfig("wlan0"));
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->CheckAndUpdateConf(""));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->CheckAndUpdateConf("wlan1"));

    std::string ipRange;
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->GetUsingIpRange("", ipRange));

    DhcpRange checkRange;
    EXPECT_EQ(false, pServerService->CheckIpAddrRange(checkRange));
    checkRange.iptype = 0;
    checkRange.strStartip = "192.168.0";
    checkRange.strEndip = "192.168.1";
    EXPECT_EQ(false, pServerService->CheckIpAddrRange(checkRange));
    checkRange.strStartip = "192.168.0.49";
    checkRange.strEndip = "192.168.1";
    EXPECT_EQ(false, pServerService->CheckIpAddrRange(checkRange));
    checkRange.strEndip = "192.168.0.1";
    EXPECT_EQ(false, pServerService->CheckIpAddrRange(checkRange));

    checkRange.iptype = 1;
    checkRange.strStartip = "fe80:fac8";
    EXPECT_EQ(false, pServerService->CheckIpAddrRange(checkRange));
    checkRange.strStartip = "fe80::fac8";
    checkRange.strEndip = "fe80:fac8";
    EXPECT_EQ(false, pServerService->CheckIpAddrRange(checkRange));

    std::string ifname;
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->AddSpecifiedInterface(ifname));
    ifname = "wlan";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->AddSpecifiedInterface(ifname));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->AddSpecifiedInterface(ifname));

    ifname.clear();
    EXPECT_EQ(DHCP_OPT_ERROR, pServerService->DelSpecifiedInterface(ifname));
    ifname = "wlan";
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->DelSpecifiedInterface(ifname));
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->DelSpecifiedInterface(ifname));

    std::string strFile = DHCP_SERVER_LEASES_FILE + "." + ifname;
    std::string strTestData = "dhcp server leases file test";
    ASSERT_TRUE(DhcpFunc::CreateFile(strFile, strTestData));
    std::vector<std::string> vecLeases;
    EXPECT_EQ(DHCP_OPT_SUCCESS, pServerService->GetLeases(ifname, vecLeases));
    ASSERT_TRUE(DhcpFunc::RemoveFile(strFile));
}

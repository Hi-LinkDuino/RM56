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

#include "dhcp_service.h"
#include "dhcp_result_notify.h"
#include "dhcp_func.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Wifi;

class DhcpServiceTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        printf("DhcpServiceTest SetUp()...\n");
        pDhcpService = std::make_unique<DhcpService>();
        if (pDhcpService == nullptr) {
            printf("DhcpServiceTest SetUp() make_unique DhcpService failed\n");
        }
    }
    virtual void TearDown()
    {
        printf("DhcpServiceTest TearDown()...\n");
        if (pDhcpService != nullptr) {
            pDhcpService.reset(nullptr);
        }
    }
public:
    std::unique_ptr<IDhcpService> pDhcpService;
};

HWTEST_F(DhcpServiceTest, DhcpClientService_Test1, TestSize.Level1)
{
    ASSERT_TRUE(pDhcpService != nullptr);

    std::string ifname = "";
    DhcpResultNotify dhcpResultNotify;
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->GetDhcpResult(ifname, &dhcpResultNotify, 0));
    DhcpServiceInfo dhcpInfo;
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->GetDhcpInfo(ifname, dhcpInfo));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->RenewDhcpClient(ifname));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->ReleaseDhcpClient(ifname));

    bool bIpv6 = false;
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->StartDhcpClient(ifname, bIpv6));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->GetDhcpResult(ifname, &dhcpResultNotify, 0));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->GetDhcpInfo(ifname, dhcpInfo));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->RenewDhcpClient(ifname));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->ReleaseDhcpClient(ifname));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->StopDhcpClient(ifname, bIpv6));
}

HWTEST_F(DhcpServiceTest, DhcpServerService_Test1, TestSize.Level1)
{
    ASSERT_TRUE(pDhcpService != nullptr);

    std::string ifname = "";
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->StartDhcpServer(ifname));
    EXPECT_EQ(0, pDhcpService->GetServerStatus());

    std::string tagName = "";
    DhcpRange putRange;
    EXPECT_EQ(DHCP_OPT_ERROR, pDhcpService->PutDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_ERROR, pDhcpService->RemoveDhcpRange(tagName, putRange));
    EXPECT_EQ(DHCP_OPT_ERROR, pDhcpService->RemoveAllDhcpRange(tagName));
    DhcpRange setRange;
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->SetDhcpRange(ifname, setRange));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->SetDhcpRange(ifname, tagName));

    unlink(DHCP_SERVER_LEASES_FILE.c_str());
    std::vector<std::string> vecLeases;
    EXPECT_EQ(DHCP_OPT_ERROR, pDhcpService->GetLeases(ifname, vecLeases));
    DhcpResultNotify dhcpResultNotify;
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->GetDhcpSerProExit(ifname, &dhcpResultNotify));
    EXPECT_EQ(DHCP_OPT_FAILED, pDhcpService->StopDhcpServer(ifname));
}

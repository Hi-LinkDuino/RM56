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
#include "dhcp_define.h"
#include "dhcp_ipv4.h"
#include "dhcp_message.h"
#include "dhcp_option.h"
#include "address_utils.h"

using namespace testing::ext;

class DhcpOptionTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        if (InitOptionList(&options)) {
            printf("failed to initialized hash table.\n");
        }
    }
    virtual void TearDown()
    {
        FreeOptionList(&options);
    }
public:
    DhcpOptionList options;
};

HWTEST_F(DhcpOptionTest, InitOptionListTest, TestSize.Level1)
{
    DhcpOptionList testOpts = {0};
    EXPECT_EQ(RET_SUCCESS, InitOptionList(&testOpts));
    FreeOptionList(&testOpts);
    EXPECT_EQ(RET_SUCCESS, InitOptionList(&options));
}

HWTEST_F(DhcpOptionTest, HasInitializedTest, TestSize.Level1)
{
    DhcpOptionList testOpts = {0};
    EXPECT_EQ(0, HasInitialized(NULL));
    EXPECT_EQ(0, HasInitialized(&testOpts));
    ASSERT_EQ(RET_SUCCESS, InitOptionList(&testOpts));
    EXPECT_EQ(1, HasInitialized(&testOpts));
    FreeOptionList(&testOpts);
}

HWTEST_F(DhcpOptionTest, PushBackOptionTest, TestSize.Level1)
{
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_SUCCESS, PushBackOption(&options, &optRouter));

    DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, 1, {DHCPOFFER, 0}};
    EXPECT_EQ(RET_ERROR, PushBackOption(NULL, &optMsgType));
    EXPECT_EQ(RET_ERROR, PushBackOption(&options, NULL));
    EXPECT_EQ(RET_SUCCESS, PushBackOption(&options, &optMsgType));
    EXPECT_TRUE(options.size == 2);
    ClearOptions(&options);
    EXPECT_TRUE(options.size == 0);
}

HWTEST_F(DhcpOptionTest, PushFrontOptionTest, TestSize.Level1)
{
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optRouter));

    DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, 1, {DHCPOFFER, 0}};
    EXPECT_EQ(RET_ERROR, PushFrontOption(NULL, &optMsgType));
    EXPECT_EQ(RET_ERROR, PushFrontOption(&options, NULL));
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optMsgType));
    EXPECT_TRUE(options.size == 2);
    ClearOptions(&options);
    EXPECT_TRUE(options.size == 0);
}

HWTEST_F(DhcpOptionTest, GetOptionNodeTest, TestSize.Level1)
{
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optRouter));
    DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, 1, {DHCPOFFER, 0}};
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optMsgType));
    EXPECT_TRUE(options.size == 2);

    DhcpOptionNode *node = GetOptionNode(&options, DHCP_MESSAGE_TYPE_OPTION);
    EXPECT_TRUE(node!=NULL);
    ClearOptions(&options);
    EXPECT_TRUE(options.size == 0);
}

HWTEST_F(DhcpOptionTest, GetOptionTest, TestSize.Level1)
{
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optRouter));
    DhcpOption optMsgType = {DHCP_MESSAGE_TYPE_OPTION, 1, {DHCPOFFER, 0}};
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optMsgType));
    EXPECT_TRUE(options.size == 2);

    DhcpOption *node = GetOption(&options, DHCP_MESSAGE_TYPE_OPTION);
    EXPECT_TRUE(node!=NULL);
    ClearOptions(&options);
    EXPECT_TRUE(options.size == 0);
}


HWTEST_F(DhcpOptionTest, RemoveOptionTest, TestSize.Level1)
{
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_SUCCESS, PushFrontOption(&options, &optRouter));
    EXPECT_TRUE(options.size == 1);
    EXPECT_EQ(RET_ERROR, RemoveOption(NULL, DOMAIN_NAME_SERVER_OPTION));
    EXPECT_EQ(RET_FAILED, RemoveOption(&options, DOMAIN_NAME_SERVER_OPTION));
    EXPECT_EQ(RET_SUCCESS, RemoveOption(&options, ROUTER_OPTION));
    EXPECT_EQ(RET_FAILED, RemoveOption(&options, ROUTER_OPTION));
    EXPECT_TRUE(options.size == 0);
    ClearOptions(&options);
}

HWTEST_F(DhcpOptionTest, FillOptionTest, TestSize.Level1)
{
    const char *serverInfo = "dhcp server 1.0";
    DhcpOption optVendorInfo = {VENDOR_SPECIFIC_INFO_OPTION, 0, {0}};
    EXPECT_EQ(RET_FAILED, FillOption(&optVendorInfo, NULL, 0));
    EXPECT_EQ(RET_ERROR, FillOption(NULL, serverInfo, strlen(serverInfo)));
    EXPECT_EQ(RET_SUCCESS, FillOption(&optVendorInfo, serverInfo, strlen(serverInfo)));
}

HWTEST_F(DhcpOptionTest, FillOptionDataTest, TestSize.Level1)
{
    uint8_t testData[] = {192, 168, 100, 254};
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_ERROR, FillOptionData(NULL, testData, sizeof(testData)));
    EXPECT_EQ(RET_FAILED, FillOptionData(&optRouter, NULL, sizeof(testData)));
    EXPECT_EQ(RET_SUCCESS, FillOptionData(&optRouter, testData, sizeof(testData)));
}

HWTEST_F(DhcpOptionTest, FillU32OptionTest, TestSize.Level1)
{
    uint32_t testIp = ParseIpAddr("192.168.100.254");
    EXPECT_TRUE(testIp != 0);
    DhcpOption optRouter = {ROUTER_OPTION, 0, {0}};
    EXPECT_EQ(RET_SUCCESS, FillU32Option(&optRouter, testIp));
    EXPECT_EQ(RET_ERROR, FillU32Option(NULL, testIp));
}

HWTEST_F(DhcpOptionTest, AppendAddressOptionTest, TestSize.Level1)
{
    uint32_t testDns1 = ParseIpAddr("192.168.100.1");
    EXPECT_TRUE(testDns1 != 0);
    uint32_t testDns2 = ParseIpAddr("192.168.100.2");
    EXPECT_TRUE(testDns2 != 0);
    uint32_t testDns3 = ParseIpAddr("192.168.100.3");
    EXPECT_TRUE(testDns3 != 0);

    DhcpOption optDns = {DOMAIN_NAME_SERVER_OPTION, 0, {0}};
    EXPECT_EQ(RET_ERROR, AppendAddressOption(NULL, testDns1));
    EXPECT_EQ(RET_SUCCESS, AppendAddressOption(&optDns, testDns1));
    EXPECT_EQ(RET_SUCCESS, AppendAddressOption(&optDns, testDns2));
    EXPECT_EQ(RET_SUCCESS, AppendAddressOption(&optDns, testDns3));
    EXPECT_EQ(12, optDns.length);
}

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

#include "dhcp_func.h"
#include "mock_system_func.h"
#include "securec.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Wifi;

class DhcpFuncTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        printf("DhcpFuncTest SetUp()...\n");
    }
    virtual void TearDown()
    {
        printf("DhcpFuncTest TearDown()...\n");
    }
};

HWTEST_F(DhcpFuncTest, Ip4StrConToInt_SUCCESS, TestSize.Level1)
{
    std::string strIp = "192.77.1.232";
    uint32_t uSerIp = 0;
    EXPECT_EQ(true, DhcpFunc::Ip4StrConToInt(strIp, uSerIp));
    printf("DhcpFuncTest Ip4StrConToInt_SUCCESS strIp:%s -> uSerIp:%u.\n", strIp.c_str(), uSerIp);
}

HWTEST_F(DhcpFuncTest, Ip4StrConToInt_FAILED, TestSize.Level1)
{
    std::string strIp = "test4";
    uint32_t uSerIp = 0;
    EXPECT_EQ(false, DhcpFunc::Ip4StrConToInt(strIp, uSerIp));

    strIp.clear();
    uSerIp = 0;
    EXPECT_EQ(false, DhcpFunc::Ip4StrConToInt(strIp, uSerIp));
}

HWTEST_F(DhcpFuncTest, Ip4IntConToStr_SUCCESS, TestSize.Level1)
{
    uint32_t uSerIp = 3226272232;
    std::string strIp = DhcpFunc::Ip4IntConToStr(uSerIp);
    EXPECT_STRNE(strIp.c_str(), "");
    printf("DhcpFuncTest Ip4IntConToStr_SUCCESS uSerIp:%u -> strIp:%s.\n", uSerIp, strIp.c_str());
}

HWTEST_F(DhcpFuncTest, Ip4IntConToStr_FAILED, TestSize.Level1)
{
    uint32_t uSerIp = 0;
    std::string strIp = DhcpFunc::Ip4IntConToStr(uSerIp);
    EXPECT_STREQ(strIp.c_str(), "0.0.0.0");
}

HWTEST_F(DhcpFuncTest, Ip6StrConToChar_SUCCESS, TestSize.Level1)
{
    std::string strIp = "fe80::20c:29ff:fed7:fac8";
    uint8_t	addr6[sizeof(struct in6_addr)] = {0};
    EXPECT_EQ(true, DhcpFunc::Ip6StrConToChar(strIp, addr6, sizeof(struct in6_addr)));
}

HWTEST_F(DhcpFuncTest, Ip6StrConToChar_FAILED, TestSize.Level1)
{
    std::string strIp = "test6";
    uint8_t	addr6[sizeof(struct in6_addr)] = {0};
    EXPECT_EQ(false, DhcpFunc::Ip6StrConToChar(strIp, addr6, sizeof(struct in6_addr)));

    strIp.clear();
    EXPECT_EQ(false, DhcpFunc::Ip6StrConToChar(strIp, addr6, sizeof(struct in6_addr)));
}

HWTEST_F(DhcpFuncTest, Ip6CharConToStr_SUCCESS, TestSize.Level1)
{
    uint8_t	addr6[sizeof(struct in6_addr)] = {0};
    EXPECT_STREQ(DhcpFunc::Ip6CharConToStr(addr6, sizeof(struct in6_addr)).c_str(), "");
}

HWTEST_F(DhcpFuncTest, Ip6CharConToStr_FAILED, TestSize.Level1)
{
    uint8_t	addr6[sizeof(struct in6_addr)] = {0};
    EXPECT_STREQ(DhcpFunc::Ip6CharConToStr(addr6, 0).c_str(), "");
}

HWTEST_F(DhcpFuncTest, CheckIpStr_SUCCESS, TestSize.Level1)
{
    std::string strIp4 = "192.77.1.232";
    EXPECT_EQ(true, DhcpFunc::CheckIpStr(strIp4));

    std::string strIp6 = "fe80::20c:29ff:fed7:fac8";
    EXPECT_EQ(true, DhcpFunc::CheckIpStr(strIp6));
}

HWTEST_F(DhcpFuncTest, CheckIpStr_FAILED, TestSize.Level1)
{
    std::string strIp1 = "192.77.232";
    EXPECT_EQ(false, DhcpFunc::CheckIpStr(strIp1));
    strIp1.clear();
    EXPECT_EQ(false, DhcpFunc::CheckIpStr(strIp1));

    std::string strIp2 = "fe80:fac8";
    EXPECT_EQ(false, DhcpFunc::CheckIpStr(strIp2));

    std::string strIp3 = "192.77.232:fe80:fac8";
    EXPECT_EQ(false, DhcpFunc::CheckIpStr(strIp3));
}

HWTEST_F(DhcpFuncTest, GetLocalIp_TEST, TestSize.Level1)
{
    std::string ifname, ip, netmask;
    EXPECT_EQ(DHCP_OPT_ERROR, DhcpFunc::GetLocalIp(ifname, ip, netmask));

    MockSystemFunc::SetMockFlag(true);

    EXPECT_CALL(MockSystemFunc::GetInstance(), socket(_, _, _)).WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), ioctl(_, _, _))
        .WillOnce(Return(-1))
        .WillOnce(Return(0)).WillOnce(Return(-1))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), close(_)).WillRepeatedly(Return(0));

    ifname = "wlan";
    EXPECT_EQ(DHCP_OPT_FAILED, DhcpFunc::GetLocalIp(ifname, ip, netmask));
    EXPECT_EQ(DHCP_OPT_FAILED, DhcpFunc::GetLocalIp(ifname, ip, netmask));
    EXPECT_EQ(DHCP_OPT_SUCCESS, DhcpFunc::GetLocalIp(ifname, ip, netmask));

    MockSystemFunc::SetMockFlag(false);
}

HWTEST_F(DhcpFuncTest, GetLocalMac_TEST, TestSize.Level1)
{
    MockSystemFunc::SetMockFlag(true);

    EXPECT_CALL(MockSystemFunc::GetInstance(), socket(_, _, _)).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), ioctl(_, _, _))
        .WillOnce(Return(-1)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockSystemFunc::GetInstance(), close(_)).WillRepeatedly(Return(0));

    std::string ifname = "wlan";
    std::string mac;
    EXPECT_EQ(-1, DhcpFunc::GetLocalMac(ifname, mac));
    EXPECT_EQ(-1, DhcpFunc::GetLocalMac(ifname, mac));
    EXPECT_EQ(0, DhcpFunc::GetLocalMac(ifname, mac));

    MockSystemFunc::SetMockFlag(false);
}

HWTEST_F(DhcpFuncTest, CheckRangeNetwork_TEST, TestSize.Level1)
{
    std::string ifname, begin, end;
    EXPECT_EQ(DHCP_OPT_ERROR, DhcpFunc::CheckRangeNetwork(ifname, begin, end));
}

HWTEST_F(DhcpFuncTest, FileManage_SUCCESS, TestSize.Level1)
{
    std::string strFile = "./wlan0.result";
    bool bExist = DhcpFunc::IsExistFile(strFile);
    if (bExist) {
        EXPECT_EQ(true, DhcpFunc::RemoveFile(strFile));
        usleep(200);
    } else {
        EXPECT_EQ(false, DhcpFunc::RemoveFile(strFile));
    }

    std::string strData = "IP4 1624421132 192.168.1.207 192.168.1.2 255.255.255.0 192.168.1.2 * 192.168.1.2 * * 43200";
    EXPECT_EQ(true, DhcpFunc::CreateFile(strFile, strData));

    std::string strAdd = "test add str";
    EXPECT_EQ(true, DhcpFunc::AddFileLineData(strFile, strData, strAdd));
    std::string strModify = "test modify str";
    EXPECT_EQ(true, DhcpFunc::ModifyFileLineData(strFile, strAdd, strModify));
    EXPECT_EQ(true, DhcpFunc::DelFileLineData(strFile, strModify));

    EXPECT_EQ(true, DhcpFunc::RemoveFile(strFile));
}

HWTEST_F(DhcpFuncTest, FileManage_FAILED, TestSize.Level1)
{
    std::string strFile = "./test/wlan0.result";
    EXPECT_EQ(false, DhcpFunc::RemoveFile(strFile));
}

HWTEST_F(DhcpFuncTest, FormatString_SUCCESS, TestSize.Level1)
{
    struct DhcpPacketResult result;
    ASSERT_TRUE(memset_s(&result, sizeof(result), 0, sizeof(result)) == EOK);
    ASSERT_TRUE(strncpy_s(result.strYiaddr, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptServerId, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptSubnet, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptDns1, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptDns2, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptRouter1, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptRouter2, INET_ADDRSTRLEN, "*", INET_ADDRSTRLEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(result.strOptVendor, DHCP_FILE_MAX_BYTES, "*", DHCP_FILE_MAX_BYTES - 1) == EOK);
    EXPECT_EQ(0, DhcpFunc::FormatString(result));
}

HWTEST_F(DhcpFuncTest, InitPidfile_TEST, TestSize.Level1)
{
    std::string pidDir, pidFile;
    EXPECT_EQ(DHCP_OPT_FAILED, DhcpFunc::InitPidfile(pidDir, pidFile));

    MockSystemFunc::SetMockFlag(true);

    EXPECT_CALL(MockSystemFunc::GetInstance(), open(_, _)).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), write(_, _, _)).WillOnce(Return(-1)).WillRepeatedly(Return(1));
    EXPECT_CALL(MockSystemFunc::GetInstance(), close(_)).WillRepeatedly(Return(0));

    pidDir = "./";
    pidFile = "./wlan.pid";
    EXPECT_EQ(DHCP_OPT_FAILED, DhcpFunc::InitPidfile(pidDir, pidFile));
    EXPECT_EQ(DHCP_OPT_FAILED, DhcpFunc::InitPidfile(pidDir, pidFile));
    EXPECT_EQ(DHCP_OPT_SUCCESS, DhcpFunc::InitPidfile(pidDir, pidFile));

    MockSystemFunc::SetMockFlag(false);
}

HWTEST_F(DhcpFuncTest, GetPID_TEST, TestSize.Level1)
{
    std::string pidDir = "./";
    std::string pidFile = "./wlan.pid";
    EXPECT_EQ(DHCP_OPT_SUCCESS, DhcpFunc::InitPidfile(pidDir, pidFile));
    EXPECT_GT(DhcpFunc::GetPID(pidFile), 0);
    unlink(pidFile.c_str());
    EXPECT_EQ(DhcpFunc::GetPID(pidFile), -1);
}

HWTEST_F(DhcpFuncTest, CreateDirs_TEST, TestSize.Level1)
{
    std::string strDir;
    EXPECT_EQ(DhcpFunc::CreateDirs(strDir), DHCP_OPT_FAILED);
}

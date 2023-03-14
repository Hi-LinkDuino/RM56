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
#include "dhcp_address_pool.h"
#include "address_utils.h"
#include "common_util.h"
#include "securec.h"

using namespace testing::ext;

class DhcpAddressPoolTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        if (InitAddressPool(&testPool, "lo", NULL)) {
            printf("failed to initialized address pool.\n");
        }
    }
    virtual void TearDown()
    {
        ResetPollConfig();
        FreeAddressPool(&testPool);
    }

    bool SamplePoolConfig()
    {
        uint32_t beginIp = ParseIpAddr("192.168.100.100");
        uint32_t endIp = ParseIpAddr("192.168.100.150");
        uint32_t netmask = ParseIpAddr("255.255.255.0");
        uint32_t gateway = ParseIpAddr("192.168.100.254");
        uint32_t serverId = ParseIpAddr("192.168.100.1");
        if (beginIp != 0 && endIp != 0 && netmask != 0 && gateway != 0) {
            testPool.addressRange.beginAddress = beginIp;
            testPool.addressRange.endAddress = endIp;
            testPool.netmask = netmask;
            testPool.gateway = gateway;
            testPool.serverId = serverId;
            testPool.leaseTime = DHCP_LEASE_TIME;
            return true;
        }
        return false;
    }

    void ResetPollConfig()
    {
        testPool.addressRange.beginAddress = 0;
        testPool.addressRange.endAddress = 0;
        testPool.netmask = 0;
        testPool.gateway = 0;
        testPool.serverId = 0;
        testPool.leaseTime = 0;
    }

public:
    DhcpAddressPool testPool;

};

HWTEST_F(DhcpAddressPoolTest, AddBindingTest, TestSize.Level1)
{
    AddressBinding bind = {0};
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.1");
    uint32_t testIp2 = ParseIpAddr("192.168.100.2");
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddBinding(&bind));
    bind.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac2[i];
    }

    EXPECT_EQ(RET_ERROR, AddBinding(NULL));
    EXPECT_EQ(RET_SUCCESS, AddBinding(&bind));
    EXPECT_EQ(RET_FAILED, AddBinding(&bind));
    EXPECT_TRUE(memset_s(bind.chaddr, sizeof(bind.chaddr), 0, sizeof(bind.chaddr)) == EOK);
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac2));
}

HWTEST_F(DhcpAddressPoolTest, AddNewBindingTest, TestSize.Level1)
{
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    ASSERT_TRUE(testPool.newBinding != NULL);
    ASSERT_TRUE(testPool.newBinding(testMac1, NULL) != NULL);
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
}

HWTEST_F(DhcpAddressPoolTest, FindBindingByIpTest, TestSize.Level1)
{
    AddressBinding bind = {0};
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.1");
    uint32_t testIp2 = ParseIpAddr("192.168.100.2");
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddBinding(&bind));
    bind.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac2[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddBinding(&bind));
    EXPECT_EQ(RET_FAILED, AddBinding(&bind));

    AddressBinding *pBind1 = FindBindingByIp(testIp1);
    AddressBinding *pBind2 = FindBindingByIp(testIp2);
    EXPECT_TRUE(pBind1 != NULL);
    EXPECT_TRUE(pBind2 != NULL);
    if (pBind1 != NULL) {
        EXPECT_EQ(testIp1, pBind1->ipAddress);
    }
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac2));
}

HWTEST_F(DhcpAddressPoolTest, AddressDistributeTest, TestSize.Level1)
{
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};

    EXPECT_TRUE(testPool.distribue(NULL, testMac1) == 0);
    EXPECT_TRUE(testPool.distribue(&testPool, testMac1) == 0);
    ASSERT_TRUE(SamplePoolConfig());
    SetDistributeMode(0);
    EXPECT_EQ(0, GetDistributeMode());
    uint32_t assertAddr = ParseIpAddr("192.168.100.101");
    EXPECT_EQ(assertAddr, testPool.distribue(&testPool, testMac1));
    assertAddr = ParseIpAddr("192.168.100.102");
    EXPECT_EQ(assertAddr, testPool.distribue(&testPool, testMac2));
    assertAddr = ParseIpAddr("192.168.100.103");
    EXPECT_EQ(assertAddr, testPool.distribue(&testPool, testMac3));
}

HWTEST_F(DhcpAddressPoolTest, IsReservedTest, TestSize.Level1)
{
    AddressBinding bind = {0};
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0b, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.1");
    ASSERT_TRUE(testIp1 != 0);
    uint32_t testIp2 = ParseIpAddr("192.168.100.2");
    ASSERT_TRUE(testIp2 != 0);
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddBinding(&bind));
    bind.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac2[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddReservedBinding(testMac2));
    EXPECT_EQ(RET_SUCCESS, AddReservedBinding(testMac2));
    EXPECT_EQ(RET_FAILED, AddBinding(&bind));
    EXPECT_EQ(0, IsReserved(testMac1));
    EXPECT_EQ(1, IsReserved(testMac2));
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac3[i];
    }
    EXPECT_EQ(0, IsReserved(testMac3));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac2));
}


HWTEST_F(DhcpAddressPoolTest, IsReservedIpTest, TestSize.Level1)
{
    AddressBinding bind = {0};
    bind.bindingMode = BIND_MODE_DYNAMIC;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.1");
    ASSERT_TRUE(testIp1 != 0);
    uint32_t testIp2 = ParseIpAddr("192.168.100.2");
    ASSERT_TRUE(testIp2 != 0);
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &bind));
    bind.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac2[i];
    }
    bind.bindingMode = BIND_MODE_RESERVED;
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &bind));
    EXPECT_EQ(0, IsReservedIp(&testPool, testIp1));
    EXPECT_EQ(1, IsReservedIp(&testPool, testIp2));
    EXPECT_EQ(DHCP_FALSE, IsReservedIp(NULL, testIp1));
    EXPECT_EQ(DHCP_FALSE, IsReservedIp(&testPool, 0));
    bind.ipAddress = testIp1;
    EXPECT_EQ(RET_SUCCESS, RemoveLease(&testPool, &bind));
    bind.ipAddress = testIp2;
    EXPECT_EQ(RET_SUCCESS, RemoveLease(&testPool, &bind));
}

HWTEST_F(DhcpAddressPoolTest, RemoveReservedBindingTest, TestSize.Level1)
{
    AddressBinding bind = {0}, bind2 = {0};
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x01, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x00, 0x02, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x00, 0x03, 0x3c, 0x65, 0x3a, 0x0b, 0};
    uint8_t testMac4[DHCP_HWADDR_LENGTH] = {0x00, 0x03, 0x3c, 0x65, 0x3a, 0x0c, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.1");
    EXPECT_TRUE(testIp1 != 0);
    uint32_t testIp2 = ParseIpAddr("192.168.100.2");
    EXPECT_TRUE(testIp2 != 0);
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddBinding(&bind));
    bind2.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind2.chaddr[i] = testMac2[i];
    }
    bind2.bindingMode = BIND_MODE_RESERVED;
    ASSERT_EQ(RET_SUCCESS, AddBinding(&bind2));
    EXPECT_EQ(RET_FAILED, RemoveReservedBinding(testMac1));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
    AddressBinding *binding = QueryBinding(testMac2, NULL);
    ASSERT_TRUE(binding != NULL);
    EXPECT_EQ(RET_SUCCESS, RemoveReservedBinding(testMac2));
    EXPECT_EQ(RET_FAILED, RemoveReservedBinding(testMac2));
    EXPECT_EQ(RET_FAILED, RemoveReservedBinding(testMac3));
    EXPECT_EQ(RET_FAILED, RemoveReservedBinding(testMac4));
}

HWTEST_F(DhcpAddressPoolTest, ReleaseBindingTest, TestSize.Level1)
{
    AddressBinding bind = {0};
    bind.bindingMode = BIND_ASSOCIATED;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x0b, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.1");
    ASSERT_TRUE(testIp1 != 0);
    uint32_t testIp2 = ParseIpAddr("192.168.100.2");
    ASSERT_TRUE(testIp2 != 0);
    bind.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac1[i];
    }
    ASSERT_EQ(RET_SUCCESS, AddBinding(&bind));
    bind.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        bind.chaddr[i] = testMac2[i];
    }
    ASSERT_EQ(RET_SUCCESS, AddBinding(&bind));
    EXPECT_EQ(RET_SUCCESS, ReleaseBinding(testMac1));
    EXPECT_EQ(RET_FAILED, ReleaseBinding(testMac3));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac2));
}

HWTEST_F(DhcpAddressPoolTest, AddLeaseTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3d, 0x65, 0x3a, 0x0a, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    ASSERT_TRUE(testIp1 != 0);
    lease.ipAddress = testIp1;
    lease.leaseTime = DHCP_LEASE_TIME;
    lease.pendingTime = 1631240659;
    lease.bindingTime = 1631240659;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    ASSERT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac2[i];
    }
    EXPECT_EQ(RET_ERROR, AddLease(NULL, &lease));
    EXPECT_EQ(RET_ERROR, AddLease(&testPool, NULL));
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    EXPECT_EQ(RET_SUCCESS, RemoveLease(&testPool, &lease));
}

HWTEST_F(DhcpAddressPoolTest, GetLeaseTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    uint32_t testIp2 = ParseIpAddr("192.168.100.110");
    ASSERT_TRUE(testIp1 != 0);
    ASSERT_TRUE(testIp2 != 0);
    lease.ipAddress = testIp1;
    lease.leaseTime = DHCP_LEASE_TIME;
    lease.pendingTime = 1631240659;
    lease.bindingTime = 1631240659;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    EXPECT_EQ(NULL, GetLease(&testPool, 0));
    EXPECT_EQ(NULL, GetLease(NULL, testIp1));
    EXPECT_EQ(NULL, GetLease(&testPool, testIp2));
    AddressBinding *leaseRec = GetLease(&testPool, testIp1);
    ASSERT_TRUE(leaseRec != NULL);
    EXPECT_EQ(lease.ipAddress, leaseRec->ipAddress);
    EXPECT_EQ(lease.leaseTime, leaseRec->leaseTime);
    EXPECT_EQ(lease.bindingMode, leaseRec->bindingMode);
    EXPECT_EQ(lease.bindingStatus, leaseRec->bindingStatus);
    EXPECT_EQ(RET_SUCCESS, RemoveLease(&testPool, &lease));
}

HWTEST_F(DhcpAddressPoolTest, UpdateLeaseTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    uint32_t testIp2 = ParseIpAddr("192.168.100.110");
    ASSERT_TRUE(testIp1 != 0);
    ASSERT_TRUE(testIp2 != 0);
    lease.ipAddress = testIp1;
    lease.leaseTime = DHCP_LEASE_TIME;
    lease.pendingTime = 1631240659;
    lease.bindingTime = 1631240659;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    AddressBinding *leaseRec = GetLease(&testPool, testIp1);
    ASSERT_TRUE(leaseRec != NULL);
    EXPECT_EQ(lease.ipAddress, leaseRec->ipAddress);
    EXPECT_EQ(lease.leaseTime, leaseRec->leaseTime);
    EXPECT_EQ(lease.bindingMode, leaseRec->bindingMode);
    EXPECT_EQ(lease.bindingStatus, leaseRec->bindingStatus);
    lease.pendingTime = 1631260680;
    lease.bindingTime = 1631260680;
    EXPECT_EQ(RET_SUCCESS, UpdateLease(&testPool, &lease));
    EXPECT_EQ(lease.leaseTime, leaseRec->leaseTime);
    EXPECT_EQ(lease.leaseTime, leaseRec->leaseTime);
    EXPECT_EQ(RET_ERROR, UpdateLease(NULL, &lease));
    EXPECT_EQ(RET_ERROR, UpdateLease(&testPool, NULL));
    lease.ipAddress = testIp2;
    EXPECT_EQ(RET_FAILED, UpdateLease(&testPool, &lease));
    lease.ipAddress = testIp1;
    EXPECT_EQ(RET_SUCCESS, RemoveLease(&testPool, &lease));
}


HWTEST_F(DhcpAddressPoolTest, LoadBindingRecodersTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    ASSERT_TRUE(testIp1 != 0);
    uint32_t testIp2 = ParseIpAddr("192.168.100.102");
    ASSERT_TRUE(testIp2 != 0);
    uint32_t testIp3 = ParseIpAddr("192.168.100.103");
    ASSERT_TRUE(testIp3!= 0);

    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    lease.pendingTime = 1631260680;
    lease.bindingTime = 1631260680;

    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x0b, 0};
    lease.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    lease.ipAddress = testIp2;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac2[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    lease.ipAddress = testIp3;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac3[i];
    }
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));

    EXPECT_EQ(RET_SUCCESS, SaveBindingRecoders(&testPool, 1));
    EXPECT_EQ(HASH_SUCCESS, ClearAll(&testPool.leaseTable));
    EXPECT_TRUE(testPool.leaseTable.size == 0);
    EXPECT_EQ(RET_FAILED, LoadBindingRecoders(NULL));
    EXPECT_EQ(RET_SUCCESS, LoadBindingRecoders(&testPool));
    EXPECT_TRUE(testPool.leaseTable.size == 3);
    EXPECT_TRUE(GetLease(&testPool, testIp1) != NULL);
    EXPECT_TRUE(GetLease(&testPool, testIp2) != NULL);
    EXPECT_TRUE(GetLease(&testPool, testIp3) != NULL);
    EXPECT_EQ(HASH_SUCCESS, ClearAll(&testPool.leaseTable));
}


extern "C" int CheckRangeAvailability(
    DhcpAddressPool *pool, uint8_t macAddr[DHCP_HWADDR_LENGTH], uint32_t distIp, int *outOfRange);

HWTEST_F(DhcpAddressPoolTest, CheckRangeAvailabilityTest, TestSize.Level1)
{
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3b, 0x0b, 0};
    uint8_t testMac4[DHCP_HWADDR_LENGTH] = {0};
    
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    uint32_t testIp2 = ParseIpAddr("192.168.100.102");
    uint32_t testIp3 = ParseIpAddr("192.168.100.3");

    int outOfRange = 0;
    EXPECT_EQ(RET_ERROR, CheckRangeAvailability(NULL, testMac1, testIp1, &outOfRange));
    EXPECT_EQ(RET_ERROR, CheckRangeAvailability(&testPool, testMac1, testIp1, &outOfRange));
    EXPECT_TRUE(SamplePoolConfig());
    EXPECT_EQ(RET_ERROR, CheckRangeAvailability(&testPool, testMac4, testIp1, &outOfRange));
    EXPECT_EQ(0, outOfRange);
    EXPECT_EQ(RET_SUCCESS, CheckRangeAvailability(&testPool, testMac1, testIp1, &outOfRange));
    EXPECT_EQ(0, outOfRange);
    EXPECT_EQ(RET_SUCCESS, CheckRangeAvailability(&testPool, testMac2, testIp2, &outOfRange));
    EXPECT_EQ(0, outOfRange);
    EXPECT_EQ(RET_FAILED, CheckRangeAvailability(&testPool, testMac3, testIp3, &outOfRange));
    EXPECT_EQ(1, outOfRange);

    

}

extern "C" int CheckIpAvailability(DhcpAddressPool *pool, uint8_t macAddr[DHCP_HWADDR_LENGTH], uint32_t distIp);
HWTEST_F(DhcpAddressPoolTest, CheckIpAvailabilityTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x0d, 0x0a, 0x3c, 0x65, 0x3a, 0x09, 0};
    uint8_t testMac2[DHCP_HWADDR_LENGTH] = {0x0d, 0x0a, 0x3d, 0x65, 0x3a, 0x0a, 0};
    uint8_t testMac3[DHCP_HWADDR_LENGTH] = {0x0d, 0x0a, 0x3d, 0x65, 0x3a, 0x0b, 0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    uint32_t testIp2 = ParseIpAddr("192.168.100.102");
    uint32_t testIp3 = ParseIpAddr("192.168.150.103");
    ASSERT_TRUE(testIp1 != 0);
    ASSERT_TRUE(testIp2 != 0);
    ASSERT_TRUE(testIp3 != 0);
    lease.ipAddress = testIp1;
    lease.leaseTime = DHCP_LEASE_TIME;
    lease.pendingTime = 1631240659;
    lease.bindingTime = 1631240659;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    EXPECT_EQ(RET_SUCCESS, AddBinding(&lease));
    ASSERT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    EXPECT_EQ(DHCP_TRUE, CheckIpAvailability(&testPool, testMac1, testIp1));
    AddressBinding *pLease = GetLease(&testPool, testIp1);
    ASSERT_TRUE(pLease != NULL);
    pLease->bindingMode = BIND_MODE_STATIC;
    EXPECT_EQ(DHCP_FALSE, CheckIpAvailability(&testPool, testMac2, testIp1));
    pLease->bindingMode = BIND_MODE_DYNAMIC;

    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac2[i];
    }
    lease.ipAddress = testIp2;
    lease.bindingMode = BIND_MODE_RESERVED;
    EXPECT_EQ(RET_SUCCESS, AddBinding(&lease));
    EXPECT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));
    EXPECT_EQ(DHCP_FALSE, CheckIpAvailability(&testPool, testMac2, testIp2));

    EXPECT_TRUE(SamplePoolConfig());
    EXPECT_EQ(DHCP_FALSE, CheckIpAvailability(NULL, testMac1, testIp1));
    EXPECT_EQ(DHCP_FALSE, CheckIpAvailability(&testPool, testMac3, testIp1));

    pLease->pendingTime = Tmspsec() - DHCP_LEASE_TIME - 2600;
    pLease->bindingTime = pLease->pendingTime;
    EXPECT_EQ(DHCP_TRUE, CheckIpAvailability(&testPool, testMac3, testIp1));
}


extern "C" AddressBinding *GetBindingByMac(HashTable *bindTable, uint8_t macAddr[DHCP_HWADDR_LENGTH]);

HWTEST_F(DhcpAddressPoolTest, GetBindingByMacTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    ASSERT_TRUE(testIp1 != 0);
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    lease.pendingTime = 1631260680;
    lease.bindingTime = 1631260680;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    lease.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    ASSERT_EQ(RET_SUCCESS, AddBinding(&lease));
    AddressBinding *binding = QueryBinding(testMac1, 0);
    ASSERT_TRUE(binding != NULL);
    HashTable tempTable = {0};
    EXPECT_TRUE(GetBindingByMac(NULL, testMac1) == NULL);
    EXPECT_TRUE(GetBindingByMac(&tempTable, testMac1) == NULL);
    EXPECT_EQ(lease.ipAddress, binding->ipAddress);
    EXPECT_EQ(lease.leaseTime, binding->leaseTime);
    EXPECT_EQ(lease.bindingMode, binding->bindingMode);
    EXPECT_EQ(lease.bindingStatus, binding->bindingStatus);
    EXPECT_EQ(RET_SUCCESS, RemoveBinding(testMac1));
}

HWTEST_F(DhcpAddressPoolTest, GetBindingByIpTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.101");
    ASSERT_TRUE(testIp1 != 0);
    lease.bindingMode = BIND_MODE_DYNAMIC;
    lease.bindingStatus = BIND_ASSOCIATED;
    lease.pendingTime = 1631260680;
    lease.bindingTime = 1631260680;
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x01, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    lease.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    ASSERT_EQ(RET_SUCCESS, AddLease(&testPool, &lease));

    HashTable tempTable = {0};
    EXPECT_TRUE(GetBindingByIp(NULL, testIp1) == NULL);
    EXPECT_TRUE(GetBindingByIp(&tempTable, testIp1) == NULL);
    AddressBinding *pLease = GetLease(&testPool, testIp1);
    ASSERT_TRUE(pLease != NULL);
    EXPECT_EQ(lease.ipAddress, pLease->ipAddress);
    EXPECT_EQ(lease.leaseTime, pLease->leaseTime);
    EXPECT_EQ(lease.bindingMode, pLease->bindingMode);
    EXPECT_EQ(lease.bindingStatus, pLease->bindingStatus);
}

HWTEST_F(DhcpAddressPoolTest, InitAddressPoolTest, TestSize.Level1)
{
    DhcpAddressPool tempPool;
    ASSERT_TRUE(memset_s(&tempPool, sizeof(DhcpAddressPool), 0, sizeof(DhcpAddressPool)) == EOK);
    tempPool.fixedOptions.size = 100;
    EXPECT_EQ(RET_ERROR, InitAddressPool(NULL, "test_if2", NULL));
    EXPECT_EQ(RET_SUCCESS, InitAddressPool(&tempPool, "test_if2", NULL));
    FreeAddressPool(NULL);
    FreeAddressPool(&tempPool);
}

extern "C" uint32_t NextIpOffset(uint32_t netmask);
HWTEST_F(DhcpAddressPoolTest, NextIpOffsetTest, TestSize.Level1)
{
    uint32_t netmask = ParseIpAddr("255.255.255.0");
    ASSERT_TRUE(netmask != 0);
    SetDistributeMode(0);
    EXPECT_TRUE(NextIpOffset(netmask) == 0);
    SetDistributeMode(1);
    usleep(10);
    uint32_t nextOffset = NextIpOffset(netmask);
    EXPECT_TRUE(nextOffset >= 0);
    SetDistributeMode(0);
}

HWTEST_F(DhcpAddressPoolTest, RemoveLeaseFailedTest, TestSize.Level1)
{
    AddressBinding lease = {0};
    uint32_t testIp1 = ParseIpAddr("192.168.100.110");
    ASSERT_TRUE(testIp1 != 0);
    uint8_t testMac1[DHCP_HWADDR_LENGTH] = {0x00, 0x0e, 0x3c, 0x65, 0x3a, 0x09, 0};
    lease.ipAddress = testIp1;
    for (int i = 0; i < MAC_ADDR_LENGTH; ++i) {
        lease.chaddr[i] = testMac1[i];
    }
    EXPECT_EQ(RET_ERROR, RemoveLease(NULL, &lease));
    EXPECT_EQ(RET_ERROR, RemoveLease(&testPool, NULL));
    EXPECT_EQ(RET_FAILED, RemoveLease(&testPool, &lease));
}
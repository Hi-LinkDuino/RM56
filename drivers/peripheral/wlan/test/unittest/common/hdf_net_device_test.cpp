/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"
#include "hdf_wifi_test.h"

using namespace testing::ext;

namespace NetDeviceTest {
class WiFiNetDeviceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WiFiNetDeviceTest::SetUpTestCase()
{
    HdfTestOpenService();
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_INIT, -1};
    HdfTestSendMsgToService(&msg);
}

void WiFiNetDeviceTest::TearDownTestCase()
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_DEINIT, -1};
    HdfTestSendMsgToService(&msg);
    HdfTestCloseService();
}

void WiFiNetDeviceTest::SetUp() {}

void WiFiNetDeviceTest::TearDown() {}

/**
  * @tc.name: WiFiNetDeviceInit001
  * @tc.desc: init netdevice function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceInit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_INIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceAdd001
  * @tc.desc: add netdevice function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceAdd001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_ADD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceGet001
  * @tc.desc: get netdevice function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceGet001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_GET, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceGetCount001
  * @tc.desc: netdevice count query function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceGetCount001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_GET_COUNT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceGetCap001
  * @tc.desc: netdevice capability query function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceGetCap001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_GET_CAP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceSetAddr001
  * @tc.desc: set netdevice addr function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceSetAddr001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_SET_ADDR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceDhcpClient001
  * @tc.desc: netdevice dhcp client function test
  * @tc.type: FUNC
  * @tc.require: AR000F869L
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceDhcpClient001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_DHCPC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceDhcpServer001
  * @tc.desc: netdevice dhcp server function test
  * @tc.type: FUNC
  * @tc.require: AR000F869L
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceDhcpServer001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_DHCPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceSetStatus001
  * @tc.desc: set netdevice status function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceSetStatus001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_SET_STATUS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceSetLinkStatus001
  * @tc.desc: set netdevice link status function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceSetLinkStatus001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_SET_LINK_STATUS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceRx001
  * @tc.desc: netdevice receive packet function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceRx001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_RX, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceDelete001
  * @tc.desc: delete netdevice function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceDelete001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_DELETE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: WiFiNetDeviceDeInit001
  * @tc.desc: deinit netdevice function test
  * @tc.type: FUNC
  * @tc.require: AR000F869H
  */
HWTEST_F(WiFiNetDeviceTest, WiFiNetDeviceDeInit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_WIFI_TYPE, WIFI_NET_DEVICE_DEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
};

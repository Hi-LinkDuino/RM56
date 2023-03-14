/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <securec.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <arpa/inet.h>
#include <net/if.h>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

#include "auth_interface.h"
#include "bus_center_info_key.h"
#include "bus_center_manager.h"
#include "lnn_network_manager.h"
#include "lnn_distributed_net_ledger.h"
#include "lnn_event_monitor.h"
#include "lnn_local_net_ledger.h"
#include "lnn_sync_item_info.h"
#include "softbus_bus_center.h"
#include "softbus_conn_interface.h"
#include "softbus_def.h"
#include "softbus_errcode.h"

namespace OHOS {
using namespace testing::ext;

static int InitServer()
{
    if (ConnServerInit() == SOFTBUS_ERR) {
        printf("softbus conn server init failed.");
        return SOFTBUS_ERR;
    }
    if (AuthInit() == SOFTBUS_ERR) {
        printf("softbus auth init failed.");
        return SOFTBUS_ERR;
    }
    if (LnnInitLocalLedger() != SOFTBUS_OK) {
        printf("init local net ledger fail!");
        return SOFTBUS_ERR;
    }
    if (LnnInitDistributedLedger() != SOFTBUS_OK) {
        printf("init distributed net ledger fail!");
        return SOFTBUS_ERR;
    }
    if (LnnInitEventMonitor() != SOFTBUS_OK) {
        printf("init event monitor failed");
        return SOFTBUS_ERR;
    }
    if (LnnInitNetworkManager() != SOFTBUS_OK) {
        printf("init lnn network manager fail!");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

class NetBusCenterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetBusCenterTest::SetUpTestCase()
{
    EXPECT_TRUE(InitServer() == SOFTBUS_OK);
    sleep(2);
}

void NetBusCenterTest::TearDownTestCase()
{
}

void NetBusCenterTest::SetUp()
{
}

void NetBusCenterTest::TearDown()
{
}

static int SetIpaddr(const std::string ip)
{
    int sockFd  = -1;
    struct sockaddr_in addr;
    struct ifreq ifr;
    if (memset_s((void *)&addr, sizeof(addr), 0, sizeof(addr)) != EOK) {
        printf("memset_s 1 fail\n");
        return SOFTBUS_ERR;
    }
    if (memset_s((void *)&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        printf("memset_s 3 fail\n");
        return SOFTBUS_ERR;
    }

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0) {
        printf("could not create IP socket\n");
        return SOFTBUS_ERR;
    }
    if (strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0", strlen("eth0")) != EOK) {
        printf("strncpy_s fail\n");
        close(sockFd);
        return SOFTBUS_ERR;
    }

    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr)) <= 0) {
        printf("inet_pton fail for ip\n");
        close(sockFd);
        return SOFTBUS_ERR;
    }
    (void)memcpy_s(&ifr.ifr_addr, sizeof(addr), &addr, sizeof(addr));
    if (ioctl(sockFd, SIOCSIFADDR, &ifr) < 0) {
        printf("error to set interface address, error: %s\n", strerror(errno));
        close(sockFd);
        return SOFTBUS_ERR;
    }

    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (ioctl(sockFd, SIOCSIFFLAGS, &ifr) < 0) {
        printf("error to set eth0 up\n");
        close(sockFd);
        return SOFTBUS_ERR;
    }
    close(sockFd);
    return SOFTBUS_OK;
}

static int SetIpDown()
{
    int sockFd  = -1;
    struct ifreq ifr;
    if (memset_s((void *)&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        printf("memset_s 1 fail\n");
        return SOFTBUS_ERR;
    }

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0) {
        printf("could not create IP socket\n");
        return SOFTBUS_ERR;
    }
    if (strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0", strlen("eth0")) != EOK) {
        printf("strncpy_s fail\n");
        close(sockFd);
        return SOFTBUS_ERR;
    }

    if (ioctl(sockFd, SIOCGIFFLAGS, &ifr) < 0) {
        printf("error to get eth0 flags\n");
        close(sockFd);
        return SOFTBUS_ERR;
    }

    ifr.ifr_flags &= ~(IFF_UP);
    if (ioctl(sockFd, SIOCSIFFLAGS, &ifr) < 0) {
        printf("error to set eth0 down\n");
        close(sockFd);
        return SOFTBUS_ERR;
    }
    close(sockFd);
    return SOFTBUS_OK;
}

/*
* @tc.name: NET_BusCenter_IP_Change_Monitor_Test_001
* @tc.desc: IP change monitor test
* @tc.type: FUNC
* @tc.require: AR000FK6J3
*/
HWTEST_F(NetBusCenterTest, NET_BusCenter_IP_Change_Monitor_Test_001, TestSize.Level0)
{
    char ipAddr[IP_LEN] = {0};
    char ip[IP_LEN] = "192.168.50.10";

    EXPECT_TRUE(SetIpaddr(ip) == SOFTBUS_OK);
    sleep(2);
    EXPECT_TRUE(LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr, IP_LEN) == SOFTBUS_OK);
    EXPECT_TRUE(strncmp(ipAddr, ip, IP_LEN) == 0);
}

/*
* @tc.name: NET_BusCenter_IP_Change_Monitor_Test_002
* @tc.desc: IP change monitor test
* @tc.type: FUNC
* @tc.require: AR000FK6J3
*/
HWTEST_F(NetBusCenterTest, NET_BusCenter_IP_Change_Monitor_Test_002, TestSize.Level0)
{
    char ipAddr[IP_LEN] = {0};
    char ip[IP_LEN] = "0.0.0.0";
    EXPECT_TRUE(SetIpaddr(ip) == SOFTBUS_OK);
    sleep(2);
    EXPECT_TRUE(LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr, IP_LEN) == SOFTBUS_OK);
    EXPECT_TRUE(strncmp(ipAddr, "127.0.0.1", IP_LEN) == 0);
}

/*
* @tc.name: NET_BusCenter_IP_Change_Monitor_Test_003
* @tc.desc: IP change monitor test
* @tc.type: FUNC
* @tc.require: AR000FK6J3
*/
HWTEST_F(NetBusCenterTest, NET_BusCenter_IP_Change_Monitor_Test_003, TestSize.Level0)
{
    char ipAddr[IP_LEN] = {0};
    EXPECT_TRUE(SetIpDown() == SOFTBUS_OK);
    sleep(2);
    EXPECT_TRUE(LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr, IP_LEN) == SOFTBUS_OK);
    EXPECT_TRUE(strncmp(ipAddr, "127.0.0.1", IP_LEN) == 0);
}
} // namespace OHOS

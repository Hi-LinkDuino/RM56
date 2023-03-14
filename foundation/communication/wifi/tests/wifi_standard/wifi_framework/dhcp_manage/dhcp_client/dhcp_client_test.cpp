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

#include "wifi_log.h"
#include "dhcp_client.h"
#include "dhcp_function.h"
#include "securec.h"

using namespace testing::ext;

namespace OHOS {
class DhcpClientFuncTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {}
    virtual void TearDown()
    {}
};

HWTEST_F(DhcpClientFuncTest, StartProcess_SUCCESS, TestSize.Level1)
{
    struct DhcpClientCfg *pCfg = GetDhcpClientCfg();
    ASSERT_TRUE(strncpy_s(pCfg->workDir, DIR_MAX_LEN, "./", DIR_MAX_LEN - 1) == EOK);
    ASSERT_TRUE(strncpy_s(pCfg->ifaceName, INFNAME_SIZE, "wlan0", INFNAME_SIZE - 1) == EOK);
    ASSERT_TRUE(snprintf_s(pCfg->confFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s", pCfg->workDir, DHCPC_CONF) >= 0);
    ASSERT_TRUE(
        snprintf_s(pCfg->pidFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s.pid", pCfg->workDir, pCfg->ifaceName) >= 0);
    ASSERT_TRUE(
        snprintf_s(pCfg->resultFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s.result", pCfg->workDir, pCfg->ifaceName) >= 0);
    ASSERT_TRUE(
        snprintf_s(pCfg->result6File, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s.result6", pCfg->workDir, pCfg->ifaceName) >= 0);
    ASSERT_TRUE(
        snprintf_s(pCfg->leaseFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s.lease", pCfg->workDir, pCfg->ifaceName) >= 0);
    pCfg->getMode = DHCP_IP_TYPE_ALL;

    EXPECT_EQ(DHCP_OPT_SUCCESS, GetLocalInterface(pCfg->ifaceName, &pCfg->ifaceIndex, pCfg->ifaceMac, NULL));

    /* Generate format: 1 + ifaceMac. */
    if (pCfg->pOptClientId != NULL) {
        free(pCfg->pOptClientId);
        pCfg->pOptClientId = NULL;
    }
    pCfg->pOptClientId = (unsigned char *)malloc(DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + MAC_ADDR_LEN + 1);
    ASSERT_TRUE(pCfg->pOptClientId != NULL);
    pCfg->pOptClientId[DHCP_OPT_CODE_INDEX] = CLIENT_IDENTIFIER_OPTION;
    pCfg->pOptClientId[DHCP_OPT_LEN_INDEX] = MAC_ADDR_LEN + 1;
    pCfg->pOptClientId[DHCP_OPT_DATA_INDEX] = NUMBER_ONE;
    ASSERT_TRUE(
        memcpy_s(pCfg->pOptClientId + DHCP_OPT_DATA_INDEX + 1, MAC_ADDR_LEN, pCfg->ifaceMac, MAC_ADDR_LEN) == EOK);
    pCfg->timeoutExit = true;

    EXPECT_EQ(DHCP_OPT_SUCCESS, StartProcess());
}

HWTEST_F(DhcpClientFuncTest, GetProStatus_SUCCESS, TestSize.Level1)
{
    char workDir[DIR_MAX_LEN] = "./";
    char pidFile[DIR_MAX_LEN] = "./wlan0.pid";
    ASSERT_EQ(DHCP_OPT_SUCCESS, InitPidfile(workDir, pidFile, getpid()));

    EXPECT_EQ(1, GetProStatus(pidFile));
    unlink(pidFile);
}

HWTEST_F(DhcpClientFuncTest, GetProStatus_FAILED, TestSize.Level1)
{
    char pidFile[DIR_MAX_LEN] = "./wlan0.pid";
    EXPECT_EQ(0, GetProStatus(pidFile));
}

HWTEST_F(DhcpClientFuncTest, StopProcess_SUCCESS, TestSize.Level1)
{
    char pidFile[DIR_MAX_LEN] = "./wlan0.pid";
    EXPECT_EQ(DHCP_OPT_SUCCESS, StopProcess(pidFile));

    char workDir[DIR_MAX_LEN] = "./";
    pid_t testPid = 12345;
    ASSERT_EQ(DHCP_OPT_SUCCESS, InitPidfile(workDir, pidFile, testPid));

    EXPECT_EQ(DHCP_OPT_SUCCESS, StopProcess(pidFile));
    unlink(pidFile);
}
}  // namespace OHOS
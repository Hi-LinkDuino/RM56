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
#include "wifi_service_manager_test.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiServiceManagerTest, CheckPreLoadServiceTest, TestSize.Level1)
{
    EXPECT_EQ(0, WifiServiceManager::GetInstance().CheckPreLoadService());
}

HWTEST_F(WifiServiceManagerTest, CheckAndEnforceService_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiServiceManager::GetInstance().CheckAndEnforceService("StaService"));
    EXPECT_EQ(0, WifiServiceManager::GetInstance().CheckAndEnforceService("ApService"));
    EXPECT_EQ(0, WifiServiceManager::GetInstance().CheckAndEnforceService("ScanService"));
    EXPECT_EQ(0, WifiServiceManager::GetInstance().CheckAndEnforceService("P2pService"));
}

HWTEST_F(WifiServiceManagerTest, CheckAndEnforceService_FAILED, TestSize.Level1)
{
    EXPECT_EQ(-1, WifiServiceManager::GetInstance().CheckAndEnforceService(""));
    EXPECT_EQ(-1, WifiServiceManager::GetInstance().CheckAndEnforceService("staservice"));
    EXPECT_EQ(-1, WifiServiceManager::GetInstance().CheckAndEnforceService("any"));
}

HWTEST_F(WifiServiceManagerTest, GetStaServiceInstTest, TestSize.Level1)
{
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() == nullptr);
    WifiServiceManager::GetInstance().CheckAndEnforceService("StaService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() != nullptr);
    WifiServiceManager::GetInstance().UnloadService("StaService");
    WifiServiceManager::GetInstance().CheckAndEnforceService("StaService", false);
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() != nullptr);
}

HWTEST_F(WifiServiceManagerTest, GetScanServiceInstTest, TestSize.Level1)
{
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetScanServiceInst() == nullptr);
    WifiServiceManager::GetInstance().CheckAndEnforceService("ScanService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetScanServiceInst() != nullptr);
    WifiServiceManager::GetInstance().UnloadService("ScanService");
    WifiServiceManager::GetInstance().CheckAndEnforceService("ScanService", false);
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetScanServiceInst() != nullptr);
}

HWTEST_F(WifiServiceManagerTest, GetApServiceInstTest, TestSize.Level1)
{
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetApServiceInst() == nullptr);
    WifiServiceManager::GetInstance().CheckAndEnforceService("ApService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetApServiceInst() != nullptr);
    WifiServiceManager::GetInstance().UnloadService("ApService");
    WifiServiceManager::GetInstance().CheckAndEnforceService("ApService", false);
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetApServiceInst() != nullptr);
}

HWTEST_F(WifiServiceManagerTest, GetP2pServiceInstTest, TestSize.Level1)
{
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetP2pServiceInst() == nullptr);
    WifiServiceManager::GetInstance().CheckAndEnforceService("P2pService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetP2pServiceInst() != nullptr);
    WifiServiceManager::GetInstance().UnloadService("P2pService");
    WifiServiceManager::GetInstance().CheckAndEnforceService("P2pService", false);
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetP2pServiceInst() != nullptr);
}

HWTEST_F(WifiServiceManagerTest, UnloadService, TestSize.Level1)
{
    WifiServiceManager::GetInstance().CheckAndEnforceService("StaService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() != nullptr);
    WifiServiceManager::GetInstance().UnloadService("StaService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() == nullptr);
}

HWTEST_F(WifiServiceManagerTest, UninstallAllService, TestSize.Level1)
{
    WifiServiceManager::GetInstance().CheckAndEnforceService("StaService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() != nullptr);
    WifiServiceManager::GetInstance().CheckAndEnforceService("ApService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetApServiceInst() != nullptr);
    WifiServiceManager::GetInstance().CheckAndEnforceService("P2pService");
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetP2pServiceInst() != nullptr);
    WifiServiceManager::GetInstance().UninstallAllService();
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetStaServiceInst() == nullptr);
    EXPECT_TRUE(WifiServiceManager::GetInstance().GetApServiceInst() == nullptr);
}
}  // namespace Wifi
}  // namespace OHOS
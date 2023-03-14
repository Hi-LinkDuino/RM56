/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wifi_ap_nat_manager.h"
#include "mock_network_interface.h"
#include "mock_system_interface.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstring>

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiApNatManager_test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}

public:
    bool WrapSetForwarding(bool enable)
    {
        return mApNatManager.SetForwarding(enable);
    }
    bool WrapSetInterfaceRoute(bool enable)
    {
        return mApNatManager.SetInterfaceRoute(enable);
    }
    bool WrapSetInterfaceNat(bool enable, const std::string &outInterfaceName)
    {
        return mApNatManager.SetInterfaceNat(enable, outInterfaceName);
    }
    bool WrapWriteDataToFile(const std::string &fileName, const std::string &content)
    {
        return mApNatManager.WriteDataToFile(fileName, content);
    }

public:
    std::string ifc1 = "wlan1";
    std::string ifc2 = "wlan2";
    std::string badifc = "wlan!!";
    WifiApNatManager mApNatManager;
};

HWTEST_F(WifiApNatManager_test, EnableInterfaceNat_SUCCESS, TestSize.Level1)
{
    int fd = 10;
    MockSystemInterface::SetMockFlag(true);
    EXPECT_CALL(MockSystemInterface::GetInstance(), system(_)).WillOnce(Return(-1)).WillRepeatedly(Return(fd));
    EXPECT_CALL(MockNetworkInterface::GetInstance(), IsValidInterfaceName(_)).WillRepeatedly(Return(true));
    EXPECT_TRUE(mApNatManager.EnableInterfaceNat(true, ifc1, ifc2));
    EXPECT_FALSE(mApNatManager.EnableInterfaceNat(true, ifc1, ifc1));

    EXPECT_CALL(MockNetworkInterface::GetInstance(), IsValidInterfaceName(_))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_FALSE(mApNatManager.EnableInterfaceNat(true, badifc, ifc1));
    EXPECT_FALSE(mApNatManager.EnableInterfaceNat(true, std::string(""), ifc1));
}

HWTEST_F(WifiApNatManager_test, DisableInterfaceNat_SUCCESS, TestSize.Level1)
{
    int fd = 10;
    MockSystemInterface::SetMockFlag(true);
    EXPECT_CALL(MockSystemInterface::GetInstance(), system(_)).WillOnce(Return(-1)).WillRepeatedly(Return(fd));
    EXPECT_CALL(MockNetworkInterface::GetInstance(), IsValidInterfaceName(_)).WillRepeatedly(Return(true));
    EXPECT_TRUE(mApNatManager.EnableInterfaceNat(false, ifc1, ifc2));
    EXPECT_FALSE(mApNatManager.EnableInterfaceNat(false, ifc1, ifc1));

    EXPECT_CALL(MockNetworkInterface::GetInstance(), IsValidInterfaceName(_))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_FALSE(mApNatManager.EnableInterfaceNat(false, badifc, ifc1));
    EXPECT_FALSE(mApNatManager.EnableInterfaceNat(false, std::string(""), ifc1));
    MockSystemInterface::SetMockFlag(false);
}

HWTEST_F(WifiApNatManager_test, SetForwarding_SUCCESS, TestSize.Level1)
{
    EXPECT_TRUE(WrapSetForwarding(true));
    EXPECT_TRUE(WrapSetForwarding(false));
}

HWTEST_F(WifiApNatManager_test, SetInterfaceRoute_SUCCESS, TestSize.Level1)
{
    int fd = 10;
    MockSystemInterface::SetMockFlag(true);
    EXPECT_CALL(MockSystemInterface::GetInstance(), system(_)).WillOnce(Return(-1)).WillRepeatedly(Return(fd));
    EXPECT_TRUE(WrapSetInterfaceRoute(true));
    EXPECT_TRUE(WrapSetInterfaceRoute(false));
    MockSystemInterface::SetMockFlag(false);
}

HWTEST_F(WifiApNatManager_test, SetInterfaceNat_SUCCESS, TestSize.Level1)
{
    int fd = 10;
    MockSystemInterface::SetMockFlag(true);
    EXPECT_CALL(MockSystemInterface::GetInstance(), system(_)).WillOnce(Return(-1)).WillRepeatedly(Return(fd));
    EXPECT_TRUE(WrapSetInterfaceNat(true, ifc2));
    EXPECT_TRUE(WrapSetInterfaceNat(false, ifc2));
    MockSystemInterface::SetMockFlag(false);
}

HWTEST_F(WifiApNatManager_test, WriteDataToFile_SUCCESS, TestSize.Level1)
{
    std::string filename = "./test.txt";
    std::string context = "1234567890";

    MockSystemInterface::SetMockFlag(true);

    EXPECT_TRUE(WrapWriteDataToFile(filename, context));
    EXPECT_TRUE(WrapWriteDataToFile(filename, context));
    EXPECT_TRUE(WrapWriteDataToFile(filename, context));

    MockSystemInterface::SetMockFlag(false);
}
} // namespace Wifi
} // namespace OHOS
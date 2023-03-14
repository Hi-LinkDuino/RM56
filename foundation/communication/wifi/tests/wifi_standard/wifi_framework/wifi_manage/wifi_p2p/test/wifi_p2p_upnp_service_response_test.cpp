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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "wifi_p2p_upnp_service_response.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pUpnpServiceResponseTest : public testing::Test {
public:
    virtual void SetUp()
    {}
    virtual void TearDown()
    {}

    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}

public:
    P2pServiceStatus status = P2pServiceStatus::PSRS_SUCCESS;
    int tranId;
    std::vector<unsigned char> data;
};

HWTEST_F(WifiP2pUpnpServiceResponseTest, GetUniqueServNames, TestSize.Level1)
{
    std::vector<std::string> strV;
    EXPECT_EQ(strV, WifiP2pUpnpServiceResponse::Create(status, tranId, data).GetUniqueServNames());
}

HWTEST_F(WifiP2pUpnpServiceResponseTest, GetVersion, TestSize.Level1)
{
    status = P2pServiceStatus::PSRS_SERVICE_PROTOCOL_NOT_AVAILABLE;
    EXPECT_EQ(-1, WifiP2pUpnpServiceResponse::Create(status, tranId, data).GetVersion());
}

HWTEST_F(WifiP2pUpnpServiceResponseTest, ParseData1, TestSize.Level1)
{
    EXPECT_EQ(false, WifiP2pUpnpServiceResponse::Create(status, tranId, data).ParseData());
}

HWTEST_F(WifiP2pUpnpServiceResponseTest, ParseData2, TestSize.Level1)
{
    data.push_back(1);
    data.push_back(0x31);
    data.push_back(0x32);
    data.push_back(0x2c);
    data.push_back(0x33);
    data.push_back(0x34);
    data.push_back(0x3b);
    data.push_back(0x04);
    data.push_back(0x73);
    data.push_back(0x76);
    data.push_back(0x72);
    data.push_back(0x4e);
    EXPECT_EQ(true, WifiP2pUpnpServiceResponse::Create(status, tranId, data).ParseData());
}
}  // namespace Wifi
}  // namespace OHOS
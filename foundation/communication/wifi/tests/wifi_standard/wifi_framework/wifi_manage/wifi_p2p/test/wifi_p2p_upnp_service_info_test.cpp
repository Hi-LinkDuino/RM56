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

#include "wifi_p2p_upnp_service_info.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pUpnpServiceInfoTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {}
    virtual void TearDown()
    {}

public:
    void WarpCreate()
    {
        std::string uuid;
        std::string device;
        std::vector<std::string> services;
        services.push_back(std::string("TestUpnpService"));
        std::string svrName("TestSvrName");
        WifiP2pUpnpServiceInfo::Create(uuid, device, services, svrName);
    }
    void WarpCreateSupQuery()
    {
        std::string uuid;
        std::string data;
        std::string device;
    	std::string svrName;
        std::vector<std::string> services;
    	WifiP2pUpnpServiceInfo::Create(uuid, device, services, svrName).BuildWpaQuery(uuid, data, svrName);
    }
};

HWTEST_F(WifiP2pUpnpServiceInfoTest, Create, TestSize.Level1)
{
    WarpCreate();
}

HWTEST_F(WifiP2pUpnpServiceInfoTest, BuildWpaQuery, TestSize.Level1)
{
    WarpCreateSupQuery();
}
}  // namespace Wifi
}  // namespace OHOS
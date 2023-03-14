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

#include "wifi_p2p_dns_sd_service_request.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pDnsSdServiceRequestTest : public testing::Test {
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
    void WarpCreate1()
    {
        WifiP2pDnsSdServiceRequest::Create();
    }
    void WarpCreate2()
    {
        WifiP2pDnsSdServiceRequest::Create("serviceType");
    }
    void WarpCreate3()
    {
        WifiP2pDnsSdServiceRequest::Create("instanceName", "serviceType");
    }
};

HWTEST_F(WifiP2pDnsSdServiceRequestTest, Create1, TestSize.Level1)
{
    WarpCreate1();
}

HWTEST_F(WifiP2pDnsSdServiceRequestTest, Create2, TestSize.Level1)
{
    WarpCreate2();
}

HWTEST_F(WifiP2pDnsSdServiceRequestTest, Create3, TestSize.Level1)
{
    WarpCreate3();
}
}  // namespace Wifi
}  // namespace OHOS
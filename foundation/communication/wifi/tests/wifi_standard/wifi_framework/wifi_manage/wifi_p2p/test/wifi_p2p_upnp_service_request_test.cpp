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

#include "wifi_p2p_upnp_service_request.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pUpnpServiceRequestTest : public testing::Test {
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
        WifiP2pUpnpServiceRequest::Create();
        WifiP2pUpnpServiceRequest::Create("searchTarget");
        WifiP2pUpnpServiceRequest::Create(std::string(""));
    }
};

HWTEST_F(WifiP2pUpnpServiceRequestTest, Create, TestSize.Level1)
{
    WarpCreate();
}
}  // namespace Wifi
}  // namespace OHOS
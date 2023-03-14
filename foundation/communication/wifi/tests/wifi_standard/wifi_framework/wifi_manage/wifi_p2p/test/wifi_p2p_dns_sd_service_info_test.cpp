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

#include "wifi_p2p_dns_sd_service_info.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pDnsSdServiceInfoTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pWifiP2pDnsSdServiceInfo.reset(new WifiP2pDnsSdServiceInfo());
    }
    virtual void TearDown()
    {
        pWifiP2pDnsSdServiceInfo.reset();
    }

public:
    std::unique_ptr<WifiP2pDnsSdServiceInfo> pWifiP2pDnsSdServiceInfo;
    void WarpTurnDnsNameToStream()
    {
        std::string dnsName = "a.txt";
        EXPECT_EQ("c00c", pWifiP2pDnsSdServiceInfo->TurnDnsNameToStream("_tcp.local."));
        EXPECT_EQ("c011", pWifiP2pDnsSdServiceInfo->TurnDnsNameToStream("local."));
        EXPECT_EQ("c01c", pWifiP2pDnsSdServiceInfo->TurnDnsNameToStream("_udp.local."));
        EXPECT_EQ("00", pWifiP2pDnsSdServiceInfo->TurnDnsNameToStream(""));
        EXPECT_EQ("01610374787400", pWifiP2pDnsSdServiceInfo->TurnDnsNameToStream(dnsName));
    }
};

HWTEST_F(WifiP2pDnsSdServiceInfoTest, Create, TestSize.Level1)
{
    std::string instanceName;
    std::string serviceType;
    std::map<std::string, std::string> txtMap;
    txtMap.insert(std::make_pair(std::string("ip"), std::string("TestIp")));
    std::string svrName("TestSvrName");
    WifiP2pDnsSdServiceInfo::Create(instanceName, serviceType, txtMap, svrName);
}

HWTEST_F(WifiP2pDnsSdServiceInfoTest, BuildRequest, TestSize.Level1)
{
    std::string dnsName;
    int dnsType = 0;
    int version = 0;
    std::string instanceName;
    std::string serviceType;
    std::string svrName;
    std::map<std::string, std::string> txtMap;
    WifiP2pDnsSdServiceInfo::Create(instanceName, serviceType, txtMap, svrName).BuildRequest(dnsName, dnsType, version);
}

HWTEST_F(WifiP2pDnsSdServiceInfoTest, TurnDnsNameToStream, TestSize.Level1)
{
    WarpTurnDnsNameToStream();
}
}  // namespace Wifi
}  // namespace OHOS
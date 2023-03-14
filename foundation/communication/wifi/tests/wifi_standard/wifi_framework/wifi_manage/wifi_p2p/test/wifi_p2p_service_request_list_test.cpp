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

#include "wifi_p2p_service_request_list.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pServiceRequestListTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        pWifiP2pServiceRequestList.reset(new WifiP2pServiceRequestList());
    }
    virtual void TearDown()
    {
        pWifiP2pServiceRequestList.reset();
    }

public:
    std::unique_ptr<WifiP2pServiceRequestList> pWifiP2pServiceRequestList;
};

HWTEST_F(WifiP2pServiceRequestListTest, AddServiceRequest, TestSize.Level1)
{
    WifiP2pServiceRequest req;
    EXPECT_TRUE(pWifiP2pServiceRequestList->AddServiceRequest(req));
    EXPECT_FALSE(pWifiP2pServiceRequestList->AddServiceRequest(req));
}

HWTEST_F(WifiP2pServiceRequestListTest, RemoveServiceRequest, TestSize.Level1)
{
    WifiP2pServiceRequest req;
    EXPECT_FALSE(pWifiP2pServiceRequestList->RemoveServiceRequest(req));
    EXPECT_TRUE(pWifiP2pServiceRequestList->AddServiceRequest(req));
    EXPECT_TRUE(pWifiP2pServiceRequestList->RemoveServiceRequest(req));
}

HWTEST_F(WifiP2pServiceRequestListTest, ClearServiceRequest, TestSize.Level1)
{
    EXPECT_EQ(true, pWifiP2pServiceRequestList->ClearServiceRequest());
}

HWTEST_F(WifiP2pServiceRequestListTest, GetServiceRequestList, TestSize.Level1)
{
    std::vector<WifiP2pServiceRequest> resps;
    WifiP2pServiceRequest req;
    resps.push_back(req);
    EXPECT_TRUE(pWifiP2pServiceRequestList->AddServiceRequest(req));
    EXPECT_EQ(resps, pWifiP2pServiceRequestList->GetServiceRequestList());
}

HWTEST_F(WifiP2pServiceRequestListTest, SetUpdateIndic, TestSize.Level1)
{
    unsigned short updateIndic = 1;
    pWifiP2pServiceRequestList->SetUpdateIndic(updateIndic);
    EXPECT_EQ(pWifiP2pServiceRequestList->GetUpdateIndic(), updateIndic);
}

HWTEST_F(WifiP2pServiceRequestListTest, SetFrequency, TestSize.Level1)
{
    int frequency = 2020;
    pWifiP2pServiceRequestList->SetFrequency(frequency);
    EXPECT_EQ(pWifiP2pServiceRequestList->GetFrequency(), frequency);
}

HWTEST_F(WifiP2pServiceRequestListTest, SetDialogToken, TestSize.Level1)
{
    int dialogToken = 2020;
    pWifiP2pServiceRequestList->SetDialogToken(dialogToken);
    EXPECT_EQ(pWifiP2pServiceRequestList->GetDialogToken(), dialogToken);
}

HWTEST_F(WifiP2pServiceRequestListTest, SetDevice, TestSize.Level1)
{
    WifiP2pDevice device;
    pWifiP2pServiceRequestList->SetDevice(device);
    EXPECT_EQ(pWifiP2pServiceRequestList->GetDevice(), device);
}

HWTEST_F(WifiP2pServiceRequestListTest, GetTlvs, TestSize.Level1)
{
    WifiP2pServiceRequest resp;
    EXPECT_TRUE(pWifiP2pServiceRequestList->AddServiceRequest(resp));
    pWifiP2pServiceRequestList->GetTlvs();
}

HWTEST_F(WifiP2pServiceRequestListTest, ParseTlvs2ReqList1, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x02);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    tlvList.push_back(0x01);
    EXPECT_TRUE(pWifiP2pServiceRequestList->ParseTlvs2ReqList(tlvList));
}

HWTEST_F(WifiP2pServiceRequestListTest, ParseTlvs2ReqList2, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x01);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    tlvList.push_back(0x01);
    EXPECT_FALSE(pWifiP2pServiceRequestList->ParseTlvs2ReqList(tlvList));
}

HWTEST_F(WifiP2pServiceRequestListTest, ParseTlvs2ReqList3, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x06);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    tlvList.push_back(0x01);
    tlvList.push_back(0x31);
    tlvList.push_back(0x32);
    tlvList.push_back(0x33);
    tlvList.push_back(0x34);
    EXPECT_TRUE(pWifiP2pServiceRequestList->ParseTlvs2ReqList(tlvList));
}
}  // namespace Wifi
}  // namespace OHOS
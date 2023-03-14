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

#include "wifi_p2p_service_response_list.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pServiceResponseListTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        pWifiP2pServiceResponseList.reset(new WifiP2pServiceResponseList());
    }
    virtual void TearDown()
    {
        pWifiP2pServiceResponseList.reset();
    }

public:
    std::unique_ptr<WifiP2pServiceResponseList> pWifiP2pServiceResponseList;
};

HWTEST_F(WifiP2pServiceResponseListTest, constructions, TestSize.Level1)
{
    WifiP2pDevice device;
    std::vector<WifiP2pServiceResponse> srvRespList;
    WifiP2pServiceResponseList wifiP2pServiceResponseList = WifiP2pServiceResponseList(device, srvRespList);
}

HWTEST_F(WifiP2pServiceResponseListTest, AddServiceResponse, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
}

HWTEST_F(WifiP2pServiceResponseListTest, RemoveServiceResponse, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    EXPECT_FALSE(pWifiP2pServiceResponseList->RemoveServiceResponse(resp));
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    EXPECT_TRUE(pWifiP2pServiceResponseList->RemoveServiceResponse(resp));
}

HWTEST_F(WifiP2pServiceResponseListTest, GetServiceResponseList, TestSize.Level1)
{
    std::vector<WifiP2pServiceResponse> resps;
    WifiP2pServiceResponse resp;
    resps.push_back(resp);
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    EXPECT_EQ(resps, pWifiP2pServiceResponseList->GetServiceResponseList());
}

HWTEST_F(WifiP2pServiceResponseListTest, FilterSerivceResponse, TestSize.Level1)
{
    pWifiP2pServiceResponseList->FilterSerivceResponse(P2pServiceStatus::PSRS_BAD_REQUEST);
    WifiP2pServiceResponse resp;
    resp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    pWifiP2pServiceResponseList->FilterSerivceResponse(P2pServiceStatus::PSRS_SUCCESS);
}

HWTEST_F(WifiP2pServiceResponseListTest, ReverseFilterSerivceResponse, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    resp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    pWifiP2pServiceResponseList->ReverseFilterSerivceResponse(P2pServiceStatus::PSRS_BAD_REQUEST);
}

HWTEST_F(WifiP2pServiceResponseListTest, MergerAndDeduplicate1, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList responseList;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    responseList.SetDevice(device);
    WifiP2pServiceResponse resp;
    pWifiP2pServiceResponseList->MergerAndDeduplicate(responseList);
}

HWTEST_F(WifiP2pServiceResponseListTest, MergerAndDeduplicate2, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList responseList;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    responseList.SetDevice(device);
    WifiP2pServiceResponse resp;
    pWifiP2pServiceResponseList->SetDevice(device);
    pWifiP2pServiceResponseList->MergerAndDeduplicate(responseList);
}

HWTEST_F(WifiP2pServiceResponseListTest, MergerAndDeduplicate3, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList responseList;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    responseList.SetDevice(device);
    WifiP2pServiceResponse resp;
    pWifiP2pServiceResponseList->SetDevice(device);
    responseList.AddServiceResponse(resp);
    pWifiP2pServiceResponseList->MergerAndDeduplicate(responseList);
}

HWTEST_F(WifiP2pServiceResponseListTest, MergerAndDeduplicate4, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList responseList;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    responseList.SetDevice(device);
    WifiP2pServiceResponse resp;
    pWifiP2pServiceResponseList->SetDevice(device);
    responseList.AddServiceResponse(resp);
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    pWifiP2pServiceResponseList->MergerAndDeduplicate(responseList);
}

HWTEST_F(WifiP2pServiceResponseListTest, GetTlvs, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    EXPECT_TRUE(pWifiP2pServiceResponseList->AddServiceResponse(resp));
    pWifiP2pServiceResponseList->GetTlvs();
}

HWTEST_F(WifiP2pServiceResponseListTest, SetUpdateIndic, TestSize.Level1)
{
    unsigned short updateIndic = 1;
    pWifiP2pServiceResponseList->SetUpdateIndic(updateIndic);
    EXPECT_EQ(pWifiP2pServiceResponseList->GetUpdateIndic(), updateIndic);
}

HWTEST_F(WifiP2pServiceResponseListTest, SetFrequency, TestSize.Level1)
{
    int frequency = 2020;
    pWifiP2pServiceResponseList->SetFrequency(frequency);
    EXPECT_EQ(pWifiP2pServiceResponseList->GetFrequency(), frequency);
}

HWTEST_F(WifiP2pServiceResponseListTest, SetDialogToken, TestSize.Level1)
{
    int dialogToken = 2020;
    pWifiP2pServiceResponseList->SetDialogToken(dialogToken);
    EXPECT_EQ(pWifiP2pServiceResponseList->GetDialogToken(), dialogToken);
}

HWTEST_F(WifiP2pServiceResponseListTest, SetDevice, TestSize.Level1)
{
    WifiP2pDevice device;
    pWifiP2pServiceResponseList->SetDevice(device);
    EXPECT_EQ(pWifiP2pServiceResponseList->GetDevice(), device);
}

HWTEST_F(WifiP2pServiceResponseListTest, ParseTlvs2RespList1, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x02);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    tlvList.push_back(0x01);
    tlvList.push_back(0x01);
    pWifiP2pServiceResponseList->ParseTlvs2RespList(tlvList);
}

HWTEST_F(WifiP2pServiceResponseListTest, ParseTlvs2RespList2, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    EXPECT_FALSE(pWifiP2pServiceResponseList->ParseTlvs2RespList(tlvList));
}

HWTEST_F(WifiP2pServiceResponseListTest, ParseTlvs2RespList3, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x03);
    tlvList.push_back(0x00);
    tlvList.push_back(0x01);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    EXPECT_TRUE(pWifiP2pServiceResponseList->ParseTlvs2RespList(tlvList));
}

HWTEST_F(WifiP2pServiceResponseListTest, ParseTlvs2RespList4, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x03);
    tlvList.push_back(0x10);
    tlvList.push_back(0x01);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    EXPECT_FALSE(pWifiP2pServiceResponseList->ParseTlvs2RespList(tlvList));
}

HWTEST_F(WifiP2pServiceResponseListTest, ParseTlvs2RespList5, TestSize.Level1)
{
    std::vector<unsigned char> tlvList;
    tlvList.push_back(0x07);
    tlvList.push_back(0x00);
    tlvList.push_back(0x01);
    tlvList.push_back(0x00);
    tlvList.push_back(0x00);
    tlvList.push_back(0x31);
    tlvList.push_back(0x32);
    tlvList.push_back(0x33);
    tlvList.push_back(0x34);
    EXPECT_TRUE(pWifiP2pServiceResponseList->ParseTlvs2RespList(tlvList));
}
}  // namespace Wifi
}  // namespace OHOS
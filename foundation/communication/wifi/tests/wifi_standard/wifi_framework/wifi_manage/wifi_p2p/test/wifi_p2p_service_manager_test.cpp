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

#include <cstdio>
#include "wifi_msg.h"
#include "wifi_p2p_service_manager.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pServiceManagerTest : public testing::Test {
public:
    WifiP2pServiceManagerTest()
    {}

    virtual void SetUp()
    {
        pWifiP2pServiceManager.reset(new WifiP2pServiceManager());
    }
    virtual void TearDown()
    {
        pWifiP2pServiceManager.reset();
    }
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    std::unique_ptr<WifiP2pServiceManager> pWifiP2pServiceManager;
};

HWTEST_F(WifiP2pServiceManagerTest, Init, TestSize.Level1)
{
    pWifiP2pServiceManager->Init();
}

HWTEST_F(WifiP2pServiceManagerTest, ClearAll, TestSize.Level1)
{
    pWifiP2pServiceManager->ClearAll();
}

HWTEST_F(WifiP2pServiceManagerTest, SetQueryId, TestSize.Level1)
{
    pWifiP2pServiceManager->SetQueryId("queryId");
    EXPECT_EQ(pWifiP2pServiceManager->GetQueryId(), "queryId");
}

HWTEST_F(WifiP2pServiceManagerTest, GetTransId, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pServiceManager->GetTransId(), 1);
}

HWTEST_F(WifiP2pServiceManagerTest, AddLocalService, TestSize.Level1)
{
    WifiP2pServiceInfo p2pSvrInfo;
    EXPECT_TRUE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));
    EXPECT_FALSE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));
}

HWTEST_F(WifiP2pServiceManagerTest, RemoveLocalService, TestSize.Level1)
{
    WifiP2pServiceInfo p2pSvrInfo;
    EXPECT_FALSE(pWifiP2pServiceManager->RemoveLocalService(p2pSvrInfo));
    EXPECT_TRUE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));
    EXPECT_TRUE(pWifiP2pServiceManager->RemoveLocalService(p2pSvrInfo));
}

HWTEST_F(WifiP2pServiceManagerTest, GetLocalServiceList, TestSize.Level1)
{
    WifiP2pServiceInfo p2pSvrInfo;
    std::vector<WifiP2pServiceInfo> localServicesInfo;
    localServicesInfo.push_back(p2pSvrInfo);
    EXPECT_TRUE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));
    EXPECT_EQ(pWifiP2pServiceManager->GetLocalServiceList(), localServicesInfo);
}

HWTEST_F(WifiP2pServiceManagerTest, AddDeviceResponses, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList p2pSvrReq;
    EXPECT_TRUE(pWifiP2pServiceManager->AddDeviceResponses(p2pSvrReq));
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    p2pSvrReq.SetDevice(device);
    WifiP2pServiceResponse resp;
    p2pSvrReq.AddServiceResponse(resp);
    EXPECT_TRUE(pWifiP2pServiceManager->AddDeviceResponses(p2pSvrReq));
}

HWTEST_F(WifiP2pServiceManagerTest, AddDeviceService, TestSize.Level1)
{
    WifiP2pServiceResponse rsp;
    WifiP2pDevice dev;
    rsp.SetServiceStatus(P2pServiceStatus::PSRS_SERVICE_PROTOCOL_NOT_AVAILABLE);
    EXPECT_FALSE(pWifiP2pServiceManager->AddDeviceService(rsp, dev));
    dev.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    rsp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    EXPECT_TRUE(pWifiP2pServiceManager->AddDeviceService(rsp, dev));
}

HWTEST_F(WifiP2pServiceManagerTest, DelServicesFormAddress, TestSize.Level1)
{
    EXPECT_FALSE(pWifiP2pServiceManager->DelServicesFormAddress("AA:BB:CC:DD:EE:FF"));
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    WifiP2pServiceResponse rsp;
    rsp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    EXPECT_TRUE(pWifiP2pServiceManager->AddDeviceService(rsp, device));
    EXPECT_TRUE(pWifiP2pServiceManager->DelServicesFormAddress("AA:BB:CC:DD:EE:FF"));
}

HWTEST_F(WifiP2pServiceManagerTest, GetDeviceServices, TestSize.Level1)
{
    std::vector<WifiP2pServiceInfo> services, services1;
    WifiP2pServiceInfo info;
    services1.push_back(info);

    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    WifiP2pServiceResponse rsp;
    rsp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    EXPECT_TRUE(pWifiP2pServiceManager->AddDeviceService(rsp, device));
    pWifiP2pServiceManager->GetDeviceServices(services);
}

HWTEST_F(WifiP2pServiceManagerTest, GetServiceResponseList, TestSize.Level1)
{
    std::vector<WifiP2pServiceResponseList> respLists;
    WifiP2pServiceResponseList respList;
    respLists.push_back(respList);
    pWifiP2pServiceManager->GetServiceResponseList(respLists);
}

HWTEST_F(WifiP2pServiceManagerTest, AddServiceResponse_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList p2pSvrReq, p2pSvrReq1;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    p2pSvrReq.SetDevice(device);
    p2pSvrReq1.SetDevice(device);
    WifiP2pServiceResponse resp, resq1;
    resq1.SetServiceStatus(P2pServiceStatus::PSRS_REQUESTED_INFORMATION_NOT_AVAILABLE);
    p2pSvrReq.AddServiceResponse(resp);
    p2pSvrReq1.AddServiceResponse(resq1);
    EXPECT_TRUE(pWifiP2pServiceManager->AddServiceResponse(p2pSvrReq));
    EXPECT_FALSE(pWifiP2pServiceManager->AddServiceResponse(p2pSvrReq));
    EXPECT_TRUE(pWifiP2pServiceManager->AddServiceResponse(p2pSvrReq1));
}

HWTEST_F(WifiP2pServiceManagerTest, RemoveServiceResponse_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceResponseList p2pSvrReq, p2pSvrReq1;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    p2pSvrReq.SetDevice(device);
    p2pSvrReq1.SetDevice(device);
    WifiP2pServiceResponse resp, resq1;
    resq1.SetServiceStatus(P2pServiceStatus::PSRS_REQUESTED_INFORMATION_NOT_AVAILABLE);
    p2pSvrReq.AddServiceResponse(resp);
    p2pSvrReq1.AddServiceResponse(resq1);

    EXPECT_FALSE(pWifiP2pServiceManager->RemoveServiceResponse(p2pSvrReq));
    EXPECT_TRUE(pWifiP2pServiceManager->AddServiceResponse(p2pSvrReq));
    EXPECT_TRUE(pWifiP2pServiceManager->RemoveServiceResponse(p2pSvrReq));
    EXPECT_TRUE(pWifiP2pServiceManager->RemoveServiceResponse(p2pSvrReq1));
    EXPECT_TRUE(pWifiP2pServiceManager->AddServiceResponse(p2pSvrReq));
    EXPECT_TRUE(pWifiP2pServiceManager->RemoveServiceResponse("AA:BB:CC:DD:EE:FF"));
    EXPECT_FALSE(pWifiP2pServiceManager->RemoveServiceResponse("AA:BB:CC:DD:EE:FF"));
}

HWTEST_F(WifiP2pServiceManagerTest, ProcessServiceRequestList, TestSize.Level1)
{
    WifiP2pServiceRequestList respList;
    WifiP2pServiceRequest req;
    respList.AddServiceRequest(req);
    WifiP2pServiceInfo p2pSvrInfo;
    pWifiP2pServiceManager->ProcessServiceRequestList(respList);
    EXPECT_TRUE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));
    pWifiP2pServiceManager->ProcessServiceRequestList(respList);
}

HWTEST_F(WifiP2pServiceManagerTest, ProcessServiceResponseList, TestSize.Level1)
{
    WifiP2pServiceResponseList respList;
    WifiP2pServiceResponse resp;
    respList.AddServiceResponse(resp);
    WifiP2pServiceInfo p2pSvrInfo;
    pWifiP2pServiceManager->ProcessServiceResponseList(respList);
    EXPECT_TRUE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));
    pWifiP2pServiceManager->ProcessServiceResponseList(respList);
}

HWTEST_F(WifiP2pServiceManagerTest, AddRequestRecord, TestSize.Level1)
{
    pWifiP2pServiceManager->AddRequestRecord("AA:BB:CC:DD:EE:FF", 10);
    pWifiP2pServiceManager->AddRequestRecord("AA:BB:CC:DD:EE:FF", 10);
    pWifiP2pServiceManager->AddRequestRecord("AA:BB:CC:DD:EE:FF", 11);
}

HWTEST_F(WifiP2pServiceManagerTest, IsRecordedRequest, TestSize.Level1)
{
    WifiP2pServiceInfo p2pSvrInfo;
    EXPECT_FALSE(pWifiP2pServiceManager->IsRecordedRequest("AA:BB:CC:DD:EE:FF", 10));
    p2pSvrInfo.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    EXPECT_TRUE(pWifiP2pServiceManager->AddLocalService(p2pSvrInfo));

    EXPECT_FALSE(pWifiP2pServiceManager->IsRecordedRequest("AA:BB:CC:DD:EE:FF", 10));
    pWifiP2pServiceManager->AddRequestRecord("AA:BB:CC:DD:EE:FF", 10);
    EXPECT_TRUE(pWifiP2pServiceManager->IsRecordedRequest("AA:BB:CC:DD:EE:FF", 10));
}

HWTEST_F(WifiP2pServiceManagerTest, RemoveRequestRecord, TestSize.Level1)
{
    pWifiP2pServiceManager->RemoveRequestRecord("AA:BB:CC:DD:EE:FF", 10);
    pWifiP2pServiceManager->AddRequestRecord("AA:BB:CC:DD:EE:FF", 10);

    pWifiP2pServiceManager->RemoveRequestRecord("AA:BB:CC:DD:EE:FF", 10);
    pWifiP2pServiceManager->RemoveRequestRecord("AA:BB:CC:DD:EE:FF", 11);
}

HWTEST_F(WifiP2pServiceManagerTest, ClearAllRequestRecord, TestSize.Level1)
{
    pWifiP2pServiceManager->ClearAllRequestRecord();
}

HWTEST_F(WifiP2pServiceManagerTest, UpdateServiceName1, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    resp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    WifiP2pDevice device;
    device.SetDeviceAddress(std::string("aa:bb:cc:dd:ee:ff"));
    EXPECT_FALSE(pWifiP2pServiceManager->UpdateServiceName(std::string("aa:bb:cc:dd:ee:ff"), resp));
}

HWTEST_F(WifiP2pServiceManagerTest, UpdateServiceName2, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    resp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    WifiP2pDevice device;
    device.SetDeviceAddress(std::string("aa:bb:cc:dd:ee:ff"));
    pWifiP2pServiceManager->AddDeviceService(resp, device);
    EXPECT_TRUE(pWifiP2pServiceManager->UpdateServiceName(std::string("aa:bb:cc:dd:ee:ff"), resp));
}

HWTEST_F(WifiP2pServiceManagerTest, UpdateServiceName3, TestSize.Level1)
{
    WifiP2pServiceResponse resp;
    resp.SetServiceStatus(P2pServiceStatus::PSRS_SUCCESS);
    resp.SetProtocolType(P2pServicerProtocolType::SERVICE_TYPE_BONJOUR);
    WifiP2pDevice device;
    device.SetDeviceAddress(std::string("aa:bb:cc:dd:ee:ff"));
    pWifiP2pServiceManager->AddDeviceService(resp, device);
    resp.SetProtocolType(P2pServicerProtocolType::SERVICE_TYPE_UP_NP);
    EXPECT_FALSE(pWifiP2pServiceManager->UpdateServiceName(std::string("aa:bb:cc:dd:ee:ff"), resp));
}
}  // namespace Wifi
}  // namespace OHOS
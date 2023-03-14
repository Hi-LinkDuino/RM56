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
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mock_wifi_ap_hal_interface.h"
#include "mock_ap_service.h"
#include "mock_wifi_settings.h"
#include "operator_overload.h"

#include "ap_stations_manager.h"

using namespace OHOS;
using ::testing::_;
using ::testing::An;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
const std::string Mac = "AA:BB:CC:DD:EE:FF";
static StationInfo staInfo = {
    "test_deviceName",
    Mac.c_str(),
    "127.0.0.1",
};
StationInfo value1 = {
    "test_deviceName",
    "AA:BB:CC:DD:EE:FF",
    "127.0.0.2",
};
StationInfo value2 = {
    "test_deviceName",
    "AA:BB:CC:DD:EE:FF",
    "127.0.0.3",
};
class ApStationsManager_test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pApStaMgr = new ApStationsManager();
    }
    virtual void TearDown()
    {
        delete pApStaMgr;
        pApStaMgr = nullptr;
    }

public:
    bool WrapAddAssociationStation(const StationInfo &info)
    {
        return pApStaMgr->AddAssociationStation(info);
    }
    bool WrapDelAssociationStation(const StationInfo &info)
    {
        return pApStaMgr->DelAssociationStation(info);
    }

public:
    ApStationsManager *pApStaMgr;
};
/* AddBlockList */
HWTEST_F(ApStationsManager_test, AddBlockList_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), AddBlockByMac(StrEq(staInfo.bssid)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));

    EXPECT_TRUE(pApStaMgr->AddBlockList(staInfo));
}
HWTEST_F(ApStationsManager_test, AddBlockList_FAILED, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), AddBlockByMac(StrEq(staInfo.bssid)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));

    EXPECT_FALSE(pApStaMgr->AddBlockList(staInfo));
}

/* DelBlockList */
HWTEST_F(ApStationsManager_test, DelBlockList_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), DelBlockByMac(StrEq(staInfo.bssid)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));

    EXPECT_TRUE(pApStaMgr->DelBlockList(staInfo));
}
HWTEST_F(ApStationsManager_test, DelBlockList_FAILED, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), DelBlockByMac(StrEq(staInfo.bssid)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));

    EXPECT_FALSE(pApStaMgr->DelBlockList(staInfo));
}

/* EnableAllBlockList */
HWTEST_F(ApStationsManager_test, EnableAllBlockList_SUCCESS, TestSize.Level1)
{
    std::vector<StationInfo> value;
    std::vector<StationInfo> valueCom;
    value.push_back(value1);
    value.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetBlockList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(value), Return(0)));
    EXPECT_CALL(WifiApHalInterface::GetInstance(), AddBlockByMac(An<const std::string &>()))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pApStaMgr->EnableAllBlockList());
}
HWTEST_F(ApStationsManager_test, EnableAllBlockList_FAILED0, TestSize.Level1)
{
    std::vector<StationInfo> value;
    std::vector<StationInfo> valueCom;
    value.push_back(value1);
    value.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetBlockList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(value), Return(-1)));
    EXPECT_FALSE(pApStaMgr->EnableAllBlockList());
}
HWTEST_F(ApStationsManager_test, EnableAllBlockList_FAILED1, TestSize.Level1)
{
    std::vector<StationInfo> value;
    std::vector<StationInfo> valueCom;
    value.push_back(value1);
    value.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetBlockList(valueCom))
        .WillOnce(DoAll(testing::SetArgReferee<0>(value), Return(0)));
    EXPECT_CALL(WifiApHalInterface::GetInstance(), AddBlockByMac(An<const std::string &>()))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_FALSE(pApStaMgr->EnableAllBlockList());
}

/* StationLeave */
HWTEST_F(ApStationsManager_test, StationLeave, TestSize.Level1)
{
    StationInfo value3 = {
        "test_deviceName",
        "DA:BB:CC:DD:EE:FF",
        "127.0.0.3",
    };
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    valueList.push_back(value2);
    valueList.push_back(value1);
    valueList.push_back(value3);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(value2), Eq(1))).WillOnce(Return(0));
    pApStaMgr->StationLeave(value1.bssid);
}
HWTEST_F(ApStationsManager_test, StationLeave1, TestSize.Level1)
{
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    StationInfo value3 = {
        "test_deviceName",
        "DA:BB:CC:DD:EE:FF",
        "127.0.0.3",
    };
    valueList.push_back(value2);
    valueList.push_back(value1);
    valueList.push_back(value3);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(1)));
    pApStaMgr->StationLeave(value1.bssid);
    EXPECT_STREQ("AA:BB:CC:DD:EE:FF", Mac.c_str());
}
HWTEST_F(ApStationsManager_test, StationLeave2, TestSize.Level1)
{
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    StationInfo value3 = {
        "test_deviceName",
        "DA:BB:CC:DD:EE:FF",
        "127.0.0.3",
    };
    valueList.push_back(value3);
    valueList.push_back(value1);
    valueList.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(value1), Eq(1))).WillOnce(Return(1));
    pApStaMgr->StationLeave(value1.bssid);
    EXPECT_STREQ("AA:BB:CC:DD:EE:FF", Mac.c_str());
}

/* StationJoin */
HWTEST_F(ApStationsManager_test, StationJoin1, TestSize.Level1)
{
    std::vector<StationInfo> valueCom;
    std::vector<StationInfo> valueList;
    StationInfo value3 = {
        "test_deviceName",
        "AA:BB:CC:DD:EE:FF",
        "127.0.0.4",
    };
    valueList.push_back(value2);
    valueList.push_back(value1);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(1)));
    pApStaMgr->StationJoin(value3);
}
HWTEST_F(ApStationsManager_test, StationJoin2, TestSize.Level1)
{
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    StationInfo value3 = {
        "test_deviceName",
        "DA:BB:CC:DD:EE:FF",
        "127.0.0.4",
    };
    valueList.push_back(value1);
    valueList.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(value3), 0)).WillOnce(Return(0));
    pApStaMgr->StationJoin(value3);
}
HWTEST_F(ApStationsManager_test, StationJoin3, TestSize.Level1)
{
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    StationInfo value3 = {
        "test_deviceName",
        "AA:BB:CC:DD:EE:FF",
        "127.0.0.4",
    };
    valueList.push_back(value2);
    valueList.push_back(value1);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(value3), 0)).WillOnce(Return(1));
    pApStaMgr->StationJoin(value3);
}
HWTEST_F(ApStationsManager_test, StationJoin4, TestSize.Level1)
{
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    StationInfo value3 = {
        "test_deviceName",
        "AA:BB:CC:DD:EE:FF",
        "127.0.0.4",
    };
    valueList.push_back(value1);
    valueList.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(value3), 0)).WillOnce(Return(0));
    pApStaMgr->StationJoin(value3);
}
HWTEST_F(ApStationsManager_test, StationJoin5, TestSize.Level1)
{
    std::vector<StationInfo> valueList;
    std::vector<StationInfo> valueCom;
    StationInfo value3 = {
        OHOS::Wifi::GETTING_INFO,
        "AA:BB:CC:DD:EE:FF",
        OHOS::Wifi::GETTING_INFO,
    };
    valueList.push_back(value1);
    valueList.push_back(value2);
    EXPECT_CALL(WifiSettings::GetInstance(), GetStationList(Eq(valueCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(valueList), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(value1), 0)).WillOnce(Return(0));
    pApStaMgr->StationJoin(value3);
}
/* DisConnectStation */
HWTEST_F(ApStationsManager_test, DisConnectStion_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), DisconnectStaByMac(StrEq(Mac)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));

    EXPECT_TRUE(pApStaMgr->DisConnectStation(staInfo));
}
HWTEST_F(ApStationsManager_test, DisConnectStion_FAILED, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), DisconnectStaByMac(StrEq(Mac)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));

    EXPECT_FALSE(pApStaMgr->DisConnectStation(staInfo));
}

/* GetAllConnectedStations */
HWTEST_F(ApStationsManager_test, GetAllConnectedStations_SUCCESS, TestSize.Level1)
{
    std::vector<std::string> staMacList;
    std::vector<std::string> staMacListCom;
    std::string staMacList1 = "test_deviceName1";
    std::string staMacList2 = "test_deviceName2";
    staMacList.push_back(staMacList1);
    staMacList.push_back(staMacList2);
    EXPECT_CALL(WifiApHalInterface::GetInstance(), GetStationList(Eq(staMacListCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(staMacList), Return(WifiErrorNo::WIFI_IDL_OPT_OK)));

    EXPECT_EQ(staMacList, pApStaMgr->GetAllConnectedStations());
}
HWTEST_F(ApStationsManager_test, GetAllConnectedStations_FAILED, TestSize.Level1)
{
    std::vector<std::string> staMacList;
    std::vector<std::string> staMacListCom;
    std::string staMacList1 = "test_deviceName1";
    std::string staMacList2 = "test_deviceName2";
    staMacList.push_back(staMacList1);
    staMacList.push_back(staMacList2);
    EXPECT_CALL(WifiApHalInterface::GetInstance(), GetStationList(Eq(staMacListCom)))
        .WillOnce(DoAll(testing::SetArgReferee<0>(staMacList), Return(WifiErrorNo::WIFI_IDL_OPT_FAILED)));
    staMacList.erase(staMacList.begin());
    EXPECT_NE(staMacList, pApStaMgr->GetAllConnectedStations());
}

/* AddAssociationStation */
HWTEST_F(ApStationsManager_test, AddAssociationStation_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(staInfo), Eq(0))).WillOnce(Return(0));
    EXPECT_TRUE(WrapAddAssociationStation(staInfo));
}
HWTEST_F(ApStationsManager_test, AddAssociationStation_FAILED, TestSize.Level1)
{
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(staInfo), Eq(0))).WillOnce(Return(-1));
    EXPECT_FALSE(WrapAddAssociationStation(staInfo));
}

/* DelAssociationStation */
HWTEST_F(ApStationsManager_test, DelAssociationStation_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(staInfo), Eq(1))).WillOnce(Return(0));
    EXPECT_TRUE(WrapDelAssociationStation(staInfo));
}
HWTEST_F(ApStationsManager_test, DelAssociationStation_FAILED, TestSize.Level1)
{
    EXPECT_CALL(WifiSettings::GetInstance(), ManageStation(Eq(staInfo), Eq(1))).WillOnce(Return(-1));
    EXPECT_FALSE(WrapDelAssociationStation(staInfo));
}
} // namespace Wifi
} // namespace OHOS
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdlib>
#include <ctime>
#include <functional>
#include <securec.h>

#include "parameter.h"
#include "dm_anonymous.h"
#include "dm_log.h"
#include "dm_constants.h"
#include "dm_random.h"
#include "hichain_connector.h"
#include "UTTest_hichain_connector.h"

namespace OHOS {
namespace DistributedHardware {
void HichainConnectorTest::SetUp()
{
}
void HichainConnectorTest::TearDown()
{
}
void HichainConnectorTest::SetUpTestCase()
{
}
void HichainConnectorTest::TearDownTestCase()
{
}
namespace {
std::shared_ptr<DeviceManagerServiceListener> listener = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<HiChainConnector> hiChainConnector = std::make_shared<HiChainConnector>();
std::shared_ptr<DmAuthManager> discoveryMgr =
    std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);

/**
 * @tc.name: CreateGroup_001
 * @tc.desc: Set the deviceGroupManager_ pointer to CreateGroup to NULlptr and return DM_INVALID_VALUE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, CreateGroup_001, testing::ext::TestSize.Level0)
{
    int64_t requestId = 123456;
    std::string groupName = "dfggg";
    hiChainConnector->deviceGroupManager_ = nullptr;
    int ret = hiChainConnector->CreateGroup(requestId, groupName);
    EXPECT_EQ(ret, DM_INVALID_VALUE);
}

/**
 * @tc.name: CreateGroup_002
 * @tc.desc: Set CreateGroup to the correct process and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, CreateGroup_002, testing::ext::TestSize.Level0)
{
    int64_t requestId = 123456;
    std::string groupName = "uuiioo";
    int ret = hiChainConnector->CreateGroup(requestId, groupName);
    EXPECT_EQ(ret, DM_INVALID_VALUE);
}

/**
 * @tc.name: IsGroupInfoInvalid_001
 * @tc.desc: GroupType is GROUP_TYPE_IDENTICAL_ACCOUNT_GROUP, group.groupVisibility is not GROUP_VISIBILITY_PUBLIC.
             Group.return true
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */

HWTEST_F(HichainConnectorTest, IsGroupInfoInvalid_001, testing::ext::TestSize.Level0)
{
    GroupInfo group;
    group.groupName = "dkdkkdkdk";
    group.groupId = 1;
    group.groupOwner = "ohos.distributedhardware.devicemanager";
    group.groupType = 7;
    group.groupVisibility = 1;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    bool ret = hiChainConnector->IsGroupInfoInvalid(group);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: IsGroupInfoInvalid_002
 * @tc.desc: GroupType is GROUP_TYPE_IDENTICAL_ACCOUNT_GROUP, group.groupVisibility is GROUP_VISIBILITY_PUBLIC,
             Grou. groupOwner is not equal to DM_PKG_NAME. The value is true
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, IsGroupInfoInvalid_002, testing::ext::TestSize.Level0)
{
    GroupInfo group;
    group.groupName = "test";
    group.groupId = 1;
    group.groupOwner = "ohos.disware";
    group.groupType = 1;
    group.groupVisibility = -1;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    bool ret = hichainConnector->IsGroupInfoInvalid(group);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: DelMemberFromGroup_001
 * @tc.desc:set  groupId, deviceId null and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, DelMemberFromGroup_001, testing::ext::TestSize.Level0)
{
    std::string groupId;
    std::string deviceId;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->DelMemberFromGroup(groupId, deviceId);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: DelMemberFromGroup_002
 * @tc.desc: The groupId "34451"; The deviceId = "123"; Can be deleted correctly
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, DelMemberFromGroup_002, testing::ext::TestSize.Level0)
{
    std::string groupId = "34451";
    std::string deviceId = "123";
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->DelMemberFromGroup(groupId, deviceId);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GenRequestId_001
 * @tc.desc:Call the GenRequestId function
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GenRequestId_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->GenRequestId();
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: from_json_001
 * @tc.desc: Pass in arguments to the from_JSON function and convert it to the correct value
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, from_json_001, testing::ext::TestSize.Level0)
{
    GroupInfo groupInfo;
    groupInfo.groupName = "aaaa";
    groupInfo.groupId = "345678";
    groupInfo.groupOwner = "lllll";
    groupInfo.groupType = 5;
    groupInfo.groupVisibility = 5;
    nlohmann::json jsonObject;
    jsonObject[FIELD_GROUP_NAME] = groupInfo.groupName;
    jsonObject[FIELD_GROUP_ID] = groupInfo.groupId;
    jsonObject[FIELD_GROUP_OWNER] = groupInfo.groupOwner;
    jsonObject[FIELD_GROUP_TYPE] = groupInfo.groupType;
    jsonObject[FIELD_GROUP_VISIBILITY] = groupInfo.groupVisibility;
    from_json(jsonObject, groupInfo);
    EXPECT_EQ(groupInfo.groupName, "aaaa");
    EXPECT_EQ(groupInfo.groupId, "345678");
    EXPECT_EQ(groupInfo.groupOwner, "lllll");
    EXPECT_EQ(groupInfo.groupType, 5);
    EXPECT_EQ(groupInfo.groupVisibility, 5);
}

/**
 * @tc.name: HiChainConnector_001
 * @tc.desc: Returns a new pointer to the HiChainConnector constructor new
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, HiChainConnector_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> m_HiChainConnector = std::make_shared<HiChainConnector>();
    ASSERT_NE(m_HiChainConnector, nullptr);
}

/**
 * @tc.name: HiChainConnector_002
 * @tc.desc: Give the HiChainConnector constructor new a new pointer and delete it
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, HiChainConnector_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> m_HiChainConnector = std::make_shared<HiChainConnector>();
    m_HiChainConnector.reset();
    EXPECT_EQ(m_HiChainConnector, nullptr);
}

/**
 * @tc.name:RegisterHiChainCallback_001
 * @tc.desc: Call the RegisterHiChainCallback function with a return value of DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, RegisterHiChainCallback_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->RegisterHiChainCallback(std::shared_ptr<IHiChainConnectorCallback>(discoveryMgr));
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AddMember_001
 * @tc.desc: set deviceGroupManager_ = nullptr;
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, AddMember_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    hichainConnector->deviceGroupManager_ = nullptr;
    std::string deviceId;
    std::string connectInfo;
    int ret = hichainConnector->AddMember(deviceId, connectInfo);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: AddMember_002
 * @tc.desc: set deviceId and connectInfo = null;
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, AddMember_002, testing::ext::TestSize.Level0)
{
    std::string deviceId;
    std::string connectInfo;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->AddMember(deviceId, connectInfo);
    EXPECT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AddMember_003
 * @tc.desc: set deviceId and connectInfo = null;
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, AddMember_003, testing::ext::TestSize.Level0)
{
    std::string deviceId = "123456";
    std::string connectInfo = "dkdkk";
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->AddMember(deviceId, connectInfo);
    ASSERT_GE(ret, 1);
}

/**
 * @tc.name: onRequest_001
 * @tc.desc:set  operationCode != GroupOperationCode::MEMBER_JOIN(3);  return nullptr ;
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, onRequest_001, testing::ext::TestSize.Level0)
{
    int64_t requestId = 2;
    int32_t operationCode = 2;
    char *reqParams;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    char *ret = hichainConnector->onRequest(requestId, operationCode, reqParams);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetConnectPara_001
 * @tc.desc: set para not null and go to the second master
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GetConnectPara_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    hichainConnector->RegisterHiChainCallback(std::shared_ptr<IHiChainConnectorCallback>(discoveryMgr));
    std::string deviceId = "23445";
    std::string reqDeviceId = "234566";
    std::string p;
    std::string ret = hichainConnector->GetConnectPara(deviceId, reqDeviceId);
    EXPECT_EQ(ret, p);
}

/**
 * @tc.name: GetConnectPara_002
 * @tc.desc:Empty deviceId so that jsonObject.is_discarded is null and the value of connectAddr is returned
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GetConnectPara_002, testing::ext::TestSize.Level0)
{
    std::string deviceId;
    std::string reqDeviceId = "234566";
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    hichainConnector->RegisterHiChainCallback(std::shared_ptr<IHiChainConnectorCallback>(discoveryMgr));
    std::string ret = hichainConnector->GetConnectPara(deviceId, reqDeviceId);
    EXPECT_EQ(ret, "");
}

/**
 * @tc.name: DeleteGroup_001
 * @tc.desc: set groupId = "34567",and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, DeleteGroup_001, testing::ext::TestSize.Level0)
{
    std::string groupId = "34567";
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->DeleteGroup(groupId);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetRelatedGroups_001
 * @tc.desc: set DeviceId 123  groupList null and return DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GetRelatedGroups_001, testing::ext::TestSize.Level0)
{
    std::string DeviceId = "123";
    std::vector<GroupInfo> groupList;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->GetRelatedGroups(DeviceId, groupList);
    EXPECT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: GetRelatedGroups_002
 * @tc.desc: set DeviceId  = 12345,groupList null and return DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GetRelatedGroups_002, testing::ext::TestSize.Level0)
{
    std::string DeviceId = "12345";
    std::vector<GroupInfo> groupList;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->GetRelatedGroups(DeviceId, groupList);
    EXPECT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: SyncGroups_001
 * @tc.desc: set deviceId = "34567",and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, SyncGroups_001, testing::ext::TestSize.Level0)
{
    std::string deviceId = "34567";
    std::vector<std::string> remoteGroupIdList;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->SyncGroups(deviceId, remoteGroupIdList);
    EXPECT_EQ(ret, DM_OK);
}

/**
 * @tc.name: GetSyncGroupList_001
 * @tc.desc: set groupList null,and return DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GetSyncGroupList_001, testing::ext::TestSize.Level0)
{
    std::vector<GroupInfo> groupList;
    std::vector<std::string> syncGroupList;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->GetSyncGroupList(groupList, syncGroupList);
    EXPECT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: GetSyncGroupList_002
 * @tc.desc: set groupList not null,and return DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(HichainConnectorTest, GetSyncGroupList_002, testing::ext::TestSize.Level0)
{
    std::vector<GroupInfo> groupList;
    GroupInfo groupList1;
    groupList1.groupName = "hichainconnector";
    groupList1.groupId = "123456";
    groupList1.groupOwner = "doftbus";
    groupList1.groupType = 1;
    groupList1.groupVisibility = 2;
    groupList.push_back(groupList1);
    std::vector<std::string> syncGroupList;
    std::shared_ptr<HiChainConnector> hichainConnector = std::make_shared<HiChainConnector>();
    int ret = hichainConnector->GetSyncGroupList(groupList, syncGroupList);
    EXPECT_EQ(ret, DM_OK);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS

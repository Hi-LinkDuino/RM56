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

#include "hichain_connector.h"

#include <securec.h>

#include <cstdlib>
#include <ctime>
#include <functional>

#include "dm_anonymous.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "dm_random.h"
#include "hichain_connector_callback.h"
#include "multiple_user_connector.h"
#include "nlohmann/json.hpp"
#include "parameter.h"

namespace OHOS {
namespace DistributedHardware {
void from_json(const nlohmann::json &jsonObject, GroupInfo &groupInfo)
{
    if (jsonObject.find(FIELD_GROUP_NAME) != jsonObject.end()) {
        groupInfo.groupName = jsonObject.at(FIELD_GROUP_NAME).get<std::string>();
    }

    if (jsonObject.find(FIELD_GROUP_ID) != jsonObject.end()) {
        groupInfo.groupId = jsonObject.at(FIELD_GROUP_ID).get<std::string>();
    }

    if (jsonObject.find(FIELD_GROUP_OWNER) != jsonObject.end()) {
        groupInfo.groupOwner = jsonObject.at(FIELD_GROUP_OWNER).get<std::string>();
    }

    if (jsonObject.find(FIELD_GROUP_TYPE) != jsonObject.end()) {
        groupInfo.groupType = jsonObject.at(FIELD_GROUP_TYPE).get<int32_t>();
    }

    if (jsonObject.find(FIELD_GROUP_VISIBILITY) != jsonObject.end()) {
        groupInfo.groupVisibility = jsonObject.at(FIELD_GROUP_VISIBILITY).get<int32_t>();
    }
}

std::shared_ptr<IHiChainConnectorCallback> HiChainConnector::hiChainConnectorCallback_ = nullptr;

HiChainConnector::HiChainConnector()
{
    LOGI("HiChainConnector::constructor");
    deviceAuthCallback_ = {.onTransmit = nullptr,
                           .onFinish = HiChainConnector::onFinish,
                           .onError = HiChainConnector::onError,
                           .onRequest = HiChainConnector::onRequest};
    InitDeviceAuthService();
    deviceGroupManager_ = GetGmInstance();
    if (deviceGroupManager_ == nullptr) {
        LOGI("HiChainConnector::constructor, failed to init group manager!");
        return;
    }
    deviceGroupManager_->regCallback(DM_PKG_NAME.c_str(), &deviceAuthCallback_);
    LOGI("HiChainConnector::constructor success.");
}

HiChainConnector::~HiChainConnector()
{
    LOGI("HiChainConnector::destructor.");
}

int32_t HiChainConnector::RegisterHiChainCallback(std::shared_ptr<IHiChainConnectorCallback> callback)
{
    hiChainConnectorCallback_ = callback;
    return DM_OK;
}

int32_t HiChainConnector::UnRegisterHiChainCallback()
{
    hiChainConnectorCallback_ = nullptr;
    return DM_OK;
}

int32_t HiChainConnector::CreateGroup(int64_t requestId, const std::string &groupName)
{
    if (deviceGroupManager_ == nullptr) {
        LOGE("HiChainConnector::CreateGroup group manager is null, requestId %lld.", requestId);
        return DM_INVALID_VALUE;
    }
    GroupInfo groupInfo;
    if (IsGroupCreated(groupName, groupInfo)) {
        DeleteGroup(groupInfo.groupId);
    }
    LOGI("HiChainConnector::CreateGroup requestId %lld", requestId);
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    std::string sLocalDeviceId = localDeviceId;
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_TYPE] = GROUP_TYPE_PEER_TO_PEER_GROUP;
    jsonObj[FIELD_DEVICE_ID] = sLocalDeviceId;
    jsonObj[FIELD_GROUP_NAME] = groupName;
    jsonObj[FIELD_USER_TYPE] = 0;
    jsonObj[FIELD_GROUP_VISIBILITY] = GROUP_VISIBILITY_PUBLIC;
    jsonObj[FIELD_EXPIRE_TIME] = FIELD_EXPIRE_TIME_VALUE;
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }

    int32_t ret = deviceGroupManager_->createGroup(userId, requestId, DM_PKG_NAME.c_str(), jsonObj.dump().c_str());
    if (ret != 0) {
        LOGE("Failed to start CreateGroup task, ret: %d, requestId %lld.", ret, requestId);
        return DM_HICHAIN_GROUP_CREATE_FAILED;
    }
    return DM_OK;
}

bool HiChainConnector::IsGroupCreated(std::string groupName, GroupInfo &groupInfo)
{
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_NAME] = groupName.c_str();
    std::string queryParams = jsonObj.dump();
    std::vector<GroupInfo> groupList;
    if (GetGroupInfo(queryParams, groupList)) {
        groupInfo = groupList[0];
        return true;
    }
    return false;
}

int32_t HiChainConnector::GetGroupInfo(const std::string &queryParams, std::vector<GroupInfo> &groupList)
{
    char *groupVec = nullptr;
    uint32_t num = 0;
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }
    int32_t ret = deviceGroupManager_->getGroupInfo(userId, DM_PKG_NAME.c_str(), queryParams.c_str(), &groupVec, &num);
    if (ret != 0) {
        LOGE("HiChainConnector::GetGroupInfo failed , ret: %d.", ret);
        return false;
    }
    if (groupVec == nullptr) {
        LOGE("HiChainConnector::GetGroupInfo failed , returnGroups is nullptr");
        return false;
    }
    if (num == 0) {
        LOGE("HiChainConnector::GetGroupInfo group failed, groupNum is 0.");
        return false;
    }
    LOGI("HiChainConnector::GetGroupInfo group(%s), groupNum(%u)", groupVec, num);
    std::string relatedGroups = std::string(groupVec);
    deviceGroupManager_->destroyInfo(&groupVec);
    nlohmann::json jsonObject = nlohmann::json::parse(relatedGroups);
    if (jsonObject.is_discarded()) {
        LOGE("returnGroups parse error");
        return false;
    }
    std::vector<GroupInfo> groupInfos = jsonObject.get<std::vector<GroupInfo>>();
    if (groupInfos.size() == 0) {
        LOGE("HiChainConnector::GetGroupInfo group failed, groupInfos is empty.");
        return false;
    }
    groupList = groupInfos;
    return true;
}

int32_t HiChainConnector::GetGroupInfo(const int32_t userId, const std::string &queryParams,
    std::vector<GroupInfo> &groupList)
{
    char *groupVec = nullptr;
    uint32_t num = 0;
    int32_t ret = deviceGroupManager_->getGroupInfo(userId, DM_PKG_NAME.c_str(), queryParams.c_str(), &groupVec, &num);
    if (ret != 0) {
        LOGE("HiChainConnector::GetGroupInfo failed , ret: %d.", ret);
        return false;
    }
    if (groupVec == nullptr) {
        LOGE("HiChainConnector::GetGroupInfo failed , returnGroups is nullptr.");
        return false;
    }
    if (num == 0) {
        LOGE("HiChainConnector::GetGroupInfo group failed, groupNum is 0.");
        return false;
    }
    LOGI("HiChainConnector::GetGroupInfo group(%s), groupNum(%ud)", groupVec, num);
    std::string relatedGroups = std::string(groupVec);
    deviceGroupManager_->destroyInfo(&groupVec);
    nlohmann::json jsonObject = nlohmann::json::parse(relatedGroups);
    if (jsonObject.is_discarded()) {
        LOGE("returnGroups parse error");
        return false;
    }
    std::vector<GroupInfo> groupInfos = jsonObject.get<std::vector<GroupInfo>>();
    if (groupInfos.size() == 0) {
        LOGE("HiChainConnector::GetGroupInfo group failed, groupInfos is empty.");
        return false;
    }
    groupList = groupInfos;
    return true;
}

int32_t HiChainConnector::AddMember(const std::string &deviceId, const std::string &connectInfo)
{
    LOGI("HiChainConnector::AddMember");
    if (deviceGroupManager_ == nullptr) {
        LOGI("HiChainConnector::AddMember group manager is null.");
        return -1;
    }
    nlohmann::json jsonObject = nlohmann::json::parse(connectInfo, nullptr, false);
    if (jsonObject.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    std::string connectInfomation = GetConnectPara(deviceId, jsonObject[TAG_DEVICE_ID]);

    int32_t pinCode = jsonObject[PIN_CODE_KEY];
    std::string groupId = jsonObject[TAG_GROUP_ID];
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_ID] = groupId;
    jsonObj[FIELD_GROUP_TYPE] = GROUP_TYPE_PEER_TO_PEER_GROUP;
    jsonObj[FIELD_PIN_CODE] = std::to_string(pinCode).c_str();
    jsonObj[FIELD_IS_ADMIN] = false;
    jsonObj[FIELD_DEVICE_ID] = localDeviceId;
    jsonObj[FIELD_GROUP_NAME] = jsonObject[TAG_GROUP_NAME];
    jsonObj[FIELD_CONNECT_PARAMS] = connectInfomation.c_str();
    std::string tmpStr = jsonObj.dump();
    int64_t requestId = jsonObject[TAG_REQUEST_ID];
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }
    int32_t ret = deviceGroupManager_->addMemberToGroup(userId, requestId, DM_PKG_NAME.c_str(), tmpStr.c_str());
    LOGI("HiChainConnector::AddMember completed");
    return ret;
}

void HiChainConnector::onFinish(int64_t requestId, int operationCode, const char *returnData)
{
    std::string data = "";
    if (returnData != nullptr) {
        data = std::string(returnData);
    }
    LOGI("HiChainConnector::onFinish reqId:%lld, operation:%d", requestId, operationCode);
    if (operationCode == GroupOperationCode::MEMBER_JOIN) {
        LOGI("Add Member To Group success");
        hiChainConnectorCallback_->OnMemberJoin(requestId, DM_OK);
    }
    if (operationCode == GroupOperationCode::GROUP_CREATE) {
        LOGI("Create group success");
        hiChainConnectorCallback_->OnGroupCreated(requestId, data);
    }
    if (operationCode == GroupOperationCode::MEMBER_DELETE) {
        LOGI("Delete Member from group success");
    }
    if (operationCode == GroupOperationCode::GROUP_DISBAND) {
        LOGI("Disband group success");
    }
}

void HiChainConnector::onError(int64_t requestId, int operationCode, int errorCode, const char *errorReturn)
{
    (void)errorReturn;
    LOGI("HichainAuthenCallBack::onError reqId:%lld, operation:%d, errorCode:%d.", requestId, operationCode, errorCode);
    if (operationCode == GroupOperationCode::MEMBER_JOIN) {
        LOGE("Add Member To Group failed");
        hiChainConnectorCallback_->OnMemberJoin(requestId, DM_FAILED);
    }
    if (operationCode == GroupOperationCode::GROUP_CREATE) {
        LOGE("Create group failed");
        hiChainConnectorCallback_->OnGroupCreated(requestId, "{}");
    }
    if (operationCode == GroupOperationCode::MEMBER_DELETE) {
        LOGE("Delete Member from group failed");
    }
    if (operationCode == GroupOperationCode::GROUP_DISBAND) {
        LOGE("Disband group failed");
    }
}

char *HiChainConnector::onRequest(int64_t requestId, int operationCode, const char *reqParams)
{
    if (operationCode != GroupOperationCode::MEMBER_JOIN) {
        LOGE("HiChainConnector::onRequest operationCode %d", operationCode);
        return nullptr;
    }
    nlohmann::json jsonObj;
    int32_t pinCode = hiChainConnectorCallback_->GetPinCode();
    if (pinCode == DM_FAILED) {
        jsonObj[FIELD_CONFIRMATION] = REQUEST_REJECTED;
    } else {
        jsonObj[FIELD_CONFIRMATION] = REQUEST_ACCEPTED;
    }
    jsonObj[FIELD_PIN_CODE] = std::to_string(pinCode).c_str();
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    jsonObj[FIELD_DEVICE_ID] = localDeviceId;

    std::string jsonStr = jsonObj.dump();
    char *buffer = strdup(jsonStr.c_str());
    return buffer;
}

int64_t HiChainConnector::GenRequestId()
{
    return GenRandLongLong(MIN_REQUEST_ID, MAX_REQUEST_ID);
}

std::string HiChainConnector::GetConnectPara(std::string deviceId, std::string reqDeviceId)
{
    LOGI("HiChainConnector::GetConnectPara get addrInfo");
    std::string connectAddr = hiChainConnectorCallback_->GetConnectAddr(deviceId);
    nlohmann::json jsonObject = nlohmann::json::parse(connectAddr, nullptr, false);
    if (jsonObject.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return connectAddr;
    }
    jsonObject[DEVICE_ID] = reqDeviceId;

    return jsonObject.dump();
}

int32_t HiChainConnector::GetRelatedGroups(const std::string &deviceId, std::vector<GroupInfo> &groupList)
{
    LOGI("HiChainConnector::GetRelatedGroups Start to get local related groups.");
    uint32_t groupNum = 0;
    char *returnGroups = nullptr;
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }
    int32_t ret =
        deviceGroupManager_->getRelatedGroups(userId, DM_PKG_NAME.c_str(), deviceId.c_str(), &returnGroups, &groupNum);
    if (ret != 0) {
        LOGE("HiChainConnector::GetRelatedGroups failed , ret: %d.", ret);
        return DM_FAILED;
    }
    if (returnGroups == nullptr) {
        LOGE("HiChainConnector::GetRelatedGroups failed , returnGroups is nullptr");
        return DM_FAILED;
    }
    if (groupNum == 0) {
        LOGE("HiChainConnector::GetRelatedGroups group failed, groupNum is 0.");
        return DM_FAILED;
    }
    std::string relatedGroups = std::string(returnGroups);
    nlohmann::json jsonObject = nlohmann::json::parse(relatedGroups);
    if (jsonObject.is_discarded()) {
        LOGE("returnGroups parse error");
        return DM_FAILED;
    }
    std::vector<GroupInfo> groupInfos = jsonObject.get<std::vector<GroupInfo>>();
    if (groupInfos.empty()) {
        LOGE("HiChainConnector::GetRelatedGroups group failed, groupInfos is empty.");
        return DM_FAILED;
    }
    groupList = groupInfos;
    return DM_OK;
}

int32_t HiChainConnector::GetSyncGroupList(std::vector<GroupInfo> &groupList, std::vector<std::string> &syncGroupList)
{
    if (groupList.empty()) {
        LOGE("groupList is empty.");
        return DM_FAILED;
    }
    for (auto group : groupList) {
        if (IsGroupInfoInvalid(group)) {
            continue;
        }
        syncGroupList.push_back(group.groupId);
    }
    return DM_OK;
}

bool HiChainConnector::IsDevicesInGroup(const std::string &hostDevice, const std::string &peerDevice)
{
    LOGI("HiChainConnector::IsDevicesInGroup");
    std::vector<GroupInfo> hostGroupInfoList;
    GetRelatedGroups(hostDevice, hostGroupInfoList);
    std::vector<GroupInfo> peerGroupInfoList;
    GetRelatedGroups(peerDevice, peerGroupInfoList);
    for (auto &hostGroupInfo : hostGroupInfoList) {
        for (auto &peerGroupInfo : peerGroupInfoList) {
            if (hostGroupInfo.groupId == peerGroupInfo.groupId && hostGroupInfo.groupName == peerGroupInfo.groupName) {
                LOGE("these are authenticated");
                return false;
            }
        }
    }
    return true;
}

bool HiChainConnector::IsGroupInfoInvalid(GroupInfo &group)
{
    if (group.groupType == GROUP_TYPE_IDENTICAL_ACCOUNT_GROUP || group.groupVisibility == GROUP_VISIBILITY_PUBLIC ||
        group.groupOwner != DM_PKG_NAME) {
        return true;
    }
    return false;
}

int32_t HiChainConnector::SyncGroups(std::string deviceId, std::vector<std::string> &remoteGroupIdList)
{
    std::vector<GroupInfo> groupInfoList;
    GetRelatedGroups(deviceId, groupInfoList);
    for (auto &groupInfo : groupInfoList) {
        if (IsGroupInfoInvalid(groupInfo)) {
            continue;
        }
        auto iter = std::find(remoteGroupIdList.begin(), remoteGroupIdList.end(), groupInfo.groupId);
        if (iter == remoteGroupIdList.end()) {
            (void)DelMemberFromGroup(groupInfo.groupId, deviceId);
        }
    }
    return DM_OK;
}

int32_t HiChainConnector::DelMemberFromGroup(const std::string &groupId, const std::string &deviceId)
{
    int64_t requestId = GenRequestId();
    LOGI("Start to delete member from group, requestId %lld, deviceId %s, groupId %s", requestId,
         GetAnonyString(deviceId).c_str(), GetAnonyString(groupId).c_str());
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_ID] = groupId;
    jsonObj[FIELD_DELETE_ID] = deviceId;
    std::string deleteParams = jsonObj.dump();
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }
    int32_t ret = deviceGroupManager_->deleteMemberFromGroup(userId, requestId,
        DM_PKG_NAME.c_str(), deleteParams.c_str());
    if (ret != 0) {
        LOGE("HiChainConnector::DelMemberFromGroup failed , ret: %d.", ret);
        return ret;
    }
    return DM_OK;
}

int32_t HiChainConnector::DeleteGroup(std::string &groupId)
{
    int64_t requestId = GenRequestId();
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_ID] = groupId;
    std::string disbandParams = jsonObj.dump();
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }

    int32_t ret = deviceGroupManager_->deleteGroup(userId, requestId, DM_PKG_NAME.c_str(), disbandParams.c_str());
    if (ret != 0) {
        LOGE("HiChainConnector::DeleteGroup failed , ret: %d.", ret);
        return DM_FAILED;
    }
    return DM_OK;
}

int32_t HiChainConnector::DeleteGroup(const int32_t userId, std::string &groupId)
{
    int64_t requestId = GenRequestId();
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_ID] = groupId;
    std::string disbandParams = jsonObj.dump();
    int32_t ret = deviceGroupManager_->deleteGroup(userId, requestId, DM_PKG_NAME.c_str(), disbandParams.c_str());
    if (ret != 0) {
        LOGE("HiChainConnector::DeleteGroup failed , ret: %d.", ret);
        return DM_FAILED;
    }
    return DM_OK;
}

int32_t HiChainConnector::DeleteTimeOutGroup(const char* deviceId)
{
    LOGI("HiChainConnector::DeleteTimeOutGroup start");
    int32_t userId = MultipleUserConnector::GetCurrentAccountUserID();
    if (userId < 0) {
        LOGE("get current process account user id failed");
        return DM_FAILED;
    }
    std::vector<GroupInfo> peerGroupInfoList;
    GetRelatedGroups(deviceId, peerGroupInfoList);
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    for (auto &group : peerGroupInfoList) {
        if (deviceGroupManager_->isDeviceInGroup(userId, DM_PKG_NAME.c_str(), group.groupId.c_str(), localDeviceId)) {
            DeleteGroup(group.groupId);
            return DM_OK;
        }
    }
    return DM_FAILED;
}
} // namespace DistributedHardware
} // namespace OHOS

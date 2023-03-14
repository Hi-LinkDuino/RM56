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

#include "auth_message_processor.h"

#include "dm_auth_manager.h"
#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
AuthMessageProcessor::AuthMessageProcessor(std::shared_ptr<DmAuthManager> authMgr) : authMgr_(authMgr)
{
    LOGI("AuthMessageProcessor constructor");
}

AuthMessageProcessor::~AuthMessageProcessor()
{
    authMgr_.reset();
}

std::vector<std::string> AuthMessageProcessor::CreateAuthRequestMessage()
{
    LOGI("AuthMessageProcessor::CreateAuthRequestMessage start.");
    std::vector<std::string> jsonStrVec;
    int32_t thumbnailSize = (int32_t)(authRequestContext_->appThumbnail.size());
    int32_t thumbnailSlice = ((thumbnailSize / MSG_MAX_SIZE) + (thumbnailSize % MSG_MAX_SIZE) == 0 ? 0 : 1);
    nlohmann::json jsonObj;
    jsonObj[TAG_VER] = DM_ITF_VER;
    jsonObj[TAG_TYPE] = MSG_TYPE_REQ_AUTH;
    jsonObj[TAG_SLICE_NUM] = thumbnailSlice + 1;
    jsonObj[TAG_INDEX] = 0;
    jsonObj[TAG_REQUESTER] = authRequestContext_->deviceName;
    jsonObj[TAG_DEVICE_ID] = authRequestContext_->deviceId;
    jsonObj[TAG_DEVICE_TYPE] = authRequestContext_->deviceTypeId;
    jsonObj[TAG_AUTH_TYPE] = authRequestContext_->authType;
    jsonObj[TAG_TOKEN] = authRequestContext_->token;
    jsonObj[TAG_VISIBILITY] = authRequestContext_->groupVisibility;
    if (authRequestContext_->groupVisibility == GROUP_VISIBILITY_IS_PRIVATE) {
        jsonObj[TAG_TARGET] = authRequestContext_->targetPkgName;
        jsonObj[TAG_HOST] = authRequestContext_->hostPkgName;
    }
    jsonObj[TAG_APP_NAME] = authRequestContext_->appName;
    jsonObj[TAG_APP_DESCRIPTION] = authRequestContext_->appDesc;
    jsonObj[TAG_APP_ICON] = authRequestContext_->appIcon;
    jsonObj[TAG_THUMBNAIL_SIZE] = thumbnailSize;
    jsonStrVec.push_back(jsonObj.dump());

    for (int32_t idx = 0; idx < thumbnailSlice; idx++) {
        nlohmann::json jsonThumbnailObj;
        jsonThumbnailObj[TAG_VER] = DM_ITF_VER;
        jsonThumbnailObj[TAG_TYPE] = MSG_TYPE_REQ_AUTH;
        jsonThumbnailObj[TAG_SLICE_NUM] = thumbnailSlice + 1;
        jsonThumbnailObj[TAG_INDEX] = idx + 1;
        jsonThumbnailObj[TAG_DEVICE_ID] = authRequestContext_->deviceId;
        jsonThumbnailObj[TAG_THUMBNAIL_SIZE] = thumbnailSize;

        int32_t leftLen = thumbnailSize - idx * MSG_MAX_SIZE;
        int32_t sliceLen = (leftLen > MSG_MAX_SIZE) ? MSG_MAX_SIZE : leftLen;
        LOGI("TAG_APP_THUMBNAIL encode, idx %d, sliceLen %d, thumbnailSize %d", idx, (uint32_t)sliceLen, thumbnailSize);
        jsonObj[TAG_APP_THUMBNAIL] = authRequestContext_->appThumbnail.substr(idx * MSG_MAX_SIZE, sliceLen);
        jsonStrVec.push_back(jsonThumbnailObj.dump());
    }
    return jsonStrVec;
}

std::string AuthMessageProcessor::CreateSimpleMessage(int32_t msgType)
{
    LOGI("AuthMessageProcessor::CreateSimpleMessage start. msgType is %d", msgType);
    nlohmann::json jsonObj;
    jsonObj[TAG_VER] = DM_ITF_VER;
    jsonObj[TAG_TYPE] = msgType;
    switch (msgType) {
        case MSG_TYPE_NEGOTIATE:
        case MSG_TYPE_RESP_NEGOTIATE:
            CreateNegotiateMessage(jsonObj);
            break;
        case MSG_TYPE_SYNC_GROUP:
            CreateSyncGroupMessage(jsonObj);
            break;
        case MSG_TYPE_RESP_AUTH:
            CreateResponseAuthMessage(jsonObj);
            break;
        case MSG_TYPE_REQ_AUTH_TERMINATE:
            CreateResponseFinishMessage(jsonObj);
            break;
        default:
            break;
    }
    return jsonObj.dump();
}

void AuthMessageProcessor::CreateNegotiateMessage(nlohmann::json &json)
{
    if (cryptoAdapter_ == nullptr) {
        json[TAG_CRYPTO_SUPPORT] = false;
    } else {
        json[TAG_CRYPTO_SUPPORT] = true;
        json[TAG_CRYPTO_NAME] = cryptoAdapter_->GetName();
        json[TAG_CRYPTO_VERSION] = cryptoAdapter_->GetVersion();
        json[TAG_DEVICE_ID] = authResponseContext_->deviceId;
    }
    json[TAG_AUTH_TYPE] = authResponseContext_->authType;
    json[TAG_REPLY] = authResponseContext_->reply;
    json[TAG_LOCAL_DEVICE_ID] = authResponseContext_->localDeviceId;
}

void AuthMessageProcessor::CreateSyncGroupMessage(nlohmann::json &json)
{
    json[TAG_DEVICE_ID] = authRequestContext_->deviceId;
    json[TAG_GROUPIDS] = authRequestContext_->syncGroupList;
}

void AuthMessageProcessor::CreateResponseAuthMessage(nlohmann::json &json)
{
    json[TAG_REPLY] = authResponseContext_->reply;
    json[TAG_DEVICE_ID] = authResponseContext_->deviceId;
    json[TAG_TOKEN] = authResponseContext_->token;
    if (authResponseContext_->reply == 0) {
        std::string groupId = authResponseContext_->groupId;
        LOGI("AuthMessageProcessor::CreateSimpleMessage groupId %d", groupId.c_str());
        nlohmann::json jsonObject = nlohmann::json::parse(groupId, nullptr, false);
        if (jsonObject.is_discarded()) {
            LOGE("DecodeRequestAuth jsonStr error");
            return;
        }
        groupId = jsonObject[TAG_GROUP_ID];
        json[TAG_NET_ID] = authResponseContext_->networkId;
        json[TAG_REQUEST_ID] = authResponseContext_->requestId;
        json[TAG_GROUP_ID] = groupId;
        json[TAG_GROUP_NAME] = authResponseContext_->groupName;
        json[TAG_AUTH_TOKEN] = authResponseContext_->authToken;
        LOGI("AuthMessageProcessor::ParseAuthResponseMessage %s,%s", groupId.c_str(),
             authResponseContext_->groupName.c_str());
    }
}

void AuthMessageProcessor::CreateResponseFinishMessage(nlohmann::json &json)
{
    json[TAG_REPLY] = authResponseContext_->reply;
}

int32_t AuthMessageProcessor::ParseMessage(const std::string &message)
{
    LOGI("AuthMessageProcessor ParseMessage");
    nlohmann::json jsonObject = nlohmann::json::parse(message, nullptr, false);
    if (jsonObject.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    if (!jsonObject.contains(TAG_TYPE)) {
        LOGE("err json string, first time");
        return DM_FAILED;
    }
    int32_t sliceNum = 0;
    int32_t msgType = jsonObject[TAG_TYPE];
    authResponseContext_->msgType = msgType;
    LOGI("AuthMessageProcessor::ParseAuthRequestMessage======== %d", authResponseContext_->msgType);
    switch (msgType) {
        case MSG_TYPE_NEGOTIATE:
            ParseNegotiateMessage(jsonObject);
            break;
        case MSG_TYPE_REQ_AUTH:
            if (!jsonObject.contains(TAG_INDEX) || !jsonObject.contains(TAG_DEVICE_ID) ||
                !jsonObject.contains(TAG_SLICE_NUM)) {
                LOGE("err json string, first time");
                return DM_FAILED;
            }
            authResponseContext_->deviceId = jsonObject[TAG_DEVICE_ID];
            authResponseContext_->authType = jsonObject[TAG_AUTH_TYPE];
            authResponseContext_->appDesc = jsonObject[TAG_APP_DESCRIPTION];
            authResponseContext_->token = jsonObject[TAG_TOKEN];
            authResponseContext_->targetPkgName = jsonObject[TAG_TARGET];
            authResponseContext_->appName = jsonObject[TAG_APP_NAME];
            LOGI("AuthMessageProcessor::ParseAuthResponseMessage %s", authResponseContext_->deviceId.c_str());
            sliceNum = jsonObject[TAG_SLICE_NUM];
            if ((int32_t)authSplitJsonList_.size() < sliceNum) {
                authSplitJsonList_.push_back(message);
            } else {
                ParseAuthRequestMessage();
            }
            break;
        case MSG_TYPE_RESP_AUTH:
            ParseAuthResponseMessage(jsonObject);
            break;
        case MSG_TYPE_REQ_AUTH_TERMINATE:
            ParseResponseFinishMessage(jsonObject);
            break;
        default:
            break;
    }
    return DM_OK;
}

void AuthMessageProcessor::ParseResponseFinishMessage(nlohmann::json &json)
{
    authResponseContext_->reply = json[TAG_REPLY];
}

void AuthMessageProcessor::ParseAuthResponseMessage(nlohmann::json &json)
{
    LOGI("AuthMessageProcessor::ParseAuthResponseMessage ");
    authResponseContext_->reply = json[TAG_REPLY];
    authResponseContext_->deviceId = json[TAG_DEVICE_ID];
    authResponseContext_->token = json[TAG_TOKEN];
    if (authResponseContext_->reply == 0) {
        authResponseContext_->networkId = json[TAG_NET_ID];
        authResponseContext_->requestId = json[TAG_REQUEST_ID];
        authResponseContext_->groupId = json[TAG_GROUP_ID];
        authResponseContext_->groupName = json[TAG_GROUP_NAME];
        authResponseContext_->authToken = json[TAG_AUTH_TOKEN];
        LOGI("AuthMessageProcessor::ParseAuthResponseMessage %s,%s", authResponseContext_->groupId.c_str(),
             authResponseContext_->groupName.c_str());
    }
    LOGI("AuthMessageProcessor::ParseAuthResponseMessage ");
}

int32_t AuthMessageProcessor::ParseAuthRequestMessage()
{
    nlohmann::json jsonObject = authSplitJsonList_.front();
    authResponseContext_->deviceId = jsonObject[TAG_DEVICE_ID];
    authResponseContext_->reply = jsonObject[TAG_REPLY];
    authResponseContext_->authType = jsonObject[TAG_AUTH_TYPE];
    LOGI("AuthMessageProcessor::ParseAuthResponseMessage %d", authResponseContext_->reply);
    LOGI("AuthMessageProcessor::ParseAuthResponseMessage %s", authResponseContext_->deviceId.c_str());
    if (authResponseContext_->reply == AUTH_REPLY_ACCEPT) {
        authResponseContext_->networkId = jsonObject[TAG_NET_ID];
        authResponseContext_->groupId = jsonObject[TAG_GROUP_ID];
        authResponseContext_->groupName = jsonObject[TAG_GROUP_NAME];
        authResponseContext_->requestId = jsonObject[TAG_REQUEST_ID];
        return DM_FAILED;
    }
    authSplitJsonList_.clear();
    return DM_OK;
}

void AuthMessageProcessor::ParseNegotiateMessage(const nlohmann::json &json)
{
    if (json.contains(TAG_CRYPTO_SUPPORT)) {
        authResponseContext_->cryptoSupport = json[TAG_CRYPTO_SUPPORT];
    }
    if (json.contains(TAG_CRYPTO_NAME)) {
        authResponseContext_->cryptoSupport = json[TAG_CRYPTO_NAME];
    }
    if (json.contains(TAG_CRYPTO_VERSION)) {
        authResponseContext_->cryptoSupport = json[TAG_CRYPTO_VERSION];
    }
    if (json.contains(TAG_DEVICE_ID)) {
        authResponseContext_->deviceId = json[TAG_DEVICE_ID];
    }
    authResponseContext_->authType = json[TAG_AUTH_TYPE];
    authResponseContext_->localDeviceId = json[TAG_LOCAL_DEVICE_ID];
    authResponseContext_->reply = json[TAG_REPLY];
}

void AuthMessageProcessor::SetRequestContext(std::shared_ptr<DmAuthRequestContext> authRequestContext)
{
    authRequestContext_ = authRequestContext;
}

void AuthMessageProcessor::SetResponseContext(std::shared_ptr<DmAuthResponseContext> authResponseContext)
{
    authResponseContext_ = authResponseContext;
}

std::shared_ptr<DmAuthResponseContext> AuthMessageProcessor::GetResponseContext()
{
    return authResponseContext_;
}

std::shared_ptr<DmAuthRequestContext> AuthMessageProcessor::GetRequestContext()
{
    return authRequestContext_;
}
} // namespace DistributedHardware
} // namespace OHOS

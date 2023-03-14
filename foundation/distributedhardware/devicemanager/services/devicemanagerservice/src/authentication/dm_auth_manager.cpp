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

#include "dm_auth_manager.h"

#include <string>

#include "auth_message_processor.h"
#include "dm_ability_manager.h"
#include "dm_config_manager.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "dm_random.h"
#include "multiple_user_connector.h"
#include "nlohmann/json.hpp"
#include "parameter.h"
#include "show_confirm.h"

namespace OHOS {
namespace DistributedHardware {
const int32_t SESSION_CANCEL_TIMEOUT = 0;
const int32_t AUTHENTICATE_TIMEOUT = 120;
const int32_t CONFIRM_TIMEOUT = 60;
const int32_t NEGOTIATE_TIMEOUT = 10;
const int32_t INPUT_TIMEOUT = 60;
const int32_t ADD_TIMEOUT = 10;
const int32_t WAIT_NEGOTIATE_TIMEOUT = 10;
const int32_t WAIT_REQUEST_TIMEOUT = 10;
const int32_t CANCEL_PIN_CODE_DISPLAY = 1;
const int32_t DEVICE_ID_HALF = 2;

static void TimeOut(void *data, DmTimer& timer)
{
    LOGI("time out %s", timer.GetTimerName().c_str());
    if (data == nullptr || timer.GetTimerName().find(TIMER_PREFIX) != TIMER_DEFAULT) {
        LOGE("time out is not our timer");
        return;
    }

    DmAuthManager *authMgr = (DmAuthManager *)data;
    if (authMgr == nullptr) {
        LOGE("authMgr is nullptr");
        return;
    }

    authMgr->HandleAuthenticateTimeout();
}

DmAuthManager::DmAuthManager(std::shared_ptr<SoftbusConnector> softbusConnector,
                             std::shared_ptr<DeviceManagerServiceListener> listener,
                             std::shared_ptr<HiChainConnector> hiChainConnector)
    : softbusConnector_(softbusConnector), hiChainConnector_(hiChainConnector), listener_(listener)
{
    LOGI("DmAuthManager constructor");
    DmConfigManager &dmConfigManager = DmConfigManager::GetInstance();
    dmConfigManager.GetAuthAdapter(authenticationMap_);
}

DmAuthManager::~DmAuthManager()
{
    LOGI("DmAuthManager destructor");
}

int32_t DmAuthManager::AuthenticateDevice(const std::string &pkgName, int32_t authType, const std::string &deviceId,
                                          const std::string &extra)
{
    LOGI("DmAuthManager::AuthenticateDevice start auth type %d", authType);
    std::shared_ptr<IAuthentication> authentication = authenticationMap_[authType];
    if (authentication == nullptr) {
        LOGE("DmAuthManager::AuthenticateDevice authType %d not support.", authType);
        listener_->OnAuthResult(pkgName, deviceId, "", AuthState::AUTH_REQUEST_INIT, DM_AUTH_NOT_SUPPORT);
        return DM_AUTH_NOT_SUPPORT;
    }

    if (authRequestState_ != nullptr || authResponseState_ != nullptr) {
        LOGE("DmAuthManager::AuthenticateDevice %s is request authentication.", pkgName.c_str());
        listener_->OnAuthResult(pkgName, deviceId, "", AuthState::AUTH_REQUEST_INIT, DM_AUTH_BUSINESS_BUSY);
        return DM_AUTH_BUSINESS_BUSY;
    }

    if (!softbusConnector_->HaveDeviceInMap(deviceId)) {
        LOGE("AuthenticateDevice failed, the discoveryDeviceInfoMap_ not have this device");
        listener_->OnAuthResult(pkgName, deviceId, "", AuthState::AUTH_REQUEST_INIT, DM_AUTH_INPUT_FAILED);
        return DM_AUTH_INPUT_FAILED;
    }
    if (extra.empty()) {
        LOGE("AuthenticateDevice failed, extra is empty");
        listener_->OnAuthResult(pkgName, deviceId, "", AuthState::AUTH_REQUEST_INIT, DM_INPUT_PARA_EMPTY);
        return DM_INPUT_PARA_EMPTY;
    }

    authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(shared_from_this());
    authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authRequestContext_->hostPkgName = pkgName;
    authRequestContext_->authType = authType;
    authRequestContext_->deviceId = deviceId;
    nlohmann::json jsonObject = nlohmann::json::parse(extra, nullptr, false);
    if (!jsonObject.is_discarded()) {
        if (jsonObject.contains(TARGET_PKG_NAME_KEY)) {
            authRequestContext_->targetPkgName = jsonObject[TARGET_PKG_NAME_KEY];
        }
        if (jsonObject.contains(APP_NAME_KEY)) {
            authRequestContext_->appName = jsonObject[APP_NAME_KEY];
        }
        if (jsonObject.contains(APP_DESCRIPTION_KEY)) {
            authRequestContext_->appDesc = jsonObject[APP_DESCRIPTION_KEY];
        }
        if (jsonObject.contains(APP_THUMBNAIL)) {
            authRequestContext_->appThumbnail = jsonObject[APP_THUMBNAIL];
        }
        if (jsonObject.contains(APP_ICON_KEY)) {
            authRequestContext_->appIcon = jsonObject[APP_ICON_KEY];
        }
    }
    authRequestContext_->token = std::to_string(GenRandInt(MIN_PIN_TOKEN, MAX_PIN_TOKEN));
    authMessageProcessor_->SetRequestContext(authRequestContext_);
    authRequestState_ = std::make_shared<AuthRequestInitState>();
    authRequestState_->SetAuthManager(shared_from_this());
    authRequestState_->SetAuthContext(authRequestContext_);
    authRequestState_->Enter();
    std::shared_ptr<DmTimer> authenticateStartTimer = std::make_shared<DmTimer>(AUTHENTICATE_TIMEOUT_TASK);
    timerMap_[AUTHENTICATE_TIMEOUT_TASK] = authenticateStartTimer;
    authenticateStartTimer->Start(AUTHENTICATE_TIMEOUT, TimeOut, this);
    LOGI("DmAuthManager::AuthenticateDevice complete");
    return DM_OK;
}

int32_t DmAuthManager::UnAuthenticateDevice(const std::string &pkgName, const std::string &deviceId)
{
    if (pkgName.empty()) {
        LOGE(" DmAuthManager::UnAuthenticateDevice failed pkgName is null");
        return DM_FAILED;
    }
    std::string deviceUdid;
    int32_t ret = SoftbusConnector::GetUdidByNetworkId(deviceId.c_str(), deviceUdid);
    if (ret != DM_OK) {
        LOGE("UnAuthenticateDevice GetNodeKeyInfo failed");
        return DM_FAILED;
    }

    std::string groupId = "";
    std::vector<OHOS::DistributedHardware::GroupInfo> groupList;
    hiChainConnector_->GetRelatedGroups(deviceUdid, groupList);
    if (groupList.size() > 0) {
        groupId = groupList.front().groupId;
        LOGI(" DmAuthManager::UnAuthenticateDevice groupId=%s, deviceId=%s, deviceUdid=%s", groupId.c_str(),
             deviceId.c_str(), deviceUdid.c_str());
        hiChainConnector_->DeleteGroup(groupId);
    } else {
        LOGE("DmAuthManager::UnAuthenticateDevice groupList.size = 0");
        return DM_FAILED;
    }
    return DM_OK;
}

int32_t DmAuthManager::VerifyAuthentication(const std::string &authParam)
{
    LOGI("DmAuthManager::VerifyAuthentication");
    if (authResponseContext_ == nullptr) {
        LOGE("authResponseContext_ is not init");
        return DM_AUTH_NOT_START;
    }
    std::shared_ptr<IAuthentication> ptr;
    if (authenticationMap_.find(authResponseContext_->authType) == authenticationMap_.end()
        || timerMap_.find(INPUT_TIMEOUT_TASK) == timerMap_.end()) {
        LOGE("DmAuthManager::authenticationMap_ is null");
        return DM_FAILED;
    }
    timerMap_[INPUT_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
    ptr = authenticationMap_[authResponseContext_->authType];
    int32_t ret = ptr->VerifyAuthentication(authResponseContext_->authToken, authParam);
    switch (ret) {
        case DM_OK:
            authRequestState_->TransitionTo(std::make_shared<AuthRequestJoinState>());
            break;
        case DM_AUTH_INPUT_FAILED:
            listener_->OnVerifyAuthResult(authRequestContext_->hostPkgName, authRequestContext_->deviceId,
                                          DM_AUTH_INPUT_FAILED, "");
            break;
        default:
            authRequestContext_->reason = DM_AUTH_INPUT_FAILED;
            authResponseContext_->state = authRequestState_->GetStateType();
            authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
            break;
    }
    LOGI("DmAuthManager::VerifyAuthentication complete");
    return DM_OK;
}

void DmAuthManager::OnSessionOpened(int32_t sessionId, int32_t sessionSide, int32_t result)
{
    LOGI("DmAuthManager::OnSessionOpened sessionId=%d result=%d", sessionId, result);
    if (sessionSide == AUTH_SESSION_SIDE_SERVER) {
        if (authResponseState_ == nullptr && authRequestState_ == nullptr) {
            authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(shared_from_this());
            authResponseState_ = std::make_shared<AuthResponseInitState>();
            authResponseState_->SetAuthManager(shared_from_this());
            authResponseState_->Enter();
            authResponseContext_ = std::make_shared<DmAuthResponseContext>();
            std::shared_ptr<DmTimer> waitStartTimer = std::make_shared<DmTimer>(WAIT_NEGOTIATE_TIMEOUT_TASK);
            timerMap_[WAIT_NEGOTIATE_TIMEOUT_TASK] = waitStartTimer;
            waitStartTimer->Start(WAIT_NEGOTIATE_TIMEOUT, TimeOut, this);
            std::shared_ptr<DmTimer> authenticateStartTimer = std::make_shared<DmTimer>(AUTHENTICATE_TIMEOUT_TASK);
            timerMap_[AUTHENTICATE_TIMEOUT_TASK] = authenticateStartTimer;
            authenticateStartTimer->Start(AUTHENTICATE_TIMEOUT, TimeOut, this);
        } else {
            std::shared_ptr<AuthMessageProcessor> authMessageProcessor =
                std::make_shared<AuthMessageProcessor>(shared_from_this());
            std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
            authResponseContext->reply = DM_AUTH_BUSINESS_BUSY;
            authMessageProcessor->SetResponseContext(authResponseContext);
            std::string message = authMessageProcessor->CreateSimpleMessage(MSG_TYPE_REQ_AUTH_TERMINATE);
            softbusConnector_->GetSoftbusSession()->SendData(sessionId, message);
        }
    } else {
        if (authResponseState_ == nullptr && authRequestState_ != nullptr &&
            authRequestState_->GetStateType() == AuthState::AUTH_REQUEST_INIT) {
            authRequestContext_->sessionId = sessionId;
            authRequestState_->SetAuthContext(authRequestContext_);
            authMessageProcessor_->SetRequestContext(authRequestContext_);
            authRequestState_->TransitionTo(std::make_shared<AuthRequestNegotiateState>());
        } else {
            LOGE("DmAuthManager::OnSessionOpened but request state is wrong");
        }
    }
}

void DmAuthManager::OnSessionClosed(const int32_t sessionId)
{
    LOGI("DmAuthManager::OnSessionOpened sessionId=%d", sessionId);
}

void DmAuthManager::OnDataReceived(const int32_t sessionId, const std::string message)
{
    if (authResponseContext_ == nullptr || authMessageProcessor_ == nullptr) {
        LOGE("OnDataReceived failed, authResponseContext or authMessageProcessor_ is nullptr.");
        return;
    }

    authResponseContext_->sessionId = sessionId;
    authMessageProcessor_->SetResponseContext(authResponseContext_);
    int32_t ret = authMessageProcessor_->ParseMessage(message);
    if (ret != DM_OK) {
        LOGE("OnDataReceived failed, parse input message error.");
        return;
    }

    if ((authRequestState_ != nullptr) && (authResponseState_ == nullptr)) {
        // source device auth process
        authRequestContext_ = authMessageProcessor_->GetRequestContext();
        authRequestState_->SetAuthContext(authRequestContext_);
        LOGI("OnDataReceived for source device, authResponseContext msgType=%d, authRequestState stateType=%d",
            authResponseContext_->msgType, authRequestState_->GetStateType());

        switch (authResponseContext_->msgType) {
            case MSG_TYPE_RESP_AUTH:
                if (authRequestState_->GetStateType() == AuthState::AUTH_REQUEST_NEGOTIATE_DONE) {
                    authRequestState_->TransitionTo(std::make_shared<AuthRequestReplyState>());
                }
                break;
            case MSG_TYPE_RESP_NEGOTIATE:
                if (authRequestState_->GetStateType() == AuthState::AUTH_REQUEST_NEGOTIATE) {
                    authRequestState_->TransitionTo(std::make_shared<AuthRequestNegotiateDoneState>());
                }
                break;
            case MSG_TYPE_REQ_AUTH_TERMINATE:
                if (authRequestState_->GetStateType() != AuthState::AUTH_REQUEST_FINISH) {
                    isFinishOfLocal_ = false;
                    authResponseContext_->state = authRequestState_->GetStateType();
                    authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
                }
                break;
            default:
                break;
        }
    } else if ((authResponseState_ != nullptr) && (authRequestState_ == nullptr)) {
        // sink device auth process
        authResponseState_->SetAuthContext(authResponseContext_);
        LOGI("OnDataReceived for sink device, authResponseContext msgType=%d, authResponseState stateType=%d",
            authResponseContext_->msgType, authResponseState_->GetStateType());

        switch (authResponseContext_->msgType) {
            case MSG_TYPE_NEGOTIATE:
                if (authResponseState_->GetStateType() == AuthState::AUTH_RESPONSE_INIT
                    && timerMap_.find(WAIT_NEGOTIATE_TIMEOUT_TASK) != timerMap_.end()) {
                    timerMap_[WAIT_NEGOTIATE_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
                    authResponseState_->TransitionTo(std::make_shared<AuthResponseNegotiateState>());
                } else {
                    LOGE("Device manager auth state error");
                }
                break;
            case MSG_TYPE_REQ_AUTH:
                if (authResponseState_->GetStateType() == AuthState::AUTH_RESPONSE_NEGOTIATE
                    && timerMap_.find(WAIT_REQUEST_TIMEOUT_TASK) != timerMap_.end()) {
                    timerMap_[WAIT_REQUEST_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
                    authResponseState_->TransitionTo(std::make_shared<AuthResponseConfirmState>());
                } else {
                    LOGE("Device manager auth state error");
                }
                break;
            case MSG_TYPE_REQ_AUTH_TERMINATE:
                if (authResponseState_->GetStateType() != AuthState::AUTH_RESPONSE_FINISH) {
                    isFinishOfLocal_ = false;
                    authResponseState_->TransitionTo(std::make_shared<AuthResponseFinishState>());
                }
                break;
            default:
                break;
        }
    } else {
        LOGE("DmAuthManager::OnDataReceived failed, authRequestState_ or authResponseState_ is invalid.");
    }
}

void DmAuthManager::OnGroupCreated(int64_t requestId, const std::string &groupId)
{
    LOGI("DmAuthManager::OnGroupCreated start");
    if (authResponseState_ == nullptr) {
        LOGE("DmAuthManager::AuthenticateDevice end");
        return;
    }
    if (groupId == "{}") {
        authResponseContext_->reply = DM_HICHAIN_GROUP_CREATE_FAILED;
        authMessageProcessor_->SetResponseContext(authResponseContext_);
        std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_RESP_AUTH);
        softbusConnector_->GetSoftbusSession()->SendData(authResponseContext_->sessionId, message);
        return;
    }

    int32_t pinCode = GeneratePincode();
    nlohmann::json jsonObj;
    jsonObj[PIN_CODE_KEY] = pinCode;
    jsonObj[PIN_TOKEN] = authResponseContext_->token;
    jsonObj[QR_CODE_KEY] = GenerateGroupName();
    jsonObj[NFC_CODE_KEY] = GenerateGroupName();
    authResponseContext_->authToken = jsonObj.dump();
    LOGI("DmAuthManager::AddMember start %s", authResponseContext_->authToken.c_str());
    authResponseContext_->groupId = groupId;
    authResponseContext_->code = pinCode;
    authMessageProcessor_->SetResponseContext(authResponseContext_);
    std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_RESP_AUTH);
    softbusConnector_->GetSoftbusSession()->SendData(authResponseContext_->sessionId, message);
    authResponseState_->TransitionTo(std::make_shared<AuthResponseShowState>());
}

void DmAuthManager::OnMemberJoin(int64_t requestId, int32_t status)
{
    LOGI("DmAuthManager OnMemberJoin start");
    if (authRequestState_ != nullptr && timerMap_.find(ADD_TIMEOUT_TASK) != timerMap_.end()) {
        timerMap_[ADD_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
        if (status != DM_OK || authResponseContext_->requestId != requestId) {
            if (authRequestState_ != nullptr) {
                authResponseContext_->state = AuthState::AUTH_REQUEST_JOIN;
                authRequestContext_->reason = DM_AUTH_INPUT_FAILED;
                authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
                return;
            }
        }
        authRequestState_->TransitionTo(std::make_shared<AuthRequestNetworkState>());
    }
}

int32_t DmAuthManager::HandleAuthenticateTimeout()
{
    LOGI("DmAuthManager::HandleAuthenticateTimeout start");
    if (authRequestState_ != nullptr && authRequestState_->GetStateType() != AuthState::AUTH_REQUEST_FINISH) {
        if (authResponseContext_ == nullptr) {
            authResponseContext_ = std::make_shared<DmAuthResponseContext>();
        }
        authResponseContext_->state = authRequestState_->GetStateType();
        authRequestContext_->reason = DM_TIME_OUT;
        authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
    }

    if (authResponseState_ != nullptr && authResponseState_->GetStateType() != AuthState::AUTH_RESPONSE_FINISH) {
        authResponseContext_->state = authResponseState_->GetStateType();
        authResponseContext_->reply = DM_TIME_OUT;
        authResponseState_->TransitionTo(std::make_shared<AuthResponseFinishState>());
    }
    LOGI("DmAuthManager::HandleAuthenticateTimeout start complete");
    return DM_OK;
}

int32_t DmAuthManager::EstablishAuthChannel(const std::string &deviceId)
{
    int32_t sessionId = softbusConnector_->GetSoftbusSession()->OpenAuthSession(deviceId);
    if (sessionId < 0) {
        LOGE("OpenAuthSession failed, stop the authentication");
        authResponseContext_ = std::make_shared<DmAuthResponseContext>();
        authResponseContext_->state = AuthState::AUTH_REQUEST_NEGOTIATE;
        authRequestContext_->reason = DM_AUTH_OPEN_SESSION_FAILED;
        authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
    }
    return DM_OK;
}

void DmAuthManager::StartNegotiate(const int32_t &sessionId)
{
    LOGI("DmAuthManager::EstablishAuthChannel session id is %d", sessionId);
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    authResponseContext_->localDeviceId = localDeviceId;
    authResponseContext_->reply = DM_AUTH_NOT_AUTH;
    authResponseContext_->authType = authRequestContext_->authType;
    authResponseContext_->deviceId = authRequestContext_->deviceId;
    authMessageProcessor_->SetResponseContext(authResponseContext_);
    std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_NEGOTIATE);
    softbusConnector_->GetSoftbusSession()->SendData(sessionId, message);
    std::shared_ptr<DmTimer> negotiateStartTimer = std::make_shared<DmTimer>(NEGOTIATE_TIMEOUT_TASK);
    timerMap_[NEGOTIATE_TIMEOUT_TASK] = negotiateStartTimer;
    negotiateStartTimer->Start(NEGOTIATE_TIMEOUT, TimeOut, this);
}

void DmAuthManager::RespNegotiate(const int32_t &sessionId)
{
    LOGI("DmAuthManager::EstablishAuthChannel session id is %d", sessionId);
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    bool ret = hiChainConnector_->IsDevicesInGroup(authResponseContext_->localDeviceId, localDeviceId);
    if (!ret) {
        LOGE("DmAuthManager::EstablishAuthChannel device is in group");
        authResponseContext_->reply = DM_AUTH_PEER_REJECT;
    } else {
        authResponseContext_->reply = DM_AUTH_NOT_AUTH;
    }

    std::shared_ptr<IAuthentication> authentication = authenticationMap_[authResponseContext_->authType];
    if (authentication == nullptr) {
        LOGE("DmAuthManager::AuthenticateDevice authType %d not support.", authResponseContext_->authType);
        authResponseContext_->reply = DM_AUTH_NOT_SUPPORT;
    }

    std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_RESP_NEGOTIATE);
    nlohmann::json jsonObject = nlohmann::json::parse(message, nullptr, false);
    if (jsonObject.is_discarded()) {
        softbusConnector_->GetSoftbusSession()->SendData(sessionId, message);
    }
    authResponseContext_ = authResponseState_->GetAuthContext();
    if (jsonObject[TAG_CRYPTO_SUPPORT] == "true" && authResponseContext_->cryptoSupport) {
        if (jsonObject[TAG_CRYPTO_NAME] == authResponseContext_->cryptoName &&
            jsonObject[TAG_CRYPTO_VERSION] == authResponseContext_->cryptoVer) {
            isCryptoSupport_ = true;
            softbusConnector_->GetSoftbusSession()->SendData(sessionId, message);
            return;
        }
    }
    jsonObject[TAG_CRYPTO_SUPPORT] = "false";
    message = jsonObject.dump();
    softbusConnector_->GetSoftbusSession()->SendData(sessionId, message);
    std::shared_ptr<DmTimer> waitStartTimer = std::make_shared<DmTimer>(WAIT_REQUEST_TIMEOUT_TASK);
    timerMap_[WAIT_REQUEST_TIMEOUT_TASK] = waitStartTimer;
    waitStartTimer->Start(WAIT_REQUEST_TIMEOUT, TimeOut, this);
}

void DmAuthManager::SendAuthRequest(const int32_t &sessionId)
{
    LOGI("DmAuthManager::EstablishAuthChannel session id");
    if (timerMap_.find(NEGOTIATE_TIMEOUT_TASK) == timerMap_.end()) {
        return;
    }
    timerMap_[NEGOTIATE_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
    if (authResponseContext_->cryptoSupport) {
        isCryptoSupport_ = true;
    }
    if (authResponseContext_->reply == DM_AUTH_PEER_REJECT) {
        authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
        return;
    }
    std::vector<std::string> messageList = authMessageProcessor_->CreateAuthRequestMessage();
    for (auto msg : messageList) {
        softbusConnector_->GetSoftbusSession()->SendData(sessionId, msg);
    }
    std::shared_ptr<DmTimer> confirmStartTimer = std::make_shared<DmTimer>(CONFIRM_TIMEOUT_TASK);
    timerMap_[CONFIRM_TIMEOUT_TASK] = confirmStartTimer;
    confirmStartTimer->Start(CONFIRM_TIMEOUT, TimeOut, this);
}

int32_t DmAuthManager::StartAuthProcess(const int32_t &action)
{
    LOGI("DmAuthManager:: StartAuthProcess");
    if (authResponseContext_ == nullptr) {
        LOGI("Authenticate is not start");
        return DM_AUTH_NOT_START;
    }

    authResponseContext_->reply = action;
    if (authResponseContext_->reply == USER_OPERATION_TYPE_ALLOW_AUTH &&
        authResponseState_->GetStateType() == AuthState::AUTH_RESPONSE_CONFIRM) {
        authResponseState_->TransitionTo(std::make_shared<AuthResponseGroupState>());
    } else {
        authMessageProcessor_->SetResponseContext(authResponseContext_);
        std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_RESP_AUTH);
        softbusConnector_->GetSoftbusSession()->SendData(authResponseContext_->sessionId, message);
    }
    return DM_OK;
}

void DmAuthManager::StartRespAuthProcess()
{
    LOGI("DmAuthManager::StartRespAuthProcess", authResponseContext_->sessionId);
    if (timerMap_.find(CONFIRM_TIMEOUT_TASK) == timerMap_.end()) {
        return;
    }

    timerMap_[CONFIRM_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
    if (authResponseContext_->reply == USER_OPERATION_TYPE_ALLOW_AUTH) {
        std::shared_ptr<DmTimer> inputStartTimer = std::make_shared<DmTimer>(INPUT_TIMEOUT_TASK);
        timerMap_[INPUT_TIMEOUT_TASK] = inputStartTimer;
        inputStartTimer->Start(INPUT_TIMEOUT, TimeOut, this);
        authRequestState_->TransitionTo(std::make_shared<AuthRequestInputState>());
    } else {
        LOGE("do not accept");
        authResponseContext_->state = AuthState::AUTH_REQUEST_REPLY;
        authRequestContext_->reason = DM_AUTH_PEER_REJECT;
        authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
    }
}

int32_t DmAuthManager::CreateGroup()
{
    LOGI("DmAuthManager:: CreateGroup");
    authResponseContext_->groupName = GenerateGroupName();
    authResponseContext_->requestId = GenRandLongLong(MIN_REQUEST_ID, MAX_REQUEST_ID);
    hiChainConnector_->CreateGroup(authResponseContext_->requestId, authResponseContext_->groupName);
    return DM_OK;
}

int32_t DmAuthManager::AddMember(const std::string &deviceId)
{
    LOGI("DmAuthManager::AddMember start");
    nlohmann::json jsonObj = nlohmann::json::parse(authResponseContext_->authToken, nullptr, false);
    if (jsonObj.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }

    LOGI("DmAuthManager::AddMember start %s", authResponseContext_->authToken.c_str());
    nlohmann::json jsonObject;
    jsonObject[TAG_GROUP_ID] = authResponseContext_->groupId;
    jsonObject[TAG_GROUP_NAME] = authResponseContext_->groupName;
    jsonObject[PIN_CODE_KEY] = jsonObj[PIN_CODE_KEY];
    jsonObject[TAG_REQUEST_ID] = authResponseContext_->requestId;
    jsonObject[TAG_DEVICE_ID] = authResponseContext_->deviceId;
    std::string connectInfo = jsonObject.dump();
    std::shared_ptr<DmTimer> joinStartTimer = std::make_shared<DmTimer>(ADD_TIMEOUT_TASK);
    timerMap_[ADD_TIMEOUT_TASK] = joinStartTimer;
    joinStartTimer->Start(ADD_TIMEOUT, TimeOut, this);
    int32_t ret = hiChainConnector_->AddMember(deviceId, connectInfo);
    if (ret != 0) {
        return DM_FAILED;
    }
    LOGI("DmAuthManager::authRequestContext CancelDisplay start");
    std::shared_ptr<IAuthentication> ptr;
    if (authenticationMap_.find(authResponseContext_->authType) == authenticationMap_.end()) {
        LOGE("DmAuthManager::authenticationMap_ is null");
        return DM_FAILED;
    }
    ptr = authenticationMap_[authResponseContext_->authType];
    ptr->CloseAuthInfo(authResponseContext_->pageId, shared_from_this());
    return DM_OK;
}

std::string DmAuthManager::GetConnectAddr(std::string deviceId)
{
    LOGI("DmAuthManager::GetConnectAddr");
    std::string connectAddr;
    softbusConnector_->GetConnectAddr(deviceId, connectAddr);
    return connectAddr;
}

int32_t DmAuthManager::JoinNetwork()
{
    LOGI("DmAuthManager JoinNetwork start");
    if (timerMap_.find(AUTHENTICATE_TIMEOUT_TASK) == timerMap_.end()) {
        return DM_FAILED;
    }
    timerMap_[AUTHENTICATE_TIMEOUT_TASK]->Stop(SESSION_CANCEL_TIMEOUT);
    authResponseContext_->state = AuthState::AUTH_REQUEST_FINISH;
    authRequestContext_->reason = DM_OK;
    authRequestState_->TransitionTo(std::make_shared<AuthRequestFinishState>());
    return DM_OK;
}

void DmAuthManager::AuthenticateFinish()
{
    LOGI("DmAuthManager::AuthenticateFinish start");
    if (authResponseState_ != nullptr) {
        if (authResponseState_->GetStateType() == AuthState::AUTH_RESPONSE_FINISH) {
            std::shared_ptr<IAuthentication> ptr;
            if (authenticationMap_.find(authResponseContext_->authType) == authenticationMap_.end()) {
                LOGE("DmAuthManager::authenticationMap_ is null");
                return ;
            }
            ptr = authenticationMap_[authResponseContext_->authType];
            ptr->CloseAuthInfo(authResponseContext_->pageId, shared_from_this());
        }
        if (isFinishOfLocal_) {
            authMessageProcessor_->SetResponseContext(authResponseContext_);
            std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_REQ_AUTH_TERMINATE);
            softbusConnector_->GetSoftbusSession()->SendData(authResponseContext_->sessionId, message);
        }
        if (!timerMap_.empty()) {
            for (auto &iter : timerMap_) {
                iter.second->Stop(SESSION_CANCEL_TIMEOUT);
            }
            timerMap_.clear();
        }
        isFinishOfLocal_ = true;
        authResponseContext_ = nullptr;
        authResponseState_ = nullptr;
        authMessageProcessor_ = nullptr;
    } else if (authRequestState_ != nullptr) {
        if (isFinishOfLocal_) {
            authMessageProcessor_->SetResponseContext(authResponseContext_);
            std::string message = authMessageProcessor_->CreateSimpleMessage(MSG_TYPE_REQ_AUTH_TERMINATE);
            softbusConnector_->GetSoftbusSession()->SendData(authResponseContext_->sessionId, message);
        } else {
            authRequestContext_->reason = authResponseContext_->reply;
        }
        if (authResponseContext_->state == AuthState::AUTH_REQUEST_INPUT) {
            std::shared_ptr<IAuthentication> ptr;
            if (authenticationMap_.find(authResponseContext_->authType) == authenticationMap_.end()) {
                LOGE("DmAuthManager::authenticationMap_ is null");
                return ;
            }
            ptr = authenticationMap_[authResponseContext_->authType];
            ptr->CloseAuthInfo(authResponseContext_->pageId, shared_from_this());
        }
        listener_->OnAuthResult(authRequestContext_->hostPkgName, authRequestContext_->deviceId,
                                authRequestContext_->token, authResponseContext_->state, authRequestContext_->reason);
        softbusConnector_->GetSoftbusSession()->CloseAuthSession(authRequestContext_->sessionId);
        if (!timerMap_.empty()) {
            for (auto &iter : timerMap_) {
                iter.second->Stop(SESSION_CANCEL_TIMEOUT);
            }
            timerMap_.clear();
        }
        isFinishOfLocal_ = true;
        authRequestContext_ = nullptr;
        authResponseContext_ = nullptr;
        authRequestState_ = nullptr;
        authMessageProcessor_ = nullptr;
    }
    LOGI("DmAuthManager::AuthenticateFinish complete");
}

void DmAuthManager::CancelDisplay()
{
    LOGI("DmAuthManager::CancelDisplay start");
    nlohmann::json jsonObj;
    jsonObj[CANCEL_DISPLAY_KEY] = CANCEL_PIN_CODE_DISPLAY;
    std::string paramJson = jsonObj.dump();
    std::string pkgName = "com.ohos.devicemanagerui";
    listener_->OnFaCall(pkgName, paramJson);
}

int32_t DmAuthManager::GeneratePincode()
{
    return GenRandInt(MIN_PIN_CODE, MAX_PIN_CODE);
}

std::string DmAuthManager::GenerateGroupName()
{
    char localDeviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, DEVICE_UUID_LENGTH);
    std::string sLocalDeviceId = localDeviceId;
    std::string groupName = authResponseContext_->targetPkgName + authResponseContext_->hostPkgName +
                            sLocalDeviceId.substr(0, sLocalDeviceId.size() / DEVICE_ID_HALF);
    return groupName;
}

bool DmAuthManager::GetIsCryptoSupport()
{
    LOGI("DmAuthManager::GetIsCryptoSupport start");
    if (authResponseState_ == nullptr) {
        return false;
    }
    if (authRequestState_ == nullptr) {
        if (authResponseState_->GetStateType() == AuthState::AUTH_REQUEST_NEGOTIATE_DONE) {
            return false;
        }
    } else {
        if (authRequestState_->GetStateType() == AuthState::AUTH_REQUEST_NEGOTIATE ||
            authRequestState_->GetStateType() == AuthState::AUTH_REQUEST_NEGOTIATE_DONE) {
            return false;
        }
    }

    return isCryptoSupport_;
}

int32_t DmAuthManager::SetAuthRequestState(std::shared_ptr<AuthRequestState> authRequestState)
{
    authRequestState_ = authRequestState;
    return DM_OK;
}

int32_t DmAuthManager::SetAuthResponseState(std::shared_ptr<AuthResponseState> authResponseState)
{
    authResponseState_ = authResponseState;
    return DM_OK;
}

int32_t DmAuthManager::GetPinCode()
{
    nlohmann::json jsonObj = nlohmann::json::parse(authResponseContext_->authToken, nullptr, false);
    if (jsonObj.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    return jsonObj[PIN_CODE_KEY];
}

void DmAuthManager::ShowConfigDialog()
{
    LOGI("ShowConfigDialog start");
    dmAbilityMgr_ = std::make_shared<DmAbilityManager>();
    nlohmann::json jsonObj;
    jsonObj[TAG_AUTH_TYPE] = AUTH_TYPE_PIN;
    jsonObj[TAG_TOKEN] = authResponseContext_->token;
    jsonObj[TARGET_PKG_NAME_KEY] = authResponseContext_->targetPkgName;
    jsonObj.dump();
    const std::string params = jsonObj.dump();
    std::shared_ptr<ShowConfirm> showConfirm_ = std::make_shared<ShowConfirm>();
    showConfirm_->ShowConfirmDialog(params, shared_from_this(), dmAbilityMgr_);
    LOGI("ShowConfigDialog end");
}

void DmAuthManager::ShowAuthInfoDialog()
{
    LOGI("DmAuthManager::ShowAuthInfoDialog start");
    std::shared_ptr<IAuthentication> ptr;
    if (authenticationMap_.find(authResponseContext_->authType) == authenticationMap_.end()) {
        LOGE("DmAuthManager::authenticationMap_ is null");
        return;
    }
    ptr = authenticationMap_[authResponseContext_->authType];
    LOGI("ShowAuthInfoDialog authToken:%s", authResponseContext_->authToken.c_str());
    ptr->ShowAuthInfo(authResponseContext_->authToken, shared_from_this());
}

void DmAuthManager::ShowStartAuthDialog()
{
    LOGI("DmAuthManager::ShowStartAuthDialog start");
    std::shared_ptr<IAuthentication> ptr;
    if (authenticationMap_.find(authResponseContext_->authType) == authenticationMap_.end()) {
        LOGE("DmAuthManager::authenticationMap_ is null");
        return;
    }
    ptr = authenticationMap_[authResponseContext_->authType];
    ptr->StartAuth(authResponseContext_->authToken, shared_from_this());
}

int32_t DmAuthManager::GetAuthenticationParam(DmAuthParam &authParam)
{
    if (dmAbilityMgr_ == nullptr) {
        LOGE("dmAbilityMgr_ is nullptr");
        return DM_POINT_NULL;
    }

    if (authResponseContext_ == nullptr) {
        LOGE("Authenticate is not start");
        return DM_AUTH_NOT_START;
    }
    
    dmAbilityMgr_->StartAbilityDone();
    AbilityRole role = dmAbilityMgr_->GetAbilityRole();
    authParam.direction = (int32_t)role;
    authParam.authType = AUTH_TYPE_PIN;
    authParam.authToken = authResponseContext_->token;

    if (role == AbilityRole::ABILITY_ROLE_PASSIVE) {
        authParam.packageName = authResponseContext_->targetPkgName;
        authParam.appName = authResponseContext_->appName;
        authParam.appDescription = authResponseContext_->appDesc;
        authParam.business = BUSINESS_FA_MIRGRATION;
        authParam.pincode = authResponseContext_->code;
    }
    return DM_OK;
}

int32_t DmAuthManager::OnUserOperation(int32_t action)
{
    if (authResponseContext_ == nullptr) {
        LOGE("Authenticate is not start");
        return DM_AUTH_NOT_START;
    }

    switch (action) {
        case USER_OPERATION_TYPE_ALLOW_AUTH:
        case USER_OPERATION_TYPE_CANCEL_AUTH:
            StartAuthProcess(action);
            break;
        case USER_OPERATION_TYPE_AUTH_CONFIRM_TIMEOUT:
            AuthenticateFinish();
            break;
        case USER_OPERATION_TYPE_CANCEL_PINCODE_DISPLAY:
            CancelDisplay();
            break;
        case USER_OPERATION_TYPE_CANCEL_PINCODE_INPUT:
            authRequestContext_->reason = DM_AUTH_DONT_AUTH;
            authResponseContext_->state = authRequestState_->GetStateType();
            AuthenticateFinish();
            break;
        default:
            LOGE("this action id not support");
            break;
    }
    return DM_OK;
}

void DmAuthManager::UserSwitchEventCallback (int32_t userId)
{
    LOGI("switch user event happen and this user groups will be deleted with userId: %d", userId);
    nlohmann::json jsonObj;
    jsonObj[FIELD_GROUP_TYPE] = GROUP_TYPE_PEER_TO_PEER_GROUP;
    std::string queryParams = jsonObj.dump();
    std::vector<GroupInfo> groupList;

    int32_t oldUserId = MultipleUserConnector::GetSwitchOldUserId();
    MultipleUserConnector::SetSwitchOldUserId(userId);
    if (!hiChainConnector_->GetGroupInfo(oldUserId, queryParams, groupList)) {
        LOGE("failed to get device join groups");
        return;
    }
    for (auto iter = groupList.begin(); iter != groupList.end(); iter++) {
        int32_t ret = hiChainConnector_->DeleteGroup(oldUserId, iter->groupId);
        if (ret != DM_OK) {
            LOGE("fail to delete group");
        }
    }

    if (!hiChainConnector_->GetGroupInfo(userId, queryParams, groupList)) {
        LOGE("failed to get device join groups");
        return;
    }
    for (auto iter = groupList.begin(); iter != groupList.end(); iter++) {
        int32_t ret = hiChainConnector_->DeleteGroup(userId, iter->groupId);
        if (ret != DM_OK) {
            LOGE("fail to delete group");
        }
    }
}

int32_t DmAuthManager::SetPageId(int32_t pageId)
{
    if (authResponseContext_ == nullptr) {
        LOGE("Authenticate is not start");
        return DM_AUTH_NOT_START;
    }
    authResponseContext_->pageId = pageId;
    return DM_OK;
}

int32_t DmAuthManager::SetReason(int32_t reason, int32_t state)
{
    if (authResponseContext_ == nullptr) {
        LOGE("Authenticate is not start");
        return DM_AUTH_NOT_START;
    }
    if (state < AuthState::AUTH_REQUEST_FINISH) {
        authRequestContext_->reason = reason;
    }
    authResponseContext_->state = state;
    authResponseContext_->reply = reason;
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS

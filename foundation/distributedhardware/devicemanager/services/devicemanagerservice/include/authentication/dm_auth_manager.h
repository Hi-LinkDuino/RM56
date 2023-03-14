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

#ifndef OHOS_DM_AUTH_MANAGER_H
#define OHOS_DM_AUTH_MANAGER_H

#include <map>
#include <string>

#include "auth_request_state.h"
#include "auth_response_state.h"
#include "authentication.h"
#include "device_manager_service_listener.h"
#include "dm_ability_manager.h"
#include "dm_adapter_manager.h"
#include "dm_constants.h"
#include "dm_device_info.h"
#include "dm_timer.h"
#include "hichain_connector.h"
#include "softbus_connector.h"
#include "softbus_session.h"

namespace OHOS {
namespace DistributedHardware {
typedef enum AuthState {
    AUTH_REQUEST_INIT = 1,
    AUTH_REQUEST_NEGOTIATE,
    AUTH_REQUEST_NEGOTIATE_DONE,
    AUTH_REQUEST_REPLY,
    AUTH_REQUEST_INPUT,
    AUTH_REQUEST_JOIN,
    AUTH_REQUEST_NETWORK,
    AUTH_REQUEST_FINISH,
    AUTH_RESPONSE_INIT = 20,
    AUTH_RESPONSE_NEGOTIATE,
    AUTH_RESPONSE_CONFIRM,
    AUTH_RESPONSE_GROUP,
    AUTH_RESPONSE_SHOW,
    AUTH_RESPONSE_FINISH,
} AuthState;

enum DmMsgType : int32_t {
    MSG_TYPE_UNKNOWN = 0,
    MSG_TYPE_NEGOTIATE = 80,
    MSG_TYPE_RESP_NEGOTIATE = 90,
    MSG_TYPE_REQ_AUTH = 100,
    MSG_TYPE_INVITE_AUTH_INFO = 102,
    MSG_TYPE_REQ_AUTH_TERMINATE = 104,
    MSG_TYPE_RESP_AUTH = 200,
    MSG_TYPE_JOIN_AUTH_INFO = 201,
    MSG_TYPE_RESP_AUTH_TERMINATE = 205,
    MSG_TYPE_CHANNEL_CLOSED = 300,
    MSG_TYPE_SYNC_GROUP = 400,
    MSG_TYPE_AUTH_BY_PIN = 500,
};

typedef struct DmAuthRequestContext {
    int32_t authType;
    std::string localDeviceId;
    std::string deviceId;
    std::string deviceName;
    std::string deviceTypeId;
    int32_t sessionId;
    int32_t groupVisibility;
    bool cryptoSupport;
    std::string cryptoName;
    std::string cryptoVer;
    std::string hostPkgName;
    std::string targetPkgName;
    std::string appName;
    std::string appDesc;
    std::string appIcon;
    std::string appThumbnail;
    std::string token;
    int32_t reason;
    std::vector<std::string> syncGroupList;
} DmAuthRequestContext;

typedef struct DmAuthResponseContext {
    int32_t authType;
    std::string deviceId;
    std::string localDeviceId;
    int32_t msgType;
    int32_t sessionId;
    bool cryptoSupport;
    std::string cryptoName;
    std::string cryptoVer;
    int32_t reply;
    std::string networkId;
    std::string groupId;
    std::string groupName;
    std::string hostPkgName;
    std::string targetPkgName;
    std::string appName;
    std::string appDesc;
    std::string appIcon;
    std::string appThumbnail;
    std::string token;
    std::string authToken;
    int32_t pageId;
    int64_t requestId;
    int32_t code;
    int32_t state;
    std::vector<std::string> syncGroupList;
} DmAuthResponseContext;

class AuthMessageProcessor;

class DmAuthManager final : public ISoftbusSessionCallback,
                            public IHiChainConnectorCallback,
                            public std::enable_shared_from_this<DmAuthManager> {
public:
    DmAuthManager(std::shared_ptr<SoftbusConnector> softbusConnector,
                  std::shared_ptr<DeviceManagerServiceListener> listener,
                  std::shared_ptr<HiChainConnector> hiChainConnector_);
    ~DmAuthManager();
    int32_t AuthenticateDevice(const std::string &pkgName, int32_t authType, const std::string &deviceId,
                               const std::string &extra);
    int32_t UnAuthenticateDevice(const std::string &pkgName, const std::string &deviceId);
    int32_t VerifyAuthentication(const std::string &authParam);
    void OnSessionOpened(int32_t sessionId, int32_t sessionSide, int32_t result);
    void OnSessionClosed(const int32_t sessionId);
    void OnDataReceived(const int32_t sessionId, const std::string message);
    void OnGroupCreated(int64_t requestId, const std::string &groupId);
    void OnMemberJoin(int64_t requestId, int32_t status);

    // auth state machine
    int32_t EstablishAuthChannel(const std::string &deviceId);
    void StartNegotiate(const int32_t &sessionId);
    void RespNegotiate(const int32_t &sessionId);
    void SendAuthRequest(const int32_t &sessionId);
    int32_t StartAuthProcess(const int32_t &authType);
    void StartRespAuthProcess();
    int32_t CreateGroup();
    int32_t AddMember(const std::string &deviceId);
    std::string GetConnectAddr(std::string deviceId);
    int32_t JoinNetwork();
    void AuthenticateFinish();
    bool GetIsCryptoSupport();
    int32_t SetAuthRequestState(std::shared_ptr<AuthRequestState> authRequestState);
    int32_t SetAuthResponseState(std::shared_ptr<AuthResponseState> authResponseState);
    int32_t GetPinCode();
    std::string GenerateGroupName();
    int32_t HandleAuthenticateTimeout();
    void CancelDisplay();
    int32_t GeneratePincode();
    void ShowConfigDialog();
    void ShowAuthInfoDialog();
    void ShowStartAuthDialog();
    int32_t GetAuthenticationParam(DmAuthParam &authParam);
    int32_t OnUserOperation(int32_t action);
    void UserSwitchEventCallback(int32_t userId);
    int32_t SetPageId(int32_t pageId);
    int32_t SetReason(int32_t reason, int32_t state);

private:
    std::shared_ptr<SoftbusConnector> softbusConnector_;
    std::shared_ptr<HiChainConnector> hiChainConnector_;
    std::shared_ptr<DeviceManagerServiceListener> listener_;
    std::shared_ptr<DmAdapterManager> adapterMgr_;
    std::map<int32_t, std::shared_ptr<IAuthentication>> authenticationMap_;
    std::shared_ptr<AuthRequestState> authRequestState_ = nullptr;
    std::shared_ptr<AuthResponseState> authResponseState_ = nullptr;
    std::shared_ptr<DmAuthRequestContext> authRequestContext_;
    std::shared_ptr<DmAuthResponseContext> authResponseContext_;
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor_;
    std::map<std::string, std::shared_ptr<DmTimer>> timerMap_;
    std::shared_ptr<DmAbilityManager> dmAbilityMgr_;
    bool isCryptoSupport_ = false;
    bool isFinishOfLocal_ = true;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_AUTH_MANAGER_H

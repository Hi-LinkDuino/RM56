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

#include "UTTest_auth_request_state.h"

#include "auth_message_processor.h"
#include "dm_auth_manager.h"
#include "dm_constants.h"

namespace OHOS {
namespace DistributedHardware {
void AuthRequestStateTest::SetUp()
{
}
void AuthRequestStateTest::TearDown()
{
}
void AuthRequestStateTest::SetUpTestCase()
{
}
void AuthRequestStateTest::TearDownTestCase()
{
}

namespace {
std::string INPUT_TIMEOUT_TASK = "inputTimeoutTask";
std::string ADD_TIMEOUT_TASK = "addTimeoutTask";

std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<HiChainConnector> hiChainConnector = std::make_shared<HiChainConnector>();
/**
 * @tc.name: AuthResponseInitState::Leave_001
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseInitState::Leave with authManager ！= null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Leave_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    int32_t ret = authRequestState->Leave();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestInitState::SetAuthContext_001
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInitState::SetAuthManager with authManager ！= null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, SetAuthContext_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<DmAuthRequestContext> context =  std::make_shared<DmAuthRequestContext>();
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->context_.use_count();
    authRequestState->context_.reset();
    ASSERT_EQ(ret, 2);
}

/**
 * @tc.name: AuthRequestInitState::SetAuthManager_002
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestInitState::SetAuthManager with authManager = null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, SetAuthContext_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authRequestState->SetAuthContext(nullptr);
    int32_t ret = authRequestState->authManager_.use_count();
    authRequestState->authManager_.reset();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AuthRequestInitState::GetAuthContext_001
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInitState::GetAuthContext with authManager ！= null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetAuthContext_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<DmAuthRequestContext> context =  std::make_shared<DmAuthRequestContext>();
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authRequestState->GetAuthContext();
    int32_t ret = authRequestState->context_.use_count();
    authRequestState->context_.reset();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AuthRequestInitState::SetAuthManager_001
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInitState::SetAuthManager with authManager ！= null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, SetAuthManager_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authRequestState->SetAuthManager(authManager);
    int32_t ret = authRequestState->authManager_.use_count();
    authRequestState->authManager_.reset();
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: AuthRequestInitState::SetAuthManager_002
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestInitState::SetAuthManager with authManager = null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, SetAuthManager_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->authManager_.use_count();
    authRequestState->authManager_.reset();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AuthRequestNegotiateState::TransitionTo_001
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestInitState::TransitionTo with authManager = null
 *           4 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, TransitionTo_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authManager = nullptr;
    authRequestState->authManager_ = authManager;
    int32_t ret = authRequestState->TransitionTo(std::make_shared<AuthRequestNegotiateState>());
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestInitState::TransitionTo_002
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInitState::TransitionTo with authManager != null
 *           4 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, TransitionTo_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateDoneState>();
    std::shared_ptr<DmTimer> negotiateStartTimer = std::make_shared<DmTimer>(NEGOTIATE_TIMEOUT_TASK);
    authManager->timerMap_[NEGOTIATE_TIMEOUT_TASK] = negotiateStartTimer;
    authManager->authRequestState_ = std::make_shared<AuthRequestNegotiateDoneState>();
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    context->sessionId = 123456;
    authManager->SetAuthRequestState(authRequestState);
    authManager->softbusConnector_->GetSoftbusSession()->RegisterSessionCallback(authManager);
    authRequestState->SetAuthContext(context);
    authRequestState->SetAuthManager(authManager);
    int32_t ret = authRequestState->TransitionTo(std::make_shared<AuthRequestNegotiateDoneState>());
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestInitState::GetStateType_001
 * @tc.desc: 1 call AuthRequestInitState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_INIT);
}

/**
 * @tc.name: AuthRequestInitState::Enter_001
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestInitState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authManager = nullptr;
    authRequestState->SetAuthManager(authManager);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestInitState::Enter_002
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInitState::Enter with authManager != null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInitState>();
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authRequestState_ = std::make_shared<AuthRequestInitState>();
    authRequestState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    context->deviceId = "123456";
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestNegotiateState::GetStateType_002
 * @tc.desc: 1 call AuthRequestNegotiateState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_NEGOTIATE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_NEGOTIATE);
}

/**
 * @tc.name: AuthRequestNegotiateState::Enter_001
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestNegotiateState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestNegotiateState::Enter_004
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestNegotiateState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateState>();
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authRequestState_ = std::make_shared<AuthRequestNegotiateState>();
    authManager->authRequestContext_->deviceId = "111";
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authRequestState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    context->deviceId = "123456";
    context->sessionId = 22222;
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestNegotiateDoneState::GetStateType_003
 * @tc.desc: 1 call AuthRequestNegotiateDoneState::GetStateType
 *           2 check ret is AuthState::AUTH_REQUEST_NEGOTIATE_DONE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateDoneState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_NEGOTIATE_DONE);
}

/**
 * @tc.name: AuthRequestNegotiateDoneState::Enter_005
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestNegotiateDoneState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_005, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateDoneState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestNegotiateDoneState::Enter_006
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestNegotiateDoneState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_006, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNegotiateDoneState>();
    std::shared_ptr<DmTimer> negotiateStartTimer = std::make_shared<DmTimer>(NEGOTIATE_TIMEOUT_TASK);
    authManager->timerMap_[NEGOTIATE_TIMEOUT_TASK] = negotiateStartTimer;
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authManager->softbusConnector_->GetSoftbusSession()->RegisterSessionCallback(authManager);
    authManager->SetAuthRequestState(authRequestState);
    authRequestState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    context->sessionId = 333333;
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestReplyState::GetStateType_004
 * @tc.desc: 1 call AuthRequestReplyState::GetStateType
 *           2 check ret is AuthState::AUTH_REQUEST_REPLY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestReplyState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_REPLY);
}

/**
 * @tc.name: AuthRequestReplyState::Enter_007
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestReplyState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_007, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestReplyState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestReplyState::Enter_008
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestReplyState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_008, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestReplyState>();
    std::shared_ptr<DmTimer> inputStartTimer = std::make_shared<DmTimer>(CONFIRM_TIMEOUT_TASK);
    authManager->timerMap_[CONFIRM_TIMEOUT_TASK] = inputStartTimer;
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authRequestState_ = std::make_shared<AuthRequestReplyState>();
    authManager->authResponseContext_->sessionId = 1;
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    authManager->SetAuthRequestState(authRequestState);
    authRequestState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    context->sessionId = 333333;
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestInputState::GetStateType_005
 * @tc.desc: 1 call AuthRequestInputState::GetStateType
 *           2 check ret is AuthState::AUTH_REQUEST_INPUT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_005, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInputState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_INPUT);
}

/**
 * @tc.name: AuthRequestInputState::Enter_009
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestInputState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_009, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInputState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestInputState::Enter_010
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInputState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_010, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestInputState>();
    authRequestState->SetAuthManager(authManager);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestJoinState::GetStateType_006
 * @tc.desc: 1 call AuthRequestJoinState::GetStateType
 *           2 check ret is AuthState::AUTH_REQUEST_JOIN
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_006, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestJoinState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_JOIN);
}

/**
 * @tc.name: AuthRequestJoinState::Enter_011
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestJoinState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_011, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestJoinState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestJoinState::Enter_012
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestJoinState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_012, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestJoinState>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authResponseContext_->groupId = "111";
    authManager->authResponseContext_->groupName = "222";
    authManager->authResponseContext_->code = 123;
    authManager->authResponseContext_->requestId = 234;
    authManager->authResponseContext_->deviceId = "234";
    authRequestState->SetAuthManager(authManager);
    authManager->SetAuthRequestState(authRequestState);
    authManager->hiChainConnector_->RegisterHiChainCallback(authManager);
    authManager->softbusConnector_->GetSoftbusSession()->RegisterSessionCallback(authManager);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    context->deviceId = "44444";
    context->sessionId = 55555;
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestNetworkState::GetStateType_007
 * @tc.desc: 1 call AuthRequestNetworkState::GetStateType
 *           2 check ret is AuthState::AUTH_REQUEST_NETWORK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_007, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNetworkState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_NETWORK);
}

/**
 * @tc.name: AuthRequestNetworkState::Enter_013
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestNetworkState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_013, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNetworkState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestNetworkState::Enter_014
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestNetworkState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_014, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestNetworkState>();
    std::shared_ptr<DmTimer> authenticateStartTimer = std::make_shared<DmTimer>(AUTHENTICATE_TIMEOUT_TASK);
    authManager->timerMap_[AUTHENTICATE_TIMEOUT_TASK] = authenticateStartTimer;
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authRequestState_ = std::make_shared<AuthRequestNetworkState>();
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    authManager->softbusConnector_->GetSoftbusSession()->RegisterSessionCallback(authManager);
    authManager->SetAuthRequestState(authRequestState);
    authRequestState->SetAuthManager(authManager);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestFinishState::GetStateType_008
 * @tc.desc: 1 call AuthRequestFinishState::GetStateType
 *           2 check ret is AuthState::AUTH_REQUEST_FINISH
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, GetStateType_008, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestFinishState>();
    int32_t ret = authRequestState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_REQUEST_FINISH);
}

/**
 * @tc.name: AuthRequestFinishState::Enter_015
 * @tc.desc: 1 set authManager to null
 *           2 call AuthRequestFinishState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_015, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestFinishState>();
    authRequestState->SetAuthManager(nullptr);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthRequestFinishState::Enter_016
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestNetworkState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthRequestStateTest, Enter_016, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthRequestState> authRequestState = std::make_shared<AuthRequestFinishState>();
    std::shared_ptr<DmTimer> inputStartTimer = std::make_shared<DmTimer>(CONFIRM_TIMEOUT_TASK);
    authManager->timerMap_[CONFIRM_TIMEOUT_TASK] = inputStartTimer;
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authResponseState_ = std::make_shared<AuthResponseFinishState>();
    authManager->authRequestState_ = std::make_shared<AuthRequestFinishState>();
    authManager->authResponseContext_->sessionId = 1;
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    authManager->SetAuthRequestState(authRequestState);
    authRequestState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthRequestContext> context = std::make_shared<DmAuthRequestContext>();
    context->sessionId = 333333;
    authRequestState->SetAuthContext(context);
    int32_t ret = authRequestState->Enter();
    ASSERT_EQ(ret, DM_OK);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS

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

#include "UTTest_auth_response_state.h"

#include "auth_message_processor.h"
#include "auth_response_state.h"
#include "dm_auth_manager.h"
#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
void AuthResponseStateTest::SetUp()
{
}
void AuthResponseStateTest::TearDown()
{
}
void AuthResponseStateTest::SetUpTestCase()
{
}
void AuthResponseStateTest::TearDownTestCase()
{
}

namespace {
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
HWTEST_F(AuthResponseStateTest, Leave_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    int32_t ret = authResponseState->Leave();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthRequestInitState::GetAuthContext_001
 * @tc.desc: 1 set authManager not null
 *           2 call AuthRequestInitState::GetAuthContext with authManager ！= null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetAuthContext_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    std::shared_ptr<DmAuthResponseContext> context =  std::make_shared<DmAuthResponseContext>();
    authResponseState->GetAuthContext();
    int32_t ret = authResponseState->context_.use_count();
    authResponseState->context_.reset();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AuthResponseInitState::SetAuthManager_001
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseInitState::SetAuthManager with authManager ！= null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, SetAuthManager_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    authResponseState->SetAuthManager(authManager);
    int32_t ret = authResponseState->authManager_.use_count();
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: AuthResponseInitState::SetAuthManager_002
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseInitState::SetAuthManager with authManager = null
 *           3 check ret is authResponseState->authManager_.use_count()
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, SetAuthManager_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    authResponseState->SetAuthManager(nullptr);
    int32_t ret = authResponseState->authManager_.use_count();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: AuthResponseInitState::TransitionTo_001
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseInitState::TransitionTo with authManager = null
 *           4 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, TransitionTo_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    authManager = nullptr;
    authResponseState->authManager_ = authManager;
    int32_t ret = authResponseState->TransitionTo(std::make_shared<AuthResponseNegotiateState>());
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthResponseInitState::TransitionTo_002
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseInitState::TransitionTo with authManager != null
 *           4 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, TransitionTo_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<DmAuthResponseContext> context = std::make_shared<DmAuthResponseContext>();
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    authManager->authResponseState_ = std::make_shared<AuthResponseInitState>();
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    context->sessionId = 123456;
    authResponseState->SetAuthContext(context);
    authResponseState->SetAuthManager(authManager);
    int32_t ret = authResponseState->TransitionTo(std::make_shared<AuthResponseInitState>());
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthResponseInitState::GetStateType_001
 * @tc.desc: 1 call AuthResponseInitState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetStateType_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    int32_t ret = authResponseState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_RESPONSE_INIT);
}

/**
 * @tc.name: AuthResponseInitState::Enter_001
 * @tc.desc: 1 call AuthResponseInitState::GetStateType
 *           2 check ret is AuthResponseInitState::Enter
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseInitState>();
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthResponseNegotiateState::GetStateType_002
 * @tc.desc: 1 call AuthResponseNegotiateState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_NEGOTIATE
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetStateType_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseNegotiateState>();
    int32_t ret = authResponseState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_RESPONSE_NEGOTIATE);
}

/**
 * @tc.name: AuthResponseNegotiateState::Enter_002
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseNegotiateState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseNegotiateState>();
    authManager = nullptr;
    authResponseState->SetAuthManager(authManager);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthResponseNegotiateState::Enter_003
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseNegotiateState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseNegotiateState>();
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authResponseState_ = std::make_shared<AuthResponseNegotiateState>();
    authManager->authRequestState_ = std::make_shared<AuthRequestNegotiateState>();
    authManager->authResponseContext_->deviceId = "111";
    authManager->authResponseContext_->localDeviceId = "222";
    authManager->authMessageProcessor_->SetResponseContext(authManager->authResponseContext_);
    authManager->authMessageProcessor_->SetRequestContext(authManager->authRequestContext_);
    authManager->softbusConnector_->GetSoftbusSession()->RegisterSessionCallback(authManager);
    authResponseState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthResponseContext> context = std::make_shared<DmAuthResponseContext>();
    context->deviceId = "123456";
    context->sessionId = 22222;
    authResponseState->SetAuthContext(context);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthResponseConfirmState::GetStateType_003
 * @tc.desc: 1 call AuthResponseConfirmState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_CONFIRM
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetStateType_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseConfirmState>();
    int32_t ret = authResponseState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_RESPONSE_CONFIRM);
}

/**
 * @tc.name: AuthResponseConfirmState::Enter_004
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseConfirmState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseConfirmState>();
    authResponseState->SetAuthManager(nullptr);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthResponseConfirmState::Enter_005
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseConfirmState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_005, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseConfirmState>();
    authResponseState->SetAuthManager(authManager);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthResponseGroupState::GetStateType_004
 * @tc.desc: 1 call AuthResponseGroupState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_GROUP
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetStateType_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseGroupState>();
    int32_t ret = authResponseState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_RESPONSE_GROUP);
}

/**
 * @tc.name: AuthResponseGroupState::Enter_006
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseGroupState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_006, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseGroupState>();
    authResponseState->SetAuthManager(nullptr);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthResponseGroupState::Enter_007
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseGroupState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_007, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseGroupState>();
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authResponseState_ = std::make_shared<AuthResponseGroupState>();
    authResponseState->SetAuthManager(authManager);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthResponseShowState::GetStateType_005
 * @tc.desc: 1 call AuthResponseShowState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_SHOW
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetStateType_005, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseShowState>();
    int32_t ret = authResponseState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_RESPONSE_SHOW);
}

/**
 * @tc.name: AuthResponseShowState::Enter_008
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseShowState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_008, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseShowState>();
    authResponseState->SetAuthManager(nullptr);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthResponseShowState::Enter_009
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseShowState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_009, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseShowState>();
    authResponseState->SetAuthManager(authManager);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthResponseFinishState::GetStateType_006
 * @tc.desc: 1 call AuthResponseShowState::GetStateType
 *           2 check ret is AuthState::AUTH_RESPONSE_SHOW
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, GetStateType_006, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseFinishState>();
    int32_t ret = authResponseState->GetStateType();
    ASSERT_EQ(ret, AuthState::AUTH_RESPONSE_FINISH);
}

/**
 * @tc.name: AuthResponseFinishState::Enter_010
 * @tc.desc: 1 set authManager to null
 *           2 call AuthResponseFinishState::Enter with authManager = null
 *           3 check ret is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_010, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseFinishState>();
    authResponseState->SetAuthManager(nullptr);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthResponseFinishState::Enter_011
 * @tc.desc: 1 set authManager not null
 *           2 call AuthResponseFinishState::Enter with authManager != null
 *           3 check ret is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthResponseStateTest, Enter_011, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
    std::shared_ptr<AuthResponseState> authResponseState = std::make_shared<AuthResponseFinishState>();
    authManager->authRequestState_ = std::make_shared<AuthRequestFinishState>();
    authManager->authResponseState_ = std::make_shared<AuthResponseFinishState>();
    authManager->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    authManager->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    authManager->authMessageProcessor_ = std::make_shared<AuthMessageProcessor>(authManager);
    authManager->listener_ = std::make_shared<DeviceManagerServiceListener>();
    authManager->authResponseContext_->sessionId = 1;
    authManager->authRequestContext_->deviceId = "2";
    authManager->authRequestContext_->hostPkgName = "3";
    authManager->authRequestContext_->token = "4";
    authManager->authResponseContext_->reply = 5;
    authManager->authRequestContext_->reason = 6;
    authManager->SetAuthResponseState(authResponseState);
    authResponseState->SetAuthManager(authManager);
    std::shared_ptr<DmAuthResponseContext> context = std::make_shared<DmAuthResponseContext>();
    authResponseState->SetAuthContext(context);
    int32_t ret = authResponseState->Enter();
    ASSERT_EQ(ret, DM_OK);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS

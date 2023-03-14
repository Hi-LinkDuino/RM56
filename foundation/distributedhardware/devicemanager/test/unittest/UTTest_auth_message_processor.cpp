/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dm_log.h"
#include "dm_constants.h"
#include "auth_message_processor.h"
#include "softbus_connector.h"
#include "softbus_session.h"
#include "dm_auth_manager.h"
#include "UTTest_auth_message_processor.h"

namespace OHOS {
namespace DistributedHardware {
void AuthMessageProcessorTest::SetUp()
{
}
void AuthMessageProcessorTest::TearDown()
{
}
void AuthMessageProcessorTest::SetUpTestCase()
{
}
void AuthMessageProcessorTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
/**
 * @tc.name: AuthMessageProcessor::AuthMessageProcessor_001
 * @tc.desc: 1 set cryptoAdapter_ to null
 *           2 call AuthMessageProcessor::AuthMessageProcessor_001 with cryptoAdapter_ = nullptr
 *           3 check ret is authMessageProcessor->CreateNegotiateMessage(jsonObj);
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, AuthMessageProcessor_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> Test =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    ASSERT_NE(Test, nullptr);
}

/**
 * @tc.name: AuthMessageProcessor::AuthMessageProcessor_001
 * @tc.desc: 1 set cryptoAdapter_ to null
 *           2 call AuthMessageProcessor::AuthMessageProcessor_001 with cryptoAdapter_ = nullptr
 *           3 check ret is authMessageProcessor->CreateNegotiateMessage(jsonObj);
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, AuthMessageProcessor_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> Test =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    Test.reset();
    EXPECT_EQ(Test, nullptr);
}

/**
 * @tc.name: AuthMessageProcessor::CreateNegotiateMessage_001
 * @tc.desc: 1 set cryptoAdapter_ to null
 *           2 call AuthMessageProcessor::CreateNegotiateMessage_001 with cryptoAdapter_ = nullptr
 *           3 check ret is authMessageProcessor->CreateNegotiateMessage(jsonObj);
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, CreateNegotiateMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    int32_t msgType = MSG_TYPE_NEGOTIATE;
    nlohmann::json jsonObj;
    jsonObj[TAG_VER] = DM_ITF_VER;
    jsonObj[TAG_TYPE] = msgType;
    jsonObj[TAG_AUTH_TYPE] = authMessageProcessor->authResponseContext_->authType;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->cryptoAdapter_ = nullptr;
    authMessageProcessor->CreateNegotiateMessage(jsonObj);
    std::string str1 = jsonObj.dump();

    nlohmann::json jsonObject;
    jsonObject[TAG_VER] = DM_ITF_VER;
    jsonObject[TAG_TYPE] = msgType;
    jsonObject[TAG_CRYPTO_SUPPORT] = false;
    jsonObject[TAG_AUTH_TYPE] = authMessageProcessor->authResponseContext_->authType;
    jsonObject[TAG_REPLY] = authMessageProcessor->authResponseContext_->reply;
    jsonObject[TAG_LOCAL_DEVICE_ID] = authMessageProcessor->authResponseContext_->localDeviceId;
    std::string str2 = jsonObject.dump();
    ASSERT_EQ(str1, str2);
}

/**
 * @tc.name: AuthMessageProcessor::CreateSyncGroupMessage_001
 * @tc.desc: Compare JSON before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, CreateSyncGroupMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    authMessageProcessor->authRequestContext_ = std::make_shared<DmAuthRequestContext>();
    nlohmann::json jsona;
    nlohmann::json jsonObj;
    authMessageProcessor->authRequestContext_->deviceId = "132416546";
    std::vector<std::string> syncGroupList;
    syncGroupList.push_back("1111");
    authMessageProcessor->authRequestContext_->syncGroupList = syncGroupList;
    jsona[TAG_DEVICE_ID] = authMessageProcessor->authRequestContext_->deviceId;
    jsona[TAG_GROUPIDS] = authMessageProcessor->authRequestContext_->syncGroupList;
    authMessageProcessor->CreateSyncGroupMessage(jsonObj);
    std::string str1 = jsona.dump();
    std::string str2 = jsonObj.dump();
    ASSERT_EQ(str1, str2);
}

/**
 * @tc.name: AuthMessageProcessor::CreateResponseAuthMessage_001
 * @tc.desc: Compare JSON before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, CreateResponseAuthMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsona;
    nlohmann::json jsonObj;
    authMessageProcessor->authResponseContext_->reply = 0;
    authMessageProcessor->authResponseContext_->deviceId = "132416546";
    authMessageProcessor->authResponseContext_->token = "11";
    nlohmann::json jsonb;
    jsonb[TAG_GROUP_ID] = "123456";
    authMessageProcessor->authResponseContext_->groupId = jsonb.dump();
    authMessageProcessor->authResponseContext_->authToken = "123456";
    authMessageProcessor->authResponseContext_->networkId = "11112222";
    authMessageProcessor->authResponseContext_->requestId = 222222;
    authMessageProcessor->authResponseContext_->groupName = "333333";
    jsona[TAG_TOKEN] = authMessageProcessor->authResponseContext_->token;
    jsona[TAG_REPLY] = authMessageProcessor->authResponseContext_->reply;
    jsona[TAG_DEVICE_ID] = authMessageProcessor->authResponseContext_->deviceId;
    jsona[TAG_AUTH_TOKEN] = authMessageProcessor->authResponseContext_->authToken;
    jsona[TAG_NET_ID] = authMessageProcessor->authResponseContext_->networkId;
    jsona[TAG_REQUEST_ID] = authMessageProcessor->authResponseContext_->requestId;
    jsona[TAG_GROUP_ID] = "123456";
    jsona[TAG_GROUP_NAME] = authMessageProcessor->authResponseContext_->groupName;
    authMessageProcessor->CreateResponseAuthMessage(jsonObj);
    std::string str1 = jsona.dump();
    std::string str2 = jsonObj.dump();
    ASSERT_EQ(str1, str2);
}

/**
 * @tc.name: AuthMessageProcessor::CreateResponseFinishMessage_001
 * @tc.desc: Compare JSON before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, CreateResponseFinishMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsona;
    nlohmann::json jsonObj;
    authMessageProcessor->authResponseContext_->reply = 1;
    jsona[TAG_REPLY] = authMessageProcessor->authResponseContext_->reply;
    authMessageProcessor->CreateResponseFinishMessage(jsonObj);
    std::string str1 = jsona.dump();
    std::string str2 = jsonObj.dump();
    ASSERT_EQ(str1, str2);
}

/**
 * @tc.name: AuthMessageProcessor::ParseResponseFinishMessage_001
 * @tc.desc: Compare JSON before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseResponseFinishMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsonObj;
    authMessageProcessor->authResponseContext_->reply = 1;
    jsonObj[TAG_REPLY] = authMessageProcessor->authResponseContext_->reply;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseResponseFinishMessage(jsonObj);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::ParseAuthResponseMessage_001
 * @tc.desc: Compare JSON before and after assi gnment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseAuthResponseMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsona;
    authResponseContext->reply = 0;
    authResponseContext->deviceId = "11111";
    authResponseContext->authToken = "123456";
    authResponseContext->networkId = "12345";
    authResponseContext->requestId = 2;
    authResponseContext->groupId = "23456";
    authResponseContext->groupName = "34567";
    authResponseContext->token = "11123";
    jsona[TAG_TOKEN] = authResponseContext->token;
    jsona[TAG_REPLY] = authResponseContext->reply;
    jsona[TAG_DEVICE_ID] = authResponseContext->deviceId;
    jsona[TAG_AUTH_TOKEN] = authResponseContext->authToken;
    jsona[TAG_NET_ID] = authResponseContext->networkId;
    jsona[TAG_REQUEST_ID] = authResponseContext->requestId;
    jsona[TAG_GROUP_ID] = authResponseContext->groupId;
    jsona[TAG_GROUP_NAME] = authResponseContext->groupName;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseAuthResponseMessage(jsona);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::ParseAuthRequestMessage_001
 * @tc.desc: Compare JSON before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseAuthRequestMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->SetResponseContext(authResponseContext);
    nlohmann::json json;
    nlohmann::json jsonThumbnail;
    authResponseContext->deviceId = "123";
    authResponseContext->reply = 0;
    authResponseContext->authType = 222;
    authResponseContext->networkId = "234";
    authResponseContext->groupId = "345";
    authResponseContext->groupName = "456";
    authResponseContext->requestId = 2333;
    jsonThumbnail[TAG_DEVICE_ID] = authResponseContext->deviceId;
    jsonThumbnail[TAG_REPLY] = authResponseContext->reply;
    jsonThumbnail[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonThumbnail[TAG_NET_ID] = authResponseContext->networkId;
    jsonThumbnail[TAG_GROUP_ID] = authResponseContext->groupId;
    jsonThumbnail[TAG_GROUP_NAME] = authResponseContext->groupName;
    jsonThumbnail[TAG_REQUEST_ID] = authResponseContext->requestId;
    authMessageProcessor->authSplitJsonList_.push_back(jsonThumbnail);
    int32_t ret = authMessageProcessor->ParseAuthRequestMessage();
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: AuthMessageProcessor::ParseAuthRequestMessage_002
 * @tc.desc: Compare JSON before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseAuthRequestMessage_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->SetResponseContext(authResponseContext);
    nlohmann::json json;
    nlohmann::json jsonThumbnail;
    authResponseContext->deviceId = "123";
    authResponseContext->reply = 1;
    authResponseContext->authType = 222;
    authResponseContext->networkId = "234";
    authResponseContext->groupId = "345";
    authResponseContext->groupName = "456";
    authResponseContext->requestId = 2333;
    jsonThumbnail[TAG_DEVICE_ID] = authResponseContext->deviceId;
    jsonThumbnail[TAG_REPLY] = authResponseContext->reply;
    jsonThumbnail[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonThumbnail[TAG_NET_ID] = authResponseContext->networkId;
    jsonThumbnail[TAG_GROUP_ID] = authResponseContext->groupId;
    jsonThumbnail[TAG_GROUP_NAME] = authResponseContext->groupName;
    jsonThumbnail[TAG_REQUEST_ID] = authResponseContext->requestId;
    authMessageProcessor->authSplitJsonList_.push_back(jsonThumbnail);
    int32_t ret = authMessageProcessor->ParseAuthRequestMessage();
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: AuthMessageProcessor::ParseNegotiateMessage_001
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseNegotiateMessage_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsonObj;
    jsonObj[TAG_CRYPTO_SUPPORT] = "CRYPTOSUPPORT";
    jsonObj[TAG_CRYPTO_SUPPORT] = authMessageProcessor->authResponseContext_->cryptoSupport;
    authResponseContext->localDeviceId = "22";
    authResponseContext->authType = 1;
    authResponseContext->reply = 33;
    jsonObj[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonObj[TAG_LOCAL_DEVICE_ID] = authResponseContext->localDeviceId;
    jsonObj[TAG_REPLY] = authResponseContext->reply;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseNegotiateMessage(jsonObj);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::ParseNegotiateMessage_002
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseNegotiateMessage_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsonObj;
    authResponseContext->localDeviceId = "22";
    authResponseContext->authType = 1;
    authResponseContext->reply = 33;
    jsonObj[TAG_CRYPTO_NAME] = "CRYPTONAME";
    jsonObj[TAG_CRYPTO_NAME] = authResponseContext->cryptoSupport;
    jsonObj[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonObj[TAG_LOCAL_DEVICE_ID] = authResponseContext->localDeviceId;
    jsonObj[TAG_REPLY] = authResponseContext->reply;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseNegotiateMessage(jsonObj);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::ParseNegotiateMessage_003
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseNegotiateMessage_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsonObj;
    authResponseContext->localDeviceId = "22";
    authResponseContext->authType = 1;
    authResponseContext->reply = 33;
    jsonObj[TAG_CRYPTO_VERSION] = "CRYPTOVERSION";
    jsonObj[TAG_CRYPTO_VERSION] = authResponseContext->cryptoSupport;
    jsonObj[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonObj[TAG_LOCAL_DEVICE_ID] = authResponseContext->localDeviceId;
    jsonObj[TAG_REPLY] = authResponseContext->reply;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseNegotiateMessage(jsonObj);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::ParseNegotiateMessage_004
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseNegotiateMessage_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsonObj;
    authResponseContext->localDeviceId = "22";
    authResponseContext->authType = 1;
    authResponseContext->reply = 33;
    jsonObj[TAG_DEVICE_ID] = "DEVICEID";
    jsonObj[TAG_DEVICE_ID] = authResponseContext->deviceId;
    jsonObj[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonObj[TAG_LOCAL_DEVICE_ID] = authResponseContext->localDeviceId;
    jsonObj[TAG_REPLY] = authResponseContext->reply;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseNegotiateMessage(jsonObj);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::ParseNegotiateMessage_005
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, ParseNegotiateMessage_005, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    nlohmann::json jsonObj;
    authResponseContext->localDeviceId = "22";
    authResponseContext->authType = 1;
    authResponseContext->reply = 33;
    jsonObj[TAG_LOCAL_DEVICE_ID] = "LOCALDEVICEID";
    jsonObj[TAG_LOCAL_DEVICE_ID] = authResponseContext->localDeviceId;
    jsonObj[TAG_AUTH_TYPE] = authResponseContext->authType;
    jsonObj[TAG_LOCAL_DEVICE_ID] = authResponseContext->localDeviceId;
    jsonObj[TAG_REPLY] = authResponseContext->reply;
    authMessageProcessor->SetResponseContext(authResponseContext);
    authMessageProcessor->ParseNegotiateMessage(jsonObj);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::SetRequestContext_001
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, SetRequestContext_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthRequestContext> authRequestContext = std::make_shared<DmAuthRequestContext>();
    authMessageProcessor->SetRequestContext(authRequestContext);
    ASSERT_EQ(authMessageProcessor->authRequestContext_, authRequestContext);
}

/**
 * @tc.name: AuthMessageProcessor::SetRequestContext_002
 * @tc.desc: Judge whether authrequestcontext is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, SetRequestContext_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthRequestContext> authRequestContext = std::make_shared<DmAuthRequestContext>();
    authMessageProcessor->SetRequestContext(nullptr);
    ASSERT_EQ(authMessageProcessor->authRequestContext_, nullptr);
}

/**
 * @tc.name: AuthMessageProcessor::SetResponseContext_001
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, SetResponseContext_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->SetResponseContext(authResponseContext);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, authResponseContext);
}

/**
 * @tc.name: AuthMessageProcessor::SetResponseContext_002
 * @tc.desc: Judge whether authrequestcontext is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, SetResponseContext_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    std::shared_ptr<DmAuthResponseContext> authResponseContext = std::make_shared<DmAuthResponseContext>();
    authMessageProcessor->SetResponseContext(nullptr);
    ASSERT_EQ(authMessageProcessor->authResponseContext_, nullptr);
}

/**
 * @tc.name: AuthMessageProcessor::GetResponseContext_001
 * @tc.desc: Compare authResponseContext before and after assignment
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, GetResponseContext_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    std::shared_ptr<DmAuthResponseContext> authResponseContext = authMessageProcessor->GetResponseContext();
    ASSERT_EQ(authResponseContext, authMessageProcessor->authResponseContext_);
}

/**
 * @tc.name: AuthMessageProcessor::GetResponseContext_002
 * @tc.desc: Judge whether authrequestcontext is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(AuthMessageProcessorTest, GetResponseContext_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<HiChainConnector> hiChainConnector_ = std::make_shared<HiChainConnector>();
    std::shared_ptr<DmAuthManager> data =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector_);
    std::shared_ptr<AuthMessageProcessor> authMessageProcessor = std::make_shared<AuthMessageProcessor>(data);
    authMessageProcessor->authResponseContext_ = std::make_shared<DmAuthResponseContext>();
    std::shared_ptr<DmAuthResponseContext> authResponseContext = authMessageProcessor->GetResponseContext();
    ASSERT_NE(authResponseContext, nullptr);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS

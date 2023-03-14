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

#include "UTTest_pin_auth.h"

#include <unistd.h>

#include <memory>

#include "dm_constants.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace DistributedHardware {
void PinAuthTest::SetUp()
{
}

void PinAuthTest::TearDown()
{
}

void PinAuthTest::SetUpTestCase()
{
}

void PinAuthTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<HiChainConnector> hiChainConnector = std::make_shared<HiChainConnector>();
std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
/**
 * @tc.name: DmAuthManager::ShowAuthInfo_001
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, ShowAuthInfo_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    std::string authToken = "123456";
    int32_t ret = pinAuth->ShowAuthInfo(authToken, authManager);
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: DmAuthManager::ShowAuthInfo_002
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, ShowAuthInfo_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json jsonObject;
    jsonObject[PIN_TOKEN] = 123456;
    std::string authToken = jsonObject.dump();
    int32_t ret = pinAuth->ShowAuthInfo(authToken, authManager);
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: DmAuthManager::ShowAuthInfo_003
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, ShowAuthInfo_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json jsonObject;
    jsonObject[PIN_CODE_KEY] = 123456;
    std::string authToken = jsonObject.dump();
    int32_t ret = pinAuth->ShowAuthInfo(authToken, nullptr);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: DmAuthManager::ShowAuthInfo_004
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, ShowAuthInfo_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json jsonObject;
    jsonObject[PIN_CODE_KEY] = 123456;
    std::string authToken = jsonObject.dump();
    int32_t ret = pinAuth->ShowAuthInfo(authToken, authManager);
    ASSERT_EQ(ret, DM_OK);
}
/**
 * @tc.name: DmAuthManager::StartAuth_001
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, StartAuth_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    std::string authToken = "";
    int32_t ret = pinAuth->StartAuth(authToken, nullptr);
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: DmAuthManager::StartAuth_002
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, StartAuth_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    std::string authToken = "";
    int32_t ret = pinAuth->StartAuth(authToken, authManager);
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: DmAuthManager::StartAuth_003
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, StartAuth_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json jsonObject;
    jsonObject[PIN_TOKEN] = 123456;
    std::string authToken = jsonObject.dump();
    int32_t ret = pinAuth->StartAuth(authToken, authManager);
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: DmAuthManager::StartAuth_004
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, StartAuth_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json jsonObject;
    jsonObject[PIN_CODE_KEY] = 123456;
    std::string authToken = jsonObject.dump();
    int32_t ret = pinAuth->StartAuth(authToken, authManager);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: DmAuthManager::VerifyAuthentication_001
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, VerifyAuthentication_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    std::string authToken;
    std::string authParam = "1";
    int32_t ret = pinAuth->VerifyAuthentication(authToken, authParam);
    ASSERT_EQ(ret, DM_FAILED);
}

/**
 * @tc.name: DmAuthManager::VerifyAuthentication_001
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, VerifyAuthentication_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    std::string authToken;
    std::string authParam = "0";
    int32_t ret = pinAuth->VerifyAuthentication(authToken, authParam);
    ASSERT_EQ(ret, DM_FAILED);
}
/**
 * @tc.name: DmAuthManager::VerifyAuthentication_003
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, VerifyAuthentication_003, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json authParamJson;
    authParamJson[PIN_CODE_KEY] = 123456;
    authParamJson[PIN_TOKEN] = 123456;
    nlohmann::json authTokenJson;
    authTokenJson[PIN_CODE_KEY] = 123456;
    authTokenJson[PIN_TOKEN] = 123456;
    std::string authToken = authTokenJson.dump();
    std::string authParam = authParamJson.dump();
    int32_t ret = pinAuth->VerifyAuthentication(authToken, authParam);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: DmAuthManager::VerifyAuthentication_004
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_AUTH_INPUT_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, VerifyAuthentication_004, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json authParamJson;
    authParamJson[PIN_CODE_KEY] = 012345;
    authParamJson[PIN_TOKEN] = 123456;
    nlohmann::json authTokenJson;
    authTokenJson[PIN_CODE_KEY] = 123456;
    authTokenJson[PIN_TOKEN] = 123456;
    std::string authToken = authTokenJson.dump();
    std::string authParam = authParamJson.dump();
    int32_t ret = pinAuth->VerifyAuthentication(authToken, authParam);
    ASSERT_EQ(ret, DM_AUTH_INPUT_FAILED);
}

/**
 * @tc.name: DmAuthManager::VerifyAuthentication_005
 * @tc.desc: Call unauthenticateddevice to check whether the return value is DM_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthTest, VerifyAuthentication_005, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuth> pinAuth = std::make_shared<PinAuth>();
    nlohmann::json authParamJson;
    authParamJson[PIN_CODE_KEY] = 012345;
    authParamJson[PIN_TOKEN] = 123456;
    nlohmann::json authTokenJson;
    authTokenJson[PIN_CODE_KEY] = 123456;
    authTokenJson[PIN_TOKEN] = 123456;
    std::string authToken = authTokenJson.dump();
    std::string authParam = authParamJson.dump();
    pinAuth->VerifyAuthentication(authToken, authParam);
    pinAuth->VerifyAuthentication(authToken, authParam);
    int32_t ret = pinAuth->VerifyAuthentication(authToken, authParam);
    ASSERT_EQ(ret, DM_FAILED);
}
}
} // namespace DistributedHardware
} // namespace OHOS

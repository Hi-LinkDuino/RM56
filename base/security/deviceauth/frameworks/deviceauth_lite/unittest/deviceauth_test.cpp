/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "deviceauth_test.h"
#include <gtest/gtest.h>
#include "securec.h"
#include "hichain.h"

#define LOG(format, ...) (printf(format"\n", ##__VA_ARGS__))

using namespace std;
using namespace testing::ext;

namespace {
const int KEY_LEN = 32;

class DeviceAuthTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DeviceAuthTest::SetUpTestCase(void) {}
void DeviceAuthTest::TearDownTestCase(void) {}
void DeviceAuthTest::SetUp() {}
void DeviceAuthTest::TearDown() {}

static struct session_identity g_serverIdentity = {
    0,
    {strlen("testServer"), "testServer"},
    {strlen("testServer"), "testServer"},
    0
};
static struct hc_pin g_testPin = {strlen("123456789012345"), "123456789012345"};
static struct hc_auth_id g_testClientAuthId = {strlen("authClient"), "authClient"};
static struct hc_auth_id g_testServerAuthId = {strlen("authServer"), "authServer"};

static void Transmit(const struct session_identity *identity, const void *data, uint32_t length)
{
    LOG("--------Transmit--------");
    LOG("identity session_id[%d] package_name[%s]", identity->session_id, identity->package_name.name);
    LOG("data[%s]", (char *)data);
    LOG("length[%u]", length);
    LOG("--------Transmit--------");
}

static void GetProtocolParams(const struct session_identity *identity, int32_t operationCode,
    struct hc_pin *pin, struct operation_parameter *para)
{
    LOG("--------GetProtocolParams--------");
    LOG("identity session_id[%d] package_name[%s]", identity->session_id, identity->package_name.name);
    LOG("operationCode[%d]", operationCode);
    *pin = g_testPin;
    para->self_auth_id = g_testServerAuthId;
    para->peer_auth_id = g_testClientAuthId;
    para->key_length = KEY_LEN;
    LOG("--------GetProtocolParams--------");
}

static void SetSessionKey(const struct session_identity *identity, const struct hc_session_key *sessionKey)
{
    LOG("--------SetSessionKey--------");
    LOG("identity session_id[%d] package_name[%s]", identity->session_id, identity->package_name.name);
    LOG("sessionKey[%s]", sessionKey->session_key);
    LOG("--------SetSessionKey--------");
}

static void SetServiceResult(const struct session_identity *identity, int32_t result)
{
    LOG("--------SetServiceResult--------");
    LOG("identity session_id[%d] package_name[%s]", identity->session_id, identity->package_name.name);
    LOG("result[%d]", result);
    LOG("--------SetServiceResult--------");
}

static int32_t ConfirmReceiveRequest(const struct session_identity *identity, int32_t operationCode)
{
    LOG("--------ConfirmReceiveRequest--------");
    LOG("identity session_id[%d] package_name[%s]", identity->session_id, identity->package_name.name);
    LOG("operationCode[%d]", operationCode);
    LOG("--------ConfirmReceiveRequest--------");
    return HC_OK;
}

static HWTEST_F(DeviceAuthTest, Test001, TestSize.Level2)
{
    LOG("--------DeviceAuthTest Test001--------");
    LOG("--------get_instance--------");
    struct hc_call_back callBack = {
        Transmit,
        GetProtocolParams,
        SetSessionKey,
        SetServiceResult,
        ConfirmReceiveRequest
    };
    hc_handle server = get_instance(&g_serverIdentity, HC_ACCESSORY, &callBack);
    ASSERT_TRUE(server != NULL);
    destroy(&server);
    LOG("--------DeviceAuthTest Test001--------");
}

static HWTEST_F(DeviceAuthTest, Test002, TestSize.Level2)
{
    LOG("--------DeviceAuthTest Test002--------");
    LOG("--------start_pake--------");
    struct hc_call_back callBack = {
        Transmit,
        GetProtocolParams,
        SetSessionKey,
        SetServiceResult,
        ConfirmReceiveRequest
    };
    hc_handle server = get_instance(&g_serverIdentity, HC_ACCESSORY, &callBack);
    const struct operation_parameter params = {g_testServerAuthId, g_testClientAuthId, KEY_LEN};
    int32_t ret = start_pake(server, &params);
    EXPECT_EQ(ret, HC_OK);
    destroy(&server);
    LOG("--------DeviceAuthTest Test002--------");
}

static HWTEST_F(DeviceAuthTest, Test003, TestSize.Level2)
{
    LOG("--------DeviceAuthTest Test003--------");
    LOG("--------is_trust_peer--------");
    struct hc_call_back callBack = {
        Transmit,
        GetProtocolParams,
        SetSessionKey,
        SetServiceResult,
        ConfirmReceiveRequest
    };
    hc_handle server = get_instance(&g_serverIdentity, HC_ACCESSORY, &callBack);
    struct hc_user_info userInfo = {g_testServerAuthId, 1};
    int32_t ret = is_trust_peer(server, &userInfo);
    EXPECT_EQ(ret, HC_OK);
    destroy(&server);
    LOG("--------DeviceAuthTest Test003--------");
}
}

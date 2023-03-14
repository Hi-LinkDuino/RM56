/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#include <securec.h>
#include <ohos_init.h>
#include <ohos_errno.h>
#include "gtest/gtest.h"
#include "iproxy_client.h"
#include "registry.h"
#include "utils/SamgrTestBase.h"

using namespace testing::ext;

static const int MAX_IO_SIZE = 8192;
static const int NORMAL_IO_SIZE = 250;

static IClientProxy *GetRemoteIUnknown(const char *serviceName, const char *featureName)
{
    IClientProxy *demoApi = NULL;
    IUnknown *iUnknown = NULL;
    if (featureName == nullptr) {
        iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(serviceName);
    } else {
        iUnknown = SAMGR_GetInstance()->GetFeatureApi(serviceName, featureName);
    }

    if (iUnknown == nullptr) {
        printf("[hcpptest][TID:0x%lx][GetDefaultFeatureApi S:%s]: error is NULL", pthread_self(), serviceName);
        return nullptr;
    }
    (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&demoApi);

    return demoApi;
}

static int CurrentCallback(IOwner owner, int code, IpcIo *reply)
{
    printf("[hcpptest]CurrentCallback run \n");
    size_t len = 0;
    char *response = (char *)IpcIoPopString(reply, &len);
    printf("[hcpptest]response %s \n", response);
    return 0;
}

static void ReleaseIUnknown(IUnknown *demoApi)
{
    if (demoApi != NULL) {
        demoApi->Release((IUnknown *)demoApi);
    }
}

class LiteIPCClientTest : public testing::Test {
protected:
    // SetUpTestCase: Testsuit setup, run before 1st testcase
    static void SetUpTestCase(void)
    {
        printf("[hcpptest]SetUpTestCase! \n");
        SystemInitProxy();
    }
    // TearDownTestCase: Testsuit teardown, run after last testcase
    static void TearDownTestCase(void)
    {
    }
    // Testcase setup
    virtual void SetUp()
    {
    }
    // Testcase teardown
    virtual void TearDown()
    {
    }
};


/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0010
 * @tc.name      : Client can get api of other process
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0010, Function | MediumTest | Level1)
{
    IClientProxy *demoApi = NULL;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi("abilityms", "AmsFeature");
    ASSERT_EQ(iUnknown != NULL, TRUE);

    int result = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&demoApi);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(demoApi != nullptr, TRUE);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0020
 * @tc.name      : Client can not get api if service name or feature name not exist in the system
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0020, Function | MediumTest | Level2)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi("notExistService");
    ASSERT_EQ(iUnknown == NULL, TRUE);

    iUnknown = SAMGR_GetInstance()->GetFeatureApi("notExistService", "");
    ASSERT_EQ(iUnknown == NULL, TRUE);

    iUnknown = SAMGR_GetInstance()->GetFeatureApi("abilityms", "notExistFeature");
    ASSERT_EQ(iUnknown == NULL, TRUE);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0040
 * @tc.name      : Client can send request to other process
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0040, Function | MediumTest | Level1)
{
    IClientProxy *remoteApi = GetRemoteIUnknown("abilityms", "AmsFeature");
    ASSERT_EQ(remoteApi != nullptr, TRUE);

    IpcIo request;
    char data[NORMAL_IO_SIZE];
    IpcIoInit(&request, data, sizeof(data), 0);
    char *buff = (char*)"test xxxx";
    IpcIoPushString(&request, buff);

    char data2[NORMAL_IO_SIZE];
    int funcId = 0;
    int result = remoteApi->Invoke(remoteApi, funcId, &request, data2, CurrentCallback);
    ASSERT_EQ(result, EC_SUCCESS);

    ReleaseIUnknown((IUnknown *)remoteApi);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0050
 * @tc.name      : User can send request which is null to other process
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0050, Function | MediumTest | Level2)
{
    IClientProxy *remoteApi = GetRemoteIUnknown("abilityms", "AmsFeature");
    ASSERT_EQ(remoteApi != nullptr, TRUE);

    char data2[NORMAL_IO_SIZE];
    int funcId = 0;
    int result = remoteApi->Invoke(remoteApi, funcId, nullptr, data2, CurrentCallback);
    printf("[hcpptest]result is: %d \n", result);
    ASSERT_EQ(result, EC_SUCCESS);

    ReleaseIUnknown((IUnknown *)remoteApi);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0060
 * @tc.name      : User can send request without processing receipts
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0060, Function | MediumTest | Level2)
{
    IClientProxy *remoteApi = GetRemoteIUnknown("abilityms", "AmsInnerFeature");
    ASSERT_EQ(remoteApi != nullptr, TRUE);

    IpcIo request;
    char data[NORMAL_IO_SIZE];
    IpcIoInit(&request, data, sizeof(data), 0);
    char *buff = (char*)"test xxxx";
    IpcIoPushString(&request, buff);

    int funcId = 0;
    int result = remoteApi->Invoke(remoteApi, funcId, &request, nullptr, nullptr);
    printf("[hcpptest]result is: %d \n", result);
    ASSERT_EQ(result, EC_SUCCESS);

    ReleaseIUnknown((IUnknown *)remoteApi);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0070
 * @tc.name      : User can not send request which longer than 2048 bytes
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0070, Function | MediumTest | Level2)
{
    IClientProxy *remoteApi = GetRemoteIUnknown("abilityms", "AmsInnerFeature");
    ASSERT_EQ(remoteApi != nullptr, TRUE);

    IpcIo request;
    char data[MAX_IO_SIZE + 1];
    IpcIoInit(&request, data, sizeof(data), 0);
    char *buff = (char*)"test xxxx";
    IpcIoPushString(&request, buff);

    char data2[NORMAL_IO_SIZE];
    int funcId = 0;
    int result = remoteApi->Invoke(remoteApi, funcId, &request, data2, CurrentCallback);
    printf("[hcpptest]result is: %d \n", result);
    ASSERT_EQ(result, EC_FAILURE);

    ReleaseIUnknown((IUnknown *)remoteApi);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0100
 * @tc.name      : User can get IPC address of remote service
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0100, Function | MediumTest | Level2)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi("abilityms", "AmsFeature");
    ASSERT_EQ(iUnknown != nullptr, TRUE);

    SvcIdentity svcIdentity = SAMGR_GetRemoteIdentity("abilityms", "AmsFeature");
    ASSERT_EQ(svcIdentity.handle != 0xffffffff, TRUE);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0110
 * @tc.name      : User can get an invalid IPC address if input para is nullptr
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0110, Function | MediumTest | Level2)
{
    SvcIdentity svcIdentity = SAMGR_GetRemoteIdentity(nullptr, "AmsFeature");
    ASSERT_EQ(svcIdentity.handle == 0xffffffff, TRUE);

    svcIdentity = SAMGR_GetRemoteIdentity("abilityms", nullptr);
    ASSERT_EQ(svcIdentity.handle == 0xffffffff, TRUE);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0120
 * @tc.name      : User can get an invalid IPC address if has not called GetFeatureApi first
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0120, Function | MediumTest | Level2)
{
    SvcIdentity svcIdentity = SAMGR_GetRemoteIdentity("samgr", nullptr);
    ASSERT_EQ(svcIdentity.handle == 0xffffffff, TRUE);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0130
 * @tc.name      : User get an invalid IPC address if service not exist
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0130, Function | MediumTest | Level2)
{
    SvcIdentity svcIdentity = SAMGR_GetRemoteIdentity("noExistService", "noExistFeature");
    ASSERT_EQ(svcIdentity.handle == 0xffffffff, TRUE);
    ASSERT_EQ(svcIdentity.token == 0xffffffff, TRUE);
};

/**
 * @tc.number    : DMSLite_SAMGR_IPCClient_0140
 * @tc.name      : User can register a proxy for remote api
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(LiteIPCClientTest, testIPCClient0140, Function | MediumTest | Level2)
{
    auto creatClient = [] (const char *service, const char *feature, uint32 size) -> void * {
        printf("[hcpptest]creatClient run \n");
        return nullptr;
    };

    auto destroyClient = [] (const char *service, const char *feature, void *iproxy) {
        printf("[hcpptest]destroyClient run \n");
    };

    // this method createClient will return a proxy.
    int result = SAMGR_RegisterFactory("abilityms", "AmsFeature", creatClient, destroyClient);
    ASSERT_EQ(result == 0, TRUE);
};
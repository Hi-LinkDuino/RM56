/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include <securec.h>
#include <semaphore.h>
#include <ohos_init.h>
#include "gtest/gtest.h"
#include "utils/SamgrTestBase.h"

using namespace testing::ext;

static const char S_SERVICE[] = "s_service";
static const char S_FEATURE[] = "s_feature";

static const char T_SERVICE[] = "t_service";
static const char T_FEATURE[] = "t_feature";

static sem_t g_sem;

enum MessageId {
    MSG_NO, // receiver and no response
    MSG_RT, // receiver then send response
    MSG_FW, // receiver then forward message
};

struct Payload {
    int id;
    const char *name;
    int value;
};

struct DemoApi {
    INHERIT_IUNKNOWN;
    bool(*SyncCall)(IUnknown *iUnknown, struct Payload *payload);
    int32 (*SAMGR_SendRequestProxy)(const Identity *identity, const Request *request, Handler handler);
};

struct DemoFeature {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(DemoApi);
    Identity identity;
    int featureCalledCount;
    BOOL featureStatus;
    char *latestRequest;
};

struct DefaultFeatureApi {
    INHERIT_IUNKNOWN;
};

struct ExampleService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(DefaultFeatureApi);
    Identity identity;
};

static bool SyncCall(IUnknown *iUnknown, struct Payload *payload)
{
    (void)iUnknown;
    if (payload != nullptr && payload->id >= 0 && payload->name != nullptr) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static int32 SAMGR_SendRequestProxy(const Identity *identity, const Request *request, Handler handler)
{
    // this api SAMGR_SendRequest is the test target
    return SAMGR_SendRequest(identity, request, handler);
}

static const char *GetName(Service *service)
{
    (void)service;
    return S_SERVICE;
}

static const char *GetName2(Service *service)
{
    (void)service;
    return T_SERVICE;
}

static BOOL Initialize(Service *service, Identity identity)
{
    ExampleService *example = (ExampleService *)service;
    example->identity = identity;
    printf("[hcpptest]serviceName %s Initialize \n", service->GetName(service));

    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *msg)
{
    (void)service;
    return FALSE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_ABOVE_NORMAL, 0x800, 20, SHARED_TASK};
    return config;
}

static ExampleService g_service = {
    .GetName = GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    .ver = 0x20,
    .ref = 1,
    .iUnknown = {
        DEFAULT_IUNKNOWN_IMPL,
    }
};

static ExampleService g_service2 = {
    .GetName = GetName2,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    .ver = 0x20,
    .ref = 1,
    .iUnknown = {
        DEFAULT_IUNKNOWN_IMPL,
    }
};

static const char *FEATURE_GetName(Feature *feature)
{
    (void)feature;
    return S_FEATURE;
}

static const char *FEATURE_GetName2(Feature *feature)
{
    (void)feature;
    return T_FEATURE;
}

static void FEATURE_OnInitialize(Feature *feature, Service *parent, Identity identity)
{
    DemoFeature *demoFeature = (DemoFeature *)feature;
    demoFeature->identity = identity;
    printf("[hcpptest]featureName %s Initialize \n", feature->GetName(feature));
    (void)parent;
}

static void FEATURE_OnStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
}

static BOOL FEATURE_OnMessage(Feature *feature, Request *request)
{
    int32 returnCode;
    DemoFeature *demoFeature = (DemoFeature *)feature;
    demoFeature->featureCalledCount++;
    
    if (demoFeature->latestRequest != nullptr) {
        free(demoFeature->latestRequest);
    }

    int requestLen = 0;
    if (request->len > 0) {
        requestLen = request->len;
    } else {
        requestLen = strlen((char *)request->data) + 1;
    }
    demoFeature->latestRequest = (char *)malloc(requestLen * sizeof(char));
    if (demoFeature->latestRequest == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s(demoFeature->latestRequest, requestLen, (char *)request->data);
    if (error != EOK) {
        ADD_FAILURE();
    }

    switch (request->msgId) {
        case MSG_NO:
            returnCode = 0;
            break;
        case MSG_RT:
        {
            char dataR[] = "Yes, you did!";
            Response response = {
                .data = dataR,
                .len = 0,
                };
            returnCode = SAMGR_SendResponse(request, &response);
            break;
        };
        case MSG_FW:
        {
            returnCode = 0;
            break;
        };
        default:
            returnCode = 0;
    }
    demoFeature->featureStatus = ((returnCode == 0) ? TRUE : FALSE);

    sem_post(&g_sem);
    return demoFeature->featureStatus;
}

static DemoFeature g_feature = {
    .GetName = FEATURE_GetName,
    .OnInitialize = FEATURE_OnInitialize,
    .OnStop = FEATURE_OnStop,
    .OnMessage = FEATURE_OnMessage,
    .ver = 0x20,
    .ref = 0,
    .iUnknown = {
        DEFAULT_IUNKNOWN_IMPL,
        .SyncCall = SyncCall,
        .SAMGR_SendRequestProxy = SAMGR_SendRequestProxy,
    },
    .identity = {-1, 1, nullptr},
    .featureCalledCount = 0,
    .featureStatus = TRUE,
    .latestRequest = nullptr,
};
static DemoFeature g_feature2 = {
    .GetName = FEATURE_GetName2,
    .OnInitialize = FEATURE_OnInitialize,
    .OnStop = FEATURE_OnStop,
    .OnMessage = FEATURE_OnMessage,
    .ver = 0x20,
    .ref = 0,
    .iUnknown = {
        DEFAULT_IUNKNOWN_IMPL,
        .SyncCall = SyncCall,
        .SAMGR_SendRequestProxy = SAMGR_SendRequestProxy,
    },
    .identity = {-1, 2, nullptr},
    .featureCalledCount = 0,
    .featureStatus = TRUE,
    .latestRequest = nullptr,
};

static void GServiceInit(void)
{
    printf("[hcpptest]GServiceInit start! \n");
    BOOL result = SAMGR_GetInstance()->RegisterService((Service *)&g_service);
    BOOL result2 = SAMGR_GetInstance()->RegisterService((Service *)&g_service2);
    if (result == FALSE || result2 == FALSE) {
        printf("[hcpptest]E RegisterService failed! \n");
    }
}
SYSEX_SERVICE_INIT(GServiceInit);

static void GFeatureInit(void)
{
    printf("[hcpptest]GFeatureInit start! \n");
    BOOL result1 = SAMGR_GetInstance()->RegisterFeature(S_SERVICE, (Feature *)&g_feature);
    BOOL result2 = SAMGR_GetInstance()->RegisterFeatureApi(S_SERVICE, S_FEATURE, 
                                                           GET_IUNKNOWN(g_feature));

    BOOL result3 = SAMGR_GetInstance()->RegisterFeature(T_SERVICE, (Feature *)&g_feature2);
    BOOL result4 = SAMGR_GetInstance()->RegisterFeatureApi(T_SERVICE, T_FEATURE, 
                                                           GET_IUNKNOWN(g_feature2));
    if (result1 == FALSE || result2 == FALSE || result3 == FALSE || result4 == FALSE) {
        printf("[hcpptest]E failed to register feature or api! \n");
    }
}
SYSEX_FEATURE_INIT(GFeatureInit);

static DemoApi *GetIUnknown(const char *serviceName, const char *featureName)
{
    DemoApi *demoApi = nullptr;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(serviceName, featureName);
    if (iUnknown == nullptr) {
        return nullptr;
    }
    int result = iUnknown->QueryInterface(iUnknown, 0x20, (void **)&demoApi);
    if (result == 0 && demoApi != nullptr) {
        return demoApi;
    } else {
        return nullptr;
    }
}

static void ReleaseIUnknown(DemoApi *demoApi)
{
    demoApi->Release((IUnknown *)demoApi);
}

static void WaitBySem(void)
{
    struct timespec ts = {};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    sem_timedwait(&g_sem, &ts);
}

static void DemoHandlerAndCheck(const Request *request, const Response *response)
{
    char requestData[] = "I wanna async call good result!";
    ASSERT_EQ(strcmp((char *)request->data, requestData), 0);

    char responseData[] = "Yes, you did!";
    ASSERT_EQ(strcmp((char *)response->data, responseData), 0);
}

class SendRequestTest : public testing::Test {
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
        usleep(OPER_INTERVAL * MS2US);
    }
    // Testcase teardown
    virtual void TearDown()
    {
        sem_destroy(&g_sem);
    }
};

/**
 * @tc.number    : DMSLite_SAMGR_SyncCall_0010
 * @tc.name      : Service support synchronous call
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSyncCall0010, Function | MediumTest | Level1) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    char nameR[] = "I wanna sync call good result!";
    struct Payload payload = {
        .id = 0, // api will return true
        .name = nameR,
        .value = 1};
    bool result = demoApi->SyncCall((IUnknown *)demoApi, &payload);
    ASSERT_EQ(result, TRUE);

    struct Payload payloadX = {
        .id = -1,  // api will return false
        .name = nameR,
        .value = 1};
    result = demoApi->SyncCall((IUnknown *)demoApi, &payloadX);
    ASSERT_EQ(result, FALSE);

    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0010
 * @tc.name      : Service support asynchronous call
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0010, Function | MediumTest | Level1) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_NO,
                       .len = 0,
                       .data = nullptr, 
                       .msgValue = 0};
    char body[] = "I wanna async call good result!";
    request.len = strlen(body) + 1;
    request.data = malloc(request.len);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, request.len, body);
    if (error != EOK) {
        printf("[hcpptest]E failed to strcpy_s \n");
    }

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

    feature->featureCalledCount = 0;
    sem_init(&g_sem, 0, 0);
    int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, nullptr);
    ASSERT_EQ(result == 0, TRUE);

    WaitBySem();
    ASSERT_EQ(feature->featureCalledCount == 1, TRUE);
    ASSERT_EQ(strcmp(feature->latestRequest, body), 0);

    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0020
 * @tc.name      : Feature can send an asynchronous message to itself
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0020, Function | MediumTest | Level2) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_RT,
                        .len = 0,
                       .data = nullptr,
                       .msgValue = 0};
    char body[] = "I wanna async call good result!";
    request.len = (uint32_t)(strlen(body) + 1);
    request.data = malloc(request.len);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, request.len, body);
    if (error != EOK) {
        printf("[hcpptest]E failed to strcpy_s \n");
    }

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(S_SERVICE, S_FEATURE);
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

    feature->featureCalledCount = 0;
    sem_init(&g_sem, 0, 0);
    int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, nullptr);
    ASSERT_EQ(result == 0, TRUE);

    WaitBySem();
    ASSERT_EQ(feature->featureCalledCount == 1, TRUE);
    ASSERT_EQ(strcmp(feature->latestRequest, body), 0);
    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0030
 * @tc.name      : Feature failed to send an asynchronous message if the specified identity does not exist
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0030, Function | MediumTest | Level2) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_RT,
                       .len = 0,
                       .data = nullptr,
                       .msgValue = 0};
    char body[] = "I wanna async call good result!";
    request.len = (uint32_t)(strlen(body) + 1);
    request.data = malloc(request.len);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, request.len, body);
    if (error != EOK) {
        ADD_FAILURE();
    }

    Identity identityX = {-1, -200, nullptr};
    int32 result = demoApi->SAMGR_SendRequestProxy(&identityX, &request, nullptr);
    free(request.data);
    ASSERT_EQ(result != 0, TRUE);

    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0040
 * @tc.name      : Feature can send an asynchronous message which length is 0
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0040, Function | MediumTest | Level2) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_RT,
                       .len = 0,
                       .data = nullptr,
                       .msgValue = 0};
    char body[] = "";
    request.len = (uint32_t)(strlen(body) + 1);
    request.data = malloc(request.len);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, request.len, body);
    if (error != EOK) {
        ADD_FAILURE();
    }

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

    feature->featureCalledCount = 0;
    sem_init(&g_sem, 0, 0);
    int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, nullptr);
    ASSERT_EQ(result == 0, TRUE);

    WaitBySem();
    ASSERT_EQ(feature->featureCalledCount == 1, TRUE);
    ASSERT_EQ(strcmp(feature->latestRequest, body), 0);

    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0050
 * @tc.name      : Feature failed to send an asynchronous message if id or request is nullptr
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0050, Function | MediumTest | Level2) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_RT, 
                       .len = 0,
                       .data = nullptr,
                       .msgValue = 0};
    char body[] = "xxxx";
    request.len = strlen(body) + 1;
    request.data = malloc(request.len);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, request.len, body);
    if (error != EOK) {
        ADD_FAILURE();
    }

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

    int32 result = demoApi->SAMGR_SendRequestProxy(NULL, &request, nullptr);
    ASSERT_EQ(result != 0, TRUE);

    result = demoApi->SAMGR_SendRequestProxy(&feature->identity, nullptr, nullptr);
    ASSERT_EQ(result != 0, TRUE);

    free(request.data);
    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0060
 * @tc.name      : Samgr will not check the matching relationship between len and data
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0060, Function | MediumTest | Level2) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_NO,
                       .len = 0,
                       .data = nullptr,
                       .msgValue = 0};
    char body[] = "I wanna async call good result!";
    request.len = 0;
    request.data = malloc(strlen(body) + 1);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, strlen(body) + 1, body);
    if (error != EOK) {
        ADD_FAILURE();
    }
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

    int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, nullptr);
    free(request.data);
    ASSERT_EQ(result == 0, TRUE);

    Request request2 = {.msgId = MSG_NO, 
                        .len = 0,
                        .data = nullptr,
                        .msgValue = 0};
    request2.len = (uint32_t)(strlen(body) + 1);
    request2.data = malloc(request2.len);
    if (request2.data == nullptr) {
        ADD_FAILURE();
    }
    error = strcpy_s((char *)request2.data, request2.len, body);
    if (error != EOK) {
        ADD_FAILURE();
    }

    result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request2, nullptr);
    ASSERT_EQ(result == 0, TRUE);

    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0070
 * @tc.name      : Feature send an asynchronous message and register handler then can receive response
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0070, Function | MediumTest | Level2) {
    DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
    if (demoApi == nullptr) {
        ADD_FAILURE();
    }
    Request request = {.msgId = MSG_RT, 
                       .len = 0,
                       .data = nullptr,
                       .msgValue = 0};
    char body[] = "I wanna async call good result!";
    request.len = strlen(body) + 1;
    request.data = malloc(request.len);
    if (request.data == nullptr) {
        ADD_FAILURE();
    }
    errno_t error = strcpy_s((char *)request.data, request.len, body);
    if (error != EOK) {
        ADD_FAILURE();
    }

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

    sem_init(&g_sem, 0, 0);

    auto onHandlerAndCheck = [] (const Request *request, const Response *response) {
        char requestData[] = "I wanna async call good result!";
        ASSERT_EQ(strcmp((char *)request->data, requestData), 0);

        char responseData[] = "Yes, you did!";
        ASSERT_EQ(strcmp((char *)response->data, responseData), 0);
    };
    int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, onHandlerAndCheck); 
    
    WaitBySem();
    ASSERT_EQ(result, 0);
    ASSERT_EQ(strcmp(feature->latestRequest, body), 0);
    
    ReleaseIUnknown(demoApi);
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0080
 * @tc.name      : Feature send an asynchronous message repeatedly, no memory leak
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0080, Function | MediumTest | Level2) {
    for (int i = 0; i < PRESSURE_L0; i++) {
        DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
        if (demoApi == nullptr) {
            ADD_FAILURE();
        }
        Request request = {.msgId = MSG_RT, 
                           .len = 0,
                           .data = nullptr,
                           .msgValue = 0};
        char body[] = "I wanna async call good result!";
        request.len = strlen(body) + 1;
        request.data = malloc(request.len);
        if (request.data == nullptr) {
            ADD_FAILURE();
        }
        errno_t error = strcpy_s((char *)request.data, request.len, body);
        if (error != EOK) {
            ADD_FAILURE();
        }
        
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
        DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

        sem_init(&g_sem, 0, 0);
        int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, DemoHandlerAndCheck); 
        ASSERT_EQ(result, 0);

        WaitBySem();
        ASSERT_EQ(strcmp(feature->latestRequest, body), 0);

        ReleaseIUnknown(demoApi);
    }
}

/**
 * @tc.number    : DMSLite_SAMGR_SendRequest_0090
 * @tc.name      : Feature send an asynchronous message with 0 len repeatedly, no memory leak
 * @tc.desc      : [C- SOFTWARE -0200]
*/
HWTEST_F(SendRequestTest, testSendRequest0090, Function | MediumTest | Level2) {
    for (int i = 0; i < PRESSURE_L0; i++) {
        DemoApi *demoApi = GetIUnknown(S_SERVICE, S_FEATURE);
        if (demoApi == nullptr) {
            ADD_FAILURE();
        }
        Request request = {.msgId = MSG_RT, 
                           .len = 0,
                           .data = nullptr,
                           .msgValue = 0};
        char body[] = "I wanna async call good result!";
        request.len = 0;
        request.data = body;

        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(T_SERVICE, T_FEATURE);
        DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);

        sem_init(&g_sem, 0, 0);
        int32 result = demoApi->SAMGR_SendRequestProxy(&feature->identity, &request, DemoHandlerAndCheck); 
        ASSERT_EQ(result, 0);

        WaitBySem();
        ASSERT_EQ(strcmp(feature->latestRequest, body), 0);

        ReleaseIUnknown(demoApi);
    }
}
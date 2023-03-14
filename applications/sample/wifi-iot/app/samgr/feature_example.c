/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "example.h"
#include <stdint.h>
#include <ohos_init.h>
#include <securec.h>
#include <los_base.h>
#include <cmsis_os.h>
#include <samgr_maintenance.h>
#include "iunknown.h"
#include "feature.h"
#include "service.h"
#include "samgr_lite.h"
#include "time_adapter.h"

#define WAIT_FEATURE_PROC 1000

enum MessageId {
    MSG_PROC,
    MSG_TIME_PROC,
};

struct Payload {
    int id;
    const char *name;
    int value;
};

typedef struct DemoApi {
    INHERIT_IUNKNOWN;
    BOOL (*AsyncCall)(IUnknown *iUnknown, const char *buff);
    BOOL (*AsyncTimeCall)(IUnknown *iUnknown);
    BOOL (*SyncCall)(IUnknown *iUnknown, struct Payload *payload);
    BOOL (*AsyncCallBack)(IUnknown *iUnknown, const char *buff, Handler handler);
} DemoApi;

typedef struct DemoFeature {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(DemoApi);
    Identity identity;
} DemoFeature;

static BOOL AsyncCall(IUnknown *iUnknown, const char *body);
static BOOL AsyncTimeCall(IUnknown *iUnknown);
static BOOL SyncCall(IUnknown *iUnknown, struct Payload *payload);
static BOOL AsyncCallBack(IUnknown *iUnknown, const char *body, Handler handler);
static const char *FEATURE_GetName(Feature *feature);
static void FEATURE_OnInitialize(Feature *feature, Service *parent, Identity identity);
static void FEATURE_OnStop(Feature *feature, Identity identity);
static BOOL FEATURE_OnMessage(Feature *feature, Request *request);
static DemoFeature g_example = {
    .GetName = FEATURE_GetName,
    .OnInitialize = FEATURE_OnInitialize,
    .OnStop = FEATURE_OnStop,
    .OnMessage = FEATURE_OnMessage,
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .AsyncCall = AsyncCall,
    .AsyncTimeCall = AsyncTimeCall,
    .SyncCall = SyncCall,
    .AsyncCallBack = AsyncCallBack,
    DEFAULT_IUNKNOWN_ENTRY_END,
    .identity = {-1, -1, NULL},
};
static uint32_t g_regStep = 0;

static const char *FEATURE_GetName(Feature *feature)
{
    (void)feature;
    return EXAMPLE_FEATURE;
}

static void FEATURE_OnInitialize(Feature *feature, Service *parent, Identity identity)
{
    DemoFeature *demoFeature = (DemoFeature *)feature;
    demoFeature->identity = identity;
    printf("[Register Test][TaskID:%u][Step:%u][Reg Finish S:%s, F:%s]Time: %llu!\n",
           (int)osThreadGetId(), g_regStep++, parent->GetName(parent), feature->GetName(feature),
           SAMGR_GetProcessTime());
}

static void FEATURE_OnStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
    g_example.identity.queueId = NULL;
    g_example.identity.featureId = -1;
    g_example.identity.serviceId = -1;
}

static volatile uint32 g_asyncStep = 0;

static BOOL FEATURE_OnMessage(Feature *feature, Request *request)
{
    (void)feature;
    if (request->msgId == MSG_PROC) {
        printf("[LPC Test][TaskID:%u][Step:%u][OnMessage: S:%s, F:%s] msgId<MSG_PROC> %s \n",
               (int)osThreadGetId(), g_asyncStep++, EXAMPLE_SERVICE, feature->GetName(feature),
               (char *)request->data);
        Response response = {.data = "Yes, you did!", .len = 0};
        SAMGR_SendResponse(request, &response);
        return TRUE;
    } else {
        if (request->msgId == MSG_TIME_PROC) {
            LOS_Msleep(WAIT_FEATURE_PROC * 11); // sleep 11 seconds
            printf("[LPC Test][TaskID:%u][OnMessage: S:%s, F:%s] Time Message Get Value<%s>!",
                (int)osThreadGetId(), EXAMPLE_SERVICE, feature->GetName(feature),
                request->msgValue ? "TRUE" : "FALSE");
            AsyncTimeCall(GET_IUNKNOWN(g_example));
            return FALSE;
        }
    }
    printf("[LPC Test][TaskID:%u][Step:%u][OnMessage S:%s, F:%s] Inner Error! \n",
           (int)osThreadGetId(), g_asyncStep++, EXAMPLE_SERVICE, feature->GetName(feature));
    return FALSE;
}

static BOOL SyncCall(IUnknown *iUnknown, struct Payload *payload)
{
    (void)iUnknown;
    if (payload != NULL && payload->id >= 0 && payload->name != NULL) {
        printf("[LPC Test][TaskID:%u][Step:%u][SyncCall API] Id:%d, name:%s, value:%d \n",
               (int)osThreadGetId(), g_asyncStep++, payload->id, payload->name, payload->value);
        return TRUE;
    }
    printf("[LPC Test][TaskID:%u][Step:%u][SyncCall API] Input Error! \n", (int)osThreadGetId(), g_asyncStep++);
    return FALSE;
}

static BOOL AsyncCall(IUnknown *iUnknown, const char *body)
{
    Request request = {.msgId = MSG_PROC, .msgValue = 0};
    request.len = (uint32_t)(strlen(body) + 1);
    request.data = malloc(request.len);
    if (request.data == NULL) {
        return FALSE;
    }
    if (strcpy_s(request.data, request.len, body) != EOK) {
        free(request.data);
        return FALSE;
    }
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCall API] Send request! \n", (int)osThreadGetId(), g_asyncStep++);
    return SAMGR_SendRequest(&feature->identity, &request, NULL);
}

static BOOL AsyncTimeCall(IUnknown *iUnknown)
{
    static int8 cnt = 0;
    cnt = (cnt + 1) % 2; // mod 2 to get async status
    Request request = {.msgId = MSG_TIME_PROC, .msgValue = cnt};
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);
    return SAMGR_SendRequest(&feature->identity, &request, NULL);
}

static BOOL AsyncCallBack(IUnknown *iUnknown, const char *body, Handler handler)
{
    Request request = {.msgId = MSG_PROC, .msgValue = 0};
    request.len = (uint32_t)(strlen(body) + 1);
    request.data = malloc(request.len);
    if (request.data == NULL) {
        return FALSE;
    }
    if (strcpy_s(request.data, request.len, body) != EOK) {
        free(request.data);
        return FALSE;
    }
    DemoFeature *feature = GET_OBJECT(iUnknown, DemoFeature, iUnknown);
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCallBack API] Send request! \n",
           (int)osThreadGetId(), g_asyncStep++);
    return SAMGR_SendRequest(&feature->identity, &request, handler);
}

static void Init(void)
{
    SAMGR_GetInstance()->RegisterFeature(EXAMPLE_SERVICE, (Feature *)&g_example);
    SAMGR_GetInstance()->RegisterFeatureApi(EXAMPLE_SERVICE, EXAMPLE_FEATURE, GET_IUNKNOWN(g_example));
    printf("[Register Test][TaskID:%u][Step:%u][Reg S:%s, F:%s]Time: %llu!\n",
           (int)osThreadGetId(), g_regStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE, SAMGR_GetProcessTime());
}

SYSEX_FEATURE_INIT(Init);

static uint32_t g_discoverStep = 0;

static DemoApi *CASE_GetIUnknown(void)
{
    DemoApi *demoApi = NULL;
    printf("[Discover Test][TaskID:%u][Step:%u][GetIUnknown S:%s, F:%s]: BEGIN\n",
           (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(EXAMPLE_SERVICE, EXAMPLE_FEATURE);
    if (iUnknown == NULL) {
        printf("[Discover Test][TaskID:%u][Step:%u][GetFeatureApi S:%s, F:%s]Error is NULL!\n",
               (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
        goto END;
    }
    int result = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&demoApi);
    if (result != 0 || demoApi == NULL) {
        printf("[Discover Test][TaskID:%u][Step:%u][QueryInterface S:%s, F:%s]Error is NULL!\n",
               (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
        goto END;
    }
    printf("[Discover Test][TaskID:%u][Step:%u][GetIUnknown S:%s, F:%s]Success\n",
           (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
END:
    printf("[Discover Test][TaskID:%u][Step:%u][GetIUnknown S:%s, F:%s]: END\n",
           (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
    return demoApi;
}

static void CASE_SyncCall(DemoApi *demoApi)
{
    if (demoApi->SyncCall == NULL) {
        return;
    }
    printf("[LPC Test][TaskID:%u][Step:%u][SyncCall]: BEGIN! \n", (int)osThreadGetId(), g_asyncStep++);
    struct Payload payload = {
        .id = 0,
        .name = "I want to sync call good result!",
        .value = 1
    };
    if (!demoApi->SyncCall((IUnknown *)demoApi, &payload)) {
        printf("[LPC Test][TaskID:%u][Step:%u][SyncCall]Error return false! \n",
               (int)osThreadGetId(), g_asyncStep++);
        goto END;
    }
    printf("[LPC Test][TaskID:%u][Step:%u][SyncCall]Success!\n",
           (int)osThreadGetId(), g_asyncStep++);
END:
    printf("[LPC Test][TaskID:%u][Step:%u][SyncCall]: END\n",
           (int)osThreadGetId(), g_asyncStep++);
}


static void CASE_AsyncCall(DemoApi *demoApi)
{
    if (demoApi->AsyncCall == NULL) {
        return;
    }
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCall]: BEGIN! \n", (int)osThreadGetId(), g_asyncStep++);
    uint32 count = g_asyncStep;
    demoApi->AsyncCall((IUnknown *)demoApi, "I want to async call good result!");
    LOS_Msleep(WAIT_FEATURE_PROC);
    if (count == g_asyncStep) {
        printf("[LPC Test][TaskID:%u][Step:%u][AsyncCall] result is failed! \n", (int)osThreadGetId(),
               g_asyncStep++);
        goto END;
    }
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCall]Success! \n", (int)osThreadGetId(), g_asyncStep++);
END:
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCall]: END! \n", (int)osThreadGetId(), g_asyncStep++);
}

static void CASE_AsyncTimeCall(DemoApi *demoApi)
{
    if (demoApi->AsyncCall == NULL) {
        return;
    }
    demoApi->AsyncTimeCall((IUnknown *)demoApi);
}

void AsyncHandler(const Request *request, const Response *response)
{
    (void)request;
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCallBack]Success! Response:%s \n",
           (int)osThreadGetId(), g_asyncStep++, (char *)response->data);
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCallBack]: END! \n", (int)osThreadGetId(), g_asyncStep++);
}

static void CASE_AsyncCallBack(DemoApi *demoApi)
{
    if (demoApi->AsyncCallBack == NULL) {
        return;
    }
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCallBack]: BEGIN! \n", (int)osThreadGetId(), g_asyncStep++);
    demoApi->AsyncCallBack((IUnknown *)demoApi, "I want to async call callback good result!", AsyncHandler);
    printf("[LPC Test][TaskID:%u][Step:%u][AsyncCallBack]Wait for response! \n",
           (int)osThreadGetId(), g_asyncStep++);
}

static void CASE_ReleaseIUnknown(DemoApi *demoApi)
{
    printf("[Discover Test][TaskID:%u][Step:%u][ReleaseIUnknown S:%s, F:%s]: BEGIN\n",
           (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
    int32 ref = demoApi->Release((IUnknown *)demoApi);
    if (ref <= 0) {
        printf("[Discover Test][TaskID:%u][Step:%u][ReleaseIUnknown S:%s, F:%s]Error ref is %d!\n",
               (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE, ref);
        goto END;
    }
    printf("[Discover Test][TaskID:%u][Step:%u][ReleaseIUnknown S:%s, F:%s]Success\n",
           (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
END:
    printf("[Discover Test][TaskID:%u][Step:%u][ReleaseIUnknown S:%s, F:%s]: END\n",
           (int)osThreadGetId(), g_discoverStep++, EXAMPLE_SERVICE, EXAMPLE_FEATURE);
}

static void RunTestCase(void)
{
    DemoApi *demoApi = CASE_GetIUnknown();
    CASE_SyncCall(demoApi);
    CASE_AsyncCall(demoApi);
    CASE_AsyncCallBack(demoApi);
    CASE_AsyncTimeCall(demoApi);
    CASE_ReleaseIUnknown(demoApi);
}

LAYER_INITCALL_DEF(RunTestCase, test, "test");

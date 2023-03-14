/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_message_test.h"
#include "message/message_types.h"
#include "message/message_router.h"
#include "message/sidecar.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "osal_time.h"

const uint32_t SEND_MESSAGE_COUNT = 40000;
const uint32_t SYNC_MESSAGE_TIMEOUT = 2;
const uint32_t ASYNC_MESSAGE_TIMEOUT = 8;
#define COMMON_SEM_TIMEOUT 300

enum ServiceList {
    SERVICE_ID_A = 10,
    SERVICE_ID_B
};

const uint8_t CUSTOM_DISPATCHER_ID = 1;
const uint8_t SINGLE_NODE_TEST_CUSTOM_DISPATCHER_PRIORITYLEVEL = 4;
const uint32_t SINGLE_NODE_TEST_CUSTOM_DISPATCHER_QUEUESIZE = 10000;

const uint16_t SMALL_LOAD_WAIT_TIME = 500;


#define CMD_LIST_MAX_SIZE 32
uint8_t g_cmdList[CMD_LIST_MAX_SIZE];
uint8_t g_cmdListCount = 0;

void ClearRecvQueue(void)
{
    g_cmdListCount = 0;
}

uint8_t GetCMDByIndex(uint8_t index)
{
    return g_cmdList[index];
}

size_t GetRecvQueueSize(void)
{
    return g_cmdListCount;
}

static ErrorCode FuncNoLoad(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    (void)reqData;
    (void)rspData;
    (void)context;
    return ME_SUCCESS;
}

static ErrorCode FuncSmallLoad(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    if (context == NULL) {
        HDF_LOGE("%s:FuncSmallLoad context NULL!", __func__);
        return HDF_FAILURE;
    }
    (void)reqData;
    (void)rspData;
    HDF_LOGI("Handle message %d\n", context->commandId);

    OsalMSleep(SMALL_LOAD_WAIT_TIME);
    if (g_cmdListCount >= CMD_LIST_MAX_SIZE) {
        HDF_LOGE("%s:Too much cmd!", __func__);
        return HDF_FAILURE;
    }
    g_cmdList[g_cmdListCount++] = context->commandId;
    HDF_LOGI("Finish handle message %d\n", context->commandId);
    return ME_SUCCESS;
}

static struct MessageDef g_testServiceACmds[] = {
    DUEMessage(0, FuncNoLoad, 1)
};

ServiceDefine(TestServiceA, SERVICE_ID_A, g_testServiceACmds);

static struct MessageDef g_testServiceBCmds[] = {
    DUEMessage(0, FuncNoLoad, 2), // commandId = 0
    DUEMessage(1, FuncSmallLoad, 0),
    DUEMessage(2, FuncSmallLoad, 1),
    DUEMessage(3, FuncSmallLoad, 2),
    DUEMessage(4, FuncSmallLoad, 3),
};

ServiceDefine(TestServiceB, SERVICE_ID_B, g_testServiceBCmds);

Service *g_serviceA = NULL;
Service *g_serviceB = NULL;
bool g_dispatcherInited = false;

int32_t StartEnv(void)
{
    int32_t errCode;
    if (!g_dispatcherInited) {
        DispatcherConfig config = {
            .dispatcherId = CUSTOM_DISPATCHER_ID,
            .priorityLevelCount = SINGLE_NODE_TEST_CUSTOM_DISPATCHER_PRIORITYLEVEL,
            .queueSize = SINGLE_NODE_TEST_CUSTOM_DISPATCHER_QUEUESIZE
        };
        MSG_RETURN_IF_FUNCTION_FAILED(errCode, AddDispatcher(&config));
        g_dispatcherInited = true;
    }
    if (g_serviceA == NULL) {
        ServiceCfg cfgA = {
            .dispatcherId = DEFAULT_DISPATCHER_ID
        };
        g_serviceA = CreateService(TestServiceA, &cfgA);
        MSG_RETURN_IF(g_serviceA == NULL);
    }

    if (g_serviceB == NULL) {
        ServiceCfg cfgB = {
            .dispatcherId = CUSTOM_DISPATCHER_ID
        };
        g_serviceB = CreateService(TestServiceB, &cfgB);
        MSG_RETURN_IF(g_serviceB == NULL);
    }
    return HDF_SUCCESS;
}

int32_t StopEnv(void)
{
    if (g_serviceA != NULL) {
        if (g_serviceA->Destroy != NULL) {
            g_serviceA->Destroy(g_serviceA);
        }
        g_serviceA = NULL;
    }

    if (g_serviceB != NULL) {
        if (g_serviceB->Destroy != NULL) {
            g_serviceB->Destroy(g_serviceB);
        }
        g_serviceB = NULL;
    }
    return HDF_SUCCESS;
}

// Repeated register
int32_t MessageSingleNodeTest001(void)
{
    ErrorCode errCode;
    ErrorCode errShutdown;
    Service *service = NULL;

    do {
        ServiceCfg cfgB = {
            .dispatcherId = CUSTOM_DISPATCHER_ID
        };

        MSG_RETURN_IF_FUNCTION_FAILED(errCode, StartEnv());

        service = CreateService(TestServiceB, &cfgB);
        MSG_BREAK_IF(errCode, service != NULL);
    } while (false);

    MSG_RETURN_IF_FUNCTION_FAILED(errShutdown, StopEnv());
    if (service != NULL) {
        if (service->Destroy != NULL) {
            service->Destroy(service);
        }
        service = NULL;
    }
    return errCode;
}

// Sync message test
int32_t MessageSingleNodeTest002(void)
{
    ErrorCode errCode;
    ErrorCode errShutdown;
    struct HdfSBuf *rspData = NULL;

    do {
        MSG_BREAK_IF_FUNCTION_FAILED(errCode, StartEnv());

        MSG_BREAK_IF(errCode, g_serviceA == NULL);

        rspData = HdfSbufObtainDefaultSize();

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, g_serviceA->SendSyncMessage(g_serviceA, SERVICE_ID_B, 0, NULL, rspData));
    } while (false);

    if (rspData != NULL) {
        HdfSbufRecycle(rspData);
        rspData = NULL;
    }

    MSG_RETURN_IF_FUNCTION_FAILED(errShutdown, StopEnv());

    return errCode;
}


// Sync Perf test
int32_t MessageSingleNodeTest003(void)
{
    ErrorCode errCode;
    ErrorCode errShutdown;
    uint32_t i;
    struct HdfSBuf *rspData = NULL;
    struct HdfSBuf *sendData = NULL;
    do {
        OsalTimespec startTime;
        OsalTimespec endTime;
        OsalTimespec diffTime;
        MSG_BREAK_IF_FUNCTION_FAILED(errCode, StartEnv());
        MSG_BREAK_IF(errCode, g_serviceA == NULL);
        rspData = HdfSbufObtainDefaultSize();
        sendData = HdfSbufObtainDefaultSize();

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalGetTime(&startTime));
        for (i = 0; i < SEND_MESSAGE_COUNT; i++) {
            MSG_BREAK_IF_FUNCTION_FAILED(errCode,
                g_serviceA->SendSyncMessage(g_serviceA, SERVICE_ID_B, 0, sendData, rspData));
        }
        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalGetTime(&endTime));

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalDiffTime(&startTime, &endTime, &diffTime));

        HDF_LOGI("Process time %llu \n", diffTime.sec);
        MSG_BREAK_IF(errCode, diffTime.sec > SYNC_MESSAGE_TIMEOUT);
    } while (false);

    if (rspData != NULL) {
        HdfSbufRecycle(rspData);
        rspData = NULL;
    }

    if (sendData != NULL) {
        HdfSbufRecycle(sendData);
        sendData = NULL;
    }

    MSG_RETURN_IF_FUNCTION_FAILED(errShutdown, StopEnv());

    return errCode;
}

OSAL_DECLARE_SEMAPHORE(g_callBackSem);

void SendMessageTestCallBack(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData,
    ErrorCode rspCode)
{
    if (context == NULL) {
        HDF_LOGE("%s:SendMessageTestCallBack context NULL!", __func__);
        return;
    }
    (void)reqData;
    (void)rspData;
    (void)rspCode;
    OsalSemPost(&g_callBackSem);
    HDF_LOGI("Receive response for CMD %d.\n", context->commandId);
}

int32_t MessageSingleNodeTest004(void)
{
    ErrorCode errCode;
    ErrorCode errShutdown = 0;

    do {
        MSG_BREAK_IF_FUNCTION_FAILED(errCode, StartEnv());
        MSG_BREAK_IF(errCode, g_serviceA == NULL);

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalSemInit(&g_callBackSem, 0));

        MSG_BREAK_IF_FUNCTION_FAILED(errCode,
            g_serviceA->SendAsyncMessage(g_serviceA, SERVICE_ID_B, 0, NULL, SendMessageTestCallBack));

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalSemWait(&g_callBackSem, COMMON_SEM_TIMEOUT));
    } while (false);
    errShutdown = errShutdown | OsalSemDestroy(&g_callBackSem);
    errShutdown = errShutdown | StopEnv();

    return errCode;
}

void SendMessagePerfTestCallBack(const RequestContext *context, struct HdfSBuf *reqData, struct HdfSBuf *rspData,
    ErrorCode rspCode)
{
    ErrorCode errCode = OsalSemPost(&g_callBackSem);
    (void)context;
    if (HDF_SUCCESS != errCode) {
        HDF_LOGE("%s:Post sem failed!ret=%d", __func__, errCode);
    }
}

const uint32_t PERF_TEST_COUNT = 10000;

int32_t MessageSingleNodeTest005(void)
{
    ErrorCode errCode;
    ErrorCode errShutdown = 0;
    uint32_t i;

    do {
        OsalTimespec startTime;
        OsalTimespec endTime;
        OsalTimespec diffTime;
        MSG_BREAK_IF_FUNCTION_FAILED(errCode, StartEnv());
        MSG_BREAK_IF(errCode, g_serviceA == NULL);

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalSemInit(&g_callBackSem, 0));

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalGetTime(&startTime));

        for (i = 0; i < PERF_TEST_COUNT; i++) {
            MSG_BREAK_IF_FUNCTION_FAILED(errCode,
                g_serviceA->SendAsyncMessage(g_serviceA, SERVICE_ID_B, 0, NULL, SendMessagePerfTestCallBack));
        }

        for (i = 0; i < PERF_TEST_COUNT; i++) {
            MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalSemWait(&g_callBackSem, COMMON_SEM_TIMEOUT));
        }

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalGetTime(&endTime));

        MSG_BREAK_IF_FUNCTION_FAILED(errCode, OsalDiffTime(&startTime, &endTime, &diffTime));

        HDF_LOGW("Process time %llu \n", diffTime.sec);
        MSG_BREAK_IF(errCode, diffTime.sec > ASYNC_MESSAGE_TIMEOUT);
    } while (false);
    errShutdown = errShutdown | OsalSemDestroy(&g_callBackSem);
    errShutdown = errShutdown | StopEnv();

    return errCode;
}

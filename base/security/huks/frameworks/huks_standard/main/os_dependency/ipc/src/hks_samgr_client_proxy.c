/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_samgr_client.h"
#include "liteipc_adapter.h"

#define PER_SECOND 1000000 /* OS_SYS_US_PER_SECOND */
#define SLEEP_SECOND_TIME (0.1 * PER_SECOND)
#define SLEEP_SECOND_TIMEOUT (0.1 * PER_SECOND)
#define IPC_MSG_OBJ_SZ_MAX 1

volatile struct HksBlob g_outBlob;
volatile bool g_messageRecived = false;
volatile int32_t g_result = HKS_ERROR_UNKNOWN_ERROR;
volatile int32_t g_sleepTimeout = SLEEP_SECOND_TIMEOUT;

static void UpdaeGlobalStatus(uint32_t outBlobSize, bool messageRecived, int32_t result, int32_t sleepTime)
{
    g_outBlob.size = outBlobSize;
    g_result = result;
    g_sleepTimeout = sleepTime;
    g_messageRecived = messageRecived;
}

static int32_t SynchronizeOutput(struct HksBlob *outBlob)
{
    while (!g_messageRecived) {
        g_sleepTimeout -= SLEEP_SECOND_TIME;
        usleep(SLEEP_SECOND_TIME);
        if (g_sleepTimeout < 0) {
            /* Check g_outBlob.data and free mem */
            UpdaeGlobalStatus(0, false, g_result, SLEEP_SECOND_TIMEOUT);
            HKS_FREE_PTR(g_outBlob.data);
            return HKS_ERROR_COMMUNICATION_TIMEOUT;
        }
    }
    if ((outBlob != NULL) && (g_outBlob.data != NULL) && (g_outBlob.size != 0)) {
        if (outBlob->size < g_outBlob.size) {
            HKS_FREE_PTR(g_outBlob.data);
            g_outBlob.size = 0;
            return HKS_ERROR_INVALID_ARGUMENT;
        } else {
            if (memcpy_s(outBlob->data, outBlob->size, g_outBlob.data, g_outBlob.size) != EOK) {
                HKS_FREE_PTR(g_outBlob.data);
                g_outBlob.size = 0;
                return HKS_ERROR_INVALID_ARGUMENT;
            }
            outBlob->size = g_outBlob.size;
            HKS_FREE_PTR(g_outBlob.data);
            g_outBlob.size = 0;
            return HKS_SUCCESS;
        }
    }
    UpdaeGlobalStatus(g_outBlob.size, false, g_result, SLEEP_SECOND_TIMEOUT);
    return g_result;
}

static int CurrentCallback(IOwner owner, int code, IpcIo *reply)
{
    g_result = IpcIoPopInt32(reply);
    BOOL isNoneResponse = IpcIoPushBool(reply);
    if (isNoneResponse) {
        UpdaeGlobalStatus(0, true, g_result, SLEEP_SECOND_TIMEOUT);
        g_outBlob.data = NULL;
        return HKS_SUCCESS;
    }
    BuffPtr *buffRsv = IpcIoPopDataBuff(reply);
    if (buffRsv == NULL) {
        g_messageRecived = true;
        return HKS_ERROR_NULL_POINTER;
    }
    size_t len = buffRsv->buffSz;
    if ((buffRsv->buff == NULL) || (len == 0)) {
        g_messageRecived = true;
        return HKS_ERROR_IPC_MSG_FAIL;
    }
    uint8_t *dataBuf = HksMalloc(len);
    if (dataBuf == NULL) {
        g_messageRecived = true;
        return HKS_ERROR_NULL_POINTER;
    }
    if (memcpy_s(dataBuf, len, buffRsv->buff, len) != EOK) {
        HKS_FREE_PTR(dataBuf);
        g_messageRecived = true;
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    FreeBuffer(NULL, buffRsv->buff);
    g_outBlob.data = dataBuf;
    UpdaeGlobalStatus(len, true, g_result, SLEEP_SECOND_TIMEOUT);
    return HKS_SUCCESS;
}

static int32_t IpcAsyncCall(IUnknown *iUnknown, enum HksMessage type, const struct HksBlob *inBlob,
    struct HksBlob *outBlob);
{
    /* Check input and inBlob */
    int32_t ret = CheckBlob(inBlob);
    HksMgrClientApi *proxy = (HksMgrClientApi *)iUnknown;
    if ((ret != HKS_SUCCESS) || (proxy == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    /* Init Global Variables and int g_outBlob dataBuff to NULL */
    UpdaeGlobalStatus(0, false, HKS_ERROR_UNKNOWN_ERROR, SLEEP_SECOND_TIMEOUT);
    HKS_FREE_PTR(g_outBlob.data);

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_MSG_OBJ_SZ_MAX);

    /* Construct the data to be sent */
    uint32_t len = inBlob->size;
    uint8_t *dataBuff = HksMalloc(len);
    if (dataBuff == NULL) {
        HKS_FREE_PTR(g_outBlob.data);
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }
    const BuffPtr requestBuff = { len, (const void*)dataBuff };

    IpcIoPushDataBuffWithFree(&request, &requestBuff, HksFree);
    ret = (int32_t)proxy->Invoke((IClientProxy *)proxy, type, &request, NULL, CurrentCallback);
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return SynchronizeOutput(outBlob);
}

void *HKS_CreatClient(const char *service, const char *feature, uint32 size)
{
    (void)service;
    (void)feature;
    uint32 len = size + sizeof(HksMgrClientEntry);
    uint8 *client = malloc(len);
    if (client == NULL) {
        return NULL;
    }
    (void)memset_s(client, len, 0, len);
    HksMgrClientEntry *entry = (HksMgrClientEntry *)&client[size];
    entry->ver = ((uint16)CLIENT_PROXY_VER | (uint16)DEFAULT_VERSION);
    entry->ref = 1;
    entry->iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    entry->iUnknown.AddRef = IUNKNOWN_AddRef;
    entry->iUnknown.Release = IUNKNOWN_Release;
    entry->iUnknown.Invoke = NULL;
    entry->iUnknown.IpcAsyncCallBack = IpcAsyncCall;
    return client;
}

void HKS_DestroyClient(const char *service, const char *feature, void *iproxy)
{
    free(iproxy);
}

int32_t HksSamgrInitialize(void)
{
    int32_t ret = SAMGR_RegisterFectory(HKS_SAMGR_SERVICE, HKS_SAMGR_FEATRURE, HKS_CreatClient, HKS_DestroyClient);
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    SAMGR_Bootstrap();
    return HKS_SUCCESS;
}

static int32_t HksSendRequestSync(enum HksMessage type, const struct HksBlob *inBlob, struct HksBlob *outBlob)
{
    HksMgrClientApi *clientProxy;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(HKS_SAMGR_SERVICE, HKS_SAMGR_FEATRURE);
    if (iUnknown == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    int32_t ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&clientProxy);
    if ((clientProxy == NULL) || (ret != HKS_SUCCESS)) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (clientProxy->IpcAsyncCallBack == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    ret = clientProxy->IpcAsyncCallBack((IUnknown *)clientProxy, type, inBlob, outBlob);
    (void)clientProxy->Release((IUnknown *)clientProxy);
    return ret;
}

int32_t HksSendRequest(enum HksMessage type, const struct HksBlob *inBlob, struct HksBlob *outBlob,
    const struct HksParamSet *paramSet)
{
    (void)paramSet;
    return HksSendRequestSync(type, inBlob, outBlob);
}

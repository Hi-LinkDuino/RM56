/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hks_samgr_server.h"
#include "hks_ipc_service.h"

static const char *FEATURE_GetName(Feature *feature);
static void FEATURE_OnInitialize(Feature *feature, Service *parent, Identity identity);
static void FEATURE_OnStop(Feature *feature, Identity identity);
static BOOL FEATURE_OnMessage(Feature *feature, Request *request);
static int32 Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply);

static HksMgrFeature g_hksMgrFeature = {
    .GetName = FEATURE_GetName,
    .OnInitialize = FEATURE_OnInitialize,
    .OnStop = FEATURE_OnStop,
    .OnMessage = FEATURE_OnMessage,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    IPROXY_END,
    .identity = { -1, -1, NULL },
};
static const char *FEATURE_GetName(Feature *feature)
{
    (void)feature;
    return HKS_SAMGR_FEATRURE;
}

static void FEATURE_OnInitialize(Feature *feature, Service *parent, Identity identity)
{
    if (feature == NULL) {
        return;
    }
    HksMgrFeature *hksMgrFeature = (HksMgrFeature *)feature;
    hksMgrFeature->identity = identity;
    hksMgrFeature->parent = parent;
}

static void FEATURE_OnStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
    g_hksMgrFeature.identity.queueId = NULL;
    g_hksMgrFeature.identity.featureId = -1;
    g_hksMgrFeature.identity.serviceId = -1;
}

static BOOL FEATURE_OnMessage(Feature *feature, Request *msg)
{
    (void)feature;
    if (msg == NULL) {
        return false;
    }
    Response response = { "Yes, you did!", 0 };
    return SAMGR_SendResponse(msg, &response);
}

static int32 Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    BuffPtr* buffRsv = IpcIoPopDataBuff(req);
    if (buffRsv == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    uint32_t len = buffRsv->buffSz;
    struct HksBlob srcData = { len, NULL };
    srcData.data = HksMalloc(len);
    if (srcData.data == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    if (memcpy_s(srcData.data, len, buffRsv->buff, len) != EOK) {
        HKS_FREE_PTR(srcData.data);
        srcData.size = 0;
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }
    FreeBuffer(NULL, buffRsv->buff);
    uint32_t callingUid = GetCallingUid(origin);
    HksIpcContext ipcContext = { reply, callingUid };
    uint32_t size = sizeof(g_hksIpcMessageHandler) / sizeof(g_hksIpcMessageHandler[0]);
    for (uint32_t i = 0; i < size; ++i) {
        if (funcId == g_hksIpcMessageHandler[i].msgId) {
            g_hksIpcMessageHandler[i].handler(&srcData, (const uint8_t *)&ipcContext);
            break;
        }
    }
    HKS_FREE_PTR(srcData.data);
    srcData.size = 0;
    return HKS_SUCCESS;
}

static void Init(void)
{
    SAMGR_GetInstance()->RegisterFeature(HKS_SAMGR_SERVICE, (Feature *)&g_hksMgrFeature);
    SAMGR_GetInstance()->RegisterFeatureApi(HKS_SAMGR_SERVICE, HKS_SAMGR_FEATRURE, GET_IUNKNOWN(g_hksMgrFeature));
}
SYSEX_FEATURE_INIT(Init);
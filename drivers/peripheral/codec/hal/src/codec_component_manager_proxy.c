/*
 * Copyright (c) 2022 Shenzhen Kaihong DID Co., Ltd.
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

#include <hdf_log.h>
#include <osal_mem.h>
#include <servmgr_hdi.h>
#include "codec_component_manager.h"
#include "codec_internal.h"
#include "codec_types.h"

#define HDF_LOG_TAG codec_hdi_client

struct CodecComponentManagerProxy {
    struct CodecComponentManager instance;
    struct HdfRemoteService *remoteOmx;
};

static struct CodecComponentManagerProxy g_codecComponentManagerProxy = {0};

static void ReleaseSbuf(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
}

static int32_t GetComponentNum()
{
    int32_t num = 0;
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain", __func__);
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (reply == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain reply", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(g_codecComponentManagerProxy.remoteOmx, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        return HDF_FAILURE;
    }

    if (g_codecComponentManagerProxy.remoteOmx->dispatcher->Dispatch(g_codecComponentManagerProxy.remoteOmx,
        CMD_CODEC_GET_COMPONENT_NUM, data, reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: dispatch request failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadInt32(reply, &num)) {
        HDF_LOGE("%{public}s: read dataBlock->role failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    ReleaseSbuf(data, reply);
    return num;
}

static int32_t GetComponentCapabilityList(CodecCompCapability *capList, int32_t count)
{
    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain", __func__);
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (reply == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain reply", __func__);
        HdfSbufRecycle(data);
        return HDF_FAILURE;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(g_codecComponentManagerProxy.remoteOmx, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        return HDF_FAILURE;
    }

    if (g_codecComponentManagerProxy.remoteOmx->dispatcher->Dispatch(g_codecComponentManagerProxy.remoteOmx,
        CMD_CODEC_GET_COMPONENT_CAPABILITY_LIST, data, reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: dispatch request failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (count <= 0) {
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    for (int32_t i = 0; i < count; i++) {
        if (!CodecCompCapabilityBlockUnmarshalling(reply, &(capList)[i])) {
            HDF_LOGE("%{public}s: read capbility %{public}d from sbuf failed!", __func__, i);
            ReleaseSbuf(data, reply);
            return HDF_FAILURE;
        }
    }
    
    ReleaseSbuf(data, reply);
    return HDF_SUCCESS;
}

static int32_t CreateComponent(struct CodecComponentType **component, char *compName, void *appData,
    int32_t appDataSize, struct CodecCallbackType *callback)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(g_codecComponentManagerProxy.remoteOmx, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, compName)) {
        HDF_LOGE("%{public}s: write paramName failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    int8_t *priAppData = (int8_t*)appData;
    if (!HdfSbufWriteInt32(data, appDataSize)) {
        HDF_LOGE("%{public}s: write appDataSize failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < appDataSize; i++) {
        if (!HdfSbufWriteInt8(data, priAppData[i])) {
            HDF_LOGE("%{public}s: write priAppData[%{public}d] failed!", __func__, i);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    if (HdfSbufWriteRemoteService(data, callback->remote) != 0) {
        HDF_LOGE("%{public}s: write callback failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = g_codecComponentManagerProxy.remoteOmx->dispatcher->Dispatch(g_codecComponentManagerProxy.remoteOmx,
        CMD_CREATE_COMPONENT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }
    *component = CodecComponentTypeGet(g_codecComponentManagerProxy.remoteOmx);

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t DestoryComponent(struct CodecComponentType *component)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(g_codecComponentManagerProxy.remoteOmx, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        return HDF_FAILURE;
    }

    ret = g_codecComponentManagerProxy.remoteOmx->dispatcher->Dispatch(g_codecComponentManagerProxy.remoteOmx,
        CMD_DESTROY_COMPONENT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }
    CodecComponentTypeRelease(component);

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t InitCodecComponentManagerProxy(void)
{
    if (g_codecComponentManagerProxy.remoteOmx != NULL) {
        return HDF_SUCCESS;
    }

    struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    if (serviceMgr == NULL) {
        HDF_LOGE("%{public}s: HDIServiceManager not found!", __func__);
        return HDF_FAILURE;
    }

    struct HdfRemoteService *remoteOmx = serviceMgr->GetService(serviceMgr, CODEC_HDI_OMX_SERVICE_NAME);
    HDIServiceManagerRelease(serviceMgr);
    if (remoteOmx == NULL) {
        HDF_LOGE("%{public}s: CodecComponentTypeService not found!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(remoteOmx, "ohos.hdi.codec_service")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remoteOmx);
        return HDF_FAILURE;
    }

    g_codecComponentManagerProxy.remoteOmx = remoteOmx;
    g_codecComponentManagerProxy.instance.GetComponentNum = GetComponentNum;
    g_codecComponentManagerProxy.instance.GetComponentCapabilityList = GetComponentCapabilityList;
    g_codecComponentManagerProxy.instance.CreateComponent = CreateComponent;
    g_codecComponentManagerProxy.instance.DestoryComponent = DestoryComponent;

    return HDF_SUCCESS;
}

struct CodecComponentManager *GetCodecComponentManager(void)
{
    if (InitCodecComponentManagerProxy() != HDF_SUCCESS) {
        return NULL;
    }
    return &g_codecComponentManagerProxy.instance;
}

void CodecComponentManagerRelease(void)
{
    if (g_codecComponentManagerProxy.remoteOmx != NULL) {
        HdfRemoteServiceRecycle(g_codecComponentManagerProxy.remoteOmx);
    }
}
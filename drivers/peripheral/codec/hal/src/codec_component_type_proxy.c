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

#include <hdf_dlist.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include <securec.h>
#include <servmgr_hdi.h>
#include "codec_component_if.h"
#include "codec_internal.h"
#include "codec_types.h"

#define HDF_LOG_TAG codec_hdi_client

struct CodecComponentTypeProxy {
    struct CodecComponentType instance;
    struct HdfRemoteService *remote;
};

static void ReleaseSbuf(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
}

static int32_t WriteInterfaceToken(struct CodecComponentType *self, struct HdfSBuf *data)
{
    struct CodecComponentTypeProxy *proxy = CONTAINER_OF(self, struct CodecComponentTypeProxy, instance);
    if (proxy == NULL) {
        HDF_LOGE("%{public}s: proxy is null", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(proxy->remote, data)) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t CodecComponentTypeProxyCall(struct CodecComponentType *self, int32_t id, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    struct CodecComponentTypeProxy *proxy = CONTAINER_OF(self, struct CodecComponentTypeProxy, instance);
    if (proxy->remote == NULL ||
        proxy->remote->dispatcher == NULL ||
        proxy->remote->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%{public}s: obj is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return proxy->remote->dispatcher->Dispatch(proxy->remote, id, data, reply);
}

static int32_t ReadValuesForGetComponentVersion(struct HdfSBuf *reply, char *compName,
    union OMX_VERSIONTYPE *compVersion, union OMX_VERSIONTYPE *specVersion, uint8_t *compUUID)
{
    int32_t ret;

    const char *componentNameCopy = HdfSbufReadString(reply);
    if (componentNameCopy == NULL) {
        HDF_LOGE("%{public}s: read componentNameCopy failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = strcpy_s(compName, OMX_MAX_STRINGNAME_SIZE, componentNameCopy);
    if (ret != EOK) {
        HDF_LOGE("%{public}s: strcpy_s compName failed, error code: %{public}d", __func__, ret);
        return HDF_FAILURE;
    }

    const union OMX_VERSIONTYPE *componentVersionCp
        = (union OMX_VERSIONTYPE *)HdfSbufReadUnpadBuffer(reply, sizeof(union OMX_VERSIONTYPE));
    if (componentVersionCp == NULL) {
        HDF_LOGE("%{public}s: read componentVersionCp failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = memcpy_s(compVersion, sizeof(union OMX_VERSIONTYPE), componentVersionCp, sizeof(union OMX_VERSIONTYPE));
    if (ret != EOK) {
        HDF_LOGE("%{public}s: memcpy_s compVersion failed, error code: %{public}d", __func__, ret);
        return HDF_FAILURE;
    }

    const union OMX_VERSIONTYPE *specVersionCp
        = (union OMX_VERSIONTYPE *)HdfSbufReadUnpadBuffer(reply, sizeof(union OMX_VERSIONTYPE));
    if (specVersionCp == NULL) {
        HDF_LOGE("%{public}s: read specVersionCp failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = memcpy_s(specVersion, sizeof(union OMX_VERSIONTYPE), specVersionCp, sizeof(union OMX_VERSIONTYPE));
    if (ret != EOK) {
        HDF_LOGE("%{public}s: memcpy_s specVersion failed, error code: %{public}d", __func__, ret);
        return HDF_FAILURE;
    }

    uint32_t compUUIDLen = sizeof(OMX_UUIDTYPE);
    for (uint32_t i = 0; i < compUUIDLen; i++) {
        if (!HdfSbufReadUint8(reply, &compUUID[i])) {
            HDF_LOGE("%{public}s: read compUUID[i] failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    return HDF_SUCCESS;
}

static int32_t CodecComponentTypeProxyGetComponentVersion(struct CodecComponentType *self, char *compName,
    union OMX_VERSIONTYPE *compVersion, union OMX_VERSIONTYPE *specVersion, uint8_t *compUUID, uint32_t compUUIDLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, compUUIDLen)) {
        HDF_LOGE("%{public}s: write compUUIDLen failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_GET_COMPONENT_VERSION, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ret = ReadValuesForGetComponentVersion(reply, compName, compVersion, specVersion, compUUID);
    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxySendCommand(struct CodecComponentType *self,
    enum OMX_COMMANDTYPE cmd, uint32_t param, int8_t *cmdData, uint32_t cmdDataLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)cmd)) {
        HDF_LOGE("%{public}s: write cmd failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, param)) {
        HDF_LOGE("%{public}s: write param failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, cmdDataLen)) {
        HDF_LOGE("%{public}s: write cmdData failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < cmdDataLen; i++) {
        if (!HdfSbufWriteInt8(data, cmdData[i])) {
            HDF_LOGE("%{public}s: write cmdData[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_SEND_COMMAND, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyGetParameter(struct CodecComponentType *self,
    uint32_t paramIndex, int8_t *paramStruct, uint32_t paramStructLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, paramIndex)) {
        HDF_LOGE("%{public}s: write paramIndex failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, paramStructLen)) {
        HDF_LOGE("%{public}s: write paramStructLen failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < paramStructLen; i++) {
        if (!HdfSbufWriteInt8(data, paramStruct[i])) {
            HDF_LOGE("%{public}s: write paramStruct[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_GET_PARAMETER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    for (uint32_t i = 0; i < paramStructLen; i++) {
        if (!HdfSbufReadInt8(reply, &paramStruct[i])) {
            HDF_LOGE("%{public}s: read paramStruct[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxySetParameter(struct CodecComponentType *self,
    uint32_t index, int8_t *paramStruct, uint32_t paramStructLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, index)) {
        HDF_LOGE("%{public}s: write index failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, paramStructLen)) {
        HDF_LOGE("%{public}s: write paramStruct failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < paramStructLen; i++) {
        if (!HdfSbufWriteInt8(data, paramStruct[i])) {
            HDF_LOGE("%{public}s: write paramStruct[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_SET_PARAMETER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyGetConfig(struct CodecComponentType *self,
    uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, index)) {
        HDF_LOGE("%{public}s: write index failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, cfgStructLen)) {
        HDF_LOGE("%{public}s: write cfgStructLen failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < cfgStructLen; i++) {
        if (!HdfSbufWriteInt8(data, cfgStruct[i])) {
            HDF_LOGE("%{public}s: write cfgStruct[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_GET_CONFIG, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    for (uint32_t i = 0; i < cfgStructLen; i++) {
        if (!HdfSbufReadInt8(reply, &cfgStruct[i])) {
            HDF_LOGE("%{public}s: read cfgStruct[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxySetConfig(struct CodecComponentType *self,
    uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, index)) {
        HDF_LOGE("%{public}s: write index failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, cfgStructLen)) {
        HDF_LOGE("%{public}s: write cfgStruct failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < cfgStructLen; i++) {
        if (!HdfSbufWriteInt8(data, cfgStruct[i])) {
            HDF_LOGE("%{public}s: write cfgStruct[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_SET_CONFIG, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyGetExtensionIndex(struct CodecComponentType *self,
    const char *paramName, uint32_t *indexType)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteString(data, paramName)) {
        HDF_LOGE("%{public}s: write paramName failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_GET_EXTENSION_INDEX, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    if (!HdfSbufReadUint32(reply, indexType)) {
        HDF_LOGE("%{public}s: read indexType failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyGetState(struct CodecComponentType *self,
    enum OMX_STATETYPE *state)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_GET_STATE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    if (!HdfSbufReadUint32(reply, (uint32_t*)state)) {
        HDF_LOGE("%{public}s: read state failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyComponentTunnelRequest(struct CodecComponentType *self,
    uint32_t port, int32_t tunneledComp, uint32_t tunneledPort, struct OMX_TUNNELSETUPTYPE *tunnelSetup)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, port)) {
        HDF_LOGE("%{public}s: write port failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteInt32(data, tunneledComp)) {
        HDF_LOGE("%{public}s: write tunneledComp failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, tunneledPort)) {
        HDF_LOGE("%{public}s: write tunneledPort failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OMX_TUNNELSETUPTYPEBlockMarshalling(data, tunnelSetup)) {
        HDF_LOGE("%{public}s: write tunnelSetup failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_COMPONENT_TUNNEL_REQUEST, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    if (!OMX_TUNNELSETUPTYPEBlockUnmarshalling(reply, tunnelSetup)) {
        HDF_LOGE("%{public}s: read tunnelSetup failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyUseBuffer(struct CodecComponentType *self,
    uint32_t portIndex, struct OmxCodecBuffer *buffer)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, portIndex)) {
        HDF_LOGE("%{public}s: write portIndex failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_USE_BUFFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    if (!OmxCodecBufferBlockUnmarshalling(reply, buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyAllocateBuffer(struct CodecComponentType *self,
    struct OmxCodecBuffer *buffer, uint32_t portIndex)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, portIndex)) {
        HDF_LOGE("%{public}s: write portIndex failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_ALLOCATE_BUFFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    if (!OmxCodecBufferBlockUnmarshalling(reply, buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyFreeBuffer(struct CodecComponentType *self,
    uint32_t portIndex, const struct OmxCodecBuffer *buffer)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, portIndex)) {
        HDF_LOGE("%{public}s: write portIndex failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_FREE_BUFFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyEmptyThisBuffer(struct CodecComponentType *self,
    const struct OmxCodecBuffer *buffer)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_EMPTY_THIS_BUFFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyFillThisBuffer(struct CodecComponentType *self,
    const struct OmxCodecBuffer *buffer)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_FILL_THIS_BUFFER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxySetCallbacks(struct CodecComponentType *self,
    struct CodecCallbackType *callback, int8_t *appData, uint32_t appDataLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (HdfSbufWriteRemoteService(data, callback->remote) != 0) {
        HDF_LOGE("%{public}s: write callback failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, appDataLen)) {
        HDF_LOGE("%{public}s: write appData failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < appDataLen; i++) {
        if (!HdfSbufWriteInt8(data, appData[i])) {
            HDF_LOGE("%{public}s: write appData[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_SET_CALLBACKS, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyComponentDeInit(struct CodecComponentType *self)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_COMPONENT_DE_INIT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyUseEglImage(struct CodecComponentType *self,
    struct OmxCodecBuffer *buffer, uint32_t portIndex, int8_t *eglImage, uint32_t eglImageLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, portIndex)) {
        HDF_LOGE("%{public}s: write portIndex failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, eglImageLen)) {
        HDF_LOGE("%{public}s: write eglImage failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < eglImageLen; i++) {
        if (!HdfSbufWriteInt8(data, eglImage[i])) {
            HDF_LOGE("%{public}s: write eglImage[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecComponentTypeProxyCall(self, CMD_USE_EGL_IMAGE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    if (!OmxCodecBufferBlockUnmarshalling(reply, buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecComponentTypeProxyComponentRoleEnum(struct CodecComponentType *self,
    uint8_t *role, uint32_t roleLen, uint32_t index)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (WriteInterfaceToken(self, data) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, roleLen)) {
        HDF_LOGE("%{public}s: write roleLen failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, index)) {
        HDF_LOGE("%{public}s: write index failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecComponentTypeProxyCall(self, CMD_COMPONENT_ROLE_ENUM, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    for (uint32_t i = 0; i < roleLen; i++) {
        if (!HdfSbufReadUint8(reply, &role[i])) {
            HDF_LOGE("%{public}s: read role[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static void CodecComponentTypeProxyConstruct(struct CodecComponentType *instance)
{
    instance->GetComponentVersion = CodecComponentTypeProxyGetComponentVersion;
    instance->SendCommand = CodecComponentTypeProxySendCommand;
    instance->GetParameter = CodecComponentTypeProxyGetParameter;
    instance->SetParameter = CodecComponentTypeProxySetParameter;
    instance->GetConfig = CodecComponentTypeProxyGetConfig;
    instance->SetConfig = CodecComponentTypeProxySetConfig;
    instance->GetExtensionIndex = CodecComponentTypeProxyGetExtensionIndex;
    instance->GetState = CodecComponentTypeProxyGetState;
    instance->ComponentTunnelRequest = CodecComponentTypeProxyComponentTunnelRequest;
    instance->UseBuffer = CodecComponentTypeProxyUseBuffer;
    instance->AllocateBuffer = CodecComponentTypeProxyAllocateBuffer;
    instance->FreeBuffer = CodecComponentTypeProxyFreeBuffer;
    instance->EmptyThisBuffer = CodecComponentTypeProxyEmptyThisBuffer;
    instance->FillThisBuffer = CodecComponentTypeProxyFillThisBuffer;
    instance->SetCallbacks = CodecComponentTypeProxySetCallbacks;
    instance->ComponentDeInit = CodecComponentTypeProxyComponentDeInit;
    instance->UseEglImage = CodecComponentTypeProxyUseEglImage;
    instance->ComponentRoleEnum = CodecComponentTypeProxyComponentRoleEnum;
}

struct CodecComponentType *CodecComponentTypeGet(struct HdfRemoteService *remote)
{
    struct CodecComponentTypeProxy *proxy
        = (struct CodecComponentTypeProxy *)OsalMemAlloc(sizeof(struct CodecComponentTypeProxy));
    if (proxy == NULL) {
        HDF_LOGE("%{public}s: malloc CodecComponentType proxy failed!", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }

    proxy->remote = remote;
    CodecComponentTypeProxyConstruct(&proxy->instance);
    return &proxy->instance;
}

void CodecComponentTypeRelease(struct CodecComponentType *instance)
{
    HDF_LOGE("%{public}s !", __func__);
    if (instance == NULL) {
        return;
    }
    struct CodecComponentTypeProxy *proxy = CONTAINER_OF(instance, struct CodecComponentTypeProxy, instance);
    OsalMemFree(proxy);
}

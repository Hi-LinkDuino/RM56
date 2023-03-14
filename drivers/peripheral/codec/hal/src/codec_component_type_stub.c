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

#include "codec_component_type_stub.h"
#include <dlfcn.h>
#include <hdf_device_desc.h>
#include <hdf_device_object.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include <securec.h>
#include "codec_callback_if.h"
#include "codec_component_capability_config.h"

#define HDF_LOG_TAG codec_hdi_server

#ifdef __ARM64__
#define DRIVER_PATH "/vendor/lib64"
#else
#define DRIVER_PATH "/vendor/lib"
#endif
#define CODEC_SERVICE_IMPL  "libcodec_hdi_omx_service_impl"

typedef void (*SERVICE_CONSTRUCT_FUNC)(struct OmxComponentManager *, struct CodecComponentType *);

static const int COMPONENT_NAME_LENGTH = 128;

static void FreeMem(int8_t *mem, uint32_t memLen)
{
    if (memLen > 0 && mem != NULL) {
        OsalMemFree(mem);
    }
}

static int32_t SerStubGetComponentNum(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    return HandleGetNumCmd(reply);
}

static int32_t SerStubGetComponentCapablityList(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    return HandleGetAllCapablityListCmd(reply);
}

static int32_t ReadParamsForCreateComponent(struct HdfSBuf *data, char **compName, int8_t **appData,
    uint32_t *appDataSize, struct CodecCallbackType **callback)
{
    const char *compNameCp = HdfSbufReadString(data);
    if (compNameCp == NULL) {
        HDF_LOGE("%{public}s: read compNameCp failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, appDataSize)) {
        HDF_LOGE("%{public}s: read appData size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    *compName = strdup(compNameCp);

    if (*appDataSize > 0) {
        *appData = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (*appDataSize));
        if (*appData == NULL) {
            HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        for (uint32_t i = 0; i < *appDataSize; i++) {
            if (!HdfSbufReadInt8(data, &(*appData)[i])) {
                HDF_LOGE("%{public}s: read appData[i] failed!", __func__);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    struct HdfRemoteService *callbackRemote = HdfSbufReadRemoteService(data);
    if (callbackRemote == NULL) {
        HDF_LOGE("%{public}s: read callbackRemote failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    *callback = CodecCallbackTypeGet(callbackRemote);

    return HDF_SUCCESS;
}

static int32_t SerStubCreateComponent(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    OMX_HANDLETYPE componentHandle;
    int8_t *appData = NULL;
    uint32_t appDataSize = 0;
    struct CodecCallbackType *callback = NULL;
    char *compName = NULL;

    ret = ReadParamsForCreateComponent(data, &compName, &appData, &appDataSize, &callback);
    if (ret != HDF_SUCCESS) {
        if (compName != NULL) {
            OsalMemFree(compName);
            compName = NULL;
        }
        if (appDataSize > 0 && appData != NULL) {
            OsalMemFree(appData);
        }
        return ret;
    }

    ret = stub->managerService.CreateComponent(&componentHandle, compName, appData, appDataSize, callback);
    if (compName != NULL) {
        OsalMemFree(compName);
        compName = NULL;
    }
    if (appDataSize > 0 && appData != NULL) {
        OsalMemFree(appData);
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CreateComponent function failed!", __func__);
        return ret;
    }
    stub->componentHandle = componentHandle;

    return ret;
}

static int32_t SerStubDestroyComponent(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;

    ret = stub->managerService.DestoryComponent(stub->componentHandle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call DestroyComponent function failed!", __func__);
    }

    return ret;
}

static int32_t SerStubGetComponentVersion(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    char compName[COMPONENT_NAME_LENGTH] = {0};
    union OMX_VERSIONTYPE compVersion;
    union OMX_VERSIONTYPE specVersion;
    uint8_t *compUUID = NULL;
    uint32_t compUUIDLen = 0;

    if (!HdfSbufReadUint32(data, &compUUIDLen)) {
        HDF_LOGE("%{public}s: read compUUIDLen failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    compUUID = (uint8_t*)OsalMemCalloc(sizeof(uint8_t) * (compUUIDLen));
    if (compUUID == NULL) {
        HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = stub->service.GetComponentVersion(&stub->service, compName,
        &compVersion, &specVersion, compUUID, compUUIDLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call GetComponentVersion function failed!", __func__);
        FreeMem((int8_t*)compUUID, compUUIDLen);
        return ret;
    }

    if (!HdfSbufWriteString(reply, compName)) {
        HDF_LOGE("%{public}s: write compName failed!", __func__);
        FreeMem((int8_t*)compUUID, compUUIDLen);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUnpadBuffer(reply, (const uint8_t *)&compVersion, sizeof(union OMX_VERSIONTYPE))) {
        HDF_LOGE("%{public}s: write compVersion failed!", __func__);
        FreeMem((int8_t*)compUUID, compUUIDLen);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUnpadBuffer(reply, (const uint8_t *)&specVersion, sizeof(union OMX_VERSIONTYPE))) {
        HDF_LOGE("%{public}s: write specVersion failed!", __func__);
        FreeMem((int8_t*)compUUID, compUUIDLen);
        return HDF_ERR_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < compUUIDLen; i++) {
        if (!HdfSbufWriteUint8(reply, compUUID[i])) {
            HDF_LOGE("%{public}s: write compUUID[i] failed!", __func__);
            FreeMem((int8_t*)compUUID, compUUIDLen);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    FreeMem((int8_t*)compUUID, compUUIDLen);
    return HDF_SUCCESS;
}

static int32_t SerStubSendCommand(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    enum OMX_COMMANDTYPE cmd;
    uint32_t param = 0;
    int8_t *cmdData = NULL;
    uint32_t cmdDataLen = 0;

    if (!HdfSbufReadUint32(data, (uint32_t*)&cmd)) {
        HDF_LOGE("%{public}s: read &cmd failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &param)) {
        HDF_LOGE("%{public}s: read &param failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &cmdDataLen)) {
        HDF_LOGE("%{public}s: read cmdData size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cmdDataLen > 0) {
        cmdData = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (cmdDataLen));
        if (cmdData == NULL) {
            HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        for (uint32_t i = 0; i < cmdDataLen; i++) {
            if (!HdfSbufReadInt8(data, &cmdData[i])) {
                HDF_LOGE("%{public}s: read &cmdData[i] failed!", __func__);
                FreeMem(cmdData, cmdDataLen);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    ret = stub->service.SendCommand(&stub->service, cmd, param, cmdData, cmdDataLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call SendCommand function failed!", __func__);
        FreeMem(cmdData, cmdDataLen);
        return ret;
    }

    FreeMem(cmdData, cmdDataLen);
    return HDF_SUCCESS;
}

static int32_t SerStubGetParameter(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t paramIndex = 0;
    int8_t *paramStruct = NULL;
    uint32_t paramStructLen = 0;

    if (!HdfSbufReadUint32(data, &paramIndex)) {
        HDF_LOGE("%{public}s: read paramIndex failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &paramStructLen)) {
        HDF_LOGE("%{public}s: read paramStructLen failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    paramStruct = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (paramStructLen));
    if (paramStruct == NULL) {
        HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    for (uint32_t i = 0; i < paramStructLen; i++) {
        if (!HdfSbufReadInt8(data, &paramStruct[i])) {
            HDF_LOGE("%{public}s: read paramStruct[%{public}d] failed!", __func__, i);
            FreeMem(paramStruct, paramStructLen);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = stub->service.GetParameter(&stub->service, paramIndex, paramStruct, paramStructLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call GetParameter function failed!", __func__);
        FreeMem(paramStruct, paramStructLen);
        return ret;
    }

    for (uint32_t i = 0; i < paramStructLen; i++) {
        if (!HdfSbufWriteInt8(reply, paramStruct[i])) {
            HDF_LOGE("%{public}s: write paramStruct[i] failed!", __func__);
            FreeMem(paramStruct, paramStructLen);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    FreeMem(paramStruct, paramStructLen);
    return HDF_SUCCESS;
}

static int32_t SerStubSetParameter(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t index = 0;
    int8_t *paramStruct = NULL;
    uint32_t paramStructLen = 0;

    if (!HdfSbufReadUint32(data, &index)) {
        HDF_LOGE("%{public}s: read &index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &paramStructLen)) {
        HDF_LOGE("%{public}s: read paramStruct size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (paramStructLen > 0) {
        paramStruct = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (paramStructLen));
        if (paramStruct == NULL) {
            HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        for (uint32_t i = 0; i < paramStructLen; i++) {
            if (!HdfSbufReadInt8(data, &paramStruct[i])) {
                HDF_LOGE("%{public}s: read &paramStruct[i] failed!", __func__);
                FreeMem(paramStruct, paramStructLen);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    ret = stub->service.SetParameter(&stub->service, index, paramStruct, paramStructLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call SetParameter function failed!", __func__);
        FreeMem(paramStruct, paramStructLen);
        return ret;
    }

    FreeMem(paramStruct, paramStructLen);
    return HDF_SUCCESS;
}

static int32_t SerStubGetConfig(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t index = 0;
    int8_t *cfgStruct = NULL;
    uint32_t cfgStructLen = 0;

    if (!HdfSbufReadUint32(data, &index)) {
        HDF_LOGE("%{public}s: read &index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &cfgStructLen)) {
        HDF_LOGE("%{public}s: read cfgStructLen failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    cfgStruct = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (cfgStructLen));
    if (cfgStruct == NULL) {
        HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    for (uint32_t i = 0; i < cfgStructLen; i++) {
        if (!HdfSbufReadInt8(data, &cfgStruct[i])) {
            HDF_LOGE("%{public}s: read cfgStruct[i] failed!", __func__);
            FreeMem(cfgStruct, cfgStructLen);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = stub->service.GetConfig(&stub->service, index, cfgStruct, cfgStructLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call GetConfig function failed!", __func__);
        FreeMem(cfgStruct, cfgStructLen);
        return ret;
    }

    for (uint32_t i = 0; i < cfgStructLen; i++) {
        if (!HdfSbufWriteInt8(reply, cfgStruct[i])) {
            HDF_LOGE("%{public}s: write cfgStruct[i] failed!", __func__);
            FreeMem(cfgStruct, cfgStructLen);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    FreeMem(cfgStruct, cfgStructLen);
    return ret;
}

static int32_t SerStubSetConfig(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t index = 0;
    int8_t *cfgStruct = NULL;
    uint32_t cfgStructLen = 0;

    if (!HdfSbufReadUint32(data, &index)) {
        HDF_LOGE("%{public}s: read &index failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &cfgStructLen)) {
        HDF_LOGE("%{public}s: read cfgStruct size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (cfgStructLen > 0) {
        cfgStruct = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (cfgStructLen));
        if (cfgStruct == NULL) {
            HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        for (uint32_t i = 0; i < cfgStructLen; i++) {
            if (!HdfSbufReadInt8(data, &cfgStruct[i])) {
                HDF_LOGE("%{public}s: read &cfgStruct[i] failed!", __func__);
                FreeMem(cfgStruct, cfgStructLen);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    ret = stub->service.SetConfig(&stub->service, index, cfgStruct, cfgStructLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call SetConfig function failed!", __func__);
        FreeMem(cfgStruct, cfgStructLen);
        return ret;
    }

    FreeMem(cfgStruct, cfgStructLen);
    return ret;
}

static int32_t SerStubGetExtensionIndex(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    char *paramName = NULL;
    uint32_t indexType = 0;

    const char *paramNameCp = HdfSbufReadString(data);
    if (paramNameCp == NULL) {
        HDF_LOGE("%{public}s: read paramNameCp failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    paramName = strdup(paramNameCp);

    ret = stub->service.GetExtensionIndex(&stub->service, paramName, &indexType);
    if (paramName != NULL) {
        OsalMemFree(paramName);
        paramName = NULL;
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call GetExtensionIndex function failed!", __func__);
        return ret;
    }

    if (!HdfSbufWriteUint32(reply, indexType)) {
        HDF_LOGE("%{public}s: write indexType failed!", __func__);
        ret = HDF_ERR_INVALID_PARAM;
    }

    return ret;
}

static int32_t SerStubGetState(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    enum OMX_STATETYPE state;

    ret = stub->service.GetState(&stub->service, &state);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call GetState function failed!", __func__);
        return ret;
    }

    if (!HdfSbufWriteUint32(reply, (uint32_t)state)) {
        HDF_LOGE("%{public}s: write state failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return ret;
}

static int32_t SerStubComponentTunnelRequest(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t port = 0;
    int32_t tunneledComp = 0;
    uint32_t tunneledPort = 0;
    struct OMX_TUNNELSETUPTYPE tunnelSetup;

    if (!HdfSbufReadUint32(data, &port)) {
        HDF_LOGE("%{public}s: read &port failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &tunneledComp)) {
        HDF_LOGE("%{public}s: read &tunneledComp failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &tunneledPort)) {
        HDF_LOGE("%{public}s: read &tunneledPort failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OMX_TUNNELSETUPTYPEBlockUnmarshalling(data, &tunnelSetup)) {
        HDF_LOGE("%{public}s: read tunnelSetup failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.ComponentTunnelRequest(&stub->service, port, tunneledComp, tunneledPort, &tunnelSetup);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call ComponentTunnelRequest function failed!", __func__);
        return ret;
    }

    if (!OMX_TUNNELSETUPTYPEBlockMarshalling(reply, &tunnelSetup)) {
        HDF_LOGE("%{public}s: write tunnelSetup failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return ret;
}

static int32_t SerStubUseBuffer(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct OmxCodecBuffer buffer;
    uint32_t portIndex = 0;

    if (!HdfSbufReadUint32(data, &portIndex)) {
        HDF_LOGE("%{public}s: read &portIndex failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OmxCodecBufferBlockUnmarshalling(data, &buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.UseBuffer(&stub->service, portIndex, &buffer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call UseBuffer function failed!", __func__);
        return ret;
    }

    if (!OmxCodecBufferBlockMarshalling(reply, &buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return ret;
}

static int32_t SerStubAllocateBuffer(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct OmxCodecBuffer buffer;
    uint32_t portIndex = 0;

    if (!OmxCodecBufferBlockUnmarshalling(data, &buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &portIndex)) {
        HDF_LOGE("%{public}s: read &portIndex failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.AllocateBuffer(&stub->service, &buffer, portIndex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call AllocateBuffer function failed!", __func__);
        return ret;
    }

    if (!OmxCodecBufferBlockMarshalling(reply, &buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return ret;
}

static int32_t SerStubFreeBuffer(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t portIndex = 0;
    struct OmxCodecBuffer buffer;

    if (!HdfSbufReadUint32(data, &portIndex)) {
        HDF_LOGE("%{public}s: read &portIndex failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OmxCodecBufferBlockUnmarshalling(data, &buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.FreeBuffer(&stub->service, portIndex, &buffer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call FreeBuffer function failed!", __func__);
        return ret;
    }

    return ret;
}

static int32_t SerStubEmptyThisBuffer(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct OmxCodecBuffer buffer;

    if (!OmxCodecBufferBlockUnmarshalling(data, &buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.EmptyThisBuffer(&stub->service, &buffer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call EmptyThisBuffer function failed!", __func__);
        return ret;
    }

    return ret;
}

static int32_t SerStubFillThisBuffer(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct OmxCodecBuffer buffer;

    if (!OmxCodecBufferBlockUnmarshalling(data, &buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.FillThisBuffer(&stub->service, &buffer);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call FillThisBuffer function failed!", __func__);
        return ret;
    }

    return ret;
}

static int32_t SerStubSetCallbacks(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct CodecCallbackType* callback = NULL;
    int8_t *appData = NULL;
    uint32_t appDataLen = 0;

    struct HdfRemoteService *callbackRemote = HdfSbufReadRemoteService(data);
    if (callbackRemote == NULL) {
        HDF_LOGE("%{public}s: read callbackRemote failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    callback = CodecCallbackTypeGet(callbackRemote);

    if (!HdfSbufReadUint32(data, &appDataLen)) {
        HDF_LOGE("%{public}s: read appData size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (appDataLen > 0) {
        appData = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (appDataLen));
        if (appData == NULL) {
            HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        for (uint32_t i = 0; i < appDataLen; i++) {
            if (!HdfSbufReadInt8(data, &appData[i])) {
                HDF_LOGE("%{public}s: read &appData[i] failed!", __func__);
                FreeMem(appData, appDataLen);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    ret = stub->service.SetCallbacks(&stub->service, callback, appData, appDataLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call SetCallbacks function failed!", __func__);
        FreeMem(appData, appDataLen);
        return ret;
    }

    FreeMem(appData, appDataLen);
    return ret;
}

static int32_t SerStubComponentDeInit(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;

    ret = stub->service.ComponentDeInit(&stub->service);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call ComponentDeInit function failed!", __func__);
    }

    return ret;
}

static int32_t SerStubUseEglImage(struct CodecComponentTypeStub *stub, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    struct OmxCodecBuffer buffer;
    uint32_t portIndex = 0;
    int8_t *eglImage = NULL;
    uint32_t eglImageLen = 0;

    if (!OmxCodecBufferBlockUnmarshalling(data, &buffer)) {
        HDF_LOGE("%{public}s: read buffer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &portIndex)) {
        HDF_LOGE("%{public}s: read &portIndex failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadUint32(data, &eglImageLen)) {
        HDF_LOGE("%{public}s: read eglImage size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (eglImageLen > 0) {
        eglImage = (int8_t*)OsalMemCalloc(sizeof(int8_t) * (eglImageLen));
        if (eglImage == NULL) {
            HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        for (uint32_t i = 0; i < eglImageLen; i++) {
            if (!HdfSbufReadInt8(data, &eglImage[i])) {
                HDF_LOGE("%{public}s: read &eglImage[i] failed!", __func__);
                FreeMem(eglImage, eglImageLen);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    ret = stub->service.UseEglImage(&stub->service, &buffer, portIndex, eglImage, eglImageLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call UseEglImage function failed!", __func__);
        FreeMem(eglImage, eglImageLen);
        return ret;
    }

    if (!OmxCodecBufferBlockMarshalling(reply, &buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        FreeMem(eglImage, eglImageLen);
        return HDF_ERR_INVALID_PARAM;
    }

    FreeMem(eglImage, eglImageLen);
    return ret;
}

static int32_t SerStubComponentRoleEnum(struct CodecComponentTypeStub *stub,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint8_t *role = NULL;
    uint32_t roleLen = 0;
    uint32_t index = 0;

    if (!HdfSbufReadUint32(data, &roleLen)) {
        HDF_LOGE("%{public}s: read &roleLen failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    role = (uint8_t*)OsalMemCalloc(sizeof(uint8_t) * (roleLen));
    if (role == NULL) {
        HDF_LOGE("%{public}s: HDF_ERR_MALLOC_FAIL!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfSbufReadUint32(data, &index)) {
        HDF_LOGE("%{public}s: read &index failed!", __func__);
        FreeMem((int8_t*)role, roleLen);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = stub->service.ComponentRoleEnum(&stub->service, role, roleLen, index);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call ComponentRoleEnum function failed!", __func__);
        FreeMem((int8_t*)role, roleLen);
        return ret;
    }

    for (uint32_t i = 0; i < roleLen; i++) {
        if (!HdfSbufWriteUint8(reply, role[i])) {
            HDF_LOGE("%{public}s: write role[i] failed!", __func__);
            FreeMem((int8_t*)role, roleLen);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    FreeMem((int8_t*)role, roleLen);
    return ret;
}

static int32_t HandleComponentManagerCmd(struct CodecComponentTypeStub *stub,
    int32_t cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    switch (cmdId) {
        case CMD_CODEC_GET_COMPONENT_NUM:
            return SerStubGetComponentNum(stub, data, reply);
        case CMD_CODEC_GET_COMPONENT_CAPABILITY_LIST:
            return SerStubGetComponentCapablityList(stub, data, reply);
        case CMD_CREATE_COMPONENT:
            return SerStubCreateComponent(stub, data, reply);
        case CMD_DESTROY_COMPONENT:
            return SerStubDestroyComponent(stub, data, reply);
        default:
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
    }
}

static int32_t HandleComponentCmd(struct CodecComponentTypeStub *stub,
    int32_t cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    switch (cmdId) {
        case CMD_GET_COMPONENT_VERSION:
            return SerStubGetComponentVersion(stub, data, reply);
        case CMD_SEND_COMMAND:
            return SerStubSendCommand(stub, data, reply);
        case CMD_GET_PARAMETER:
            return SerStubGetParameter(stub, data, reply);
        case CMD_SET_PARAMETER:
            return SerStubSetParameter(stub, data, reply);
        case CMD_GET_CONFIG:
            return SerStubGetConfig(stub, data, reply);
        case CMD_SET_CONFIG:
            return SerStubSetConfig(stub, data, reply);
        case CMD_GET_EXTENSION_INDEX:
            return SerStubGetExtensionIndex(stub, data, reply);
        case CMD_GET_STATE:
            return SerStubGetState(stub, data, reply);
        case CMD_COMPONENT_TUNNEL_REQUEST:
            return SerStubComponentTunnelRequest(stub, data, reply);
        case CMD_USE_BUFFER:
            return SerStubUseBuffer(stub, data, reply);
        case CMD_ALLOCATE_BUFFER:
            return SerStubAllocateBuffer(stub, data, reply);
        case CMD_FREE_BUFFER:
            return SerStubFreeBuffer(stub, data, reply);
        case CMD_EMPTY_THIS_BUFFER:
            return SerStubEmptyThisBuffer(stub, data, reply);
        case CMD_FILL_THIS_BUFFER:
            return SerStubFillThisBuffer(stub, data, reply);
        case CMD_SET_CALLBACKS:
            return SerStubSetCallbacks(stub, data, reply);
        case CMD_COMPONENT_DE_INIT:
            return SerStubComponentDeInit(stub, data, reply);
        case CMD_USE_EGL_IMAGE:
            return SerStubUseEglImage(stub, data, reply);
        case CMD_COMPONENT_ROLE_ENUM:
            return SerStubComponentRoleEnum(stub, data, reply);
        default:
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
    }
}

int32_t CodecComponentTypeServiceOnRemoteRequest(struct CodecComponentTypeStub *service,
    int32_t cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (!HdfDeviceObjectCheckInterfaceDesc(service->device, data)) {
        HDF_LOGE("%{public}s: check interface token failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    
    switch (cmdId) {
        case CMD_CODEC_GET_COMPONENT_NUM:
        case CMD_CODEC_GET_COMPONENT_CAPABILITY_LIST:
        case CMD_CREATE_COMPONENT:
        case CMD_DESTROY_COMPONENT:
            return HandleComponentManagerCmd(service, cmdId, data, reply);
        case CMD_GET_COMPONENT_VERSION:
        case CMD_SEND_COMMAND:
        case CMD_GET_PARAMETER:
        case CMD_SET_PARAMETER:
        case CMD_GET_CONFIG:
        case CMD_SET_CONFIG:
        case CMD_GET_EXTENSION_INDEX:
        case CMD_GET_STATE:
        case CMD_COMPONENT_TUNNEL_REQUEST:
        case CMD_USE_BUFFER:
        case CMD_ALLOCATE_BUFFER:
        case CMD_FREE_BUFFER:
        case CMD_EMPTY_THIS_BUFFER:
        case CMD_FILL_THIS_BUFFER:
        case CMD_SET_CALLBACKS:
        case CMD_COMPONENT_DE_INIT:
        case CMD_USE_EGL_IMAGE:
        case CMD_COMPONENT_ROLE_ENUM:
            return HandleComponentCmd(service, cmdId, data, reply);
        default:
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
    }
}

static void *LoadServiceHandler(void)
{
    char *libPath = NULL;
    void *handler = NULL;

    libPath = HDF_LIBRARY_FULL_PATH(CODEC_SERVICE_IMPL);
    handler = dlopen(libPath, RTLD_LAZY);
    if (handler == NULL) {
        HDF_LOGE("%{public}s: dlopen failed %{public}s", __func__, dlerror());
        return NULL;
    }

    return handler;
}

struct CodecComponentTypeStub *CodecComponentTypeStubGetInstance(void)
{
    SERVICE_CONSTRUCT_FUNC serviceConstructFunc = NULL;
    struct CodecComponentTypeStub *stub
        = (struct CodecComponentTypeStub *)OsalMemAlloc(sizeof(struct CodecComponentTypeStub));
    if (stub == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc obj failed!", __func__);
        return NULL;
    }

    stub->dlHandler = LoadServiceHandler();
    if (stub->dlHandler == NULL) {
        HDF_LOGE("%{public}s: stub->dlHanlder is null", __func__);
        OsalMemFree(stub);
        return NULL;
    }

    serviceConstructFunc = (SERVICE_CONSTRUCT_FUNC)dlsym(stub->dlHandler, "CodecComponentTypeServiceConstruct");
    if (serviceConstructFunc == NULL) {
        HDF_LOGE("%{public}s: dlsym failed %{public}s", __func__, dlerror());
        dlclose(stub->dlHandler);
        OsalMemFree(stub);
        return NULL;
    }

    serviceConstructFunc(&stub->managerService, &stub->service);
    return stub;
}

void CodecComponentTypeStubRelease(struct CodecComponentTypeStub *instance)
{
    if (instance == NULL) {
        return;
    }

    dlclose(instance->dlHandler);
    OsalMemFree(instance);
}
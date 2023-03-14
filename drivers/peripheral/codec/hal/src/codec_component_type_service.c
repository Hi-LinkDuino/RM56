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

#include "codec_component_type_service.h"
#include <hdf_base.h>
#include <hdf_log.h>

#define HDF_LOG_TAG codec_hdi_server

int32_t OmxManagerCreateComponent(OMX_HANDLETYPE *compHandle, char *compName, void *appData, int32_t appDataSize,
    struct CodecCallbackType *callbacks)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t OmxManagerDestroyComponent(OMX_HANDLETYPE compHandle)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeGetComponentVersion(struct CodecComponentType *self,
    char* compName, union OMX_VERSIONTYPE *compVersion, union OMX_VERSIONTYPE *specVersion,
    uint8_t *compUUID, uint32_t compUUIDLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeSendCommand(struct CodecComponentType *self,
    enum OMX_COMMANDTYPE cmd, uint32_t param, int8_t *cmdData, uint32_t cmdDataLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeGetParameter(struct CodecComponentType *self,
    uint32_t paramIndex, int8_t *paramStruct, uint32_t paramStructLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeSetParameter(struct CodecComponentType *self,
    uint32_t index, int8_t *paramStruct, uint32_t paramStructLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeGetConfig(struct CodecComponentType *self,
    uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeSetConfig(struct CodecComponentType *self,
    uint32_t index, int8_t *cfgStruct, uint32_t cfgStructLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeGetExtensionIndex(struct CodecComponentType *self,
    const char *paramName, uint32_t *indexType)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeGetState(struct CodecComponentType *self,
    enum OMX_STATETYPE *state)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeComponentTunnelRequest(struct CodecComponentType *self,
    uint32_t port, int32_t tunneledComp, uint32_t tunneledPort,
    struct OMX_TUNNELSETUPTYPE *tunnelSetup)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeUseBuffer(struct CodecComponentType *self,
    uint32_t portIndex, struct OmxCodecBuffer *buffer)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeAllocateBuffer(struct CodecComponentType *self,
    struct OmxCodecBuffer *buffer, uint32_t portIndex)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeFreeBuffer(struct CodecComponentType *self, uint32_t portIndex,
    const struct OmxCodecBuffer *buffer)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeEmptyThisBuffer(struct CodecComponentType *self,
    const struct OmxCodecBuffer *buffer)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeFillThisBuffer(struct CodecComponentType *self,
    const struct OmxCodecBuffer *buffer)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeSetCallbacks(struct CodecComponentType *self,
    struct CodecCallbackType* callback, int8_t *appData, uint32_t appDataLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeComponentDeInit(struct CodecComponentType *self)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeUseEglImage(struct CodecComponentType *self,
    struct OmxCodecBuffer *buffer, uint32_t portIndex, int8_t *eglImage, uint32_t eglImageLen)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

int32_t CodecComponentTypeComponentRoleEnum(struct CodecComponentType *self,
    uint8_t *role, uint32_t roleLen, uint32_t index)
{
    HDF_LOGI("%{public}s, service impl!", __func__);
    return HDF_SUCCESS;
}

void CodecComponentTypeServiceConstruct(struct OmxComponentManager *manager, struct CodecComponentType *instance)
{
    manager->CreateComponent = OmxManagerCreateComponent;
    manager->DestoryComponent = OmxManagerDestroyComponent;
    instance->GetComponentVersion = CodecComponentTypeGetComponentVersion;
    instance->SendCommand = CodecComponentTypeSendCommand;
    instance->GetParameter = CodecComponentTypeGetParameter;
    instance->SetParameter = CodecComponentTypeSetParameter;
    instance->GetConfig = CodecComponentTypeGetConfig;
    instance->SetConfig = CodecComponentTypeSetConfig;
    instance->GetExtensionIndex = CodecComponentTypeGetExtensionIndex;
    instance->GetState = CodecComponentTypeGetState;
    instance->ComponentTunnelRequest = CodecComponentTypeComponentTunnelRequest;
    instance->UseBuffer = CodecComponentTypeUseBuffer;
    instance->AllocateBuffer = CodecComponentTypeAllocateBuffer;
    instance->FreeBuffer = CodecComponentTypeFreeBuffer;
    instance->EmptyThisBuffer = CodecComponentTypeEmptyThisBuffer;
    instance->FillThisBuffer = CodecComponentTypeFillThisBuffer;
    instance->SetCallbacks = CodecComponentTypeSetCallbacks;
    instance->ComponentDeInit = CodecComponentTypeComponentDeInit;
    instance->UseEglImage = CodecComponentTypeUseEglImage;
    instance->ComponentRoleEnum = CodecComponentTypeComponentRoleEnum;
}
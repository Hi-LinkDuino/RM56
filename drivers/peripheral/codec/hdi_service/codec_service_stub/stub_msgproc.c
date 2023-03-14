/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "stub_msgproc.h"
#include <hdf_log.h>

int32_t CodecSerPackAlignment(struct HdfSBuf *reply, Alignment *alignment)
{
    if (reply == NULL || alignment == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteInt32(reply, alignment->widthAlignment)) {
        HDF_LOGE("%{public}s: Write widthAlignment failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(reply, alignment->heightAlignment)) {
        HDF_LOGE("%{public}s: Write heightAlignment failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackRect(struct HdfSBuf *reply, Rect *rectangle)
{
    if (reply == NULL || rectangle == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteInt32(reply, rectangle->width)) {
        HDF_LOGE("%{public}s: Write width failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(reply, rectangle->height)) {
        HDF_LOGE("%{public}s: Write height failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackArray(struct HdfSBuf *reply, ResizableArray *resArr)
{
    if (reply == NULL || resArr == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(reply, resArr->actualLen)) {
        HDF_LOGE("%{public}s: Write actualLen failed!", __func__);
        return HDF_FAILURE;
    }
    for (uint32_t i = 0; i < resArr->actualLen; i++) {
        if (!HdfSbufWriteUint32(reply, resArr->element[i])) {
            HDF_LOGE("%{public}s: Write HdfSubf failed!", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackCapbility(struct HdfSBuf *reply, CodecCapbility *cap)
{
    if (reply == NULL || cap == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)cap->mime)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)cap->type)) {
        return HDF_FAILURE;
    }
    if (CodecSerPackAlignment(reply, &cap->whAlignment) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (CodecSerPackRect(reply, &cap->minSize) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (CodecSerPackRect(reply, &cap->maxSize) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, cap->minBitRate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, cap->maxBitRate)) {
        return HDF_FAILURE;
    }
    if (CodecSerPackArray(reply, &cap->supportProfiles) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (CodecSerPackArray(reply, &cap->supportLevels) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (CodecSerPackArray(reply, &cap->supportPixelFormats) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, cap->minInputBufferNum)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, cap->minOutputBufferNum)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, cap->allocateMask)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, cap->capsMask)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerParseParam(struct HdfSBuf *data, Param *param)
{
    if (data == NULL || param == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&param->key)) {
        HDF_LOGE("%{public}s: Read key failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&param->val)) {
        HDF_LOGE("%{public}s: Read val failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, (int32_t*)&param->size)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackParam(struct HdfSBuf *reply, Param *param)
{
    if (reply == NULL || param == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)param->key)) {
        HDF_LOGE("%{public}s: write key failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)&param->val)) {
        HDF_LOGE("%{public}s: Write val failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(reply, param->size)) {
        HDF_LOGE("%{public}s: write size failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackBufferHandle(struct HdfSBuf *reply, CodecBufferHandle *bufferHandle)
{
    if (reply == NULL || bufferHandle == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint8(reply, (uint8_t)&bufferHandle->virAddr)) {
        HDF_LOGE("%{public}s: Write virAddr failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)bufferHandle->handle)) {
        HDF_LOGE("%{public}s: Write handle failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackBufferInfo(struct HdfSBuf *reply, CodecBufferInfo *buffers)
{
    if (reply == NULL || buffers == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, buffers->type)) {
        HDF_LOGE("%{public}s: Write tempType failed!", __func__);
        return HDF_FAILURE;
    }
    if (buffers->type == BUFFER_TYPE_VIRTUAL) {
        if (!HdfSbufWriteUint8(reply, (uint8_t)&buffers->addr)) {
            HDF_LOGE("%{public}s: Write addr failed!", __func__);
            return HDF_FAILURE;
        }
    } else if (buffers->type == BUFFER_TYPE_FD) {
        if (!HdfSbufWriteFileDescriptor(reply, buffers->fd)) {
            HDF_LOGE("%{public}s: Write fd failed!", __func__);
            return HDF_FAILURE;
        }
    } else if (buffers->type == BUFFER_TYPE_HANDLE) {
        if (CodecSerPackBufferHandle(reply, &buffers->handle)) {
            HDF_LOGE("%{public}s: Write handle failed!", __func__);
            return HDF_FAILURE;
        }
    }
    if (!HdfSbufWriteUint32(reply, buffers->offset)) {
        HDF_LOGE("%{public}s: Write offset failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, buffers->length)) {
        HDF_LOGE("%{public}s: Write length failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, buffers->size)) {
        HDF_LOGE("%{public}s: Write size failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerPackInputInfo(struct HdfSBuf *reply, InputInfo *inputData)
{
    if (reply == NULL || inputData == NULL || inputData->buffers == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_FAILURE;
    }
    for (uint32_t i = 0; i < inputData->bufferCnt; i++) {
        if (CodecSerPackBufferInfo(reply, &inputData->buffers[i])) {
            HDF_LOGE("%{public}s: Write buffers failed!", __func__);
            return HDF_FAILURE;
        }
    }
    if (!HdfSbufWriteInt64(reply, inputData->pts)) {
        HDF_LOGE("%{public}s: Write pts failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(reply, inputData->flag)) {
        HDF_LOGE("%{public}s: Write flag failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerParseBufferHandle(struct HdfSBuf *reply, CodecBufferHandle *bufferHandle)
{
    if (reply == NULL || bufferHandle == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint8(reply, bufferHandle->virAddr)) {
        HDF_LOGE("%{public}s: read virAddr failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, (uint32_t *)&bufferHandle->handle)) {
        HDF_LOGE("%{public}s: read handle failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerParseBufferInfo(struct HdfSBuf *data, CodecBufferInfo *buffers)
{
    if (data == NULL || buffers == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_FAILURE;
    }
    uint32_t tempType = 0;
    if (buffers == NULL) {
        HDF_LOGE("%{public}s: buffers is NULL!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &tempType)) {
        HDF_LOGE("%{public}s: read type failed!", __func__);
        return HDF_FAILURE;
    }
    buffers->type = (BufferType)tempType;
    if (buffers->type == BUFFER_TYPE_VIRTUAL) {
        if (!HdfSbufReadUint8(data, buffers->addr)) {
            HDF_LOGE("%{public}s: read addr failed!", __func__);
            return HDF_FAILURE;
        }
    } else if (buffers->type == BUFFER_TYPE_FD) {
        buffers->fd = HdfSbufReadFileDescriptor(data);
        if (buffers->fd < 0) {
            HDF_LOGE("%{public}s: read fd failed!", __func__);
            return HDF_FAILURE;
        }
    } else if (buffers->type == BUFFER_TYPE_HANDLE) {
        if (CodecSerParseBufferHandle(data, &buffers->handle)) {
            HDF_LOGE("%{public}s: read handle failed!", __func__);
            return HDF_FAILURE;
        }
    }
    if (!HdfSbufReadUint32(data, &buffers->offset)) {
        HDF_LOGE("%{public}s: read offset failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &buffers->length)) {
        HDF_LOGE("%{public}s: read length failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &buffers->size)) {
        HDF_LOGE("%{public}s: read size failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerParseInputInfo(struct HdfSBuf *data, InputInfo *inputData)
{
    if (data == NULL || inputData == NULL || inputData->buffers == NULL) {
        HDF_LOGE("%{public}s: params NULL!", __func__);
        return HDF_FAILURE;
    }
    for (uint32_t i = 0; i < inputData->bufferCnt; i++) {
        if (CodecSerParseBufferInfo(data, &inputData->buffers[i])) {
            HDF_LOGE("%{public}s: read buffers failed!", __func__);
            return HDF_FAILURE;
        }
    }
    if (!HdfSbufReadInt64(data, &inputData->pts)) {
        HDF_LOGE("%{public}s: read pts failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(data, &inputData->flag)) {
        HDF_LOGE("%{public}s: read flag failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecSerParseOutputInfo(struct HdfSBuf *data, OutputInfo *outInfo)
{
    if (data == NULL || outInfo == NULL || outInfo->buffers == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_FAILURE;
    }
    for (uint32_t i = 0; i < outInfo->bufferCnt; i++) {
        if (CodecSerParseBufferInfo(data, &outInfo->buffers[i])) {
            HDF_LOGE("%{public}s: read buffers failed!", __func__);
            return HDF_FAILURE;
        }
    }
    if (!HdfSbufReadInt64(data, &outInfo->timeStamp)) {
        HDF_LOGE("%{public}s: read timeStamp failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &outInfo->sequence)) {
        HDF_LOGE("%{public}s: read sequence failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &outInfo->flag)) {
        HDF_LOGE("%{public}s: read flag failed!", __func__);
        return HDF_FAILURE;
    }
    uint32_t tempType = 0;
    if (!HdfSbufReadUint32(data, &tempType)) {
        HDF_LOGE("%{public}s: read tempType failed!", __func__);
        return HDF_FAILURE;
    }
    outInfo->type = (CodecType)tempType;
    if (!HdfSbufReadUint64(data, (uint64_t *)&outInfo->vendorPrivate)) {
        HDF_LOGE("%{public}s: read vendorPrivate failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
int32_t CodecSerPackOutputInfo(struct HdfSBuf *reply, OutputInfo *outInfo)
{
    if (reply == NULL || outInfo == NULL || outInfo->buffers == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_FAILURE;
    }
    for (uint32_t i = 0; i < outInfo->bufferCnt; i++) {
        if (CodecSerPackBufferInfo(reply, &outInfo->buffers[i])) {
            HDF_LOGE("%{public}s: write buffers failed!", __func__);
            return HDF_FAILURE;
        }
    }
    if (!HdfSbufWriteInt64(reply, outInfo->timeStamp)) {
        HDF_LOGE("%{public}s: write timeStamp failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, outInfo->sequence)) {
        HDF_LOGE("%{public}s: write sequence failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, outInfo->flag)) {
        HDF_LOGE("%{public}s: write flag failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, outInfo->type)) {
        HDF_LOGE("%{public}s: write outInfo->type failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, (uint64_t)&outInfo->vendorPrivate)) {
        HDF_LOGE("%{public}s: write vendorPrivate failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
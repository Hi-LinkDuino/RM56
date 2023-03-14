/*
 * Copyright (c) 2021 Shenzhen Kaihong DID Co., Ltd.
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

#include "codec_config_reader.h"
#include <hdf_log.h>
#include <hdf_sbuf.h>
#include <servmgr_hdi.h>
#include "config_parser.h"
#include "codec_utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HDF_LOG_TAG "codec_config_client"

static int32_t DeserializeCapAlignment(struct HdfSBuf *reply, Alignment *alignment)
{
    if (reply == NULL || alignment == NULL) {
        HDF_LOGE("%{public}s: params NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reply, &alignment->widthAlignment)) {
        HDF_LOGE("%{public}s: read widthAlignment failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reply, &alignment->heightAlignment)) {
        HDF_LOGE("%{public}s: read heightAlignment failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t DeserializeCapRect(struct HdfSBuf *reply, Rect *rectangle)
{
    if (reply == NULL || rectangle == NULL) {
        HDF_LOGE("%{public}s: params NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(reply, &rectangle->width)) {
        HDF_LOGE("%{public}s: read width failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reply, &rectangle->height)) {
        HDF_LOGE("%{public}s: read height failed!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t DeserializeCapArray(struct HdfSBuf *reply, ResizableArray *resArr)
{
    if (reply == NULL || resArr == NULL) {
        HDF_LOGE("%{public}s: params NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    resArr->actualLen = 0;
    if (!HdfSbufReadUint32(reply, &resArr->actualLen)) {
        HDF_LOGE("%{public}s: read actualLen failed!", __func__);
        return HDF_FAILURE;
    }
    for (uint32_t i = 0; i < resArr->actualLen; i++) {
        if (!HdfSbufReadUint32(reply, &resArr->element[i])) {
            HDF_LOGE("%{public}s: read element failed!", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t DeserializeCodecCapability(struct HdfSBuf *reply, CodecCapbility *cap)
{
    if (reply == NULL || cap == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(reply, (uint32_t*)&cap->mime)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, (uint32_t*)&cap->type)) {
        return HDF_FAILURE;
    }
    if (DeserializeCapAlignment(reply, &cap->whAlignment) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (DeserializeCapRect(reply, &cap->minSize) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (DeserializeCapRect(reply, &cap->maxSize) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint64(reply, &cap->minBitRate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint64(reply, &cap->maxBitRate)) {
        return HDF_FAILURE;
    }
    if (DeserializeCapArray(reply, &cap->supportProfiles) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (DeserializeCapArray(reply, &cap->supportLevels) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (DeserializeCapArray(reply, &cap->supportPixelFormats) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &cap->minInputBufferNum)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &cap->minOutputBufferNum)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &cap->allocateMask)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &cap->capsMask)) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

struct HdfRemoteService *GetConfigService(void)
{
    struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    if (serviceMgr == NULL) {
        HDF_LOGE("%{public}s: HDIServiceManager not found!", __func__);
        return NULL;
    }

    struct HdfRemoteService *remote = serviceMgr->GetService(serviceMgr, "codec_capability_config_service");
    if (remote == NULL) {
        HDF_LOGE("%{public}s: HdfRemoteService not found!", __func__);
        return NULL;
    }
    if (!HdfRemoteServiceSetInterfaceDesc(remote, "ohos.hdi.codec_service")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }

    return remote;
}

static void ReleaseSbuf(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
}

int32_t EnumrateCapability(struct HdfRemoteService *remote, int index, CodecCapbility *cap)
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
    if (!HdfRemoteServiceWriteInterfaceToken(remote, data)) {
        HDF_LOGE("write interface token failed");
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, index)) {
        HDF_LOGE("%{public}s: write input index failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (remote->dispatcher->Dispatch(remote, CODEC_CONFIG_CMD_ENUMERATE_CAP, data, reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: dispatch request failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (DeserializeCodecCapability(reply, cap) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: DeserializeCodecCapbility failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    ReleaseSbuf(data, reply);
    return HDF_SUCCESS;
}

int32_t GetCapability(struct HdfRemoteService *remote,
    AvCodecMime mime, CodecType type, uint32_t flags, CodecCapbility *cap)
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
    if (!HdfRemoteServiceWriteInterfaceToken(remote, data)) {
        HDF_LOGE("write interface token failed");
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)mime)) {
        HDF_LOGE("%{public}s: write input mime failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, (uint32_t)type)) {
        HDF_LOGE("%{public}s: write input type failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, flags)) {
        HDF_LOGE("%{public}s: write input flags failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (remote->dispatcher->Dispatch(remote, CODEC_CONFIG_CMD_GET_CAP, data, reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: dispatch request failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (DeserializeCodecCapability(reply, cap) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: DeserializeCodecCapbility failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    ReleaseSbuf(data, reply);
    return HDF_SUCCESS;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

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

#include <hdf_sbuf.h>
#include <hdf_device_desc.h>
#include <hdf_device_object.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "codec_type.h"
#include "config_parser.h"
#include "codec_utils.h"

#define HDF_LOG_TAG "codec_config_service"

static CodecCapablites codecCapabilites = {0};
static const struct DeviceResourceNode *resourceNode;

static int32_t SerializeCapAlignment(struct HdfSBuf *reply, Alignment *alignment)
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

static int32_t SerializeCapRect(struct HdfSBuf *reply, Rect *rectangle)
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

static int32_t SerializeCapArray(struct HdfSBuf *reply, ResizableArray *resArr)
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

static int32_t SerializeCodecCapbility(struct HdfSBuf *reply, CodecCapbility *cap)
{
    if (reply == NULL || cap == NULL) {
        HDF_LOGE("%{public}s: params NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)cap->mime)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(reply, (uint32_t)cap->type)) {
        return HDF_FAILURE;
    }
    if (SerializeCapAlignment(reply, &cap->whAlignment) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (SerializeCapRect(reply, &cap->minSize) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (SerializeCapRect(reply, &cap->maxSize) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, cap->minBitRate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint64(reply, cap->maxBitRate)) {
        return HDF_FAILURE;
    }
    if (SerializeCapArray(reply, &cap->supportProfiles) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (SerializeCapArray(reply, &cap->supportLevels) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (SerializeCapArray(reply, &cap->supportPixelFormats) != HDF_SUCCESS) {
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

static int32_t EnumrateCapablity(int32_t index, CodecCapbility **capability)
{
    int32_t loopIndex;
    int32_t cursor = index;
    CodecCapablityGroup *codeCapGroups[] = {&(codecCapabilites.videoHwEncoderGroup),
                                            &(codecCapabilites.videoHwDecoderGroup),
                                            &(codecCapabilites.videoSwEncoderGroup),
                                            &(codecCapabilites.videoSwDecoderGroup),
                                            &(codecCapabilites.audioHwEncoderGroup),
                                            &(codecCapabilites.audioHwDecoderGroup),
                                            &(codecCapabilites.audioSwEncoderGroup),
                                            &(codecCapabilites.audioSwDecoderGroup)};

    for (loopIndex = 0; loopIndex < CODEC_CAPABLITY_GROUP_NUM; loopIndex++) {
        if (cursor + 1 <= codeCapGroups[loopIndex]->num) {
            *capability = &(codeCapGroups[loopIndex]->capablitis[cursor]);
            return HDF_SUCCESS;
        } else {
            cursor -= codeCapGroups[loopIndex]->num;
        }
    }

    return HDF_FAILURE;
}

static int32_t GetCapability(AvCodecMime mime, CodecType type, uint32_t flag, CodecCapbility **capability)
{
    int32_t groupIndex;
    int32_t capIndex;
    CodecCapablityGroup *group;
    CodecCapbility *cap;
    bool flagMatched = false;
    CodecCapablityGroup *codeCapGroups[] = {&(codecCapabilites.videoHwEncoderGroup),
                                            &(codecCapabilites.videoHwDecoderGroup),
                                            &(codecCapabilites.audioHwEncoderGroup),
                                            &(codecCapabilites.audioHwDecoderGroup),
                                            &(codecCapabilites.videoSwEncoderGroup),
                                            &(codecCapabilites.videoSwDecoderGroup),
                                            &(codecCapabilites.audioSwEncoderGroup),
                                            &(codecCapabilites.audioSwDecoderGroup)};
    const int32_t hardAndSoftSplitNumber = 4;

    for (groupIndex = 0; groupIndex < CODEC_CAPABLITY_GROUP_NUM; groupIndex++) {
        group = codeCapGroups[groupIndex];
        for (capIndex = 0; capIndex < group->num; capIndex++) {
            cap = &group->capablitis[capIndex];
            flagMatched = flag == 0 ? groupIndex < hardAndSoftSplitNumber : groupIndex >= hardAndSoftSplitNumber;
            if (mime == cap->mime && type == cap->type && flagMatched) {
                *capability = &group->capablitis[capIndex];
                return HDF_SUCCESS;
            }
        }
    }

    return HDF_FAILURE;
}

static int32_t ReloadCapabilities()
{
    ClearCapabilityGroup(&codecCapabilites);
    LoadCodecCapabilityFromHcs(resourceNode, &codecCapabilites);
    return HDF_SUCCESS;
}

static int32_t HandleEnumrateCmd(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    CodecCapbility *capbility = NULL;
    uint32_t index;

    if (!HdfSbufReadUint32(data, &index)) {
        HDF_LOGE("%{public}s: read index failed!", __func__);
        return HDF_FAILURE;
    }
    if (!codecCapabilites.inited) {
        return HDF_FAILURE;
    }
    if (EnumrateCapablity(index, &capbility) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: EnumrateCapablity - no more capability to Enumrate!", __func__);
        return HDF_FAILURE;
    }
    if (capbility->mime == MEDIA_MIMETYPE_INVALID) {
        HDF_LOGE("%{public}s: Capablity invalid, discard!", __func__);
        return HDF_FAILURE;
    }
    if (SerializeCodecCapbility(reply, capbility) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write capbility to sbuf failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t HandleGetCmd(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    CodecCapbility *capbility = NULL;
    AvCodecMime mime;
    CodecType type;
    uint32_t flags;

    if (!HdfSbufReadUint32(data, (uint32_t*)&mime)) {
        HDF_LOGE("%{public}s: read input mime failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, (uint32_t*)&type)) {
        HDF_LOGE("%{public}s: read input type failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(data, &flags)) {
        HDF_LOGE("%{public}s: read input flags failed!", __func__);
        return HDF_FAILURE;
    }
    if (GetCapability(mime, type, flags, &capbility) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: GetCapability - got nothing!", __func__);
        return HDF_FAILURE;
    }
    if (SerializeCodecCapbility(reply, capbility) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write capbility to sbuf failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t CodecCapabilityDispatch(struct HdfDeviceIoClient *client, int id,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t result = HDF_SUCCESS;

    if (client == NULL || data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: params NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        HDF_LOGE("check interface token failed");
        return HDF_ERR_INVALID_PARAM;
    }
    if (!codecCapabilites.inited) {
        ReloadCapabilities();
    }

    switch (id) {
        case CODEC_CONFIG_CMD_ENUMERATE_CAP: {
            result = HandleEnumrateCmd(data, reply);
            break;
        }

        case CODEC_CONFIG_CMD_GET_CAP: {
            result = HandleGetCmd(data, reply);
            break;
        }

        default:
            break;
    }

    return result;
}

static int32_t CodecCapabilityBind(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }

    static struct IDeviceIoService deviceIoService = {
        .Dispatch = CodecCapabilityDispatch,
    };
    deviceObject->service = &deviceIoService;

    return HDF_SUCCESS;
}

static int32_t CodecCapabilityInit(struct HdfDeviceObject *deviceObject)
{
    if (deviceObject == NULL) {
        return HDF_FAILURE;
    }
    resourceNode = deviceObject->property;
    if (LoadCodecCapabilityFromHcs(deviceObject->property, &codecCapabilites) != HDF_SUCCESS) {
        ClearCapabilityGroup(&codecCapabilites);
    }
    return HDF_SUCCESS;
}

static void CodecCapabilityRelease(struct HdfDeviceObject *deviceObject)
{
    ClearCapabilityGroup(&codecCapabilites);
    return;
}

struct HdfDriverEntry g_codecConfigDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "codec_capability_config",  // corresponding serviceName is "codec_capability_config_service";
    .Bind = CodecCapabilityBind,
    .Init = CodecCapabilityInit,
    .Release = CodecCapabilityRelease,
};

HDF_INIT(g_codecConfigDriverEntry);

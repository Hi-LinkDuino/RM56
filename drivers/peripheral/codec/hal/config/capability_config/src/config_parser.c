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

#include "config_parser.h"
#include <osal_mem.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "codec_utils.h"

#define HDF_LOG_TAG "codec_config_parser"

static int32_t GetGroupCapabilitiesNumber(const struct DeviceResourceNode *node,
    const char *nodeName, int32_t *num)
{
    int result = 0;
    const struct DeviceResourceNode *codecGroupNode = NULL;
    struct DeviceResourceNode *childNode = NULL;
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    *num = 0;
    if (iface ==NULL || node == NULL || nodeName == NULL) {
        HDF_LOGE("%{public}s, failed for codecs %{public}s, variable NULL!", __func__, nodeName);
        return HDF_FAILURE;
    }

    codecGroupNode = iface->GetChildNode(node, nodeName);
    if (codecGroupNode == NULL) {
        HDF_LOGE("%{public}s, failed to get child node %{public}s,!", __func__, nodeName);
        return HDF_FAILURE;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(codecGroupNode, childNode) {
        result++;
    }
    *num = result;

    return HDF_SUCCESS;
}

static int32_t GetUintTableConfig(const struct DeviceResourceIface *iface,
    const struct DeviceResourceNode *node, const char *attrName, ResizableArray *table)
{
    int32_t count = iface->GetElemNum(node, attrName);

    table->actualLen = 0;
    if (count <= 0) {
        return HDF_FAILURE;
    }
    if (count > ELEMENT_MAX_LEN) {
        HDF_LOGE("%{public}s, table size: %{public}d, exceeded max size %{public}d!",
            __func__, count, ELEMENT_MAX_LEN);
        return HDF_FAILURE;
    }

    iface->GetUint32Array(node, attrName, table->element, count, 0);
    table->actualLen = count;

    return HDF_SUCCESS;
}

static int32_t GetMaskedConfig(const struct DeviceResourceIface *iface,
    const struct DeviceResourceNode *node, const char *attrName, uint32_t *mask)
{
    int32_t index = 0;
    uint32_t *values = NULL;
    int32_t count = iface->GetElemNum(node, attrName);

    *mask = 0;
    if (count <= 0) {
        return HDF_FAILURE;
    }

    values = (uint32_t *)OsalMemAlloc(sizeof(uint32_t) * count);
    if (values == NULL) {
        HDF_LOGE("%{public}s, failed to allocate mem for %{public}s!", __func__, attrName);
        return HDF_FAILURE;
    }
    iface->GetUint32Array(node, attrName, values, count, 0);
    for (index = 0; index < count; index++) {
        *mask |= values[index];
    }
    OsalMemFree(values);

    return HDF_SUCCESS;
}

static int32_t GetOneCapability(const struct DeviceResourceIface *iface,
    const struct DeviceResourceNode *childNode, CodecCapbility *cap)
{
    if (iface == NULL || childNode == NULL || cap == NULL) {
        return HDF_FAILURE;
    }
    if (iface->GetUint32(childNode, CODEC_CONFIG_KEY_MIME,
        (uint32_t*)&cap->mime, MEDIA_MIMETYPE_INVALID) != HDF_SUCCESS) {
        cap->mime = MEDIA_MIMETYPE_INVALID;
        HDF_LOGE("%{public}s, failed to get mime for: %{public}s! Discarded", __func__, childNode->name);
        return HDF_FAILURE;
    }

    if (iface->GetUint32(childNode, CODEC_CONFIG_KEY_TYPE, (uint32_t*)&cap->type, INVALID_TYPE) != HDF_SUCCESS) {
        cap->mime = MEDIA_MIMETYPE_INVALID;
        cap->type = INVALID_TYPE;
        HDF_LOGE("%{public}s, failed to get type for: %{public}s! Discarded", __func__, childNode->name);
        return HDF_FAILURE;
    }

    if (iface->GetUint32(childNode, CODEC_CONFIG_KEY_WIDTH_ALIGNMENT,
        (uint32_t*)&cap->whAlignment.widthAlignment, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_HEIGHT_ALIGNMENT,
            (uint32_t*)&cap->whAlignment.heightAlignment, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MIN_WIDTH, (uint32_t*)&cap->minSize.width, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MIN_HEIGHT, (uint32_t*)&cap->minSize.height, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MAX_WIDTH, (uint32_t*)&cap->maxSize.width, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MAX_HEIGHT, (uint32_t*)&cap->maxSize.height, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MIN_BITRATE, (uint32_t*)&cap->minBitRate, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MAX_BITRATE, (uint32_t*)&cap->maxBitRate, 0) != HDF_SUCCESS
        || GetUintTableConfig(iface, childNode, CODEC_CONFIG_KEY_SUPPORT_PROFILES,
            &(cap->supportProfiles)) != HDF_SUCCESS
        || GetUintTableConfig(iface, childNode, CODEC_CONFIG_KEY_SUPPORT_LEVELS, &(cap->supportLevels)) != HDF_SUCCESS
        || GetUintTableConfig(iface, childNode, CODEC_CONFIG_KEY_SUPPORT_PIXELF_MTS,
            &(cap->supportPixelFormats)) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MIN_INPUT_BUFFER_NUM,
            &cap->minInputBufferNum, 0) != HDF_SUCCESS
        || iface->GetUint32(childNode, CODEC_CONFIG_KEY_MIN_OUTPUT_BUFFER_NUM,
            &cap->minOutputBufferNum, 0) != HDF_SUCCESS
        || GetMaskedConfig(iface, childNode, CODEC_CONFIG_KEY_ALLOCATE_MASK, &cap->allocateMask) != HDF_SUCCESS
        || GetMaskedConfig(iface, childNode, CODEC_CONFIG_KEY_CAPS_MASK, &cap->capsMask) != HDF_SUCCESS) {
            cap->mime = MEDIA_MIMETYPE_INVALID;
            return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t GetGroupCapabilities(const struct DeviceResourceNode *node,
    const char *nodeName, CodecCapablityGroup *capsGroup)
{
    CodecCapbility *cap;
    int32_t index = 0;
    const struct DeviceResourceNode *codecGroupNode = NULL;
    struct DeviceResourceNode *childNode = NULL;
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface ==NULL || node == NULL || nodeName == NULL) {
        HDF_LOGE("%{public}s, failed for node %{public}s, variable NULL!", __func__, nodeName);
        return HDF_FAILURE;
    }

    codecGroupNode = iface->GetChildNode(node, nodeName);
    if (codecGroupNode == NULL) {
        HDF_LOGE("%{public}s, failed to get child node: %{public}s!", __func__, nodeName);
        return HDF_FAILURE;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(codecGroupNode, childNode) {
        cap = &(capsGroup->capablitis[index++]);
        GetOneCapability(iface, childNode, cap);
    }

    return HDF_SUCCESS;
}

int32_t LoadCodecCapabilityFromHcs(const struct DeviceResourceNode *node, CodecCapablites *caps)
{
    CodecCapablityGroup *codecCapGroup;
    int32_t index;
    int32_t codecNum = 0;

    if (node == NULL) {
        HDF_LOGE("%{public}s, load capability failed, node is null!", __func__);
        return HDF_FAILURE;
    }

    char *codecGroupsNodeName[] = {
        NODE_VIDEO_HARDWARE_ENCODERS, NODE_VIDEO_HARDWARE_DECODERS,
        NODE_VIDEO_SOFTWARE_ENCODERS, NODE_VIDEO_SOFTWARE_DECODERS,
        NODE_AUDIO_HARDWARE_ENCODERS, NODE_AUDIO_HARDWARE_DECODERS,
        NODE_AUDIO_SOFTWARE_ENCODERS, NODE_AUDIO_SOFTWARE_DECODERS
    };
    CodecCapablityGroup *codecCapGroups[] = {
        &(caps->videoHwEncoderGroup), &(caps->videoHwDecoderGroup),
        &(caps->videoSwEncoderGroup), &(caps->videoSwDecoderGroup),
        &(caps->audioHwEncoderGroup), &(caps->audioHwDecoderGroup),
        &(caps->audioSwEncoderGroup), &(caps->audioSwDecoderGroup)
    };

    for (index = 0; index < CODEC_CAPABLITY_GROUP_NUM; index++) {
        if (GetGroupCapabilitiesNumber(node, codecGroupsNodeName[index], &codecNum) == HDF_SUCCESS) {
            codecCapGroup = codecCapGroups[index];
            codecCapGroup->num = codecNum;
            if (codecNum > 0) {
                codecCapGroup->capablitis
                    = (CodecCapbility *)OsalMemAlloc(sizeof(CodecCapbility) * codecNum);
            } else {
                codecCapGroup->capablitis = NULL;
            }
            if (codecNum > 0 && codecCapGroup->capablitis == NULL) {
                codecCapGroup->num = 0;
                HDF_LOGE("%{public}s, MemAlloc for capability group failed!", __func__);
                return HDF_FAILURE;
            }
        }
    }

    for (index = 0; index < CODEC_CAPABLITY_GROUP_NUM; index++) {
        if (GetGroupCapabilities(node, codecGroupsNodeName[index], codecCapGroups[index]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
    }

    caps->inited = true;
    return HDF_SUCCESS;
}

int32_t ClearCapabilityGroup(CodecCapablites *caps)
{
    int32_t index;
    CodecCapablityGroup *codecCapGroup;
    CodecCapablityGroup *codecCapGroups[] = {
        &(caps->videoHwEncoderGroup), &(caps->videoHwDecoderGroup),
        &(caps->videoSwEncoderGroup), &(caps->videoSwDecoderGroup),
        &(caps->audioHwEncoderGroup), &(caps->audioHwDecoderGroup),
        &(caps->audioSwEncoderGroup), &(caps->audioSwDecoderGroup)
    };
    for (index = 0; index < CODEC_CAPABLITY_GROUP_NUM; index++) {
        codecCapGroup = codecCapGroups[index];
        if (codecCapGroup->capablitis != NULL) {
            OsalMemFree(codecCapGroup->capablitis);
            codecCapGroup->num = 0;
            codecCapGroup->capablitis = NULL;
        }
    }
    caps->inited = false;

    return HDF_SUCCESS;
}
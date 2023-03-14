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

#include "codec_component_capability_config.h"
#include <hdf_log.h>
#include "codec_config_parser.h"
#include "codec_types.h"

#define HDF_LOG_TAG codec_hdi_server

static CodecCapablites g_codecCapabilites = {0};
static const struct DeviceResourceNode *g_resourceNode = NULL;

static int32_t AllCapabilityMarshalling(struct HdfSBuf *reply)
{
    int32_t groupIndex;
    int32_t capIndex;
    CodecCapablityGroup *group = NULL;
    CodecCompCapability *cap = NULL;
    CodecCapablityGroup *codeCapGroups[] = {
        &(g_codecCapabilites.videoHwEncoderGroup), &(g_codecCapabilites.videoHwDecoderGroup),
        &(g_codecCapabilites.audioHwEncoderGroup), &(g_codecCapabilites.audioHwDecoderGroup),
        &(g_codecCapabilites.videoSwEncoderGroup), &(g_codecCapabilites.videoSwDecoderGroup),
        &(g_codecCapabilites.audioSwEncoderGroup), &(g_codecCapabilites.audioSwDecoderGroup)
    };

    for (groupIndex = 0; groupIndex < CODEC_CAPABLITY_GROUP_NUM; groupIndex++) {
        group = codeCapGroups[groupIndex];
        for (capIndex = 0; capIndex < group->num; capIndex++) {
            cap = &group->capablitis[capIndex];
            if (!CodecCompCapabilityBlockMarshalling(reply, cap)) {
                HDF_LOGE("%{public}s: write capbility to sbuf failed!", __func__);
                return HDF_FAILURE;
            }
        }
    }

    return HDF_SUCCESS;
}

int32_t InitDataNode(const struct DeviceResourceNode *node)
{
    if (node == NULL) {
        HDF_LOGE("%{public}s: data node is null!", __func__);
        return HDF_FAILURE;
    }
    g_resourceNode = node;
    return HDF_SUCCESS;
}

int32_t ClearCapabilityData()
{
    return ClearCapabilityGroup(&g_codecCapabilites);
}

int32_t LoadCapabilityData()
{
    return LoadCodecCapabilityFromHcs(g_resourceNode, &g_codecCapabilites);
}

int32_t HandleGetNumCmd(struct HdfSBuf *reply)
{
    if (!g_codecCapabilites.inited) {
        HDF_LOGE("%{public}s: g_codecCapabilites not init!", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteInt32(reply, g_codecCapabilites.total)) {
        HDF_LOGE("%{public}s: write num failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t HandleGetAllCapablityListCmd(struct HdfSBuf *reply)
{
    if (!g_codecCapabilites.inited) {
        HDF_LOGE("%{public}s: g_codecCapabilites not init!", __func__);
        return HDF_FAILURE;
    }

    if (AllCapabilityMarshalling(reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write all capbility to sbuf failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

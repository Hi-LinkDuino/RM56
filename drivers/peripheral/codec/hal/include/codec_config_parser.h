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

#ifndef HDI_CODECCONFIGPARSER_H
#define HDI_CODECCONFIGPARSER_H

#include "codec_component_type.h"
#include "device_resource_if.h"

#define NODE_VIDEO_HARDWARE_ENCODERS    "VideoHwEncoders"
#define NODE_VIDEO_HARDWARE_DECODERS    "VideoHwDecoders"
#define NODE_VIDEO_SOFTWARE_ENCODERS    "VideoSwEncoders"
#define NODE_VIDEO_SOFTWARE_DECODERS    "VideoSwDecoders"
#define NODE_AUDIO_HARDWARE_ENCODERS    "AudioHwEncoders"
#define NODE_AUDIO_HARDWARE_DECODERS    "AudioHwDecoders"
#define NODE_AUDIO_SOFTWARE_ENCODERS    "AudioSwEncoders"
#define NODE_AUDIO_SOFTWARE_DECODERS    "AudioSwDecoders"

#define CODEC_CONFIG_KEY_ROLE                   "role"
#define CODEC_CONFIG_KEY_TYPE                   "type"
#define CODEC_CONFIG_KEY_NAME                   "name"
#define CODEC_CONFIG_KEY_SUPPORT_PROFILES       "supportProfiles"
#define CODEC_CONFIG_KEY_MAX_INST               "maxInst"
#define CODEC_CONFIG_KEY_IS_SOFTWARE_CODEC      "isSoftwareCodec"
#define CODEC_CONFIG_KEY_PROCESS_MODE_MASK      "processModeMask"
#define CODEC_CONFIG_KEY_CAPS_MASK              "capsMask"
#define CODEC_CONFIG_KEY_MIN_BITRATE            "minBitRate"
#define CODEC_CONFIG_KEY_MAX_BITRATE            "maxBitRate"

#define CODEC_CONFIG_KEY_MIN_WIDTH              "minWidth"
#define CODEC_CONFIG_KEY_MIN_HEIGHT             "minHeight"
#define CODEC_CONFIG_KEY_MAX_WIDTH              "maxWidth"
#define CODEC_CONFIG_KEY_MAX_HEIGHT             "maxHeight"
#define CODEC_CONFIG_KEY_WIDTH_ALIGNMENT        "widthAlignment"
#define CODEC_CONFIG_KEY_HEIGHT_ALIGNMENT       "heightAlignment"
#define CODEC_CONFIG_KEY_MIN_BLOCK_COUNT        "minBlockCount"
#define CODEC_CONFIG_KEY_MAX_BLOCK_COUNT        "maxBlockCount"
#define CODEC_CONFIG_KEY_MIN_BLOCKS_PER_SECOND  "minBlocksPerSecond"
#define CODEC_CONFIG_KEY_MAX_BLOCKS_PER_SECOND  "maxBlocksPerSecond"
#define CODEC_CONFIG_KEY_SUPPORT_PIXEL_FMTS     "supportPixelFmts"
#define CODEC_CONFIG_KEY_BLOCK_SIZE_WIDTH       "blockSizeWidth"
#define CODEC_CONFIG_KEY_BLOCK_SIZE_HEIGHT      "blockSizeHeight"

#define CODEC_CONFIG_KEY_SAMPLE_FORMATS         "sampleFormats"
#define CODEC_CONFIG_KEY_SAMPLE_RATE            "sampleRate"
#define CODEC_CONFIG_KEY_CHANNEL_LAYOUTS        "channelLayouts"
#define CODEC_CONFIG_KEY_CHANNEL_COUNT          "channelCount"

#define CODEC_CAPABLITY_GROUP_NUM  8

typedef struct {
    int32_t num;
    CodecCompCapability *capablitis;
} CodecCapablityGroup;

typedef struct {
    CodecCapablityGroup videoHwEncoderGroup;
    CodecCapablityGroup videoHwDecoderGroup;
    CodecCapablityGroup videoSwEncoderGroup;
    CodecCapablityGroup videoSwDecoderGroup;
    CodecCapablityGroup audioHwEncoderGroup;
    CodecCapablityGroup audioHwDecoderGroup;
    CodecCapablityGroup audioSwEncoderGroup;
    CodecCapablityGroup audioSwDecoderGroup;
    int32_t total;
    bool inited;
} CodecCapablites;

typedef struct {
    const char *attrName;
    uint32_t *valueAddr;
    uint32_t defaultValue;
} ConfigUintNodeAttr;

typedef struct {
    const char *attrName;
    int32_t *array;
    int32_t length;
    int32_t endValue;
} ConfigUintArrayNodeAttr;

int32_t LoadCodecCapabilityFromHcs(const struct DeviceResourceNode *node, CodecCapablites *caps);
int32_t ClearCapabilityGroup(CodecCapablites *caps);

#endif  // HDI_CODECCONFIGPARSER_H

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

#ifndef DRIVERS_PERIPHERAL_CODEC_CONFIG_PARSER_H_
#define DRIVERS_PERIPHERAL_CODEC_CONFIG_PARSER_H_

#define NODE_VIDEO_HARDWARE_ENCODERS    "VideoHwEncoders"
#define NODE_VIDEO_HARDWARE_DECODERS    "VideoHwDecoders"
#define NODE_VIDEO_SOFTWARE_ENCODERS    "VideoSwEncoders"
#define NODE_VIDEO_SOFTWARE_DECODERS    "VideoSwDecoders"
#define NODE_AUDIO_HARDWARE_ENCODERS    "AudioHwEncoders"
#define NODE_AUDIO_HARDWARE_DECODERS    "AudioHwDecoders"
#define NODE_AUDIO_SOFTWARE_ENCODERS    "AudioSwEncoders"
#define NODE_AUDIO_SOFTWARE_DECODERS    "AudioSwDecoders"

#define CODEC_CONFIG_KEY_MIME                   "mime"
#define CODEC_CONFIG_KEY_TYPE                   "type"
#define CODEC_CONFIG_KEY_SUPPORT_PROFILES       "supportProfiles"
#define CODEC_CONFIG_KEY_IS_SOFTWARE_CODEC      "isSoftwareCodec"
#define CODEC_CONFIG_KEY_CAPS_MASK              "capsMask"
#define CODEC_CONFIG_KEY_ALLOCATE_MASK          "allocateMask"
#define CODEC_CONFIG_KEY_MIN_INPUT_BUFFER_NUM   "minInputBufferNum"
#define CODEC_CONFIG_KEY_MAX_INPUT_BUFFER_NUM   "maxInputBufferNum"
#define CODEC_CONFIG_KEY_MIN_OUTPUT_BUFFER_NUM  "minOutputBufferNum"
#define CODEC_CONFIG_KEY_MAX_OUTPUT_BUFFER_NUM  "maxOutputBufferNum"
#define CODEC_CONFIG_KEY_MIN_BITRATE            "minBitRate"
#define CODEC_CONFIG_KEY_MAX_BITRATE            "maxBitRate"
#define CODEC_CONFIG_KEY_BUFFER_SIZE            "bufferSize"
#define CODEC_CONFIG_KEY_MIN_WIDTH              "minWidth"
#define CODEC_CONFIG_KEY_MIN_HEIGHT             "minHeight"
#define CODEC_CONFIG_KEY_MAX_WIDTH              "maxWidth"
#define CODEC_CONFIG_KEY_MAX_HEIGHT             "maxHeight"
#define CODEC_CONFIG_KEY_WIDTH_ALIGNMENT        "widthAlignment"
#define CODEC_CONFIG_KEY_HEIGHT_ALIGNMENT       "heightAlignment"
#define CODEC_CONFIG_KEY_SUPPORT_PIXELF_MTS     "supportPixelFmts"
#define CODEC_CONFIG_KEY_SUPPORT_LEVELS         "supportLevels"
#define CODEC_CONFIG_KEY_SAMPLE_FORMATS          "sampleFormats"
#define CODEC_CONFIG_KEY_SAMPLE_RATE             "sampleRate"
#define CODEC_CONFIG_KEY_CHANNEL_COUNT           "channelCount"

#define CODEC_CAPABLITY_GROUP_NUM  8

#include <device_resource_if.h>
#include "codec_type.h"

typedef enum {
    CODEC_CONFIG_CMD_ENUMERATE_CAP,
    CODEC_CONFIG_CMD_GET_CAP
} CodecInterfaceCmd;

typedef struct {
    int num;
    CodecCapbility *capablitis;
} CodecCapablityGroup;

typedef struct {
    bool inited;
    CodecCapablityGroup videoHwEncoderGroup;
    CodecCapablityGroup videoHwDecoderGroup;
    CodecCapablityGroup videoSwEncoderGroup;
    CodecCapablityGroup videoSwDecoderGroup;
    CodecCapablityGroup audioHwEncoderGroup;
    CodecCapablityGroup audioHwDecoderGroup;
    CodecCapablityGroup audioSwEncoderGroup;
    CodecCapablityGroup audioSwDecoderGroup;
} CodecCapablites;

int32_t LoadCodecCapabilityFromHcs(const struct DeviceResourceNode *node, CodecCapablites *caps);
int32_t ClearCapabilityGroup(CodecCapablites *caps);

#endif  // DRIVERS_PERIPHERAL_CODEC_CONFIG_PARSER_H_

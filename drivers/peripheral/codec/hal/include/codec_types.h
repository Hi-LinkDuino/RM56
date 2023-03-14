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

#ifndef HDI_CODECTYPES_H
#define HDI_CODECTYPES_H

#include "codec_component_type.h"
#include "hdf_sbuf.h"
#include "OMX_Core.h"
#include "OMX_Index.h"
#include "OMX_Types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool OMX_TUNNELSETUPTYPEBlockMarshalling(struct HdfSBuf *data, const struct OMX_TUNNELSETUPTYPE *dataBlock);

bool OMX_TUNNELSETUPTYPEBlockUnmarshalling(struct HdfSBuf *data, struct OMX_TUNNELSETUPTYPE *dataBlock);

void OMX_TUNNELSETUPTYPEFree(struct OMX_TUNNELSETUPTYPE *dataBlock, bool freeSelf);

bool OmxCodecBufferBlockMarshalling(struct HdfSBuf *data, const struct OmxCodecBuffer *dataBlock);

bool OmxCodecBufferBlockUnmarshalling(struct HdfSBuf *data, struct OmxCodecBuffer *dataBlock);

// for config marshall
bool RangeValueBlockMarshalling(struct HdfSBuf *data, const RangeValue *dataBlock);

bool RangeValueBlockUnmarshalling(struct HdfSBuf *data, RangeValue *dataBlock);

bool RectBlockMarshalling(struct HdfSBuf *data, const Rect *dataBlock);

bool RectBlockUnmarshalling(struct HdfSBuf *data, Rect *dataBlock);

bool AlignmentBlockMarshalling(struct HdfSBuf *data, const Alignment *dataBlock);

bool AlignmentBlockUnmarshalling(struct HdfSBuf *data, Alignment *dataBlock);

bool VideoPortCapBlockMarshalling(struct HdfSBuf *data, const VideoPortCap *dataBlock);

bool VideoPortCapBlockUnmarshalling(struct HdfSBuf *data, VideoPortCap *dataBlock);

void VideoPortCapFree(VideoPortCap *dataBlock, bool freeSelf);

bool AudioPortCapBlockMarshalling(struct HdfSBuf *data, const AudioPortCap *dataBlock);

bool AudioPortCapBlockUnmarshalling(struct HdfSBuf *data, AudioPortCap *dataBlock);

void AudioPortCapFree(AudioPortCap *dataBlock, bool freeSelf);

bool CodecCompCapabilityBlockMarshalling(struct HdfSBuf *data, const CodecCompCapability *dataBlock);

bool CodecCompCapabilityBlockUnmarshalling(struct HdfSBuf *data, CodecCompCapability *dataBlock);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_CODECTYPES_H
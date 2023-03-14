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

#ifndef HDI_CODEC_CLIENT_INF_H
#define HDI_CODEC_CLIENT_INF_H

#include "codec_callback.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfRemoteService;

enum {
    CMD_CODEC_INIT,
    CMD_CODEC_DEINIT,
    CMD_CODEC_ENUM_CAP,
    CMD_CODEC_GET_CAP,
    CMD_CODEC_CREATE,
    CMD_CODEC_DESTROY,
    CMD_CODEC_SET_MODE,
    CMD_CODEC_SET_PARAMS,
    CMD_CODEC_GET_PARAMS,
    CMD_CODEC_START,
    CMD_CODEC_STOP,
    CMD_CODEC_FLUSH,
    CMD_CODEC_QUEQUE_INPUT,
    CMD_CODEC_DEQUEQUE_INPUT,
    CMD_CODEC_QUEQUE_OUTPUT,
    CMD_CODEC_DEQUEQUE_OUTPUT,
    CMD_CODEC_SET_CBK,
};

struct ICodec {
    struct HdfRemoteService *remote;
    int32_t (*CodecInit)(struct ICodec *self);
    int32_t (*CodecDeinit)(struct ICodec *self);
    int32_t (*CodecEnumerateCapbility)(struct ICodec *self, uint32_t index, CodecCapbility *cap);
    int32_t (*CodecGetCapbility)(struct ICodec *self, AvCodecMime mime, CodecType type,
                                 uint32_t flags, CodecCapbility *cap);
    int32_t (*CodecCreate)(struct ICodec *self, const char* name, const Param *attr,
                           int len, CODEC_HANDLETYPE *handle);
    int32_t (*CodecDestroy)(struct ICodec *self, CODEC_HANDLETYPE handle);
    int32_t (*CodecSetPortMode)(struct ICodec *self, CODEC_HANDLETYPE handle, DirectionType type, BufferMode mode);
    int32_t (*CodecSetParameter)(struct ICodec *self, CODEC_HANDLETYPE handle, const Param *params, int paramCnt);
    int32_t (*CodecGetParameter)(struct ICodec *self, CODEC_HANDLETYPE handle, Param *params, int paramCnt);
    int32_t (*CodecStart)(struct ICodec *self, CODEC_HANDLETYPE handle);
    int32_t (*CodecStop)(struct ICodec *self, CODEC_HANDLETYPE handle);
    int32_t (*CodecFlush)(struct ICodec *self, CODEC_HANDLETYPE handle, DirectionType directType);
    int32_t (*CodecQueueInput)(struct ICodec *self, CODEC_HANDLETYPE handle,
                               const InputInfo *inputData, uint32_t timeoutMs);
    int32_t (*CodecDequeInput)(struct ICodec *self, CODEC_HANDLETYPE handle,
                               uint32_t timeoutMs, InputInfo *inputData);
    int32_t (*CodecQueueOutput)(struct ICodec *self, CODEC_HANDLETYPE handle, OutputInfo *outInfo,
                                uint32_t timeoutMs, int releaseFenceFd);
    int32_t (*CodecDequeueOutput)(struct ICodec *self, CODEC_HANDLETYPE handle, uint32_t timeoutMs,
                                  int *acquireFd, OutputInfo *outInfo);
    int32_t (*CodecSetCallback)(struct ICodec *self, CODEC_HANDLETYPE handle,
                                struct ICodecCallback *cb, UINTPTR instance);
};

struct ICodec *HdiCodecGet(const char *serviceName);
void HdiCodecRelease(struct ICodec *instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_SAMPLE_CLIENT_C_INF_H
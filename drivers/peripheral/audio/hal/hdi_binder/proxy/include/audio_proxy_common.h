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
#ifndef AUDIO_PROXY_COMMON_H
#define AUDIO_PROXY_COMMON_H

#include "audio_internal.h"
#include "hdf_audio_server.h"

#define CONFIG_CHANNEL_COUNT  2 // two channels
#define GAIN_MAX 50.0
#define STR_MAX 512

struct HdfSBuf *AudioProxyObtainHdfSBuf();
void AudioProxyBufReplyRecycle(struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t AudioProxyPreprocessSBuf(struct HdfSBuf **data, struct HdfSBuf **reply);
int32_t AudioProxyDispatchCall(struct HdfRemoteService *self, int32_t id, struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t AudioProxyPreprocessRender(struct AudioHwRender *render, struct HdfSBuf **data, struct HdfSBuf **reply);
int32_t AudioProxyPreprocessCapture(struct AudioHwCapture *capture, struct HdfSBuf **data, struct HdfSBuf **reply);
int32_t AudioProxyWriteSampleAttributes(struct HdfSBuf *data, const struct AudioSampleAttributes *attrs);
int32_t AudioProxyReadSapmleAttrbutes(struct HdfSBuf *reply, struct AudioSampleAttributes *attrs);
int32_t AudioProxyCommonSetRenderCtrlParam(int cmId, AudioHandle handle, float param);
int32_t AudioProxyCommonGetRenderCtrlParam(int cmId, AudioHandle handle, float *param);
int32_t AudioProxyCommonSetCaptureCtrlParam(int cmId, AudioHandle handle, float param);
int32_t AudioProxyCommonGetCaptureCtrlParam(int cmId, AudioHandle handle, float *param);
int32_t AudioProxyGetMmapPositionRead(struct HdfSBuf *reply, uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioProxyReqMmapBufferWrite(struct HdfSBuf *data, int32_t reqSize,
    const struct AudioMmapBufferDescripter *desc);

#endif

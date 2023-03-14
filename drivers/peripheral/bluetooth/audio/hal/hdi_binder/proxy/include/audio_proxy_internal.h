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

#ifndef AUDIO_PROXY_INTERNAL_H
#define AUDIO_PROXY_INTERNAL_H

#include "audio_adapter.h"
namespace OHOS::HDI::Audio_Bluetooth {
int32_t AudioProxyAdapterInitAllPorts(struct AudioAdapter *adapter);
int32_t AudioProxyAdapterCreateRender(struct AudioAdapter *adapter,
    const struct AudioDeviceDescriptor *desc,
    const struct AudioSampleAttributes *attrs,
    struct AudioRender **render);
int32_t AudioProxyAdapterDestroyRender(struct AudioAdapter *adapter,
    struct AudioRender *render);
int32_t AudioProxyAdapterGetPortCapability(struct AudioAdapter *adapter,
    const struct AudioPort *port, struct AudioPortCapability *capability);
int32_t AudioProxyAdapterSetPassthroughMode(struct AudioAdapter *adapter,
    const struct AudioPort *port, AudioPortPassthroughMode mode);
int32_t AudioProxyAdapterGetPassthroughMode(struct AudioAdapter *adapter,
    const struct AudioPort *port, AudioPortPassthroughMode *mode);
int32_t AudioProxyRenderStart(AudioHandle handle);
int32_t AudioProxyRenderStop(AudioHandle handle);
int32_t AudioProxyRenderPause(AudioHandle handle);
int32_t AudioProxyRenderResume(AudioHandle handle);
int32_t AudioProxyRenderFlush(AudioHandle handle);
int32_t AudioProxyRenderGetFrameSize(const AudioHandle handle, uint64_t *size);
int32_t AudioProxyRenderGetFrameCount(const AudioHandle handle, uint64_t *count);
int32_t AudioProxyRenderSetSampleAttributes(const AudioHandle handle,
    const struct AudioSampleAttributes *attrs);
int32_t AudioProxyRenderGetSampleAttributes(const AudioHandle handle,
    struct AudioSampleAttributes *attrs);
int32_t AudioProxyRenderGetCurrentChannelId(const AudioHandle handle, uint32_t *channelId);
int32_t AudioProxyRenderCheckSceneCapability(const AudioHandle handle,
    const struct AudioSceneDescriptor *scene, bool *supported);
int32_t AudioProxyRenderSelectScene(const AudioHandle handle,
    const struct AudioSceneDescriptor *scene);
int32_t AudioProxyRenderSetMute(const AudioHandle handle, bool mute);
int32_t AudioProxyRenderGetMute(const AudioHandle handle, bool *mute);
int32_t AudioProxyRenderSetVolume(const AudioHandle handle, float volume);
int32_t AudioProxyRenderGetVolume(const AudioHandle handle, float *volume);
int32_t AudioProxyRenderGetGainThreshold(AudioHandle handle, float *min, float *max);
int32_t AudioProxyRenderGetGain(const AudioHandle handle, float *gain);
int32_t AudioProxyRenderSetGain(const AudioHandle handle, float gain);
int32_t AudioProxyRenderGetLatency(struct AudioRender *render, uint32_t *ms);
int32_t AudioProxyRenderRenderFrame(struct AudioRender *render, const void *frame,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t AudioProxyRenderGetRenderPosition(struct AudioRender *render,
    uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioProxyRenderSetRenderSpeed(struct AudioRender *render, float speed);
int32_t AudioProxyRenderGetRenderSpeed(struct AudioRender *render, float *speed);
int32_t AudioProxyRenderSetChannelMode(struct AudioRender *render, AudioChannelMode mode);
int32_t AudioProxyRenderGetChannelMode(struct AudioRender *render, AudioChannelMode *mode);
int32_t AudioProxyRenderSetExtraParams(AudioHandle render, const char *keyValueList);
int32_t AudioProxyRenderGetExtraParams(AudioHandle render, char *keyValueList, int32_t listLenth);
int32_t AudioProxyRenderReqMmapBuffer(AudioHandle render, int32_t reqSize, struct AudioMmapBufferDescripter *desc);
int32_t AudioProxyRenderGetMmapPosition(AudioHandle render, uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioProxyRenderTurnStandbyMode(AudioHandle render);
int32_t AudioProxyRenderAudioDevDump(AudioHandle render, int32_t range, int32_t fd);
int32_t AudioProxyRenderRegCallback(struct AudioRender *render, RenderCallback callback, void *cookie);
int32_t AudioProxyRenderDrainBuffer(struct AudioRender *render, AudioDrainNotifyType *type);
}
#endif

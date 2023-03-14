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

#ifndef HDF_AUDIO_SERVER_RENDER_H
#define HDF_AUDIO_SERVER_RENDER_H

#include "hdf_device_desc.h"
namespace OHOS::HDI::Audio_Bluetooth {
int32_t HdiServiceCreatRender(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderDestory(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderStart(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderStop(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderPause(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderResume(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderFlush(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetFrameSize(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetFrameCount(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetSampleAttr(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetSampleAttr(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetCurChannelId(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderCheckSceneCapability(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSelectScene(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetMute(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetMute(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetVolume(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetVolume(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetGainThreshold(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetGain(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetGain(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetLatency(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderRenderFrame(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetRenderPosition(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetSpeed(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetSpeed(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetChannelMode(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetChannelMode(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderSetExtraParams(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetExtraParams(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderReqMmapBuffer(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderGetMmapPosition(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderTurnStandbyMode(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderDevDump(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderRegCallback(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceRenderDrainBuffer(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
}
#endif

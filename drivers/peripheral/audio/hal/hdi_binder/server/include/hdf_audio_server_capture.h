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
#ifndef HDF_AUDIO_SERVER_CAPTURE_H
#define HDF_AUDIO_SERVER_CAPTURE_H

#include "hdf_device_desc.h"

int32_t HdiServiceCreatCapture(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureDestory(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureStart(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureStop(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCapturePause(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureResume(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureFlush(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetFrameSize(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetFrameCount(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureSetSampleAttr(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetSampleAttr(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetCurChannelId(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureCheckSceneCapability(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureSelectScene(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetMute(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureSetMute(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureSetVolume(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetVolume(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetGainThreshold(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetGain(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureSetGain(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureCaptureFrame(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetCapturePosition(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureSetExtraParams(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetExtraParams(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureReqMmapBuffer(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureGetMmapPosition(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureTurnStandbyMode(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);
int32_t HdiServiceCaptureDevDump(const struct HdfDeviceIoClient *client,
    struct HdfSBuf *data, struct HdfSBuf *reply);

#endif

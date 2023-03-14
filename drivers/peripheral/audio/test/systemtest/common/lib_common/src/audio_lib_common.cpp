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

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Test audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver ADM interface lib.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_lib_common.h
 *
 * @brief Declares APIs for operations related to the audio ADM interface lib.
 *
 * @since 1.0
 * @version 1.0
 */

#include "audio_lib_common.h"

using namespace std;

namespace HMOS {
namespace Audio {
int32_t InitRenderFramepara(struct AudioFrameRenderMode& frameRenderMode)
{
    InitAttrs(frameRenderMode.attrs);
    frameRenderMode.frames = AUDIO_FORMAT_PCM_16_BIT;
    frameRenderMode.mode = AUDIO_CHANNEL_BOTH_RIGHT;
    frameRenderMode.periodSize = G_PERIODSIZE;
    frameRenderMode.periodCount = G_PERIODCOUNT;
    frameRenderMode.byteRate = G_BYTERATE;
    frameRenderMode.bufferFrameSize = G_BUFFERFRAMESIZE;
    frameRenderMode.bufferSize = G_BUFFERSIZE1;
    frameRenderMode.buffer = NULL;
    frameRenderMode.silenceThreshold = frameRenderMode.periodSize * frameRenderMode.periodCount;
    frameRenderMode.silenceSize = G_SILENCETHRESHOLE;
    frameRenderMode.startThreshold = frameRenderMode.periodSize;
    frameRenderMode.stopThreshold = frameRenderMode.periodSize * frameRenderMode.periodCount;
    return HDF_SUCCESS;
}

int32_t InitHwCaptureFramepara(struct AudioFrameCaptureMode& frameCaptureMode)
{
    InitAttrs(frameCaptureMode.attrs);
    frameCaptureMode.mode = AUDIO_CHANNEL_BOTH_RIGHT;
    frameCaptureMode.byteRate = G_BYTERATE;
    frameCaptureMode.periodSize = G_PERIODSIZE;
    frameCaptureMode.periodCount = G_PERIODCOUNT;
    frameCaptureMode.startThreshold = frameCaptureMode.periodSize;
    frameCaptureMode.stopThreshold = frameCaptureMode.periodSize * frameCaptureMode.periodCount;
    frameCaptureMode.silenceThreshold = frameCaptureMode.periodSize * frameCaptureMode.periodCount;
    frameCaptureMode.silenceSize = G_SILENCETHRESHOLE;
    frameCaptureMode.buffer = NULL;
    frameCaptureMode.bufferFrameSize = G_BUFFERFRAMESIZE;
    frameCaptureMode.bufferSize = G_BUFFERSIZE1;
    return HDF_SUCCESS;
}

int32_t InitHwRenderMode(struct AudioHwRenderMode& renderMode)
{
    renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    renderMode.hwInfo.portDescript.dir = PORT_OUT;
    renderMode.hwInfo.portDescript.portId = G_PORTID;
    renderMode.hwInfo.portDescript.portName = "AOP";
    renderMode.hwInfo.deviceDescript.portId = G_PORTID;
    renderMode.hwInfo.deviceDescript.pins = PIN_OUT_SPEAKER;
    renderMode.hwInfo.deviceDescript.desc = nullptr;
    return HDF_SUCCESS;
}

int32_t InitHwCaptureMode(struct AudioHwCaptureMode& captureMode)
{
    captureMode.hwInfo.card = AUDIO_SERVICE_IN;
    captureMode.hwInfo.portDescript.dir = PORT_IN;
    captureMode.hwInfo.portDescript.portId = 0;
    captureMode.hwInfo.portDescript.portName = "AIP";
    captureMode.hwInfo.deviceDescript.portId = 0;
    captureMode.hwInfo.deviceDescript.pins = PIN_IN_MIC;
    captureMode.hwInfo.deviceDescript.desc = nullptr;
    return HDF_SUCCESS;
}

uint32_t InitHwRender(struct AudioHwRender *&hwRender, const std::string adapterNameCase)
{
    int ret = -1;
    if (hwRender == nullptr) {
        return HDF_FAILURE;
    }
    if (InitHwRenderMode(hwRender->renderParam.renderMode) ||
        InitRenderFramepara(hwRender->renderParam.frameRenderMode)) {
        return HDF_FAILURE;
    }
    hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    ret = strcpy_s(hwRender->renderParam.renderMode.hwInfo.adapterName,
        NAME_LEN, adapterNameCase.c_str());
    if (ret < 0) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

uint32_t InitHwCapture(struct AudioHwCapture *&hwCapture, const std::string adapterNameCase)
{
    int ret = -1;
    if (hwCapture == nullptr) {
        return HDF_FAILURE;
    }
    if (InitHwCaptureMode(hwCapture->captureParam.captureMode) ||
        InitHwCaptureFramepara(hwCapture->captureParam.frameCaptureMode)) {
        return HDF_FAILURE;
    }
    ret = strcpy_s(hwCapture->captureParam.captureMode.hwInfo.adapterName,
        NAME_LEN, adapterNameCase.c_str());
    if (ret < 0) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
}
}

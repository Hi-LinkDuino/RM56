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

#ifndef AUDIO_INTERFACE_LIB_RENDER_H
#define AUDIO_INTERFACE_LIB_RENDER_H

#include "audio_interface_lib_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RENDER_CMD  "render" // For Bind render service

enum AudioCriBuffStatus {
    CIR_BUFF_NO_FULL = 1,
    CIR_BUFF_FULL,
};

enum AudioInterfaceLibRenderCtrl {
    AUDIODRV_CTL_IOCTL_ELEM_INFO = CTRL_NUM, // Ctrl
    AUDIODRV_CTL_IOCTL_ELEM_READ,
    AUDIODRV_CTL_IOCTL_ELEM_WRITE,
    AUDIODRV_CTL_IOCTL_MUTE_READ,     // mute
    AUDIODRV_CTL_IOCTL_MUTE_WRITE,
    AUDIODRV_CTL_IOCTL_PAUSE_WRITE,
    AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ,
    AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE,
    AUDIODRV_CTL_IOCTL_GAIN_WRITE,    // gain
    AUDIODRV_CTL_IOCTL_GAIN_READ,
    AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE, // scene selsect
    AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ, // scene threshold
    AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN,   // acodec change in
    AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_OUT,  // acodec change out
    AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ,  // get VolThreshold
};

typedef int32_t (*InterfaceLibCtlRender)(struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData);

struct InterfaceLibCtlRenderList {
    enum AudioInterfaceLibRenderCtrl cmd;
    InterfaceLibCtlRender func;
};

struct DevHandle *AudioBindServiceRender(const char *name);
void AudioCloseServiceRender(const struct DevHandle *handle);
int32_t AudioInterfaceLibModeRender(const struct DevHandle *handle,
    struct AudioHwRenderParam *handleData, int cmdId);
int32_t AudioOutputRenderHwParams(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderWrite(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderStop(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderStartPrepare(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetVolume(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetVolume(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetMuteStu(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetMuteStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetPauseStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetChannelMode(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetChannelMode(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetGainStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetGainStu(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSceneSelect(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSceneGetGainThreshold(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderSetAcodecMode(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);
int32_t AudioCtlRenderGetVolThreshold(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData);
int32_t AudioOutputRenderReqMmapBuffer(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData);

#ifdef __cplusplus
}
#endif
#endif /* AUDIO_INTERFACE_LIB_RENDER_H */

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_STREAM_DISP_H
#define AUDIO_STREAM_DISP_H

#include "audio_host.h"
#include "audio_codec_if.h"
#include "audio_platform_if.h"
#include "audio_dai_if.h"
#include "audio_dsp_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum StreamDispMethodCmd {
    AUDIO_DRV_PCM_IOCTL_HW_PARAMS,
    AUDIO_DRV_PCM_IOCTL_RENDER_PREPARE,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_PREPARE,
    AUDIO_DRV_PCM_IOCTL_WRITE,
    AUDIO_DRV_PCM_IOCTL_READ,
    AUDIO_DRV_PCM_IOCTL_RENDER_START,
    AUDIO_DRV_PCM_IOCTL_RENDER_STOP,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_START,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_STOP,
    AUDIO_DRV_PCM_IOCTL_RENDER_PAUSE,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_PAUSE,
    AUDIO_DRV_PCM_IOCTL_RENDER_RESUME,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_RESUME,
    AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER,
    AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER_CAPTURE,
    AUDIO_DRV_PCM_IOCTL_MMAP_POSITION,
    AUDIO_DRV_PCM_IOCTL_MMAP_POSITION_CAPTURE,
    AUDIO_DRV_PCM_IOCTL_RENDER_OPEN,
    AUDIO_DRV_PCM_IOCTL_RENDER_CLOSE,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_OPEN,
    AUDIO_DRV_PCM_IOCTL_CAPTURE_CLOSE,
    AUDIO_DRV_PCM_IOCTL_DSPDECODE,
    AUDIO_DRV_PCM_IOCTL_DSPENCODE,
    AUDIO_DRV_PCM_IOCTL_DSPEQUALIZER,
    AUDIO_DRV_PCM_IOCTL_BUTT,
};

struct StreamDispCmdHandleList {
    enum StreamDispMethodCmd cmd;
    int32_t (*func)(const struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply);
};

struct StreamHost {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    void *priv;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_STREAM_DISP_H */

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DMA_BASE_H
#define AUDIO_DMA_BASE_H

#include "audio_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t AudioDmaBufAlloc(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaBufFree(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaRequestChannel(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaConfigChannel(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaPrep(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaSubmit(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaPending(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaPause(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaResume(struct PlatformData *data, enum AudioStreamType streamType);
int32_t AudioDmaPointer(struct PlatformData *data, enum AudioStreamType streamType, uint32_t *pointer);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

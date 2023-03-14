/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "audio_platform_if.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

int32_t AudioDmaBufAlloc(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaBufAlloc != NULL) {
        return data->ops->DmaBufAlloc(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaBufFree(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaBufFree != NULL) {
        return data->ops->DmaBufFree(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaRequestChannel(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaConfigChannel != NULL) {
        return data->ops->DmaRequestChannel(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaConfigChannel(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaConfigChannel != NULL) {
        return data->ops->DmaConfigChannel(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaPrep(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaPrep != NULL) {
        return data->ops->DmaPrep(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaSubmit(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaSubmit != NULL) {
        return data->ops->DmaSubmit(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaPending(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaPending != NULL) {
        return data->ops->DmaPending(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaPause(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaPause != NULL) {
        return data->ops->DmaPause(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaResume(struct PlatformData *data, enum AudioStreamType streamType)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaResume != NULL) {
        return data->ops->DmaResume(data, streamType);
    }
    return HDF_FAILURE;
}

int32_t AudioDmaPointer(struct PlatformData *data, enum AudioStreamType streamType, uint32_t *pointer)
{
    if (data != NULL && data->ops != NULL && data->ops->DmaPointer != NULL) {
        return data->ops->DmaPointer(data, streamType, pointer);
    }
    return HDF_FAILURE;
}

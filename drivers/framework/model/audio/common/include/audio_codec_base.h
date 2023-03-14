/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_CODEC_BASE_H
#define AUDIO_CODEC_BASE_H

#include "audio_codec_if.h"
#include "audio_core.h"
#include "osal_io.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t CodecGetServiceName(const struct HdfDeviceObject *device, const char **drvCodecName);
int32_t CodecGetDaiName(const struct HdfDeviceObject *device, const char **drvDaiName);
int32_t CodecGetConfigInfo(const struct HdfDeviceObject *device, struct CodecData *codecData);
int32_t CodecSetConfigInfo(struct CodecData *codeData, struct DaiData *daiData);
int32_t CodecSetCtlFunc(struct CodecData *codeData, const void *aiaoGetCtrl, const void *aiaoSetCtrl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

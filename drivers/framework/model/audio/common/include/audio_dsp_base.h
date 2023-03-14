/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DSP_BASE_H
#define AUDIO_DSP_BASE_H

#include "audio_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t DspGetServiceName(const struct HdfDeviceObject *device, const char **drvDspName);
int32_t DspGetDaiName(const struct HdfDeviceObject *device, const char **drvDaiName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_DSP_BASE_H */

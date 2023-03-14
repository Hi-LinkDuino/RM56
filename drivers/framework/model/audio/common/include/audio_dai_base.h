/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DAI_BASE_H
#define AUDIO_DAI_BASE_H

#include "audio_codec_if.h"
#include "audio_core.h"
#include "osal_io.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct DaiData *DaiDataFromCard(const struct AudioCard *card);
int32_t DaiGetConfigInfo(const struct HdfDeviceObject *device, struct DaiData *data);
int32_t DaiCheckSampleRate(unsigned int sampleRates);
int32_t DaiSetConfigInfo(struct DaiData *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_CODEC_OPS_H
#define HI3516_CODEC_OPS_H

#include "audio_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t CodecDeviceInit(struct AudioCard *audioCard, const struct CodecDevice *codec);
int32_t CodecDaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device);
int32_t CodecDaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param);
int32_t CodecDaiStartup(const struct AudioCard *card, const struct DaiDevice *device);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HI3516_CODEC_OPS_H */

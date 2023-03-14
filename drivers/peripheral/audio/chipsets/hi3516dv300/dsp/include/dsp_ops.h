/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DSP_OPS_H
#define DSP_OPS_H

#include "audio_host.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t DspDaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device);
int32_t DspDeviceInit(const struct DspDevice *device);
int32_t DspDeviceReadReg(const struct DspDevice *device, const void *msgs, const uint32_t len);
int32_t DspDeviceWriteReg(const struct DspDevice *device, const void *msgs, const uint32_t len);
int32_t DspDaiStartup(const struct AudioCard *card, const struct DaiDevice *device);
int32_t DspDaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param);
int32_t DspDecodeAudioStream(const struct AudioCard *card, const uint8_t *buf, const struct DspDevice *device);
int32_t DspEncodeAudioStream(const struct AudioCard *card, const uint8_t *buf, const struct DspDevice *device);
int32_t DspEqualizerActive(const struct AudioCard *card, const uint8_t *buf, const struct DspDevice *device);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DSP_OPS_H */

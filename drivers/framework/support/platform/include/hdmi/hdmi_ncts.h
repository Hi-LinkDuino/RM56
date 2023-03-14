/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_NCTS_H
#define HDMI_NCTS_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* N/CTS(Cycle Time Stamp), see hdmi1.4 spec section 7.2. */
#define HDMI_NCTS_FACTOR 128
#define HDMI_NCTS_N_DEFAULT 6144
#define HDMI_NCTS_TMDS_DEVIATION 20

struct HdmiAudioNCts {
    uint32_t sampleRate;   /* audio sample rate(fs) */
    uint32_t tmdsClock;
    uint32_t n;
    uint32_t cts;
};

uint32_t HdmiGetN(uint32_t sampleRate, uint32_t tmdsClock);
uint32_t HdmiGetCts(uint32_t sampleRate, uint32_t tmdsClock);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_NCTS_H */

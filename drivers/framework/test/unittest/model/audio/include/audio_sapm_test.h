/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_SAPM_TEST_H
#define AUDIO_SAPM_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif

int32_t AudioSapmNewComponentsTest(void);
int32_t AudioSapmAddRoutesTest(void);
int32_t AudioSapmNewControlsTest(void);
int32_t AudioSapmSleepTest(void);
int32_t AudioSampPowerUpTest(void);
int32_t AudioSampSetPowerMonitorTest(void);
int32_t AudioCodecSapmSetCtrlOpsTest(void);
int32_t AudioCodecSapmGetCtrlOpsTest(void);
int32_t AudioAccessorySapmSetCtrlOpsTest(void);
int32_t AudioAccessorySapmGetCtrlOpsTest(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

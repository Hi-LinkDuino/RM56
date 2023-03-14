/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_CORE_TEST_H
#define AUDIO_CORE_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t AudioDeviceReadRegTest(void);
int32_t AudioDeviceWriteRegTest(void);
int32_t AudioSocRegisterPlatformTest(void);
int32_t AudioSocRegisterDaiTest(void);
int32_t AudioRegisterDspTest(void);
int32_t AudioRegisterCodecTest(void);
int32_t AudioRegisterAccessoryTest(void);
int32_t AudioBindDaiLinkTest(void);
int32_t AudioUpdateCodecRegBitsTest(void);
int32_t AudioUpdateAccessoryRegBitsTest(void);
int32_t AudioUpdateDaiRegBitsTest(void);
int32_t AudioKcontrolGetCpuDaiTest(void);
int32_t AudioKcontrolGetCodecTest(void);
int32_t AudioKcontrolGetAccessoryTest(void);
int32_t AudioAddControlsTest(void);
int32_t AudioAddControlTest(void);
int32_t AudioGetCtrlOpsRRegTest(void);
int32_t AudioGetCtrlOpsRegTest(void);
int32_t AudioSetCtrlOpsRegTest(void);
int32_t AudioSetCtrlOpsRRegTest(void);
int32_t AudioDaiReadRegTest(void);
int32_t AudioDaiWriteRegTest(void);
int32_t AudioCodecReadRegTest(void);
int32_t AudioCodecWriteRegTest(void);
int32_t AudioAccessoryReadRegTest(void);
int32_t AudioAccessoryWriteRegTest(void);
int32_t AudioInfoCtrlOpsTest(void);
int32_t AudioCodecGetCtrlOpsTest(void);
int32_t AudioCodecSetCtrlOpsTest(void);
int32_t AudioAccessoryGetCtrlOpsTest(void);
int32_t AudioAccessorySetCtrlOpsTset(void);
int32_t AudioCpuDaiSetCtrlOpsTest(void);
int32_t AudioCpuDaiGetCtrlOpsTest(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_CORE_TEST_H */

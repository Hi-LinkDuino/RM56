/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_PLATFORM_BASE_TEST_H
#define AUDIO_PLATFORM_BASE_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t PlatformDataFromCardTest(void);
int32_t AudioBytesToFramesTest(void);
int32_t AudioDataBigEndianChangeTest(void);
int32_t AudioFramatToBitWidthTest(void);
int32_t AudioSetPcmInfoTest(void);
int32_t AudioSetRenderBufInfoTest(void);
int32_t AudioSetCaptureBufInfoTest(void);
int32_t AudioPcmWriteTest(void);
int32_t AudioPcmReadTest(void);
int32_t AudioPcmMmapWriteTest(void);
int32_t AudioPcmMmapReadTest(void);
int32_t AudioRenderOpenTest(void);
int32_t AudioCaptureOpenTest(void);
int32_t AudioRenderCloseTest(void);
int32_t AudioPcmPointerTest(void);
int32_t AudioCaptureCloseTest(void);
int32_t AudioHwParamsTest(void);
int32_t AudioRenderPrepareTest(void);
int32_t AudioCapturePrepareTest(void);
int32_t AudioRenderTriggerTest(void);
int32_t AudioCaptureTriggerTest(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_PLATFORM_BASE_TEST_H */

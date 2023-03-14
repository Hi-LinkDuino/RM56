/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_DMA_BASE_TEST_H
#define AUDIO_DMA_BASE_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t AudioDmaBufAllocTest(void);
int32_t AudioDmaBufFreeTest(void);
int32_t AudioDmaRequestChannelTest(void);
int32_t AudioDmaConfigChannelTest(void);
int32_t AudioDmaPrepTest(void);
int32_t AudioDmaSubmitTest(void);
int32_t AudioDmaPendingTest(void);
int32_t AudioDmaPauseTest(void);
int32_t AudioDmaResumeTest(void);
int32_t AudioDmaPointerTest(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_DMA_BASE_TEST_H */

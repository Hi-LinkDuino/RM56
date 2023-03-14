/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_DMA_OPS_TEST_H
#define HI3516_DMA_OPS_TEST_H

#include "hdf_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestAudioDmaDeviceInit(void);
int32_t TestHi3516DmaBufAlloc(void);
int32_t TestHi3516DmaBufFree(void);
int32_t TestHi3516DmaRequestChannel(void);
int32_t TestHi3516DmaConfigChannel(void);
int32_t TestHi3516DmaPrep(void);
int32_t TestHi3516DmaSubmit(void);
int32_t TestHi3516DmaPending(void);
int32_t TestHi3516DmaPause(void);
int32_t TestHi3516DmaResume(void);
int32_t TestHi3516DmaPointer(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


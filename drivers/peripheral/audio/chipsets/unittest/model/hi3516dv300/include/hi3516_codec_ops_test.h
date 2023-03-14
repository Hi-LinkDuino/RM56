/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_CODEC_OPS_TEST_H
#define HI3516_CODEC_OPS_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestCodecDeviceInit(void);
int32_t TestCodecDeviceInitFail(void);
int32_t TestCodecDaiDeviceInit(void);
int32_t TestCodecDaiDeviceInitFail(void);
int32_t TestCodecDaiStartup(void);
int32_t TestCodecDaiHwParams(void);
int32_t TestCodecDaiInvalidBitWidthParam(void);
int32_t TestCodecDaiInvalidRateParam(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

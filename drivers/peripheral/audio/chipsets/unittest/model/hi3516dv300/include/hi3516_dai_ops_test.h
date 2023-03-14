/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_DAI_OPS_TEST_H
#define HI3516_DAI_OPS_TEST_H

#include "hdf_types.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestDaiDeviceInit(void);
int32_t TestDaiStartup(void);
int32_t TestDaiHwParams(void);
int32_t TestDaiInvalidRateParam(void);
int32_t TestDaiInvalidRenderBitWidthParam(void);
int32_t TestDaiInvalidCaptureBitWidthParam(void);
int32_t TestDaiInvalidStreamTypeParam(void);
int32_t TestDaiTrigger(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

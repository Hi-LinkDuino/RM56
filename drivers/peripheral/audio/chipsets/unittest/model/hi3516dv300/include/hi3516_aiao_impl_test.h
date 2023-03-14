/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_AIAO_IMPL_TEST_H
#define HI3516_AIAO_IMPL_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestAiopRegCfg(void);
int32_t TestAiaoHalSysInit(void);
int32_t TestAiaoClockReset(void);
int32_t TestAiaoHalReadReg(void);
int32_t TestAopHalSetBuffRptr(void);
int32_t TestAopHalSetBuffWptr(void);
int32_t TestAipHalSetBuffWptr(void);
int32_t TestAipHalSetBuffRptr(void);
int32_t TestAopHalSetTxStart(void);
int32_t TestAipHalSetRxStart(void);
int32_t TestAopHalDevEnable(void);
int32_t TestAiaoDeviceInit(void);
int32_t TestAudioAoInit(void);
int32_t TestAudioAiInit(void);
int32_t TestAiaoSysPinMux(void);
int32_t TestAiaoGetMclk(void);
int32_t TestAiaoSetSysCtlRegValue(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

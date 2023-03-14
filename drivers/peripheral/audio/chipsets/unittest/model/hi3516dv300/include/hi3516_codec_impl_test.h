/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_CODEC_IMPL_TEST_H
#define HI3516_CODEC_IMPL_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestCodecHalSysInit(void);
int32_t TestCodecRegBitsRead(void);
int32_t TestCodecRegBitsUpdate(void);
int32_t TestCodecRegDefaultInit(void);
int32_t TestCodecSetAdcTuneEnable(void);
int32_t TestCodecDaiParamsUpdate(void);
int32_t TestAudioCodecAiaoSetCtrlOps(void);
int32_t TestAudioCodecAiaoGetCtrlOps(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

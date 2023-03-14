/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_CODEC_BASE_TEST_H
#define AUDIO_CODEC_BASE_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t CodecGetServiceNameTest(void);
int32_t CodecGetDaiNameTest(void);
int32_t CodecGetConfigInfoTest(void);
int32_t CodecSetConfigInfoTest(void);
int32_t CodecSetCtlFuncTest(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_CODEC_BASE_TEST_H */

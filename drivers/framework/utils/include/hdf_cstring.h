/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CSTRING_H
#define HDF_CSTRING_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfCString {
    int size;
    char value[0];
};

uint32_t HdfStringMakeHashKey(const char *key, uint32_t mask);
struct HdfCString *HdfCStringObtain(const char *str);
void HdfCStringRecycle(struct HdfCString *inst);
char *HdfStringCopy(const char *src);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_CSTRING_H */

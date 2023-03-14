/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_cstring.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define CSTRING_MAX 1024

uint32_t HdfStringMakeHashKey(const char *key, uint32_t mask)
{
    uint32_t hashValue = 0;
    const uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
    while ((key != NULL) && *key) {
        hashValue = hashValue * seed + (*key++);
    }
    return (hashValue & 0x7FFFFFFF) | mask;
}

struct HdfCString *HdfCStringObtain(const char *str)
{
    struct HdfCString *instance = NULL;
    size_t size;
    if (str != NULL) {
        size_t strLen = strlen(str);
        if (strLen > CSTRING_MAX) {
            return NULL;
        }
        size = sizeof(struct HdfCString) + strLen + 1;
        instance = (struct HdfCString *)OsalMemCalloc(size);
        if (instance == NULL) {
            HDF_LOGE("HdfCStringObtain failed, alloc memory failed");
            return NULL;
        }
        if (strncpy_s(instance->value, strLen + 1, str, strLen) != EOK) {
            HDF_LOGE("HdfCStringObtain failed, strncpy_s failed");
            OsalMemFree(instance);
            return NULL;
        }
        instance->size = (int)strLen;
    }
    return instance;
}

void HdfCStringRecycle(struct HdfCString *inst)
{
    if (inst != NULL) {
        OsalMemFree(inst);
    }
}

char *HdfStringCopy(const char *src)
{
    char *newStr = NULL;
    size_t srcSize;
    size_t dstSize;

    if (src == NULL) {
        return NULL;
    }
    srcSize = strlen(src);
    dstSize = srcSize + 1;
    newStr = OsalMemAlloc(dstSize);
    if (newStr == NULL) {
        return NULL;
    }

    if (strncpy_s(newStr, dstSize, src, srcSize) != EOK) {
        OsalMemFree(newStr);
        newStr = NULL;
    }

    return newStr;
}

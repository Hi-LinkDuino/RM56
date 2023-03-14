/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_mem.h"
#include <stdint.h>
#include <stdlib.h>
#include <securec.h>
#define MEM_MAX (1024 * 1024)

void *HcsMemAlloc(uint32_t size)
{
    if (size == 0 || size > MEM_MAX) {
        return NULL;
    }

    return malloc(size);
}

void *HcsMemZalloc(uint32_t size)
{
    void *newMem = HcsMemAlloc(size);
    if (newMem == NULL) {
        return NULL;
    }
    (void)memset_s(newMem, size, 0, size);
    return newMem;
}

void HcsMemFree(void *ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}
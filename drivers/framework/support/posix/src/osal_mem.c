/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_mem.h"
#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "hdf_log.h"

#define HDF_LOG_TAG osal_mem

void *OsalMemAlloc(size_t size)
{
    void *buf = NULL;

    if (size == 0) {
        HDF_LOGE("%s invalid param", __func__);
        return NULL;
    }

    buf = malloc(size);

    return buf;
}

void *OsalMemCalloc(size_t size)
{
    void *buf = NULL;

    if (size == 0) {
        HDF_LOGE("%s invalid param", __func__);
        return NULL;
    }

    buf = OsalMemAlloc(size);
    if (buf != NULL) {
        (void)memset_s(buf, size, 0, size);
    }

    return buf;
}

void *OsalMemAllocAlign(size_t alignment, size_t size)
{
    void *buf = NULL;
    int ret;

    if (size == 0) {
        HDF_LOGE("%s invalid param", __func__);
        return NULL;
    }

    ret = posix_memalign(&buf, alignment, size);
    if (ret != 0) {
        HDF_LOGE("%s memory alloc fail %d", __func__, ret);
        buf = NULL;
    }

    return buf;
}

void OsalMemFree(void *mem)
{
    if (mem != NULL) {
        free(mem);
    }
}


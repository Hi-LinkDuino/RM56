/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "display_gralloc.h"
#include <inttypes.h>
#include <securec.h>
#include "display_common.h"
#include "display_gralloc_gbm.h"

int32_t AllocMem(const AllocInfo *info, BufferHandle **handle)
{
    DISPLAY_CHK_RETURN((info == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("info is null"));
    DISPLAY_CHK_RETURN((handle == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("handle is null"));
#ifdef GRALLOC_GBM_SUPPORT
    if (info->usage & HBM_USE_MEM_DMA) {
        return GbmAllocMem(info, handle);
    }
#endif
    DISPLAY_LOGE("the usage is not support 0x%{public}" PRIx64 "", info->usage);
    return DISPLAY_NOT_SUPPORT;
}

void FreeMem(BufferHandle *handle)
{
    DISPLAY_CHK_RETURN_NOT_VALUE((handle == NULL), DISPLAY_LOGE("handle is null"));
#ifdef GRALLOC_GBM_SUPPORT
    if (handle->usage & HBM_USE_MEM_DMA) {
        GbmFreeMem(handle);
        return;
    }
#endif
}

void *Mmap(BufferHandle *handle)
{
    DISPLAY_CHK_RETURN((handle == NULL), NULL, DISPLAY_LOGE("handle is null"));
#ifdef GRALLOC_GBM_SUPPORT
    if (handle->usage & HBM_USE_MEM_DMA) {
        return GbmMmap(handle);
    }
#endif
    return NULL;
}

int32_t Unmap(BufferHandle *handle)
{
    DISPLAY_CHK_RETURN((handle == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("handle is null"));
#ifdef GRALLOC_GBM_SUPPORT
    if (handle->usage & HBM_USE_MEM_DMA) {
        return GbmUnmap(handle);
    }
#endif
    return DISPLAY_NOT_SUPPORT;
}

int32_t FlushCache(BufferHandle *handle)
{
    DISPLAY_CHK_RETURN((handle == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("handle is null"));
#ifdef GRALLOC_GBM_SUPPORT
    if (handle->usage & HBM_USE_MEM_DMA) {
        return GbmFlushCache(handle);
    }
#endif
    return DISPLAY_NOT_SUPPORT;
}

int32_t InvalidateCache(BufferHandle *handle)
{
    DISPLAY_CHK_RETURN((handle == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("handle is null"));
#ifdef GRALLOC_GBM_SUPPORT
    if (handle->usage & HBM_USE_MEM_DMA) {
        return GbmInvalidateCache(handle);
    }
#endif
    return DISPLAY_NOT_SUPPORT;
}

int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    DISPLAY_CHK_RETURN(funcs == NULL, DISPLAY_PARAM_ERR, DISPLAY_LOGE("funcs is null"));
    DISPLAY_LOGD();
#ifdef GRALLOC_GBM_SUPPORT
    if (GbmGrallocUninitialize() != DISPLAY_SUCCESS) {
        DISPLAY_LOGE("gbm uninit failed");
    }
#endif
    free(funcs);
    return DISPLAY_SUCCESS;
}

int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    DISPLAY_LOGD();
    int ret;
    DISPLAY_CHK_RETURN((funcs == NULL), DISPLAY_PARAM_ERR, DISPLAY_LOGE("funcs is null"));
    GrallocFuncs *grallocFuncs = (GrallocFuncs *)malloc(sizeof(GrallocFuncs));
    DISPLAY_CHK_RETURN((grallocFuncs == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("memset_s failed"));
    errno_t eok = memset_s(grallocFuncs, sizeof(GrallocFuncs), 0, sizeof(GrallocFuncs));
    DISPLAY_CHK_RETURN((eok != EOK), DISPLAY_FAILURE, DISPLAY_LOGE("memset_s failed"));
    // initialize gbm gralloc
#ifdef GRALLOC_GBM_SUPPORT
    ret = GbmGrallocInitialize();
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_LOGE("gbm initial"); free(grallocFuncs));
#endif
    grallocFuncs->AllocMem = AllocMem;
    grallocFuncs->FreeMem = FreeMem;
    grallocFuncs->Mmap = Mmap;
    grallocFuncs->Unmap = Unmap;
    grallocFuncs->InvalidateCache = InvalidateCache;
    grallocFuncs->FlushCache = FlushCache;
    *funcs = grallocFuncs;
    return DISPLAY_SUCCESS;
}
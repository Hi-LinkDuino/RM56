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

#ifndef DISPLAY_GRALLOC_GBM_H
#define DISPLAY_GRALLOC_GBM_H
#include "display_type.h"
#include "hdf_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct gbm_device *gbmDevice;
    int drmFd;
    struct DListHead gbmBoHead;
    int32_t referCount;
} GrallocManager;

typedef struct {
    struct DListHead entry;
    struct gbm_bo *bo;
    int fd;
} GbmBoList;

int32_t GbmAllocMem(const AllocInfo *info, BufferHandle **buffer);
void GbmFreeMem(BufferHandle *buffer);
void *GbmMmap(BufferHandle *buffer);
int32_t GbmUnmap(BufferHandle *buffer);
int32_t GbmInvalidateCache(BufferHandle *buffer);
int32_t GbmFlushCache(BufferHandle *buffer);
int32_t GbmGrallocUninitialize();
int32_t GbmGrallocInitialize();

#ifdef GRALLOC_LOCK_DEBUG
#define GRALLOC_LOCK(format, ...)                                                                                    \
    do {                                                                                                             \
        HDF_LOGD("[%{public}s@%{public}s:%{public}d]" format "\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__); \
        pthread_mutex_lock(&g_lock);                                                                                 \
    } while (0)

#define GRALLOC_UNLOCK(format, ...)                                                                                  \
    do {                                                                                                             \
        HDF_LOGD("[%{public}s@%{public}s:%{public}d]" format "\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__); \
        pthread_mutex_unlock(&g_lock);                                                                               \
    } while (0)
#else
#define GRALLOC_LOCK(format, ...)    \
    do {                             \
        pthread_mutex_lock(&g_lock); \
    } while (0)

#define GRALLOC_UNLOCK(format, ...)    \
    do {                               \
        pthread_mutex_unlock(&g_lock); \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_GRALLOC_GBM_H
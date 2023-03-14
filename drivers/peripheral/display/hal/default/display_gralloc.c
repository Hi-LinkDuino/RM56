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
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <securec.h>
#include "buffer_handle.h"
#include "display_type.h"
#include "disp_common.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define DEFAULT_READ_WRITE_PERMISSIONS   0666
#define MAX_MALLOC_SIZE                  0x10000000L
#define SHM_MAX_KEY                      10000
#define SHM_START_KEY                    1
#define INVALID_SHMID -1
#define BITS_PER_BYTE 8

#define DIV_ROUND_UP(n, d)   (((n) + (d)-1) / (d))
#define ALIGN_UP(x, a)       ((((x) + ((a)-1)) / (a)) * (a))
#define HEIGHT_ALIGN         2U
#define WIDTH_ALIGN          8U
#define MAX_PLANES           3

#undef  HDF_LOG_TAG
#define HDF_LOG_TAG          display_gralloc_c

typedef struct {
    BufferHandle hdl;
    int32_t shmid;
} PriBufferHandle;

typedef struct {
    uint32_t numPlanes;
    uint32_t radio[MAX_PLANES];
} PlaneLayoutInfo;

typedef struct {
    uint32_t format;
    uint32_t bitsPerPixel; // bits per pixel for first plane
    const PlaneLayoutInfo *planes;
} FormatInfo;

struct GrallocManager {
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
    int32_t count;
};

static struct GrallocManager g_grallocManager;

static const PlaneLayoutInfo g_yuv420SPLayout = {
    .numPlanes = 2,
    .radio = { 4, 2 },
};

static const PlaneLayoutInfo g_yuv420PLayout = {
    .numPlanes = 3,
    .radio = { 4, 1, 1 },
};

static void GetGrallocMgr(void)
{
    g_grallocManager.count++;
}

static int32_t PutGrallocMgr(void)
{
    g_grallocManager.count--;
    return g_grallocManager.count;
}

static void LockGrallocMgr(void)
{
    pthread_mutex_lock(&g_grallocManager.mutex);
}

static void UnlockGrallocMgr(void)
{
    pthread_mutex_unlock(&g_grallocManager.mutex);
}

static const FormatInfo *GetFormatInfo(uint32_t format)
{
    static const FormatInfo fmtInfos[] = {
        {PIXEL_FMT_RGBX_8888,  32, NULL},  {PIXEL_FMT_RGBA_8888, 32,  NULL},
        {PIXEL_FMT_BGRX_8888,  32, NULL},  {PIXEL_FMT_BGRA_8888, 32,  NULL},
        {PIXEL_FMT_RGB_888,    24, NULL},  {PIXEL_FMT_BGR_565,   16,  NULL},
        {PIXEL_FMT_RGBA_5551,  16, NULL},  {PIXEL_FMT_RGB_565,   16,  NULL},
        {PIXEL_FMT_BGRX_4444,  16, NULL},  {PIXEL_FMT_BGRA_4444, 16,  NULL},
        {PIXEL_FMT_RGBA_4444,  16, NULL},  {PIXEL_FMT_RGBX_4444, 16,  NULL},
        {PIXEL_FMT_BGRX_5551,  16, NULL},  {PIXEL_FMT_BGRA_5551, 16,  NULL},
        {PIXEL_FMT_YCBCR_420_SP, 8, &g_yuv420SPLayout}, {PIXEL_FMT_YCRCB_420_SP, 8, &g_yuv420SPLayout},
        {PIXEL_FMT_YCBCR_420_P, 8, &g_yuv420PLayout}, {PIXEL_FMT_YCRCB_420_P, 8, &g_yuv420PLayout},
    };

    for (uint32_t i = 0; i < sizeof(fmtInfos) / sizeof(FormatInfo); i++) {
        if (fmtInfos[i].format == format) {
            return &fmtInfos[i];
        }
    }
    HDF_LOGE("the format can not support %d %d", format, PIXEL_FMT_RGBA_8888);
    return NULL;
}

static uint32_t AdjustStrideFromFormat(uint32_t format, uint32_t width)
{
    const FormatInfo *fmtInfo = GetFormatInfo(format);
    if ((fmtInfo != NULL) && (fmtInfo->planes != NULL)) {
        uint32_t sum = fmtInfo->planes->radio[0];
        for (uint32_t i = 1; (i < fmtInfo->planes->numPlanes) && (i < MAX_PLANES); i++) {
            sum += fmtInfo->planes->radio[i];
        }
        if (sum > 0) {
            width = DIV_ROUND_UP((width * sum), fmtInfo->planes->radio[0]);
        }
    }
    return width;
}

static int32_t InitBufferHandle(PriBufferHandle* buffer, const AllocInfo* info)
{
    int32_t size;
    int32_t stride;
    int32_t h = ALIGN_UP(info->height, HEIGHT_ALIGN);
    const FormatInfo *fmtInfo = GetFormatInfo(info->format);
    if (fmtInfo == NULL) {
        HDF_LOGE("can not get format information : %d", buffer->hdl.format);
        return DISPLAY_FAILURE;
    }

    stride = ALIGN_UP(AdjustStrideFromFormat(info->format, info->width), WIDTH_ALIGN) *
        fmtInfo->bitsPerPixel / BITS_PER_BYTE;
    size = h * stride;
    buffer->hdl.width = info->width;
    buffer->hdl.stride = stride;
    buffer->hdl.height = info->height;
    buffer->hdl.size = size;
    buffer->hdl.usage = info->usage;
    buffer->hdl.fd = -1;
    buffer->shmid = INVALID_SHMID;
    buffer->hdl.format = info->format;
    buffer->hdl.reserveInts = (sizeof(PriBufferHandle) - sizeof(BufferHandle) -
        buffer->hdl.reserveFds * sizeof(uint32_t)) / sizeof(uint32_t);
    return DISPLAY_SUCCESS;
}

static int32_t AllocShm(BufferHandle *buffer)
{
    static int32_t key = SHM_START_KEY;
    int32_t shmid;

    while ((shmid = shmget(key, buffer->size, IPC_CREAT | IPC_EXCL | DEFAULT_READ_WRITE_PERMISSIONS)) < 0) {
        if (errno != EEXIST) {
            HDF_LOGE("%s: fail to alloc the shared memory, errno = %d", __func__, errno);
            return DISPLAY_FAILURE;
        }
        key++;
        if (key >= SHM_MAX_KEY) {
            key = SHM_START_KEY;
        }
    }
    void *pBase = shmat(shmid, NULL, 0);
    if (pBase == ((void *)-1)) {
        HDF_LOGE("%s: Fail to attach the shared memory, errno = %d", __func__, errno);
        if (shmctl(shmid, IPC_RMID, 0) == -1) {
            HDF_LOGE("%s: Fail to free shmid, errno = %d", __func__, errno);
        }
        return DISPLAY_FAILURE;
    }
    buffer->virAddr = pBase;
    buffer->key = key;
    ((PriBufferHandle*)buffer)->shmid = shmid;
    key++;
    if (memset_s(pBase, buffer->size, 0x0, buffer->size) != EOK) {
        HDF_LOGE("memset_s failure");
        if (shmctl(shmid, IPC_RMID, 0) == -1) {
            HDF_LOGE("%s: Fail to free shmid, errno = %d", __func__, errno);
        }
        return DISPLAY_FAILURE;
    }
    if (key >= SHM_MAX_KEY) {
        key = SHM_START_KEY;
    }
    return DISPLAY_SUCCESS;
}

static int32_t AllocMem(const AllocInfo* info, BufferHandle **buffer)
{
    int32_t ret;

    DISPLAY_CHK_RETURN((buffer == NULL), DISPLAY_NULL_PTR, HDF_LOGE("%s: in buffer is null", __func__));
    DISPLAY_CHK_RETURN((info == NULL), DISPLAY_NULL_PTR, HDF_LOGE("%s: in info is null", __func__));
    PriBufferHandle* priBuffer = calloc(1, sizeof(PriBufferHandle));
    DISPLAY_CHK_RETURN((priBuffer == NULL), DISPLAY_NULL_PTR, HDF_LOGE("%s: can not calloc errno : %d",
        __func__, errno));
    ret = InitBufferHandle(priBuffer, info);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, HDF_LOGE("%s: can not init buffe handle",
        __func__); goto OUT);

    BufferHandle *bufferHdl = &priBuffer->hdl;
    DISPLAY_CHK_RETURN(((bufferHdl->size > MAX_MALLOC_SIZE) || (bufferHdl->size == 0)),
        DISPLAY_FAILURE, HDF_LOGE("%s: size is invalid %d ", __func__, bufferHdl->size); goto OUT);
    LockGrallocMgr();
    if (bufferHdl->usage == HBM_USE_MEM_SHARE) {
        ret = AllocShm(bufferHdl);
    } else {
        HDF_LOGE("%s: not support memory usage: 0x%" PRIx64 "", __func__, bufferHdl->usage);
        ret = DISPLAY_NOT_SUPPORT;
    }

OUT:
    if ((ret != DISPLAY_SUCCESS) && (bufferHdl != NULL)) {
        free(bufferHdl);
        bufferHdl = NULL;
    }
    *buffer = bufferHdl;
    UnlockGrallocMgr();
    return ret;
}

static void FreeShm(BufferHandle *buffer)
{
    CHECK_NULLPOINTER_RETURN(buffer->virAddr);
    if (shmdt(buffer->virAddr) == -1) {
        HDF_LOGE("%s: Fail to free shared memory, errno = %d", __func__, errno);
    }
    if (shmctl(((PriBufferHandle*)buffer)->shmid, IPC_RMID, 0) == -1) {
        HDF_LOGE("%s: Fail to free shmid, errno = %d", __func__, errno);
    }
}

static void FreeMem(BufferHandle *buffer)
{
    CHECK_NULLPOINTER_RETURN(buffer);
    if ((buffer->size > MAX_MALLOC_SIZE) || (buffer->size == 0)) {
        HDF_LOGE("%s: size is invalid, buffer->size = %d", __func__, buffer->size);
        return;
    }

    LockGrallocMgr();
    switch (buffer->usage) {
        case HBM_USE_MEM_SHARE:
            FreeShm(buffer);
            break;
        default:
            HDF_LOGE("%s: not support memory usage: 0x%" PRIx64 "", __func__, buffer->usage);
    }
    UnlockGrallocMgr();
}

static void *MmapShm(BufferHandle *buffer)
{
    int32_t shmid;

    shmid = shmget(buffer->key, buffer->size, IPC_EXCL | DEFAULT_READ_WRITE_PERMISSIONS);
    if (shmid < 0) {
        HDF_LOGE("%s: Fail to mmap the shared memory, errno = %d", __func__, errno);
        return NULL;
    }
    void *pBase = shmat(shmid, NULL, 0);
    if (pBase == ((void *)-1)) {
        HDF_LOGE("%s: Fail to attach the shared memory, errno = %d", __func__, errno);
        return NULL;
    }
    ((PriBufferHandle*)buffer)->shmid = shmid;
    HDF_LOGI("%s: Mmap shared memory succeed", __func__);
    return pBase;
}

static void *Mmap(BufferHandle *buffer)
{
    void *temp = NULL;

    CHECK_NULLPOINTER_RETURN_VALUE(buffer, NULL);
    if ((buffer->size > MAX_MALLOC_SIZE) || (buffer->size == 0)) {
        HDF_LOGE("%s: size is invalid, buffer->size = %d", __func__, buffer->size);
        return NULL;
    }

    LockGrallocMgr();
    switch (buffer->usage) {
        case HBM_USE_MEM_SHARE:
            temp = MmapShm(buffer);
            break;
        default:
            HDF_LOGE("%s: not support memory usage: 0x%" PRIx64 "", __func__, buffer->usage);
            break;
    }
    UnlockGrallocMgr();
    return temp;
}

static int32_t UnmapShm(BufferHandle *buffer)
{
    if (shmdt(buffer->virAddr) == -1) {
        HDF_LOGE("%s: Fail to unmap shared memory errno =  %d", __func__, errno);
        return DISPLAY_FAILURE;
    }
    int32_t shmid = ((PriBufferHandle*)buffer)->shmid;
    if ((shmid != INVALID_SHMID) && (shmctl(shmid, IPC_RMID, 0) == -1)) {
        HDF_LOGE("%s: Fail to free shmid, errno = %d", __func__, errno);
    }
    return DISPLAY_SUCCESS;
}

static int32_t Unmap(BufferHandle *buffer)
{
    int32_t ret;

    CHECK_NULLPOINTER_RETURN_VALUE(buffer, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(buffer->virAddr, DISPLAY_NULL_PTR);
    if ((buffer->size > MAX_MALLOC_SIZE) || (buffer->size == 0)) {
        HDF_LOGE("%s: size is invalid, buffer->size = %d", __func__, buffer->size);
        return DISPLAY_FAILURE;
    }
    LockGrallocMgr();
    switch (buffer->usage) {
        case  HBM_USE_MEM_SHARE:
            ret = UnmapShm(buffer);
            break;
        default:
            HDF_LOGE("%s: not support memory usage: 0x%" PRIx64 "", __func__, buffer->usage);
            ret = DISPLAY_FAILURE;
            break;
    }
    UnlockGrallocMgr();
    return ret;
}

int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    static GrallocFuncs *gFuncs = NULL;

    if (funcs == NULL) {
        HDF_LOGE("%s: funcs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (gFuncs == NULL) {
        gFuncs = (GrallocFuncs *)OsalMemCalloc(sizeof(GrallocFuncs));
        if (gFuncs == NULL) {
            HDF_LOGE("%s: gFuncs is null", __func__);
            return DISPLAY_NULL_PTR;
        }
        pthread_mutexattr_init(&g_grallocManager.mutexattr);
        pthread_mutexattr_setpshared(&g_grallocManager.mutexattr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&g_grallocManager.mutex, &g_grallocManager.mutexattr);
        gFuncs->AllocMem = AllocMem;
        gFuncs->FreeMem = FreeMem;
        gFuncs->Mmap = Mmap;
        gFuncs->Unmap = Unmap;
    }
    *funcs = gFuncs;
    GetGrallocMgr();
    HDF_LOGI("%s: gralloc initialize success", __func__);
    return DISPLAY_SUCCESS;
}

int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    if (funcs == NULL) {
        HDF_LOGE("%s: funcs is null", __func__);
        return DISPLAY_NULL_PTR;
    }
    if (PutGrallocMgr() == 0) {
        pthread_mutexattr_destroy(&g_grallocManager.mutexattr);
        pthread_mutex_destroy(&g_grallocManager.mutex);
        OsalMemFree(funcs);
    }
    HDF_LOGI("%s: gralloc uninitialize success", __func__);
    return DISPLAY_SUCCESS;
}

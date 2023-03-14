/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "hiview_util.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "cmsis_os.h"

#if defined(CHIP_VER_Hi3861) || \
    defined(CHIP_VER_Hi3861L) || \
    defined(CHIP_VER_Hi3881)
#include "los_hwi.h"
#else
#include "../../../kernel/liteos_m/arch/include/los_interrupt.h"
#endif

extern void HAL_NVIC_SystemReset(void);
extern void __disable_irq(void);
extern void __enable_irq(void);

#define HIVIEW_WAIT_FOREVER           osWaitForever
#define HIVIEW_MS_PER_SECOND          1000
#define HIVIEW_NS_PER_MILLISECOND     1000000
#define BUFFER_SIZE                   128

void *HIVIEW_MemAlloc(uint8 modId, uint32 size)
{
    (void)modId;
    return malloc(size);
}

void HIVIEW_MemFree(uint8 modId, void *pMem)
{
    (void)modId;
    free(pMem);
}

uint64 HIVIEW_GetCurrentTime()
{
    struct timespec current = {0};
    int ret = clock_gettime(CLOCK_REALTIME, &current);
    if (ret != 0) {
        return 0;
    }
    return (uint64)current.tv_sec * HIVIEW_MS_PER_SECOND + current.tv_nsec / HIVIEW_NS_PER_MILLISECOND;
}

int32 HIVIEW_RtcGetCurrentTime(uint64 *val, HIVIEW_RtcTime *time)
{
    (void)val;
    (void)time;
    return OHOS_SUCCESS;
}

HiviewMutexId_t HIVIEW_MutexInit()
{
    return (HiviewMutexId_t)osMutexNew(NULL);
}

int32 HIVIEW_MutexLock(HiviewMutexId_t mutex)
{
    if (mutex == NULL) {
        return -1;
    }
    return (int32)osMutexAcquire((osMutexId_t)mutex, HIVIEW_WAIT_FOREVER);
}

int32 HIVIEW_MutexLockOrWait(HiviewMutexId_t mutex, uint32 timeout)
{
    if (mutex == NULL) {
        return -1;
    }
    return (int32)osMutexAcquire((osMutexId_t)mutex, timeout);
}

int32 HIVIEW_MutexUnlock(HiviewMutexId_t mutex)
{
    if (mutex == NULL) {
        return -1;
    }
    return (int32)osMutexRelease((osMutexId_t)mutex);
}

uint32 HIVIEW_IntLock()
{
    return LOS_IntLock();
}

void HIVIEW_IntRestore(uint32 intSave)
{
    LOS_IntRestore(intSave);
}

uint32 HIVIEW_GetTaskId()
{
    return (uint32)osThreadGetId();
}

void HIVIEW_UartPrint(const char *str)
{
    printf("%s\n", str);
}

void HIVIEW_Sleep(uint32 ms)
{
    osDelay(ms / HIVIEW_MS_PER_SECOND);
}

int32 HIVIEW_FileOpen(const char *path)
{
    int32 handle = open(path, O_RDWR | O_CREAT, 0);
    if (handle < 0) {
        printf("HIVIEW_FileOpen %s fail, errno:%d\n", path, errno);
    }
    return handle;
}

int32 HIVIEW_FileClose(int32 handle)
{
    if (handle < 0) {
        return -1;
    }
    return close(handle);
}

int32 HIVIEW_FileRead(int32 handle, uint8 *buf, uint32 len)
{
    if (handle < 0) {
        return -1;
    }
    return read(handle, (char *)buf, len);
}

int32 HIVIEW_FileWrite(int32 handle, const uint8 *buf, uint32 len)
{
    if (handle < 0) {
        return -1;
    }
    return write(handle, (const char *)buf, len);
}

int32 HIVIEW_FileSeek(int32 handle, int32 offset, int32 whence)
{
    if (handle < 0) {
        return -1;
    }
    return lseek(handle, (off_t)offset, whence);
}

int32 HIVIEW_FileSize(int32 handle)
{
    if (handle < 0) {
        return -1;
    }
    return lseek(handle, 0, SEEK_END);
}

int32 HIVIEW_FileSync(int32 handle)
{
    if (handle < 0) {
        return -1;
    }
    return fsync(handle);
}

int32 HIVIEW_FileUnlink(const char *path)
{
    return unlink(path);
}

int32 HIVIEW_FileCopy(const char *src, const char *dest)
{
    if (src == NULL || dest == NULL) {
        HIVIEW_UartPrint("HIVIEW_FileCopy input param is NULL");
        return -1;
    }
    int32 fdSrc = open(src, O_RDONLY, 0);
    if (fdSrc < 0) {
        HIVIEW_UartPrint("HIVIEW_FileCopy open src file fail");
        return fdSrc;
    }
    int32 fdDest = open(dest, O_RDWR | O_CREAT | O_TRUNC, 0);
    if (fdDest < 0) {
        HIVIEW_UartPrint("HIVIEW_FileCopy open dest file fail");
        HIVIEW_FileClose(fdSrc);
        return fdDest;
    }
    boolean copyFailed = TRUE;
    uint8 *dataBuf = (uint8 *)HIVIEW_MemAlloc(MEM_POOL_HIVIEW_ID, BUFFER_SIZE);
    if (dataBuf == NULL) {
        HIVIEW_UartPrint("HIVIEW_FileCopy malloc error");
        goto MALLOC_ERROR;
    }
    int32 nLen = HIVIEW_FileRead(fdSrc, dataBuf, BUFFER_SIZE);
    while (nLen > 0) {
        if (HIVIEW_FileWrite(fdDest, dataBuf, nLen) != nLen) {
            goto EXIT;
        }
        nLen = HIVIEW_FileRead(fdSrc, dataBuf, BUFFER_SIZE);
    }
    copyFailed = (nLen < 0);

EXIT:
    free(dataBuf);
MALLOC_ERROR:
    HIVIEW_FileClose(fdSrc);
    HIVIEW_FileClose(fdDest);
    if (copyFailed) {
        HIVIEW_UartPrint("HIVIEW_FileCopy copy failed");
        HIVIEW_FileUnlink(dest);
        return -1;
    }

    return 0;
}

int32 HIVIEW_FileMove(const char *src, const char *dest)
{
    int32 ret = HIVIEW_FileCopy(src, dest);
    if (HIVIEW_FileUnlink(src) != 0 || ret != 0) {
        return -1;
    }
    return 0;
}

void HIVIEW_WatchDogSystemReset()
{
    /* reset MCU Core */
    HAL_NVIC_SystemReset();
}

uint8 HIVIEW_WdgResetFlag()
{
    /* Depend:HAL_WdgGetResetFlag */
    return 1;
}

uint32 Change32Endian(uint32 num)
{
    unsigned char *buffer = (unsigned char *)&num;
    uint32 newEndian = (buffer[3] & 0xFF);
    newEndian |= ((buffer[2] << 8) & 0xFF00);
    newEndian |= ((buffer[1] << 16) & 0xFF0000);
    newEndian |= ((buffer[0] << 24) & 0xFF000000);
    return newEndian;
}

uint16 Change16Endian(uint16 num)
{
    unsigned char* buffer = (unsigned char*)&num;
    uint16 newEndian = (buffer[1] & 0xFF);
    newEndian |= ((buffer[0] << 8) & 0xFF00);
    return newEndian;
}

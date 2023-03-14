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

#ifndef HOS_LITE_HIVIEW_UTIL_H
#define HOS_LITE_HIVIEW_UTIL_H

#include "ohos_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* The definition value is consistent with MEM_POOL_COMMON_ID. */
#define MEM_POOL_HIVIEW_ID     0

#ifndef HIVIEW_SEEK_SET
#define HIVIEW_SEEK_SET 0
#endif

#ifndef HIVIEW_SEEK_CUR
#define HIVIEW_SEEK_CUR 1
#endif

#ifndef HIVIEW_SEEK_END
#define HIVIEW_SEEK_END 2
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LITTLE_ENDIAN_SYSTEM 1
#endif

#define OPTION_MASK 0x07
#define GETOPTION(opt) ((opt) & OPTION_MASK)

typedef void *HiviewMutexId_t;

typedef struct {
    uint8 minutes;
    uint8 hour;
    uint8 day;
    uint8 month;
    uint8 weekday;
    uint8 seconds;
    uint16 year;
    uint16 millisecond;
} HIVIEW_RtcTime;

uint64 HIVIEW_GetCurrentTime(void);
int32 HIVIEW_RtcGetCurrentTime(uint64 *val, HIVIEW_RtcTime *time);

void *HIVIEW_MemAlloc(uint8 modId, uint32 size);
void HIVIEW_MemFree(uint8 modId, void *pMem);
HiviewMutexId_t HIVIEW_MutexInit(void);
int32 HIVIEW_MutexLock(HiviewMutexId_t mutex);
int32 HIVIEW_MutexLockOrWait(HiviewMutexId_t mutex, uint32 timeout);
int32 HIVIEW_MutexUnlock(HiviewMutexId_t mutex);
uint32 HIVIEW_IntLock(void);
void HIVIEW_IntRestore(uint32 intSave);
uint32 HIVIEW_GetTaskId(void);
void HIVIEW_UartPrint(const char *str);
void HIVIEW_Sleep(uint32 ms);

/* File system */
int32 HIVIEW_FileOpen(const char *path);
int32 HIVIEW_FileClose(int32 handle);
int32 HIVIEW_FileRead(int32 handle, uint8 *buf, uint32 len);
int32 HIVIEW_FileWrite(int32 handle, const uint8 *buf, uint32 len);
int32 HIVIEW_FileSeek(int32 handle, int32 offset, int32 whence);
int32 HIVIEW_FileSize(int32 handle);
int32 HIVIEW_FileSync(int32 handle);
int32 HIVIEW_FileUnlink(const char *path);
int32 HIVIEW_FileCopy(const char *src, const char *dest);
int32 HIVIEW_FileMove(const char *src, const char *dest);

uint8 *HIVIEW_GetStackBegin(void);
uint32 HIVIEW_GetStackSize(void);
void HIVIEW_WatchDogSystemReset(void);
uint8 HIVIEW_WdgResetFlag(void);

uint16 Change16Endian(uint16 num);
uint32 Change32Endian(uint32 num);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_UTIL_H */

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

#include "hiview_output_event.h"
#include "securec.h"
#include "ohos_types.h"
#include "hiview_def.h"
#include "hiview_util.h"
#include "hiview_event.h"
#include "hiview_cache.h"
#include "hiview_config.h"
#include "hiview_log.h"
#include "hiview_file.h"
#include "hiview_service.h"

#define EVENT_PAYLOAD_MAX_SIZE   (5 * 16)

#ifdef SYNC_FILE
#undef SYNC_FILE
#endif
#define SYNC_FILE (1 << 6)

static HiviewCache g_faultEventCache = {
    .size = 0,
    .buffer = NULL,
};
static HiviewCache g_ueEventCache = {
    .size = 0,
    .buffer = NULL,
};
static HiviewCache g_statEventCache = {
    .size = 0,
    .buffer = NULL,
};
static HiviewFile g_faultEventFile = {
    .path = HIVIEW_FILE_PATH_FAULT_EVENT,
    .outPath = HIVIEW_FILE_OUT_PATH_FAULT_EVENT,
    .pFunc = NULL,
    .mutex = NULL,
    .fhandle = -1,
};
static HiviewFile g_ueEventFile = {
    .path = HIVIEW_FILE_PATH_UE_EVENT,
    .outPath = HIVIEW_FILE_OUT_PATH_UE_EVENT,
    .pFunc = NULL,
    .mutex = NULL,
    .fhandle = -1,
};
static HiviewFile g_statEventFile = {
    .path = HIVIEW_FILE_PATH_STAT_EVENT,
    .outPath = HIVIEW_FILE_OUT_PATH_STAT_EVENT,
    .pFunc = NULL,
    .mutex = NULL,
    .fhandle = -1,
};

typedef struct EventFlushInfo EventFlushInfo;
struct EventFlushInfo {
    HiviewMutexId_t mutex;
};
static EventFlushInfo g_eventFlushInfo;
static HieventProc g_hieventOutputProc = NULL;

typedef struct OutputEventInfo OutputEventInfo;
struct OutputEventInfo {
    HiviewMutexId_t mutex;
};
static OutputEventInfo g_outputEventInfo;

static int32 g_retryFaultInitCount = 0;
static int32 g_retryUeInitCount = 0;
static int32 g_retryStatInitCount = 0;
#define MAX_RETRY_COUNT 100

/* Output the event to UART using plaintext. */
static void OutputEventRealtime(const Request *req);
/* Output the event to FLASH using binary. */
static void OutputEvent2Flash(const Request *req);
static void InitFaultEventOutput(void);
static void InitUeEventOutput(void);
static void InitStatEventOutput(void);
static void Output2Flash(uint8 eventType);
static void CloseEventOutputFile(uint8 type);
static void GetEventCache(uint8 type, HiviewCache **c, HiviewFile **f);

void InitCoreEventOutput(void)
{
    g_eventFlushInfo.mutex = HIVIEW_MutexInit();
    g_outputEventInfo.mutex = HIVIEW_MutexInit();
    HiviewRegisterMsgHandle(HIVIEW_MSG_OUTPUT_EVENT_BIN_FILE, OutputEvent2Flash);
    HiviewRegisterMsgHandle(HIVIEW_MSG_OUTPUT_EVENT_FLOW, OutputEventRealtime);
}

void InitEventOutput(void)
{
    InitFaultEventOutput();
    InitUeEventOutput();
    InitStatEventOutput();
}

void ClearEventOutput(void)
{
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (opt == OUTPUT_OPTION_BIN_FILE || opt == OUTPUT_OPTION_TEXT_FILE) {
        CloseEventOutputFile(FAULT_EVENT_CACHE);
        CloseEventOutputFile(UE_EVENT_CACHE);
        CloseEventOutputFile(STAT_EVENT_CACHE);
    }
}

static void InitFaultEventOutput(void)
{
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (InitHiviewCache(&g_faultEventCache, FAULT_EVENT_CACHE, EVENT_CACHE_SIZE) == FALSE) {
        printf("malloc[%d] fail.", FAULT_EVENT_CACHE);
    }
    if (opt == OUTPUT_OPTION_DEBUG || opt == OUTPUT_OPTION_FLOW) {
        return;
    }
    if (InitHiviewFile(&g_faultEventFile, HIVIEW_FAULT_EVENT_FILE, FAULT_EVENT_FILE_SIZE) == FALSE) {
        printf("Open file[%d] failed.", HIVIEW_FAULT_EVENT_FILE);
    }
    g_faultEventFile.mutex = g_outputEventInfo.mutex;
}

static void InitUeEventOutput(void)
{
    if (InitHiviewCache(&g_ueEventCache, UE_EVENT_CACHE, EVENT_CACHE_SIZE) == FALSE) {
        printf("malloc[%d] fail.", UE_EVENT_CACHE);
    }
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (opt == OUTPUT_OPTION_DEBUG || opt == OUTPUT_OPTION_FLOW) {
        return;
    }
    if (InitHiviewFile(&g_ueEventFile, HIVIEW_UE_EVENT_FILE, UE_EVENT_FILE_SIZE) == FALSE) {
        printf("Open file[%d] failed.", HIVIEW_UE_EVENT_FILE);
    }
    g_ueEventFile.mutex = g_outputEventInfo.mutex;
}

static void InitStatEventOutput(void)
{
    if (InitHiviewCache(&g_statEventCache, STAT_EVENT_CACHE, EVENT_CACHE_SIZE) == FALSE) {
        printf("malloc[%d] fail.", STAT_EVENT_CACHE);
    }
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (opt == OUTPUT_OPTION_DEBUG || opt == OUTPUT_OPTION_FLOW) {
        return;
    }
    if (InitHiviewFile(&g_statEventFile, HIVIEW_STAT_EVENT_FILE, STAT_EVENT_FILE_SIZE) == FALSE) {
        printf("Open file[%d] failed.", HIVIEW_STAT_EVENT_FILE);
    }
    g_statEventFile.mutex = g_outputEventInfo.mutex;
}

static void CloseEventOutputFile(uint8 type)
{
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;
    GetEventCache(type, &c, &f);
    if (c != NULL && c->usedSize > 0) {
        Output2Flash(type);
    }
    CloseHiviewFile(f);
}

static void ReInitHiEventFile(uint8 eventType)
{
    if (eventType & HIEVENT_FAULT) {
        if (g_retryFaultInitCount > MAX_RETRY_COUNT) {
            return;
        }
        if (InitHiviewFile(&g_faultEventFile, HIVIEW_FAULT_EVENT_FILE, FAULT_EVENT_FILE_SIZE) == FALSE) {
            HILOG_ERROR(HILOG_MODULE_HIVIEW, "Open file[%d] failed.", HIVIEW_FAULT_EVENT_FILE);
            g_retryFaultInitCount++;
        } else {
            // once success, clean retry count
            g_retryFaultInitCount = 0;
        }
    } else if (eventType & HIEVENT_UE) {
        if (g_retryUeInitCount > MAX_RETRY_COUNT) {
            return;
        }
        if (InitHiviewFile(&g_ueEventFile, HIVIEW_UE_EVENT_FILE, UE_EVENT_FILE_SIZE) == FALSE) {
            HILOG_ERROR(HILOG_MODULE_HIVIEW, "Open file[%d] failed.", HIVIEW_UE_EVENT_FILE);
            g_retryUeInitCount++;
        } else {
            // once success, clean retry count
            g_retryUeInitCount = 0;
        }
    } else if (eventType & HIEVENT_STAT) {
        if (g_retryStatInitCount > MAX_RETRY_COUNT) {
            return;
        }
        if (InitHiviewFile(&g_statEventFile, HIVIEW_STAT_EVENT_FILE, STAT_EVENT_FILE_SIZE) == FALSE) {
            HILOG_ERROR(HILOG_MODULE_HIVIEW, "Open file[%d] failed.", HIVIEW_STAT_EVENT_FILE);
            g_retryStatInitCount++;
        } else {
            // once success, clean retry count
            g_retryStatInitCount = 0;
        }
    }
}

void OutputEvent(const uint8 *data)
{
    if (data == NULL) {
        return;
    }

    if (g_hieventOutputProc != NULL && g_hieventOutputProc((HiEvent *)data) == TRUE) {
        return;
    }

    HiEvent *event = (HiEvent *)data;
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;
    GetEventCache(event->type, &c, &f);
    if (c == NULL) {
        return;
    }
#if LITTLE_ENDIAN_SYSTEM
    event->common.eventId = Change16Endian(event->common.eventId);
    event->common.time = Change32Endian(event->common.time);
#endif
    boolean reachMaxThreshold  = FALSE;
    /* If the cache reach the max size, output and do not write cache. */
    if ((c->usedSize + sizeof(HiEventCommon) + event->common.len) > EVENT_CACHE_SIZE) {
        HIVIEW_UartPrint("HiEvent have no sufficient space to write event info to cache!\n");
        reachMaxThreshold = TRUE;
    } else if (WriteToCache(c, (uint8 *)&(event->common), sizeof(HiEventCommon)) == sizeof(HiEventCommon)) {
        WriteToCache(c, event->payload, event->common.len);
    }

    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (c->usedSize >= HIVIEW_HIEVENT_FILE_BUF_SIZE || opt == OUTPUT_OPTION_DEBUG) {
        switch (opt) {
            /* Event do not support the text format */
            case OUTPUT_OPTION_TEXT_FILE:
            case OUTPUT_OPTION_BIN_FILE:
                HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_EVENT_BIN_FILE, event->type);
                break;
            case OUTPUT_OPTION_FLOW:
            case OUTPUT_OPTION_DEBUG:
                HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_EVENT_FLOW, event->type);
                break;
            default:
                break;
        }
    }

    /* The cache reach the max size, output and then write cache. */
    if (reachMaxThreshold) {
        if ((c->usedSize + sizeof(HiEventCommon) + event->common.len) > EVENT_CACHE_SIZE) {
            if (WriteToCache(c, (uint8 *)&(event->common), sizeof(HiEventCommon)) == sizeof(HiEventCommon)) {
                WriteToCache(c, event->payload, event->common.len);
            }
        } else {
            printf("HiEvent have no sufficient space to write, drop event id %d\n", event->common.eventId);
        }
    }
}

static void OutputEventRealtime(const Request *req)
{
    HIVIEW_MutexLock(g_eventFlushInfo.mutex);
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;
    uint16 payloadLen;
    HiEvent event;
    uint8 payload[EVENT_PAYLOAD_MAX_SIZE];
    char tmpBuffer[LOG_FMT_MAX_LEN] = {0};

    event.type = (uint8)req->msgValue;
    GetEventCache(event.type, &c, &f);
    while (ReadFromCache(c, (uint8 *)&(event.common), sizeof(HiEventCommon)) == sizeof(HiEventCommon)) {
        if (event.common.mark != EVENT_INFO_HEAD) {
            DiscardCacheData(c);
            printf("Discard cache[%d] data.", c->type);
            break;
        }
#if LITTLE_ENDIAN_SYSTEM
        event.common.eventId = Change16Endian(event.common.eventId);
        event.common.time = Change32Endian(event.common.time);
#endif
        payloadLen = event.common.len;
        if (payloadLen > sizeof(payload)) {
            payloadLen = sizeof(payload);
        }
        if (payloadLen > 0 && ReadFromCache(c, payload, payloadLen) != payloadLen) {
            break;
        }
        event.payload = (payloadLen > 0) ? payload : NULL;
        EventContentFmt(tmpBuffer, LOG_FMT_MAX_LEN, (uint8 *)&event);
        HIVIEW_UartPrint(tmpBuffer);
    }
    HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
}

static void OutputEvent2Flash(const Request *req)
{
    Output2Flash((uint8)req->msgValue);
}

static void Output2Flash(uint8 eventType)
{
    HIVIEW_MutexLock(g_eventFlushInfo.mutex);
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;
    uint8 *tmpBuffer = NULL;
    HiEventCommon *pEventCommon = NULL;
    uint16 len = 0, payloadLen, outputSize;

    GetEventCache((uint8)eventType, &c, &f);
    if (c == NULL) {
        HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
        return;
    }
    outputSize = c->usedSize;
    if (outputSize < sizeof(HiEventCommon)) {
        HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
        return;
    }
    tmpBuffer = (uint8 *)HIVIEW_MemAlloc(MEM_POOL_HIVIEW_ID, outputSize);
    if (tmpBuffer == NULL) {
        HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
        return;
    }
    while (c->usedSize >= sizeof(HiEventCommon) && outputSize > (len + sizeof(HiEventCommon))) {
        if (ReadFromCache(c, tmpBuffer + len, sizeof(HiEventCommon)) != sizeof(HiEventCommon)) {
            continue;
        }
        pEventCommon = (HiEventCommon *)(tmpBuffer + len);
        len += sizeof(HiEventCommon);
        if (pEventCommon->mark != EVENT_INFO_HEAD) {
            DiscardCacheData(c);
            printf("Discard cache[%d] data.", c->type);
            break;
        }
        payloadLen = pEventCommon->len;
        if (payloadLen > 0) {
            if (ReadFromCache(c, tmpBuffer + len, payloadLen) != payloadLen) {
                continue;
            }
            len += payloadLen;
        }
    }
    if (f->fhandle < 0) {
        ReInitHiEventFile(eventType);
    }
    if (len > 0 && WriteToFile(f, tmpBuffer, len) != len) {
        g_hiviewConfig.writeFailureCount++;
        HIVIEW_UartPrint("Failed to write event data.");
    }
    HIVIEW_MemFree(MEM_POOL_HIVIEW_ID, tmpBuffer);
    HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
    if ((eventType & SYNC_FILE) != 0) {
        HIVIEW_FileSync(f->fhandle);
    }
}

uint32 GetEventFileSize(uint8 eventType)
{
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;

    GetEventCache(eventType, &c, &f);
    return GetFileUsedSize(f);
}

uint32 ReadEventFile(uint8 eventType, uint8 *buf, uint32 len)
{
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;
    uint32 usedSize;

    if (buf == NULL) {
        return 0;
    }
    GetEventCache(eventType, &c, &f);
    usedSize = GetFileUsedSize(f);
    if (usedSize < len) {
        len = usedSize;
    }
    if (ReadFromFile(f, buf, len) != (int32)len) {
        return 0;
    }

    return len;
}

int32 EventContentFmt(char *outStr, int32 outStrLen, const uint8 *pEvent)
{
    if (outStrLen < TAIL_LINE_BREAK) {
        return -1;
    }
    if (pEvent == NULL) {
        return -1;
    }
    int32 len;
    uint32 time, hour, mte, sec;
    HiEvent *event = (HiEvent *)pEvent;

    time = event->common.time;
    hour = time % SECONDS_PER_DAY / SECONDS_PER_HOUR;
    mte = time % SECONDS_PER_HOUR / SECONDS_PER_MINUTE;
    sec = time % SECONDS_PER_MINUTE;
    if (event->payload == NULL) {
        len = snprintf_s(outStr, outStrLen, outStrLen - 1,
            "EVENT: time=%02d:%02d:%02d id=%d type=%d data=null",
            hour, mte, sec, event->common.eventId, event->type);
    } else {
        len = snprintf_s(outStr, outStrLen, outStrLen - 1,
            "EVENT: time=%02d:%02d:%02d id=%d type=%d data=%p",
            hour, mte, sec, event->common.eventId, event->type, event->payload);
    }

    if (len < 0) {
        return -1;
    }

    if (len >= outStrLen - 1) {
        outStr[outStrLen - TAIL_LINE_BREAK] = '\n';
        outStr[outStrLen - 1] = '\0';
    } else {
        outStr[len++] = '\n';
        outStr[len++] = '\0';
    }

    return len;
}

static void GetEventCache(uint8 type, HiviewCache **c, HiviewFile **f)
{
    if (type & HIEVENT_FAULT) {
        *c = &g_faultEventCache;
        *f = &g_faultEventFile;
    } else if (type & HIEVENT_UE) {
        *c = &g_ueEventCache;
        *f = &g_ueEventFile;
    } else if (type & HIEVENT_STAT) {
        *c = &g_statEventCache;
        *f = &g_statEventFile;
    } else {
        *c = NULL;
        *f = NULL;
    }
}

static void FlushEventAsync(const uint8 type)
{
    switch (GETOPTION(g_hiviewConfig.outputOption)) {
        /* Event do not support the text format */
        case OUTPUT_OPTION_TEXT_FILE:
        case OUTPUT_OPTION_BIN_FILE:
            HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_EVENT_BIN_FILE, type | SYNC_FILE);
            break;
        case OUTPUT_OPTION_FLOW:
            HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_EVENT_FLOW, type);
            break;
        default:
            break;
    }
}

static void FlushEventInfo(const uint8 type, const HiviewCache *c, boolean syncFlag)
{
    if (c == NULL) {
        return;
    }
    Request request = {0};
    request.msgValue = type;
    if (c->usedSize > 0) {
        if (syncFlag == FALSE) {
            /* If syncFlag is FALSE, refresh event information asynchronously */
            FlushEventAsync(type);
        } else {
            switch (GETOPTION(g_hiviewConfig.outputOption)) {
                /* Event do not support the text format */
                case OUTPUT_OPTION_TEXT_FILE:
                case OUTPUT_OPTION_BIN_FILE:
                    request.msgValue |= SYNC_FILE;
                    OutputEvent2Flash(&request);
                    break;
                case OUTPUT_OPTION_FLOW:
                    OutputEventRealtime(&request);
                    break;
                default:
                    break;
            }
        }
    }
}

static void FlushFaultEvent(boolean syncFlag)
{
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;

    GetEventCache(HIEVENT_FAULT, &c, &f);
    if (c == NULL || f == NULL) {
        return;
    }
    FlushEventInfo(HIEVENT_FAULT, c, syncFlag);
}

static void FlushUeEvent(boolean syncFlag)
{
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;

    GetEventCache(HIEVENT_UE, &c, &f);
    if (c == NULL || f == NULL) {
        return;
    }
    FlushEventInfo(HIEVENT_UE, c, syncFlag);
}

static void FlushStatEvent(boolean syncFlag)
{
    HiviewCache *c = NULL;
    HiviewFile *f = NULL;

    GetEventCache(HIEVENT_STAT, &c, &f);
    if (c == NULL || f == NULL) {
        return;
    }
    FlushEventInfo(HIEVENT_STAT, c, syncFlag);
}

void FlushEvent(boolean syncFlag)
{
    FlushFaultEvent(syncFlag);
    FlushUeEvent(syncFlag);
    FlushStatEvent(syncFlag);
}

void HiviewRegisterHieventProc(HieventProc func)
{
    g_hieventOutputProc = func;
}

void HiviewUnRegisterHieventProc(HieventProc func)
{
    (void)func;
    if (g_hieventOutputProc != NULL) {
        g_hieventOutputProc = NULL;
    }
}

int HiEventFileProcImp(uint8 type, const char *dest, uint8 mode)
{
    Output2Flash(type);
    HIVIEW_MutexLock(g_eventFlushInfo.mutex);
    HiviewCache* c = NULL;
    HiviewFile* f = NULL;

    GetEventCache(type, &c, &f);
    if (f == NULL || strcmp(f->path, dest) == 0) {
        HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
        return -1;
    }
    int ret = ProcFile(f, dest, mode);
    HIVIEW_MutexUnlock(g_eventFlushInfo.mutex);
    return ret;
}

void HiviewRegisterHieventFileWatcher(uint8 type, FileProc func, const char *path)
{
    if (func == NULL) {
        return;
    }
    HiviewCache* c = NULL;
    HiviewFile* f = NULL;
    GetEventCache(type, &c, &f);
    RegisterFileWatcher(f, func, path);
}

void HiviewUnRegisterHieventFileWatcher(uint8 type, FileProc func)
{
    if (func == NULL) {
        return;
    }
    HiviewCache* c = NULL;
    HiviewFile* f = NULL;
    GetEventCache(type, &c, &f);
    UnRegisterFileWatcher(f, func);
}

void HiEventOutputFileLockImp(void)
{
    HIVIEW_MutexLock(g_outputEventInfo.mutex);
}

void HiEventOutputFileUnLockImp(void)
{
    HIVIEW_MutexUnlock(g_outputEventInfo.mutex);
}
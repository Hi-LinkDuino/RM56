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

#include "hiview_output_log.h"
#include "hiview_cache.h"
#include "hiview_config.h"
#include "hiview_def.h"
#include "hiview_file.h"
#include "hiview_log.h"
#include "hiview_log_limit.h"
#include "hiview_service.h"
#include "hiview_util.h"
#include "message.h"
#include "ohos_types.h"
#include "securec.h"

#include <time.h>

#define SINGLE_FMT_MAX_LEN      8
#define FMT_CONVERT_TRMINATOR   2

static char g_logLevelInfo[HILOG_LV_MAX] = {
    'N', // "NONE"
    'D', // "DEBUG"
    'I', // "INFO"
    'W', // "WARN"
    'E', // "ERROR"
    'F'  // "FATAL"
};

#ifndef DISABLE_HILOG_CACHE
static uint8 g_logCacheBuffer[LOG_STATIC_CACHE_SIZE];
#endif
static HiviewCache g_logCache = {
    .size = 0,
    .buffer = NULL,
};
static HiviewFile g_logFile = {
    .path = HIVIEW_FILE_PATH_LOG,
    .outPath = HIVIEW_FILE_OUT_PATH_LOG,
    .pFunc = NULL,
    .mutex = NULL,
    .fhandle = -1,
};

typedef struct LogFlushInfo LogFlushInfo;
struct LogFlushInfo {
    HiviewMutexId_t mutex;
};
static LogFlushInfo g_logFlushInfo;
static HilogProc g_hilogOutputProc = NULL;

typedef struct OutputLogInfo OutputLogInfo;
struct OutputLogInfo {
    HiviewMutexId_t mutex;
};
static OutputLogInfo g_outputLogInfo;

static int32 g_retryInitCount = 0;
#define MAX_RETRY_COUNT 100

/* Output the log to UART using plaintext. */
static void OutputLogRealtime(const Request *req);
/* Output the log to FLASH using text. */
static void OutputLog2TextFile(const Request *req);
/* Output the log to FLASH using binary. */
static void OutputLog2BinFile(const Request *req);
static int32 LogCommonFmt(char *outStr, int32 outStrlen, const HiLogCommon *commonContentPtr);
static int32 LogValuesFmt(char *desStrPtr, int32 desLen, const HiLogContent *logContentPtr);
static int32 LogDebugValuesFmt(char *desStrPtr, int32 desLen, const HiLogContent *logContentPtr);
static int32 LogValuesFmtHash(char *desStrPtr, int32 desLen, const HiLogContent *logContentPtr);

void InitCoreLogOutput(void)
{
    g_logFlushInfo.mutex = HIVIEW_MutexInit();
    g_outputLogInfo.mutex = HIVIEW_MutexInit();
#ifndef DISABLE_HILOG_CACHE
    InitHiviewStaticCache(&g_logCache, LOG_CACHE, g_logCacheBuffer, sizeof(g_logCacheBuffer));
#endif
    HiviewRegisterMsgHandle(HIVIEW_MSG_OUTPUT_LOG_TEXT_FILE, OutputLog2TextFile);
    HiviewRegisterMsgHandle(HIVIEW_MSG_OUTPUT_LOG_BIN_FILE, OutputLog2BinFile);
    HiviewRegisterMsgHandle(HIVIEW_MSG_OUTPUT_LOG_FLOW, OutputLogRealtime);
}

void InitLogOutput(void)
{
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (opt == OUTPUT_OPTION_DEBUG || opt == OUTPUT_OPTION_FLOW) {
        return;
    }
    HiviewFileType type = HIVIEW_LOG_TEXT_FILE;
    if (opt == OUTPUT_OPTION_BIN_FILE) {
        type = HIVIEW_LOG_BIN_FILE;
    }
    if (InitHiviewFile(&g_logFile, type,
        (HIVIEW_LOG_FILE_SIZE / sizeof(HiLogContent)) * sizeof(HiLogContent)) == FALSE) {
        HIVIEW_UartPrint("Open file[HIVIEW_LOG_BIN_FILE] failed.");
    }
    g_logFile.mutex = g_outputLogInfo.mutex;
}

void ClearLogOutput(void)
{
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (g_logCache.usedSize > 0) {
        if (opt == OUTPUT_OPTION_TEXT_FILE) {
            OutputLog2TextFile(NULL);
        } else if (opt == OUTPUT_OPTION_BIN_FILE) {
            OutputLog2BinFile(NULL);
        }
    }
    CloseHiviewFile(&g_logFile);
}

void OutputLog(const uint8 *data, uint32 len)
{
    if (data == NULL) {
        return;
    }

    HiLogContent *hiLogContent = (HiLogContent *)data;
    if (g_hilogOutputProc != NULL) {
        if (g_hilogOutputProc(hiLogContent, len) == TRUE) {
            return;
        }
    }

#ifdef DISABLE_HILOG_CACHE
    boolean isDisableCache = TRUE;
#else
    boolean isDisableCache = FALSE;
#endif

#ifdef DISABLE_HILOG_LITE_PRINT_LIMIT
    boolean isDisablePrintLimited = TRUE;
#else
    boolean isDisablePrintLimited = FALSE;
#endif
    boolean isLogLimited = LogIsLimited(hiLogContent->commonContent.module);

    if (!isDisablePrintLimited && isLogLimited) {
        // The console output adopts the same restriction strategy as the file output,
        // and the log output to the file is restricted.
        return;
    }

    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    boolean isPrint = g_hiviewConfig.outputOption >= OUTPUT_OPTION_PRINT;
    if (opt == OUTPUT_OPTION_DEBUG || isPrint || isDisableCache) {
        char tempOutStr[LOG_FMT_MAX_LEN] = {0};
        if (LogContentFmt(tempOutStr, sizeof(tempOutStr), data) > 0) {
            HIVIEW_UartPrint(tempOutStr);
        }
    }

    if (opt == OUTPUT_OPTION_DEBUG || isDisableCache || isLogLimited) {
        return;
    }

    /* When the init of kernel is not finished, data is cached in the cache. */
    if (g_hiviewConfig.hiviewInited == FALSE) {
        if (WriteToCache(&g_logCache, data, len) != (int32)len) {
            HIVIEW_UartPrint("Write log to cache failed.");
        }
        return;
    }

    boolean writeFail = FALSE;
    if (WriteToCache(&g_logCache, (uint8 *)data, len) != (int32)len) {
        HIVIEW_UartPrint("Hilog writeToCache error!\n");
        writeFail = TRUE;
    }
    if (g_logCache.usedSize >= HIVIEW_HILOG_FILE_BUF_SIZE) {
        switch (opt) {
            case OUTPUT_OPTION_TEXT_FILE:
                HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_LOG_TEXT_FILE, 0);
                break;
            case OUTPUT_OPTION_BIN_FILE:
                HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_LOG_BIN_FILE, 0);
                break;
            case OUTPUT_OPTION_FLOW:
                HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_LOG_FLOW, 0);
                break;
            default:
                break;
        }
    }

    /* If the cache fails to be written, write the cache again. */
    if (writeFail) {
        WriteToCache(&g_logCache, (uint8 *)data, len);
    }
}

static void OutputLogRealtime(const Request *req)
{
    HIVIEW_MutexLock(g_logFlushInfo.mutex);
    HiLogContent logContent;
    char tempOutStr[LOG_FMT_MAX_LEN] = {0};
    int32 len;
    (void)req;

    while (ReadFromCache(&g_logCache, (uint8 *)&(logContent.commonContent),
        sizeof(HiLogCommon)) == sizeof(HiLogCommon)) {
        if (logContent.commonContent.head != LOG_INFO_HEAD) {
            DiscardCacheData(&g_logCache);
            HIVIEW_UartPrint("Discard cache[LOG_CACHE] data.");
            break;
        }
        len = logContent.commonContent.valueNumber * sizeof(uint32);
        if (len > 0 && ReadFromCache(&g_logCache, (uint8 *)&(logContent.values), len) != len) {
            continue;
        }
        len = LogContentFmt(tempOutStr, sizeof(tempOutStr), (uint8 *)&logContent);
        if (len <= 0) {
            continue;
        }
        HIVIEW_UartPrint(tempOutStr);
    }
    HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
}

static void OutputLog2TextFile(const Request *req)
{
    HIVIEW_MutexLock(g_logFlushInfo.mutex);
    HiLogContent logContent;
    char tempOutStr[LOG_FMT_MAX_LEN] = {0};

    if (g_logCache.usedSize < sizeof(HiLogCommon)) {
        HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
        return;
    }

    int32 len;
    while (ReadFromCache(&g_logCache, (uint8 *)&(logContent.commonContent),
        sizeof(HiLogCommon)) == sizeof(HiLogCommon)) {
        if (logContent.commonContent.head != LOG_INFO_HEAD) {
            DiscardCacheData(&g_logCache);
            HIVIEW_UartPrint("Discard cache[LOG_CACHE] data.");
            break;
        }
        len = logContent.commonContent.valueNumber * sizeof(uint32);
        if (len > 0 && ReadFromCache(&g_logCache, (uint8 *)&(logContent.values), len) != len) {
            continue;
        }
        len = LogContentFmt(tempOutStr, sizeof(tempOutStr), (uint8 *)&logContent);
        if (len > 0 && tempOutStr[len - 1] == '\0') {
            // prevent writing '\0' character to file
            len--;
        }
        if (g_logFile.fhandle < 0) {
            if (g_retryInitCount < MAX_RETRY_COUNT) {
                InitLogOutput();
            }
            g_retryInitCount++;
        } else {
            // once success, clean retry count
            g_retryInitCount = 0;
        }
        if (len > 0 && WriteToFile(&g_logFile, (uint8 *)tempOutStr, len) != len) {
            g_hiviewConfig.writeFailureCount++;
        }
    }
    HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
    if (req != NULL && req->msgValue == SYNC_FILE) {
        HIVIEW_FileSync(g_logFile.fhandle);
    }
}

static void OutputLog2BinFile(const Request *req)
{
    HIVIEW_MutexLock(g_logFlushInfo.mutex);
    HiLogCommon *pCommonContent = NULL;
    uint16 len = 0;
    uint16 valueLen;
    uint8 *tmpBuffer = NULL;
    uint16 outputSize = g_logCache.usedSize;

    if (outputSize < sizeof(HiLogCommon)) {
        HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
        return;
    }
    tmpBuffer = (uint8 *)HIVIEW_MemAlloc(MEM_POOL_HIVIEW_ID, outputSize);
    if (tmpBuffer == NULL) {
        HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
        return;
    }
    while (g_logCache.usedSize >= sizeof(HiLogCommon) && outputSize > (len + sizeof(HiLogCommon))) {
        if (ReadFromCache(&g_logCache, tmpBuffer + len, sizeof(HiLogCommon)) != sizeof(HiLogCommon)) {
            continue;
        }
        pCommonContent = (HiLogCommon *)(tmpBuffer + len);
        len += sizeof(HiLogCommon);
        if (pCommonContent->head != LOG_INFO_HEAD) {
            DiscardCacheData(&g_logCache);
            HIVIEW_UartPrint("Discard cache[LOG_CACHE] data.");
            break;
        }
        valueLen = pCommonContent->valueNumber * sizeof(uint32);
        if (valueLen > 0) {
            if ((int32)len + (int32)valueLen > (int32)outputSize) {
                DiscardCacheData(&g_logCache);
                HIVIEW_UartPrint("Discard cache[LOG_CACHE] data.");
                break;
            }
            if (ReadFromCache(&g_logCache, tmpBuffer + len, valueLen) != valueLen) {
                continue;
            }
            len += valueLen;
        }
    }
    if (g_logFile.fhandle < 0) {
        if (g_retryInitCount < MAX_RETRY_COUNT) {
            InitLogOutput();
        }
        g_retryInitCount++;
    } else {
        // once success, clean retry count
        g_retryInitCount = 0;
    }
    if (len > 0 && WriteToFile(&g_logFile, tmpBuffer, len) != len) {
        g_hiviewConfig.writeFailureCount++;
        HIVIEW_UartPrint("Failed to write log data.");
    }
    HIVIEW_MemFree(MEM_POOL_HIVIEW_ID, tmpBuffer);
    HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
    if (req != NULL && req->msgValue == SYNC_FILE) {
        HIVIEW_FileSync(g_logFile.fhandle);
    }
}

uint32 GetLogFileSize(void)
{
    return GetFileUsedSize(&g_logFile);
}

uint32 ReadLogFile(uint8 *buf, uint32 len)
{
    if (buf == NULL) {
        return 0;
    }
    uint32 usedSize = GetFileUsedSize(&g_logFile);
    if (usedSize < len) {
        len = usedSize;
    }
    if (ReadFromFile(&g_logFile, buf, len) != (int32)len) {
        return 0;
    }

    return len;
}

int32 LogContentFmt(char *outStr, int32 outStrLen, const uint8 *pLogContent)
{
    int32 len;
    HiLogContent *logContentPtr = (HiLogContent *)pLogContent;

    len = LogCommonFmt(outStr, outStrLen, &(logContentPtr->commonContent));
    boolean isHash = CHECK_HASH_FLAG(logContentPtr->commonContent.level);
    if (len >= 0) {
        if (isHash) {
            len += LogValuesFmtHash(outStr + len, outStrLen - len, logContentPtr);
        } else if (GETOPTION(g_hiviewConfig.outputOption) == OUTPUT_OPTION_DEBUG) {
            len += LogDebugValuesFmt(outStr + len, outStrLen - len, logContentPtr);
        } else {
            len += LogValuesFmt(outStr + len, outStrLen - len, logContentPtr);
        }
    }

    if (len < 0) {
        return len;
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

static int32 LogCommonFmt(char *outStr, int32 outStrLen, const HiLogCommon *commonContentPtr)
{
    int32 ret;
    time_t time;
    uint32 month, day, hour, min, sec;
    uint8_t level;
    struct tm nowTime = {0};

    time = commonContentPtr->time;
    localtime_r(&time, &nowTime);
    month = nowTime.tm_mon + 1;
    day = nowTime.tm_mday;
    hour = nowTime.tm_hour;
    min = nowTime.tm_min;
    sec = nowTime.tm_sec;
    level = CLEAR_HASH_FLAG(commonContentPtr->level);
    if (level >= HILOG_LV_MAX) {
        level = 0;
    }
    ret = snprintf_s(outStr, outStrLen, outStrLen - 1, "%02d-%02d %02d:%02d:%02d.%03d 0 %d %c %d/%s: ",
        month, day, hour, min, sec, commonContentPtr->milli, commonContentPtr->task, g_logLevelInfo[level],
        commonContentPtr->module, HiLogGetModuleName(commonContentPtr->module));

    return ret;
}

static int32 LogValuesFmt(char *desStrPtr, int32 desLen, const HiLogContent *logContentPtr)
{
    int32 i;
    int32 outLen = 0;
    int32 len;
    char fmtStr[SINGLE_FMT_MAX_LEN];
    uint32 valNum = logContentPtr->commonContent.valueNumber;
    const char *fmt = logContentPtr->commonContent.fmt;
    uint32 valueIndex = 0;
    for (i = 0; fmt[i] != 0 && outLen < desLen;) {
        if (fmt[i] != '%') {
            desStrPtr[outLen++] = fmt[i++];
            continue;
        }
        if (fmt[i + 1] == '%') {
            desStrPtr[outLen++] = fmt[i++];
            desStrPtr[outLen++] = fmt[i++];
            continue;
        }
        fmtStr[0] = fmt[i++];
        uint32 t = 1;
        while (fmt[i] != 0 && t < sizeof(fmtStr) - 1) {
            /* %s %ms %-ms %m.ns %-m.ns convert to %p */
            if ((fmt[i] == 's' || fmt[i] == 'S') &&
                (fmt[i - 1] == '%' || (fmt[i - 1] >= '0' && fmt[i - 1] <= '9'))) {
                fmtStr[1] = 'p';
                fmtStr[FMT_CONVERT_TRMINATOR] = 0;
                i++;
                break;
            }
            if ((fmt[i] >= 'a' && fmt[i] <= 'z') || (fmt[i] >= 'A' && fmt[i] <= 'Z')) {
                fmtStr[t++] = fmt[i++];
                fmtStr[t] = 0;
                break;
            }
            fmtStr[t++] = fmt[i++];
        }
        if (valueIndex < valNum) {
            len = snprintf_s(&desStrPtr[outLen], desLen - outLen, desLen - outLen - 1,
                fmtStr, logContentPtr->values[valueIndex]);
            if (len < 0) {
                break;
            }
            outLen += len;
            valueIndex++;
        }
    }

    return outLen;
}

static int32 LogDebugValuesFmt(char *desStrPtr, int32 desLen, const HiLogContent *logContentPtr)
{
    int32 ret = 0;
    switch (logContentPtr->commonContent.valueNumber) {
        case LOG_MULTI_PARA_0:
            ret = strncpy_s(desStrPtr, desLen, logContentPtr->commonContent.fmt, desLen - 1);
            if (ret != EOK) {
                ret = -1;
            }
            break;
        case LOG_MULTI_PARA_1:
            ret = snprintf_s(desStrPtr, desLen, desLen - 1, logContentPtr->commonContent.fmt,
                logContentPtr->values[0]);
            break;
        case LOG_MULTI_PARA_2:
            ret = snprintf_s(desStrPtr, desLen, desLen - 1, logContentPtr->commonContent.fmt,
                logContentPtr->values[0], logContentPtr->values[1]);
            break;
        case LOG_MULTI_PARA_3:
            ret = snprintf_s(desStrPtr, desLen, desLen - 1, logContentPtr->commonContent.fmt,
                logContentPtr->values[0], logContentPtr->values[1], logContentPtr->values[LOG_MULTI_PARA_2]);
            break;
        case LOG_MULTI_PARA_4:
            ret = snprintf_s(desStrPtr, desLen, desLen - 1, logContentPtr->commonContent.fmt,
                logContentPtr->values[0], logContentPtr->values[1], logContentPtr->values[LOG_MULTI_PARA_2],
                logContentPtr->values[LOG_MULTI_PARA_3]);
            break;
        case LOG_MULTI_PARA_5:
            ret = snprintf_s(desStrPtr, desLen, desLen - 1, logContentPtr->commonContent.fmt,
                logContentPtr->values[0], logContentPtr->values[1], logContentPtr->values[LOG_MULTI_PARA_2],
                logContentPtr->values[LOG_MULTI_PARA_3], logContentPtr->values[LOG_MULTI_PARA_4]);
            break;
        case LOG_MULTI_PARA_MAX:
            ret = snprintf_s(desStrPtr, desLen, desLen - 1, logContentPtr->commonContent.fmt,
                logContentPtr->values[0], logContentPtr->values[1], logContentPtr->values[LOG_MULTI_PARA_2],
                logContentPtr->values[LOG_MULTI_PARA_3], logContentPtr->values[LOG_MULTI_PARA_4],
                logContentPtr->values[LOG_MULTI_PARA_5]);
            break;
        default:
            break;
    }

    return (ret < 0) ? 0 : ret;
}

static int32 LogValuesFmtHash(char *desStrPtr, int32 desLen, const HiLogContent *logContentPtr)
{
    int32 outLen = 0;
    uint32 paraNum = logContentPtr->commonContent.valueNumber;
    int32 len = snprintf_s(&desStrPtr[outLen], desLen - outLen, desLen - outLen - 1,
        "hash:%u para:", (uint32)logContentPtr->commonContent.fmt);
    if (len < 0) {
        return len;
    }
    outLen += len;

    for (uint32 i = 0; i < paraNum && i < LOG_MULTI_PARA_MAX; i++) {
        len = snprintf_s(&desStrPtr[outLen], desLen - outLen, desLen - outLen - 1,
            "%d ", (int32)logContentPtr->values[i]);
        if (len < 0) {
            return len;
        }
        outLen += len;
    }
    return outLen;
}

void FlushLog(boolean syncFlag)
{
    int8 opt = GETOPTION(g_hiviewConfig.outputOption);
    if (g_logCache.usedSize > 0) {
        if (syncFlag == FALSE) {
            switch (opt) {
                case OUTPUT_OPTION_TEXT_FILE:
                    HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_LOG_TEXT_FILE, SYNC_FILE);
                    break;
                case OUTPUT_OPTION_BIN_FILE:
                    HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_LOG_BIN_FILE, SYNC_FILE);
                    break;
                case OUTPUT_OPTION_FLOW:
                    HiviewSendMessage(HIVIEW_SERVICE, HIVIEW_MSG_OUTPUT_LOG_FLOW, SYNC_FILE);
                    break;
                default:
                    break;
            }
        } else {
            Request request = {0};
            request.msgValue = SYNC_FILE;
            switch (opt) {
                case OUTPUT_OPTION_TEXT_FILE:
                    OutputLog2TextFile(&request);
                    break;
                case OUTPUT_OPTION_BIN_FILE:
                    OutputLog2BinFile(&request);
                    break;
                case OUTPUT_OPTION_FLOW:
                    OutputLogRealtime(NULL);
                    break;
                default:
                    break;
            }
        }
    }
}

void HiviewRegisterHilogProc(HilogProc func)
{
    g_hilogOutputProc = func;
}

uint32 HiviewGetConfigOption(void)
{
    return GETOPTION(g_hiviewConfig.outputOption);
}

void HiviewUnRegisterHilogProc(HilogProc func)
{
    (void)func;
    if (g_hilogOutputProc != NULL) {
        g_hilogOutputProc = NULL;
    }
}

void HiviewRegisterHiLogFileWatcher(FileProc func, const char *path)
{
    if (func == NULL) {
        return;
    }
    RegisterFileWatcher(&g_logFile, func, path);
}

void HiviewUnRegisterHiLogFileWatcher(FileProc func)
{
    if (func == NULL) {
        return;
    }
    UnRegisterFileWatcher(&g_logFile, func);
}

int HiLogFileProcImp(const char* dest, uint8 mode)
{
    FlushLog(TRUE);
    HIVIEW_MutexLock(g_logFlushInfo.mutex);
    int ret = ProcFile(&g_logFile, dest, mode);
    HIVIEW_MutexUnlock(g_logFlushInfo.mutex);
    return ret;
}

void HiLogOutputFileLockImp(void)
{
    HIVIEW_MutexLock(g_outputLogInfo.mutex);
}

void HiLogOutputFileUnLockImp(void)
{
    HIVIEW_MutexUnlock(g_outputLogInfo.mutex);
}
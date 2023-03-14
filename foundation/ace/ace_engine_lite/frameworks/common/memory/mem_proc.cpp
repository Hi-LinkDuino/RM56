/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "mem_proc.h"
#ifdef SIMULATOR_MEMORY_ANALYSIS
#include <cstdio>
#include <windows.h>
#include <psapi.h>
#include "ace_log.h"
#include "gfx_utils/file.h"
#include "jerryscript-core.h"
#include "jerryscript.h"
#include "js_fwk_common.h"
#include "securec.h"
#include "time.h"

namespace OHOS {
namespace ACELite {
constexpr uint8_t MSG_LENGTH = 100;

constexpr uint32_t UNIT = 1024;

AceMemProc *AceMemProc::GetInstance()
{
    static AceMemProc instance;
    return &instance;
}

void AceMemProc::ClearUp()
{
    if (!IsEnabled()) {
        return;
    }
    // backup prev txt, create a new txt
    struct stat fileStat;

    int32_t state = stat(MEM_LOG_FILE_PATH, &fileStat);
    if ((state != 0) || (fileStat.st_size == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Back up file error.");
        return;
    }
    // it exists and not empty
    time_t seconds = time(NULL);
    char markdata[MSG_LENGTH];
    if (sprintf_s(markdata, MSG_LENGTH, "%ti.txt", seconds) < 0) {
        close(state);
        state = -1;
        return;
    }
    char *fullPath = RelocateJSSourceFilePath(MEM_BACK_UP_LOG_FILE_PREFIX, markdata);
    if (fullPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Back up file error.");
    } else {
        rename(MEM_LOG_FILE_PATH, fullPath);
        ace_free(fullPath);
        fullPath = nullptr;
    }
    close(state);
    state = -1;
}

void AceMemProc::SysMemTracing()
{
    if (!IsEnabled()) {
        return;
    }
    HANDLE handle = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc = {0};
    if (!GetProcessMemoryInfo(handle, reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc))) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get process memory error.");
        return;
    }

    FILE *fp = fopen(MEM_LOG_FILE_PATH, "a+");
    if (fp == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get file error.");
        return;
    }

    // physical memory
    char markdata[MSG_LENGTH];
    if (sprintf_s(markdata, MSG_LENGTH, "Phy:%d(KB)\n", uint32_t(pmc.WorkingSetSize / UNIT)) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Output file error.");
        fclose(fp);
        fp = nullptr;
        return;
    }
    fputs(markdata, fp);

    // virtual memory
    if (sprintf_s(markdata, MSG_LENGTH, "Vir:%d(KB)\n", uint32_t(pmc.PagefileUsage / UNIT)) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Output file error.");
        fclose(fp);
        fp = nullptr;
        return;
    }
    fputs(markdata, fp);

    fflush(fp);
    fclose(fp);
    fp = nullptr;
}

void AceMemProc::JerryMemTracing()
{
    if (!IsEnabled()) {
        return;
    }
    jerry_heap_stats_t stats = {0};
    if (!jerry_get_memory_stats(&stats)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get jerry heap stats error.");
        return;
    }

    FILE *fp = fopen(MEM_LOG_FILE_PATH, "a+");
    if (fp == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get file error");
        return;
    }

    size_t total = stats.size;
    size_t current = stats.allocated_bytes;
    size_t peak = stats.peak_allocated_bytes;
    // jerry total memory
    char markdata[MSG_LENGTH];
    if (sprintf_s(markdata, MSG_LENGTH, "jerry heap total:%d(KB)\n", uint32_t(total / UNIT)) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Output file error.");
        fclose(fp);
        fp = nullptr;
        return;
    }
    fputs(markdata, fp);

    // jerry current memory
    if (sprintf_s(markdata, MSG_LENGTH, "jerry heap current:%d(KB)\n", uint32_t(current / UNIT)) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Output file error.");
        fclose(fp);
        fp = nullptr;
        return;
    }
    fputs(markdata, fp);

    // jerry peak memory
    if (sprintf_s(markdata, MSG_LENGTH, "jerry heap peak:%d(KB)\n", uint32_t(peak / UNIT)) < 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Output file error.");
        fclose(fp);
        fp = nullptr;
        return;
    }
    fputs(markdata, fp);

    fflush(fp);
    fclose(fp);
    fp = nullptr;
}

bool AceMemProc::IsEnabled()
{
    return IsFileExisted(MEM_PROC_ENABLE_FLAG_FILE);
}
} // namespace ACELite
} // namespace OHOS
#endif // SIMULATOR_MEMORY_ANALYSIS

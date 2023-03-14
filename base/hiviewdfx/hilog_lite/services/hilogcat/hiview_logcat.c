/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <securec.h>
#include <unistd.h>
#include <time.h>

#include "hilog_command.h"

#define HILOG_DRIVER "/dev/hilog"
#define HILOG_LOGBUFFER 1024

#undef LOG_TAG
#define LOG_TAG "hilogcat"

int main(int argc, char *argv[])
{
    int ret;
    bool printFlag = true;
    if (argc > 1) {
        ret = HilogCmdProc(LOG_TAG, argc, argv);
        if (ret == -1 || g_hiviewConfig.silenceMod == SILENT_MODE_ON) {
            return 0;
        }
    }
    int fd = open(HILOG_DRIVER, O_RDONLY);
    if (fd < 0) {
        HILOG_ERROR(LOG_CORE, "open hilog driver failed\n");
        return 0;
    }
    char *buf = malloc(HILOG_LOGBUFFER);
    if (buf == NULL) {
        close(fd);
        return 0;
    }
    while (1) {
        (void)memset_s(buf, HILOG_LOGBUFFER, 0, HILOG_LOGBUFFER);
        ret = read(fd, buf, HILOG_LOGBUFFER);
        if (ret < sizeof(struct HiLogEntry)) {
            continue;
        }
        struct HiLogEntry *head = (struct HiLogEntry *)buf;

        time_t rawtime;
        struct tm *info = NULL;
        struct tm nowTime = {0};
        unsigned int sec = head->sec;
        rawtime = (time_t)sec;
        /* Get local time */
        info = localtime_r(&rawtime, &nowTime);

        printFlag = FilterLevelLog(g_hiviewConfig.level, *(head->msg));
        if (!printFlag) {
            continue;
        }
#define MODULE_OFFSET 2
        printFlag = FilterModuleLog(g_hiviewConfig.logOutputModule, (head->msg) + MODULE_OFFSET);
        if (!printFlag) {
            continue;
        }
        if (info == NULL) {
            continue;
        }
        buf[HILOG_LOGBUFFER - 1] = '\0';
        printf("%02d-%02d %02d:%02d:%02d.%03d %d %d %s\n", info->tm_mon + 1, info->tm_mday, info->tm_hour, info->tm_min,
            info->tm_sec, head->nsec / NANOSEC_PER_MIRCOSEC, head->pid, head->taskId, head->msg);
    }
    free(buf);
    close(fd);
    return 0;
}

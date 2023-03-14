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

#include <errno.h>
#include <fcntl.h>
#include <securec.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "hilog_command.h"

#define HILOG_LOGBUFFER 2048
#ifndef HILOG_DIR
#define HILOG_DIR "/storage/data/log"
#endif
#define HILOG_PATH1 HILOG_DIR "/hilog1.txt"
#define HILOG_PATH2 HILOG_DIR "/hilog2.txt"

#undef LOG_TAG
#define LOG_TAG "apphilogcat"

static int file1Size = 0;
static int file2Size = 0;

int FlushAndSync(FILE *fp);

static int FileSize(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return -1;
    }

    int size = 0;
    int ret = fseek(fp, 0L, SEEK_END);
    if (ret == 0) {
        size = ftell(fp);
    }
    fclose(fp);

    return size;
}

static FILE *FileClear(FILE **fp, const char *filename)
{
    if (*fp != NULL) {
        fclose(*fp);
    }
    *fp = fopen(filename, "w");
    if (*fp == NULL) {
        return NULL;
    }
    printf("write file switch %s\n", filename);
    return *fp;
}

FILE *SelectWriteFile(FILE **fp1, FILE *fp2)
{
    file1Size = FileSize(HILOG_PATH1);
    file2Size = FileSize(HILOG_PATH2);
    if (file1Size < HILOG_MAX_FILELEN) {
        return *fp1;
    } else if (file2Size < HILOG_MAX_FILELEN) {
        return fp2;
    } else { // clear file1 write file 1
        file1Size = 0;
        return FileClear(fp1, HILOG_PATH1);
    }
}

FILE *SwitchWriteFile(FILE **fp1, FILE **fp2, FILE *curFp)
{
    // select file, if file1 is full, record file2, file2 is full, record file1
    if (file1Size < HILOG_MAX_FILELEN) {
        return *fp1;
    } else if (file2Size < HILOG_MAX_FILELEN) {
        return *fp2;
    } else if (curFp == *fp2) { // clear file1 write file 1
        FlushAndSync(*fp2);
        file1Size = 0;
        return FileClear(fp1, HILOG_PATH1);
    } else {
        FlushAndSync(*fp1);
        file2Size = 0;
        return FileClear(fp2, HILOG_PATH2);
    }
}

int FlushAndSync(FILE *fp)
{
    if (fp == NULL) {
        return 0;
    }
    if (fflush(fp) != 0) {
        return -1;
    }
    int fd = fileno(fp);
    if (fsync(fd) != 0) {
        return -1;
    }
    return 0;
}

bool NeedFlush(const char *buf)
{
#define FLUSH_LOG_ARG_0 0
#define FLUSH_LOG_ARG_1 1
#define FLUSH_LOG_FLAG  0x07
    if (buf[FLUSH_LOG_ARG_0] == FLUSH_LOG_FLAG && buf[FLUSH_LOG_ARG_1] == FLUSH_LOG_FLAG) {
        return true;
    }
    return false;
}

static void FileClose(FILE *file)
{
    if (file != NULL) {
        fclose(file);
    }
}

int main(int argc, char *argv[])
{
#define HILOG_UMASK 0027
    int fd = -1;
    int ret = -1;
    FILE *fpWrite = NULL;
    bool printFlag = true;

    if (argc > 1) {
        ret = HilogCmdProc(LOG_TAG, argc, argv);
        if (ret == -1) {
            return 0;
        }
    }

    fd = open(HILOG_DRIVER, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        printf("hilog fd failed fd=%d\n", fd);
        return 0;
    }

    umask(HILOG_UMASK);
    FILE *fp1 = fopen(HILOG_PATH1, "at");
    if (fp1 == NULL) {
        printf("open err fp1 %s\n", strerror(errno));
        close(fd);
        return 0;
    }

    FILE *fp2 = fopen(HILOG_PATH2, "at");
    if (fp2 == NULL) {
        printf("open err fp2 %s\n", strerror(errno));
        FileClose(fp1);
        close(fd);
        return 0;
    }
    // First select
    fpWrite = SelectWriteFile(&fp1, fp2);
    if (fpWrite == NULL) {
        printf("SelectWriteFile open err\n");
        close(fd);
        FileClose(fp1);
        FileClose(fp2);
        return 0;
    }
    char *buf = malloc(HILOG_LOGBUFFER + 1);
    if (buf == NULL) {
        close(fd);
        FileClose(fp1);
        FileClose(fp2);
        return 0;
    }
    while (1) {
        (void)memset_s(buf, HILOG_LOGBUFFER + 1, 0, HILOG_LOGBUFFER + 1);
        ret = read(fd, buf, HILOG_LOGBUFFER);
        if (ret < sizeof(struct HiLogEntry)) {
            continue;
        }
        struct HiLogEntry *head = (struct HiLogEntry *)buf;

        if (NeedFlush(head->msg)) {
            if (FlushAndSync(fpWrite) != 0) {
                printf("flush and sync file err\n");
            }
            continue;
        }

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

        if (g_hiviewConfig.silenceMod == SILENT_MODE_OFF) {
            printf("%02d-%02d %02d:%02d:%02d.%03d %d %d %s\n", info->tm_mon + 1, info->tm_mday, info->tm_hour,
                info->tm_min, info->tm_sec, head->nsec / NANOSEC_PER_MIRCOSEC, head->pid, head->taskId, head->msg);
        }

        ret =
            fprintf(fpWrite, "%02d-%02d %02d:%02d:%02d.%03d %d %d %s\n", info->tm_mon + 1, info->tm_mday, info->tm_hour,
                info->tm_min, info->tm_sec, head->nsec / NANOSEC_PER_MIRCOSEC, head->pid, head->taskId, head->msg);
        if (ret < 0) {
            printf("[FATAL]File can't write fpWrite %s\n", strerror(errno));
            close(fd);
            FileClose(fp1);
            FileClose(fp2);
            return 0;
        }
        if (fpWrite == fp1) {
            file1Size += ret;
        } else if (fpWrite == fp2) {
            file2Size += ret;
        }
        // select file, if file1 is full, record file2, file2 is full, record file1
        fpWrite = SwitchWriteFile(&fp1, &fp2, fpWrite);
        if (fpWrite == NULL) {
            printf("[FATAL]SwitchWriteFile failed\n");
            close(fd);
            FileClose(fp1);
            FileClose(fp2);
            return 0;
        }
    }
    free(buf);
    close(fd);
    FileClose(fp1);
    FileClose(fp2);
    return 0;
}

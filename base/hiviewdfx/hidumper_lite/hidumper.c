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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "securec.h"

#define HIDUMPER_DEVICE  "/dev/hidumper"
#define USER_FAULT_ADDR  0x3
#define USER_FAULT_VALUE 0x4
#define PATH_MAX_LEN     256

#define ONE_OF_ARGC_PARAMETERS           1
#define TWO_OF_ARGC_PARAMETERS           2
#define THREE_OF_ARGC_PARAMETERS         3
#define FOUR_OF_ARGC_PARAMETERS          4
#define FIVE_OF_ARGC_PARAMETERS          5

#define BUF_SIZE_16                      16

enum MemDumpType {
    DUMP_TO_STDOUT,
    DUMP_REGION_TO_STDOUT,
    DUMP_TO_FILE,
    DUMP_REGION_TO_FILE
};

struct MemDumpParam {
    enum MemDumpType type;
    unsigned long long start;
    unsigned long long size;
    char filePath[PATH_MAX_LEN];
};

#define HIDUMPER_IOC_BASE            'd'
#define HIDUMPER_DUMP_ALL            _IO(HIDUMPER_IOC_BASE, 1)
#define HIDUMPER_CPU_USAGE           _IO(HIDUMPER_IOC_BASE, 2)
#define HIDUMPER_MEM_USAGE           _IO(HIDUMPER_IOC_BASE, 3)
#define HIDUMPER_TASK_INFO           _IO(HIDUMPER_IOC_BASE, 4)
#define HIDUMPER_INJECT_KERNEL_CRASH _IO(HIDUMPER_IOC_BASE, 5)
#define HIDUMPER_DUMP_FAULT_LOG      _IO(HIDUMPER_IOC_BASE, 6)
#define HIDUMPER_MEM_DATA            _IOW(HIDUMPER_IOC_BASE, 7, struct MemDumpParam)

static void Usage(void)
{
    printf("Usage:\n"
        "    hidumper                              dump cpu usage, memory usage and all tasks.\n"
        "or:\n"
        "    hidumper -dc                          dump the cpu usage.\n"
        "    hidumper -df                          dump the fault logs.\n"
        "    hidumper -dm                          dump the memory usage.\n"
        "    hidumper -dt                          dump all the tasks.\n"
        "    hidumper -h                           help text for the tool.\n"
        "    hidumper -ikc                         inject kernel crash for debug version.\n"
        "    hidumper -iuc                         inject user crash for debug version.\n");
    printf("    hidumper -m                           dump memory to stdout in hex format for debug version.\n"
        "    hidumper -m filepath                  dump memory to filepath in the device in hex format for "
        "debug version.\n"
        "    hidumper -m memstart memsize          dump memory with starting address memstart(hex) and\n"
        "                                          size memsize(hex) to stdout in hex format for debug version.\n"
        "    hidumper -m memstart memsize filepath dump memory with starting address memstart(hex) and\n"
        "                                          size memsize(hex) to filepath in hex format for debug version.\n");
}

static void ExecAction(int fd, unsigned int cmd)
{
    if (fd < 0) {
        printf("Invalid fd: %d\n", fd);
        return;
    }

    int ret = ioctl(fd, cmd, NULL);
    if (ret < 0) {
        printf("Failed to ioctl [%u], error [%s]\n", cmd, strerror(errno));
    }
}

static void DumpALLInfo(int fd)
{
    ExecAction(fd, HIDUMPER_DUMP_ALL);
}

static void DumpCpuUsage(int fd)
{
    ExecAction(fd, HIDUMPER_CPU_USAGE);
}

static void DumpMemUsage(int fd)
{
    ExecAction(fd, HIDUMPER_MEM_USAGE);
}

static void DumpTaskInfo(int fd)
{
    ExecAction(fd, HIDUMPER_TASK_INFO);
}

static void DumpMemData(int fd, struct MemDumpParam *param)
{
#ifdef OHOS_DEBUG
    int ret = ioctl(fd, HIDUMPER_MEM_DATA, param);
    if (ret < 0) {
        printf("Failed to ioctl [%s], error [%s]\n", HIDUMPER_DEVICE, strerror(errno));
    }
#else
    (void)fd;
    (void)param;
    printf("Unsupported!\n");
#endif
}

static void InjectKernelCrash(int fd)
{
#ifdef OHOS_DEBUG
    ExecAction(fd, HIDUMPER_INJECT_KERNEL_CRASH);
#else
    (void)fd;
    printf("Unsupported!\n");
#endif
}

static void DumpFaultLog(int fd)
{
    ExecAction(fd, HIDUMPER_DUMP_FAULT_LOG);
}

static void InjectUserCrash(void)
{
#ifdef OHOS_DEBUG
    *((int *)USER_FAULT_ADDR) = USER_FAULT_VALUE;
#else
    printf("Unsupported!\n");
#endif
}

int ParameterMatching(int argc, const char *argv[], int fd)
{
    struct MemDumpParam param;
    if (argc == ONE_OF_ARGC_PARAMETERS) {
        DumpALLInfo(fd);
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-dc") == 0) {
        DumpCpuUsage(fd);
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-df") == 0) {
        DumpFaultLog(fd);
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-dm") == 0) {
        DumpMemUsage(fd);
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-dt") == 0) {
        DumpTaskInfo(fd);
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-ikc") == 0) {
        InjectKernelCrash(fd);
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-iuc") == 0) {
        InjectUserCrash();
    }  else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-m") == 0) {
        param.type = DUMP_TO_STDOUT;
        DumpMemData(fd, &param);
    } else if (argc == THREE_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-m") == 0) {
        param.type = DUMP_TO_FILE;
        if (strncpy_s(param.filePath, sizeof(param.filePath),
            argv[TWO_OF_ARGC_PARAMETERS], sizeof(param.filePath) - 1) != EOK) {
            printf("param.filePath is not enough or strncpy_s failed\n");
            return -1;
        }
        DumpMemData(fd, &param);
    } else if (argc == FOUR_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-m") == 0) {
        param.type = DUMP_TO_STDOUT;
        param.start = strtoull(argv[TWO_OF_ARGC_PARAMETERS], NULL, BUF_SIZE_16);
        param.size = strtoull(argv[THREE_OF_ARGC_PARAMETERS], NULL, BUF_SIZE_16);
        DumpMemData(fd, &param);
    }  else if (argc == FIVE_OF_ARGC_PARAMETERS && strcmp(argv[ONE_OF_ARGC_PARAMETERS], "-m") == 0) {
        param.type = DUMP_TO_FILE;
        param.start = strtoull(argv[TWO_OF_ARGC_PARAMETERS], NULL, BUF_SIZE_16);
        param.size = strtoull(argv[THREE_OF_ARGC_PARAMETERS], NULL, BUF_SIZE_16);
        if (strncpy_s(param.filePath, sizeof(param.filePath),
            argv[FOUR_OF_ARGC_PARAMETERS], sizeof(param.filePath) - 1) != EOK) {
            printf("param.filePath is not enough or strncpy_s failed\n");
            return -1;
        }
        DumpMemData(fd, &param);
    } else {
        Usage();
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    int fd = -1;
    fd = open(HIDUMPER_DEVICE, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open [%s], error [%s]\n", HIDUMPER_DEVICE, strerror(errno));
        return -1;
    }

    int ret = ParameterMatching(argc, argv, fd);
    close(fd);
    return ret;
}

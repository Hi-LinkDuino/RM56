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

#include <securec.h>
#include <stdio.h>
#include <stdlib.h>

#include "hidumper.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FAULT_ADDR          0x123
#define FAULT_VALUE         0x456
#define SYS_INFO_HEADER     "*********** sys info ***********"
#define CPU_USAGE_HEADER    "*********** cpu usage ***********"
#define MEM_USAGE_HEADER    "*********** mem usage ***********"
#define TASK_INFO_HEADER    "*********** task info ***********"

static int g_isAdapterRegistered = 0;
static struct HiDumperAdapter g_hidumperAdapter;

static void Usage(void)
{
    printf("usage:\r\n"
        "AT+HIDUMPER=                                dump cpu usage, memory usage and all tasks\r\n"
        "or:\r\n");
    printf("AT+HIDUMPER=-dc                             dump the cpu usage\r\n"
        "AT+HIDUMPER=-df                             dump the latest fault logs\r\n");
    printf("AT+HIDUMPER=-dm                             dump the memory usage\r\n"
        "AT+HIDUMPER=-dt                             dump all the tasks\r\n");
    printf("AT+HIDUMPER=-h                              help text for the tool\r\n"
        "AT+HIDUMPER=-ikc                            inject kernel crash\r\n");
    printf("AT+HIDUMPER=-m                              dump memory to stdout in hex format\r\n"
        "AT+HIDUMPER=-m,filepath                     dump memory to filepath in the device in hex format\r\n");
    printf("AT+HIDUMPER=-m,memstart,memsize             dump memory with starting address memstart(hex) and\r\n"
        "                                            size memsize(hex) to stdout in hex format\r\n");
    printf("AT+HIDUMPER=-m,memstart,memsize,filepath    dump memory with starting address memstart(hex) and\r\n"
        "                                            size memsize(hex) to filepath in the device in hex format\r\n");
}

static void DumpAllInfo(void)
{
    printf("%s\n", SYS_INFO_HEADER);
    g_hidumperAdapter.DumpSysInfo();
    printf("%s\n", CPU_USAGE_HEADER);
    g_hidumperAdapter.DumpCpuUsage();
    printf("%s\n", MEM_USAGE_HEADER);
    g_hidumperAdapter.DumpMemUsage();
    printf("%s\n", TASK_INFO_HEADER);
    g_hidumperAdapter.DumpTaskInfo();
}

static void InjectKernelCrash(void)
{
#ifdef OHOS_DEBUG
    int *ptr = (int *)FAULT_ADDR;
    *ptr = FAULT_VALUE;
#else
    printf("Unsupported!\n");
#endif
}

int HiDumperRegisterAdapter(struct HiDumperAdapter *pAdapter)
{
    if (pAdapter == NULL) {
        printf("Invalid pAdapter: %p\n", pAdapter);
        return -1;
    }

    if (pAdapter->DumpSysInfo == NULL ||
        pAdapter->DumpCpuUsage == NULL ||
        pAdapter->DumpMemUsage == NULL ||
        pAdapter->DumpTaskInfo == NULL ||
        pAdapter->DumpFaultLog == NULL ||
        pAdapter->DumpMemRegion == NULL ||
        pAdapter->DumpAllMem == NULL) {
            printf("Invalid adapter funcs!\n");
            return -1;
    }
    if (memcpy_s(&g_hidumperAdapter, sizeof(g_hidumperAdapter),
        pAdapter, sizeof(*pAdapter)) != EOK) {
            printf("memcpy_s is error\n");
    }
    g_isAdapterRegistered = 1;

    return 0;
}

void ParameterMatching(int argc, const char *argv[])
{
    if (argc == 0) {
        DumpAllInfo();
    } else if (argc == ONE_OF_ARGC_PARAMETERS) {
        if (strcmp(argv[0], "-h") == 0) {
            Usage();
        } else if (strcmp(argv[0], "-dc") == 0) {
            printf("%s\n", CPU_USAGE_HEADER);
            g_hidumperAdapter.DumpCpuUsage();
        } else if (strcmp(argv[0], "-df") == 0) {
            g_hidumperAdapter.DumpFaultLog();
        } else if (strcmp(argv[0], "-dm") == 0) {
            printf("%s\n", MEM_USAGE_HEADER);
            g_hidumperAdapter.DumpMemUsage();
        } else if (strcmp(argv[0], "-dt") == 0) {
            printf("%s\n", TASK_INFO_HEADER);
            g_hidumperAdapter.DumpTaskInfo();
        } else if (strcmp(argv[0], "-ikc") == 0) {
            InjectKernelCrash();
        } else if (strcmp(argv[0], "-m") == 0) {
#ifdef OHOS_DEBUG
            g_hidumperAdapter.DumpAllMem();
#else
            printf("Unsupported!\n");
#endif
        } else {
            Usage();
        }
    } else if (argc == TWO_OF_ARGC_PARAMETERS && strcmp(argv[0], "-m") == 0) {
        printf("Unsupported!\n");
    } else if (argc == THREE_OF_ARGC_PARAMETERS) {
        if (strcmp(argv[0], "-m") == 0) {
#ifdef OHOS_DEBUG
            g_hidumperAdapter.DumpMemRegion(
                strtoull(argv[ONE_OF_ARGC_PARAMETERS], NULL, BUF_SIZE_16),
                strtoull(argv[TWO_OF_ARGC_PARAMETERS], NULL, BUF_SIZE_16));
#else
            printf("Unsupported!\n");
#endif
        } else {
            Usage();
        }
    } else if (argc == FOUR_OF_ARGC_PARAMETERS && strcmp(argv[0], "-m") == 0) {
        printf("Unsupported!\r\n");
    } else {
        Usage();
    }
}

unsigned int at_hidumper(unsigned int argc, const char **argv)
{
    if (g_isAdapterRegistered == 0) {
        printf("No adapter has been registered!\n");
        return 1;
    }

    ParameterMatching(argc, argv);

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
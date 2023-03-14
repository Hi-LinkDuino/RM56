/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "hal_cmu.h"
#include "hm_sys.h"
#include "pmu.h"
#include "hal_norflash.h"
#include "cmsis.h"
#include "hal_trace.h"
#include "los_memory.h"
#include "los_task.h"
#include "console.h"
#include "hiview_util.h"
#include "hiview_output_log.h"
#include "hiview_log.h"
#include "devmgr_service_start.h"
#include "ohos_mem_pool.h"
//#include "threading_alt.h"
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ohos_init.h"
#include "app_ble_include.h"

#define BTIF_BAM_NOT_ACCESSIBLE     0x00    /* Non-discoverable or connectable */
#define BTIF_BAM_GENERAL_ACCESSIBLE 0x03    /* General discoverable and connectable */
const static uint8_t adv_addr_set[6] = {0x66, 0x34, 0x33, 0x23, 0x22, 0x11};
extern void app_bt_accessmode_set_by_AT_command(uint8_t mode);
extern void bes_bt_init(void);
extern bool app_is_stack_ready(void);

void OsShowInfo(void)
{
    static uint32_t last_pool_total_used_size = 0;
    LOS_MEM_POOL_STATUS mem_pool_status = {0};
    LOS_MemInfoGet(OS_SYS_MEM_ADDR, &mem_pool_status);

    int diff = mem_pool_status.totalUsedSize - last_pool_total_used_size;
    last_pool_total_used_size = mem_pool_status.totalUsedSize;

    printf("====================================================\n");
    printf("freeNodeNum     = %d\n", mem_pool_status.freeNodeNum);
    printf("maxFreeNodeSize = %d\n", mem_pool_status.maxFreeNodeSize);
    printf("totalFreeSize   = %d\n", mem_pool_status.totalFreeSize);
    printf("totalUsedSize   = %d\n", mem_pool_status.totalUsedSize);
    printf("usage_mem       = %d\n", diff);
    printf("usageWaterLine  = %d\n", mem_pool_status.usageWaterLine);
    printf("usedNodeNum     = %d\n", mem_pool_status.usedNodeNum);
    printf("====================================================\n");
    OsGetAllTskInfo();
}

static int doShowOsInfo(cmd_tbl_t *cmd, int argc, char *argv[])
{
    OsShowInfo();
    return 0;
}

static int doFsLs(cmd_tbl_t *cmd, int argc, char *argv[])
{
    if (argc < 2) {
        printf("AT+LS=<path>\n");
        return -1;
    }
    const char *path = argv[1];
    DIR *dir;
    struct dirent *dp;
    if ((dir = opendir(path)) == NULL) {
        printf("opendir %s failed, %s\n", path, strerror(errno));
        return -1;
    }
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        struct stat st_buf = {0};
        char realpath[260];
        snprintf(realpath, sizeof(realpath), "%s/%s", path, dp->d_name);
        if (stat(realpath, &st_buf) != 0) {
            printf("can not access %s\n", realpath);
            closedir(dir);
            return -1;
        }
        if ((st_buf.st_mode & S_IFMT) == S_IFDIR) {
            printf("DIR %s\n", realpath);
        } else {
            printf("FILE %s, %ld bytes\n", realpath, st_buf.st_size);
        }
    }
    closedir(dir);
    printf("+ok\r\n");
    return 0;
}

static int doBtInit(cmd_tbl_t *cmd, int argc, char *argv[])
{
    uint8_t BtEnable;
    if (argc < 2) {
        printf("Input pare error, AT+INITBT=1 is ok.\n");
        return -1;
    }

    BtEnable = atoi(argv[1]);

    if (BtEnable) {
        if (!app_is_stack_ready()) {
            bes_bt_init();
        } else {
            printf("%s  bt already init\n",__func__);
        }
    }
    return 0;
}

int doBtScanEnable(cmd_tbl_t *cmd, int argc, char *argv[])
{
    uint8_t advEnable;
    if (argc < 2) {
        printf("Input pare error, AT+OPENBT=1 is ok\n");
        return -1;
    }
    advEnable = atoi(argv[1]);

    printf( "%s() %s %d\n",__func__,argv[0],advEnable);

    if (advEnable) {
        app_bt_accessmode_set_by_AT_command(BTIF_BAM_GENERAL_ACCESSIBLE);
    } else {
        app_bt_accessmode_set_by_AT_command(BTIF_BAM_NOT_ACCESSIBLE);
    }

    return 0;
}

int doBleAdvEnable(cmd_tbl_t *cmd, int argc, char *argv[])
{
    uint8_t advEnable;
    uint16_t advInterval;
    char *advname = "ble_adv_test";
    char advdata[31] = {0};

    if (argc < 3) {
        printf("Input pare error, AT+OPENBLE=1,interval (interval is 20 ~ 10000ms) is ok\n");
        return -1;
    }
    advEnable = atoi(argv[1]);
    advInterval = atoi(argv[2]);
    advdata[0] = 0x02;
    advdata[1] = 0x01;
    advdata[2] = 0x06;
    advdata[3] = 0x1b;
    advdata[4] = 0x09;
    memcpy(&advdata[5],advname,strlen(advname));

    printf("%s() %s %d  %d\n",__func__,argv[0],advEnable,advInterval);

    if (advEnable) {
        app_ble_custom_init();
        app_ble_custom_adv_write_data(0,
                                    true,
                                    (uint8_t *)adv_addr_set,
                                    NULL,
                                    advInterval,
                                    ADV_TYPE_UNDIRECT,
                                    12,
                                    (uint8_t *)advdata, 31,
                                    NULL, 0);

        app_ble_custom_adv_start(0);
    } else {
        app_ble_custom_adv_stop(0);
    }

    return 0;
}

int doBleScanEenable(cmd_tbl_t *cmd, int argc, char *argv[])
{
    uint8_t scanEnable;
    uint16_t scanWindow, scanInterval;

    if (argc < 4) {
        printf("Input pare error, AT+OPENBLE=1,scanWindow(is 5~10000ms),\
                interval(10 ~ 10000ms), scanWindow < interval is ok\n");
        return -1;
    }

    scanEnable = atoi(argv[1]);
    scanWindow = atoi(argv[2]);
    scanInterval = atoi(argv[3]);
    printf( "%s() %s %d  %d  %d\n",__func__,argv[0],scanEnable,scanWindow,scanInterval);

    if (scanEnable) {
        app_ble_start_scan(0, scanWindow, scanInterval);
    } else {
        app_ble_stop_scan();
    }

    return 0;
}

void RegisterCustomATCmd()
{
    cmd_tbl_t cmd_list[] = {
        {"AT+SHOWOSINFO", 1, doShowOsInfo, "AT+SHOWOSINFO - show memory and cpu usage\n"},
        {"AT+LS", 2, doFsLs, "AT+LS  - list file/dir in fs\n"},
        {"AT+INITBT", 2, doBtInit, "AT+INITBT  - init bt\n"},
        {"AT+OPENBT", 2, doBtScanEnable, "AT+OPENBT  - Open bt,\n"},
        {"AT+OPENBLE", 3, doBleAdvEnable, "AT+OPENBLE  - Open ble,\n"},
        {"AT+SCANBLE", 4, doBleScanEenable, "AT+SCANBLE  - Scanning ble,\n"},
    };
    for (int i = 0; i < sizeof(cmd_list) / sizeof(cmd_tbl_t); i++) {
        console_cmd_add(&cmd_list[i]);
    }
}
SYS_SERVICE_INIT(RegisterCustomATCmd);

void HAL_NVIC_SystemReset(void)
{
}

/**
 * @brief implement for hilog_lite featured/mini
 * @notice hilog_lite mini converts '%s' to '%p'
 */
boolean HilogProc_Impl(const HiLogContent *hilogContent, uint32 len)
{
    char tempOutStr[LOG_FMT_MAX_LEN] = {0};
    if (LogContentFmt(tempOutStr, sizeof(tempOutStr), hilogContent) > 0) {
        printf(tempOutStr);
#ifdef LOG_FLUSH
        hal_trace_flush_buffer();
#endif
    }
    return TRUE;
}

int init_trace_system(void)
{
    int ret = 1;
    HiviewRegisterHilogProc(HilogProc_Impl);
    return ret;
}

int HiLogWriteInternal(const char *buffer, size_t bufLen)
{
    if (!buffer)
        return -1;
    // because it's called as HiLogWriteInternal(buf, strlen(buf) + 1)
    if (bufLen < 2)
        return 0;
    if (buffer[bufLen - 2] != '\n') {
        *((char *)buffer + bufLen - 1) = '\n';
    } else {
        bufLen--;
    }
    int ret = hal_trace_output((const unsigned char *)buffer, bufLen);
#ifdef LOG_FLUSH
    hal_trace_flush_buffer();
#endif
    return ret;
}

int OhosSystemAdapterHooks(void)
{
    init_trace_system();
    DeviceManagerStart();
    return 0;
}

/**
 * @brief implement for ohos_mem_pool.h
 */
void *OhosMalloc(MemType type, uint32 size)
{
    return malloc(size);
}

void OhosFree(void *ptr)
{
    free(ptr);
}

/**
 * @brief adapter for js_ability.cpp
 * #ifdef OHOS_ACELITE_PRODUCT_WATCH 
 */
void RestoreSystemWrapper(const char *crashMessage)
{
    printf("%s\n", crashMessage);
    hal_trace_flush_buffer();
}


void IoTWatchDogKick(void) {}

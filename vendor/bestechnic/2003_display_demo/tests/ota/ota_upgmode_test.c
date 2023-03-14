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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hal_timer.h"
#include "hal_trace.h"
#include "tool_msg.h"
#include "flash.h"
#include "cmsis_os2.h"
#include "console.h"
#include "at.h"
#include "curl/curl.h"
#include "kfifo.h"
#include "bwifi_interface.h"

#include "log.h"
#undef LOG_TAG
#define LOG_TAG "OTA"

#define LOG_E(fmt, ...) HILOG_ERROR(HILOG_MODULE_APP, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) HILOG_INFO(HILOG_MODULE_APP, fmt, ##__VA_ARGS__)

#define OTA_DEBUG 0

#define OTA_FIFO_SIZE           (32*1024)
#define FLASH_BLOCK_SIZE        (32*1024)
static struct kfifo ota_fifo;
static unsigned char ota_fifo_buffer[OTA_FIFO_SIZE];

static enum ota_link ota_update_link = OTA_LINK_MAX;
static size_t ota_bin_len = 0;
static size_t total_size = 0;

static char *bes_trim(char *str)
{
    if (str == NULL) return NULL;

    char *p = str;
    char *p1 = p + strlen(str) - 1;

    while (*p && isspace((int)*p)) {
        p++;
    }
    while (p1 > p && isspace((int)*p1)) {
        *p1-- = 0;
    }
    return p;
}

static size_t rcv_ota_hdata(void *buffer, size_t size, size_t nitems, void *outstream)
{
    LOG_I("%s", buffer);
    char *key = strtok((char *)buffer, ":");
    if (!strncmp(key,"Content-Length",strlen("Content-Length")))
    {
        ota_bin_len = atoi(bes_trim(strtok(NULL,":")));
    }
    return nitems;
}

static size_t write_data_to_ota_thread(void *ptr, size_t size)
{
    unsigned int sendlen = 0;
    while (1)
    {
        if (size > (int)kfifo_available(&ota_fifo))
        {
            osDelay(100);
            continue;
        }
        sendlen = kfifo_put(&ota_fifo, (unsigned char*)ptr, size);
        break;
    }

    total_size += sendlen;
    osDelay(100);
    return sendlen;
}

static size_t rcv_ota_bdata(void *buffer, size_t size, size_t nitems, void *outstream)
{
    // todo: write to flash
    write_data_to_ota_thread(buffer, nitems);
    return nitems;

}

static int libcurl_ota_request(char* http_ota_url)
{
    LOG_I("%s begin",__func__);
    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != return_code)
    {
        LOG_E("curl_global_init() failed: %s ",curl_easy_strerror(return_code));
        return -1;
    }

    CURL *easy_handle = curl_easy_init();
    if (NULL == easy_handle)
    {
        curl_global_cleanup();
        return -1;
    }

    char *stream = NULL;
    curl_easy_setopt(easy_handle, CURLOPT_URL, http_ota_url);
    curl_easy_setopt(easy_handle, CURLOPT_HTTPGET, 1L);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Connection: close");
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(easy_handle, CURLOPT_FORBID_REUSE, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, rcv_ota_hdata);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, rcv_ota_bdata);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, stream);
    return_code = curl_easy_perform(easy_handle);
    if (NULL != headers)
    {
        curl_slist_free_all(headers);
    }

    if (CURLE_OK != return_code)
    {
        LOG_E("curl_easy_perform() failed: %s ", curl_easy_strerror(return_code));
        curl_easy_cleanup(easy_handle);
        curl_global_cleanup();
        return -1;
    }
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
    LOG_I("%s done",__func__);
    return 0;
}

void ota_seconds_to_reboot_system(uint32_t seconds)
{
    int n = seconds;
    do {
        osDelay(1000);
        LOG_I("reset after %d seconds", n);
    }while(n--);

    hal_cmu_sys_reboot();
}

static int ota_upgrade_set_to_zone(void)
{
    int32_t ret = 0;
    bootinfo_block block;
    MiscDataInfo boot_info;
    if (ota_update_link != OTA_LINK_MAX) {
        block = ota_get_valid_bootinfo_block();
        ret = ota_get_bootinfo(&boot_info, block, ota_get_bootinfo_zone_num(block));
        if (ret) {
            LOG_E("error %s %d, ret:%d", __func__, __LINE__, ret);
            return -1;
        }

        if (ota_update_link == OTA_LINK_BOOT2A) {
            boot_info.curbootArea = BOOT_AREA_A;
        } else if (ota_update_link == OTA_LINK_BOOT2B) {
            boot_info.curbootArea = BOOT_AREA_B;
        } else if (ota_update_link == OTA_LINK_RTOSA) {
            boot_info.upgMode = UPG_MODE_NORMAL;
        } else if (ota_update_link == OTA_LINK_RTOSB) {
            boot_info.upgMode = UPG_MODE_OTA;
        }

        ret = ota_set_bootinfo_to_zoneAB(&boot_info);
        if (ret) {
            LOG_E("error %s %d, ota_set_bootinfo return %d", __func__, __LINE__, ret);
        }
    } else {
        LOG_E("error! not correctly set ota_update_link.");
        return -1;
    }
    LOG_I("ota_upgrade_finish!!!");
    return ret;
}

static void ota_thread(void *param)
{
    LOG_I("enter %s %d", __func__, __LINE__);
    hal_logic_partition_t partition_info;
    bootinfo_block block;
    MiscDataInfo boot_info;
    hal_partition_t partition_index = *((hal_partition_t *)param);
    uint32_t offset = 0;
    int ret = 0;

    block = ota_get_valid_bootinfo_block();
    ret = ota_get_bootinfo(&boot_info, block, ota_get_bootinfo_zone_num(block));
    if (ret) {
        LOG_E("error %s %d, ret:%d", __func__, __LINE__, ret);
        return;
    }

    if (boot_info.curbootArea == BOOT_AREA_B) {
        if (partition_index == HAL_PARTITION_BOOT2B) {
            partition_index = HAL_PARTITION_BOOT2A;
        }
    }
    if (boot_info.upgMode == UPG_MODE_OTA) {
        if (partition_index == HAL_PARTITION_SYSTEM_MINI) {
            partition_index = HAL_PARTITION_CM33_MAIN;
        }
    }

    if (partition_index == HAL_PARTITION_BOOT2A) {
        ota_update_link = OTA_LINK_BOOT2A;
    } else if (partition_index == HAL_PARTITION_BOOT2B) {
        ota_update_link = OTA_LINK_BOOT2B;
    } else if (partition_index == HAL_PARTITION_CM33_MAIN) {
        ota_update_link = OTA_LINK_RTOSA;
    } else if (partition_index == HAL_PARTITION_SYSTEM_MINI) {
        ota_update_link = OTA_LINK_RTOSB;
    }

    hal_flash_info_get(partition_index, &partition_info);
    if (ret) {
        LOG_E("error %s %d, ret:%d", __func__, __LINE__, ret);
        return;
    }
    LOG_I("partition_info.partition_owner = %d",partition_info.partition_owner);
    LOG_I("partition_info.partition_description = %s",partition_info.partition_description);
    LOG_I("partition_info.partition_start_addr = 0x%x",partition_info.partition_start_addr);
    LOG_I("partition_info.partition_length = 0x%x",partition_info.partition_length);
    LOG_I("partition_info.partition_options = %d",partition_info.partition_options);
    LOG_I("partition_index = %d", partition_index);
    LOG_I("ota_update_link = %d", ota_update_link);

    while (1)
    {
        uint8_t ota_buf[FLASH_BLOCK_SIZE] = {0};
        size_t read_len = kfifo_get(&ota_fifo, ota_buf, sizeof(ota_buf));
        if (read_len == 0)
        {
            osDelay(100);
            continue;
        }
        ret = ota_flash_write(partition_index, offset, ota_buf, read_len);
        if (ret)
        {
            LOG_E("error %s %d, ota_flash_write return %d", __func__, __LINE__, ret);
            break;
        }
        offset += read_len;
        LOG_I("[%s] OTA is running, flash is writing [%d Byte]", __func__, offset);
        if (offset == ota_bin_len && ota_bin_len != 0)
        {
            LOG_I("%s %d, flash write bin finish!", __func__, __LINE__);
            ret = ota_upgrade_set_to_zone();
            break;
        }
        osDelay(100);
    }

    if (!ret)
    {
        LOG_I("The device will reboot after 5 seconds");
        ota_seconds_to_reboot_system(5);
        while (1) {
            LOG_I("board is rebooting, please wait...");
            osDelay(100);
        }
    }
}

static int ota_upgmode_init(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    LOG_I("enter %s %d", __func__, __LINE__);
    char *ota_url = NULL;
    int ret = -1;
    bootinfo_block block;
    MiscDataInfo boot_info;
    BWIFI_STATUS_T status;

    if (argc < 2) {
        LOG_E("param err");
        return -1;
    }
    ota_url = argv[1];
    block = ota_get_valid_bootinfo_block();
    ret = ota_get_bootinfo(&boot_info, block, ota_get_bootinfo_zone_num(block));
    if (ret) {
        LOG_E("error %s %d, ret:%d", __func__, __LINE__, ret);
        return -1;
    }

    status = bwifi_get_current_status();
    if (status < BWIFI_STATUS_GOT_IP) {
        LOG_E("Wifi is not connected, please connect wifi first! Wifi status:%d", status);
        return -2;
    }

    hal_partition_t partition_index = HAL_PARTITION_SYSTEM_MINI;
    kfifo_init(&ota_fifo, ota_fifo_buffer, OTA_FIFO_SIZE);

    // thread for ota operation
    osThreadAttr_t attr;
    attr.name = "OtaTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 64*1024;
    attr.priority = 24;
    if (osThreadNew((osThreadFunc_t)ota_thread, (void *)&partition_index, &attr) == NULL)
    {
        LOG_E("Failed to Create ota_cloud_thread");
        return -1;
    }
    ret = libcurl_ota_request(ota_url);
    osDelay(100);
    RESP_OK(handler);
    return ret;
}

// just for debug test
#if OTA_DEBUG
static void ota_show_bootinfo(MiscDataInfo *ctrl, bootinfo_block block_num, bootinfo_zone zone_num)
{
    LOG_I("show block(%d)zone(%d)bootinfo:", block_num, zone_num);

    if (ctrl == NULL) {
        MiscDataInfo info;

        ota_get_bootinfo(&info, block_num, zone_num);
        LOG_I("headerMagic:0x%x",                info.headerMagic);
        LOG_I("crcVal:0x%x",                     info.crcVal);
        LOG_I("len:0x%x",                        info.len);
        LOG_I("curbootArea:0x%x",                info.curbootArea);
        LOG_I("upgMode:0x%x",                    info.upgMode);
        LOG_I("quietUpgFlg:0x%x",                info.quietUpgFlg);
        LOG_I("timerRebootFlg:0x%x",             info.timerRebootFlg);
        LOG_I("bootTimes:0x%x",                  info.bootTimes);
        LOG_I("rdDataLen:0x%x",                  info.rdMode.rdDataLen);
        LOG_I("\n");
    } else {
        LOG_I("headerMagic:0x%x",                ctrl->headerMagic);
        LOG_I("crcVal:0x%x",                     ctrl->crcVal);
        LOG_I("len:0x%x",                        ctrl->len);
        LOG_I("curbootArea:0x%x",                ctrl->curbootArea);
        LOG_I("upgMode:0x%x",                    ctrl->upgMode);
        LOG_I("quietUpgFlg:0x%x",                ctrl->quietUpgFlg);
        LOG_I("timerRebootFlg:0x%x",             ctrl->timerRebootFlg);
        LOG_I("bootTimes:0x%x",                  ctrl->bootTimes);
        LOG_I("rdDataLen:0x%x",                  ctrl->rdMode.rdDataLen);
        LOG_I("\n");
    }
}

static int do_show_bootinfo(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    LOG_I("enter %s %d", __func__, __LINE__);
    bootinfo_block block;

    if (argc < 2) {
        LOG_E("param err");
        return -1;
    }

    if (!strcmp(argv[1], "BOOTINFO_ORIGIN")) {
        block = BOOTINFO_ORIGIN;
    } else if (!strcmp(argv[1], "BOOTINFO_BACKUP")) {
        block = BOOTINFO_BACKUP;
    } else {
        LOG_E("[%s] bootinfo block err! Input BOOTINFO_ORIGIN or BOOTINFO_BACKUP!", __func__);
        return -1;
    }

    ota_show_bootinfo(NULL, block, ota_get_bootinfo_zone_num(block));

    RESP_OK(handler);
    return 0;
}
#endif

static int do_sys_reboot(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    LOG_I("enter %s %d", __func__, __LINE__);

    hal_cmu_sys_reboot();

    RESP_OK(handler);
    return 0;
}

static int do_set_bootinfo(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    LOG_I("enter %s %d", __func__, __LINE__);
    int ret = 0;
    bootinfo_block block;
    MiscDataInfo boot_info;

    if (argc < 2) {
        LOG_E("param err");
        return -1;
    }

    block = ota_get_valid_bootinfo_block();
    ret = ota_get_bootinfo(&boot_info, block, ota_get_bootinfo_zone_num(block));
    if (ret) {
        LOG_E("error %s %d, ret:%d", __func__, __LINE__, ret);
        return -1;
    }

    if (!strcmp(argv[1], "NORMAL")) {
        boot_info.upgMode = UPG_MODE_NORMAL;
        LOG_I("enter %s %d, set normal mode!", __func__, __LINE__);
    } else if (!strcmp(argv[1], "OTA")) {
        boot_info.upgMode = UPG_MODE_OTA;
        LOG_I("enter %s %d, set OTA mode!", __func__, __LINE__);
    } else {
        LOG_E("[%s] bootinfo block err! Input NORMAL or OTA!", __func__);
        return -1;
    }

    ret = ota_set_bootinfo_to_zoneAB(&boot_info);
    if (ret) {
        LOG_E("error %s %d, ota_set_bootinfo return %d", __func__, __LINE__, ret);
    }

    LOG_I("The device will reboot after 3 seconds");
    ota_seconds_to_reboot_system(3);
    while (1) {
        LOG_I("Waiting for rebooting O_o");
    }

    RESP_OK(handler);
    return 0;
}


// add OTA test command
void add_ota_cmd()
{
    int i, j;
    cmd_tbl_t cmd_list[] = {
        {
            "AT+OTA_UPGMODE_URL",      2,   ota_upgmode_init,
            "AT+OTA_UPGMODE_URL        - ota upgmode test\n"
        },
#if OTA_DEBUG
        {
            "AT+SHOW_BOOTINFO",      2,   do_show_bootinfo,
            "AT+SHOW_BOOTINFO        - AT+SHOW_BOOTINFO=<BOOTINFO_ORIGIN or BOOTINFO_BACKUP>\n"
        },
#endif
        {
            "AT+REBOOT",            1,   do_sys_reboot,
            "AT+REBOOT             - AT+REBOOT\n"
        },
        {
            "AT+SET_BOOTINFO",      2,   do_set_bootinfo,
            "AT+SET_BOOTINFO        - AT+SET_BOOTINFO=<NORMAL or OTA>\n"
        },
    };
    j = sizeof(cmd_list) / sizeof(cmd_tbl_t);
    for (i = 0; i < j; i++) {
        if (0 != console_cmd_add(&cmd_list[i])) {
            AT_PRINT("Add cmd failed\r\n");
        }
    }
}

void ota_upgmode_test(void)
{
    add_ota_cmd();
}

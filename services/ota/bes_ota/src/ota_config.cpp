/**
 * @file ota_config.cpp
 * @author BES AI team
 * @version 0.1
 * @date 2020-06-20
 * 
 * @copyright Copyright (c) 2015-2020 BES Technic.
 * All rights reserved. All unpublished rights reserved.
 * 
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 * 
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 */

/**
 * NOTE: This file is used to initialize the BES OTA related configurations
 * 
 */

/*****************************header include********************************/
#include "ota_dbg.h"
#include "norflash_api.h"
#include "nvrecord_prompt.h"
#include "ota_config.h"
#include "ota_control.h"
#include "nvrecord_combo.h"

#ifdef IBRT
#include "app_ibrt_ota_cmd.h"
#endif

/*********************external function declearation************************/
extern uint32_t __flash_end[];
extern uint32_t __prompt_start[];
extern uint32_t __prompt_end[];
extern uint32_t __combo_start[];
extern uint32_t __combo_end[];
extern uint32_t __ota_upgrade_log_start[];
extern uint32_t __ota_upgrade_log_end[];
extern uint32_t __userdata_start[];
extern uint32_t __userdata_end[];

/************************private macro defination***************************/

/************************private type defination****************************/

/**********************private function declearation************************/

/************************private variable defination************************/
#ifdef COMBO_CUSBIN_IN_FLASH
static CONTENT_INFO_T* combo_info = NULL;

static FLASH_MODULE_INFO_T combo_flash_mapping_table[COMBO_INFO_NUM_MAX] = { \
    {NORFLASH_API_MODULE_ID_OTA,COMBO_SOC_FW,(OTA_FLASH_LOGIC_ADDR+NEW_IMAGE_FLASH_OFFSET),(uint32_t)__flash_end},\
    {NORFLASH_API_MODULE_ID_TEST2,COMBO_MCU_FW,(uint32_t)__combo_start,(uint32_t)__combo_end},\
};
#endif
/****************************function defination****************************/
#if defined(COMBO_CUSBIN_IN_FLASH) || defined(PROMPT_IN_FLASH)
static uint32_t _read_uint32_little_endian_and_increment(const uint8_t **read_ptr)
{
    uint8_t i = 0;
    uint32_t ret = 0;
    for (i = 0; i < 4; i++)
    {
        ret |= (**read_ptr << (8 * i));
        (*read_ptr)++;
    }
    return ret;
}

static void _parse_custom_image(void *addr, CUSTOM_IMAGE_HEADER_T *info)
{
    ASSERT(addr, "invalid file address");
    ASSERT(info, "invalid info pointer");

    const uint8_t *pRead = (const uint8_t *)addr;

    info->mainInfo = _read_uint32_little_endian_and_increment(&pRead);
    LOG_D("main-info:0x%x", info->mainInfo);

    info->version = _read_uint32_little_endian_and_increment(&pRead);
    LOG_D("version:0x%x", info->version);

    info->contentNum = _read_uint32_little_endian_and_increment(&pRead);
    LOG_D("content number:0x%x", info->contentNum);
    ASSERT(CONTENT_INFO_MAX >= info->contentNum, "content number exceed %d", CONTENT_INFO_MAX);

    for (uint8_t i = 0; i < info->contentNum; i++)
    {
        info->info[i].id = _read_uint32_little_endian_and_increment(&pRead);
        LOG_D("content %d id:0x%x", i, info->info[i].id);

        info->info[i].offset = _read_uint32_little_endian_and_increment(&pRead);
        LOG_D("content %d offset:0x%x", i, info->info[i].offset);

        info->info[i].length = _read_uint32_little_endian_and_increment(&pRead);
        LOG_D("content %d len:0x%x", i, info->info[i].length);
    }

    info->crc32 =  _read_uint32_little_endian_and_increment(&pRead);
    LOG_D("crc32:0x%x", info->crc32);
}
#endif

#ifdef PROMPT_IN_FLASH
static void _language_package_start(void)
{
    LOG_D("%s", __func__);

    // TODO: update flash offset
    uint32_t addr = nv_record_prompt_get_package_addr(LANGUAGE_ID_MAX, true, LANGUAGE_PACKAGE_SIZE);
    ota_control_update_flash_offset(addr - (uint32_t)__prompt_start);
}

static void _language_package_reception_done(void)
{
#ifdef IBRT
    uint8_t ret = 1;
    ota_tws_send_handle(OTA_RSP_RESULT, (uint8_t *)&ret, 1, IBRT_OTA_TWS_IMAGE_CRC_CMD);
#else
    ota_control_send_result_response(true);
#endif
}

static void _language_package_apply(uint32_t startAddr, uint32_t imageLen)
{
    LOG_D("%s", __func__);

    /// parse the image header
    PACKAGE_NODE_T info;
    CUSTOM_IMAGE_HEADER_T header;

    _parse_custom_image((void*)startAddr, &header);

    info.startAddr = startAddr;
    info.len = imageLen;
    info.id = header.mainInfo;

    nv_record_prompt_rec_add_new_package((void*)(&info));
}

static void _register_language_package_user(void)
{
    LOG_D("%s", __func__);

    BES_OTA_USER_INFO_T info;
    info.user = BES_OTA_USER_LANGUAGE_PACKAGE;
    info.norflashId = NORFLASH_API_MODULE_ID_TEST1;
    info.startAddr = (uint32_t)__prompt_start;
    info.length = (uint32_t)__prompt_end - (uint32_t)__prompt_start;
    info.start = _language_package_start;
    info.reception_done = _language_package_reception_done;
    info.apply = _language_package_apply;

    ota_control_register_ota_user(info);
}
#endif

static void _firmware_apply(uint32_t startAddr, uint32_t imageLen)
{
    LOG_D("%s", __func__);

    ota_update_info();
    ota_check_and_reboot_to_use_new_image();
}

static void _register_firmware_user(void)
{
    LOG_D("%s", __func__);

    BES_OTA_USER_INFO_T info;
    info.user = BES_OTA_USER_FIRMWARE;
    info.norflashId = NORFLASH_API_MODULE_ID_OTA;
    info.startAddr = OTA_FLASH_LOGIC_ADDR + NEW_IMAGE_FLASH_OFFSET;
    info.length = hal_norflash_get_flash_total_size(HAL_FLASH_ID_0) - NEW_IMAGE_FLASH_OFFSET - 2*4096;
    info.start = NULL;
    info.reception_done = NULL;
    info.apply = _firmware_apply;

    ota_control_register_ota_user(info);
}

static void _register_userdata_user(void)
{
    LOG_D("%s", __func__);

    BES_OTA_USER_INFO_T info;
    info.user = BES_OTA_USER_USERDATA_POOL;
    info.norflashId = NORFLASH_API_MODULE_ID_USERDATA;
    info.startAddr = (uint32_t)__userdata_start;
    info.length = (uint32_t)__userdata_end - (uint32_t)__userdata_start;
    info.start = NULL;
    info.reception_done = NULL;
    info.apply = NULL;

    ota_control_register_ota_user(info);
}

static void _register_bootinfo_user(void)
{
    LOG_D("%s", __func__);

    BES_OTA_USER_INFO_T info;
    info.user = BES_OTA_USER_BOOTUP_INFO;
    info.norflashId = NORFLASH_API_MODULE_ID_BOOTUP_INFO;
    info.startAddr = ota_common_get_boot_info_flash_offset();
    info.length = FLASH_SECTOR_SIZE;
    info.start = NULL;
    info.reception_done = NULL;
    info.apply = NULL;

    ota_control_register_ota_user(info);
}

static void _register_upgradelog_user(void)
{
    LOG_D("%s", __func__);

    BES_OTA_USER_INFO_T info;
    info.user = BES_OTA_USER_UPGRADE_LOG;
    info.norflashId = NORFLASH_API_MODULE_ID_UPGRADE_LOG;
    info.startAddr = (uint32_t)__ota_upgrade_log_start;
    info.length = (uint32_t)__ota_upgrade_log_end - (uint32_t)__ota_upgrade_log_start;
    info.start = NULL;
    info.reception_done = NULL;
    info.apply = NULL;

    ota_control_register_ota_user(info);
}

#ifdef COMBO_CUSBIN_IN_FLASH
static uint32_t _combo_bin_get_addr_by_id(uint32_t id)
{
    uint8_t index = 0;

    //LOG_D("%s:fw id %x", __func__,id);

    for(index = 0; index < COMBO_INFO_NUM_MAX ; index++)
    {
        if(id == combo_flash_mapping_table[index].FwId)
        {
            return combo_flash_mapping_table[index].StartAddr;
        }
    }
    return 0;
}

static uint8_t _combo_bin_package_process(uint8_t evt_type,uint8_t* parm0,uint32_t* parm1)
{
    uint32_t val = 0;
    CUSTOM_IMAGE_HEADER_T header;
    OTA_COMBO_STATUS status = OTA_COMBO_PROCESS_FAILED;
    enum combin_process_evt evt = (enum combin_process_evt)(evt_type);

    //LOG_D("%s:evt %x", __func__,evt);

    switch (evt)
    {
        case COMBIN_PARASE_HEADER_EVT:
            _parse_custom_image((void*)parm0, &header);
            status = OTA_COMBO_PROCESS_PARAM_INVAILD;
            if((header.mainInfo == MAIN_INFO_TYPE_IMAGE_COMBO)&&\
               (header.contentNum <= COMBO_INFO_NUM_MAX))
            {
                nv_record_combo_bin_update_info((void*)(&header),header.crc32);
                combo_info = (CONTENT_INFO_T*)nv_record_combo_bin_info_get_ptr();
                val = _combo_bin_get_addr_by_id(combo_info->id);
                if(val)
                {
                    ota_control_update_flash_offset(val- OTA_FLASH_LOGIC_ADDR);
                    status = OTA_COMBO_PROCESS_SUCCESS;
                }
            }
            break;

        case COMBIN_SET_START_ADDRESS:
            val = _combo_bin_get_addr_by_id(*parm1);
            if(val)
            {
                ota_control_update_flash_offset(val - OTA_FLASH_LOGIC_ADDR);
                status = OTA_COMBO_PROCESS_SUCCESS;
            }
            else
            {
                status  = OTA_COMBO_PROCESS_PARAM_INVAILD;
            }
            break;
        case COMBIN_GET_START_ADDRESS:
            val = nv_record_combo_bin_get_fw_id(*parm0);
            *parm1 = _combo_bin_get_addr_by_id(val);
            break;
        default:
            break;
    }

    return status;
}

static void _combobin_package_apply(uint32_t startAddr, uint32_t imageLen)
{
    LOG_D("%s", __func__);
    nv_record_combo_bin_clear_info();
    ota_update_info();
    ota_check_and_reboot_to_use_new_image();
}

void _register_cusfirmware_user(void)
{
    LOG_D("%s", __func__);
    BES_OTA_USER_INFO_T info;
    info.user = BES_OTA_USER_COMBOFIRMWARE;
    info.norflashId = NORFLASH_API_MODULE_ID_OTA;
    info.startAddr = OTA_FLASH_LOGIC_ADDR + NEW_IMAGE_FLASH_OFFSET;
    info.length = hal_norflash_get_flash_total_size(HAL_FLASH_ID_0) - NEW_IMAGE_FLASH_OFFSET - 2*4096;
    info.start = NULL;
    info.apply = _combobin_package_apply;
    info.process = _combo_bin_package_process;
    info.reception_done = ota_combo_bin_data_check_image_crc;
    ota_control_register_ota_user(info);
    nv_record_combo_bin_rec_init();
}
#endif

void bes_ota_init_handler(void)
{
    /// register main OTA user
    _register_firmware_user();

    /// register user data pool user
    _register_userdata_user();

    /// register bootup info user
    _register_bootinfo_user();

    /// register upgrade log user
    _register_upgradelog_user();

#ifdef PROMPT_IN_FLASH
    _register_language_package_user();
#endif

#ifdef COMBO_CUSBIN_IN_FLASH
    _register_cusfirmware_user();
#endif
}

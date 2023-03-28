/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
 *
 ****************************************************************************/
#include "plat_types.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_chipid.h"
#include "bt_drv.h"
#include "bt_drv_interface.h"
#include "bt_drv_2003_internal.h"
#include "bt_patch_2003_t0.h"
#include "bt_patch_2003_t0_testmode.h"
#include "bt_patch_2003_t1.h"
#include "bt_patch_2003_t2.h"
#include "bt_patch_2006_t0.h"
#include <string.h>

//address remapping
#define BTDRV_PATCH_ROM_ADDRESS_OFFSET      (0xA0000000 - 0x00000000)          //0x00000000 --> 0xA0000000
#define BTDRV_PATCH_RAM_ADDRESS_OFFSET      (0xC0000000 - 0xA0200000)          //0xC0000000 --> 0xA0200000

//instruction requirement
#define BTDRV_PATCH_ARM_ADDR(a)          (a & 0xFFFFFFFE)
#define BTDRV_PATCH_THUMB_INS(a)         ((a & 0xFFFFFFFE) | 0x1)
#define CO_BIT(pos) (1UL<<(pos))


/*
 * clone of controller patch.h
 */
#define BT_PATCH_ENTRY_NUM  (64)

typedef struct
{
    uint32_t  text_start;
    uint32_t  bss_start;
    uint32_t  bss_end;
} patch_info_t;


typedef struct
{
    uint32_t  active;
    uint32_t  source_addr;
    uint32_t  patch_addr;
} patch_entry_t;

typedef struct
{
    uint32_t  copy_section_num;
    uint32_t  memset_section_num;
} patch_sec_num_t;

typedef struct
{
    uint32_t  ram_text_start;
    uint32_t  flash_text_start;
    uint32_t  size;
} patch_text_t;

typedef struct
{
    uint32_t  bss_start;
    uint32_t  size;
} patch_bss_t;

/*
 * Generate B instrcution:jump from ROM origin func to RAM patch func
 */
uint32_t func_b_jump_ins(uint32_t old_func, uint32_t new_func)
{
    union
    {
        uint32_t d32;
        uint16_t d16[2];
    } ins;
    uint32_t immd;
    uint8_t j1, j2, s;

    ins.d32 = 0x9000F000;
    immd = (new_func & ~1) - ((old_func + 4) & ~1);
    s = immd >> 31;
    j1 = s ^ !((immd >> 23) & 1);
    j2 = s ^ !((immd >> 22) & 1);
    ins.d16[0] |= (s << 10) | ((immd >> 12) & 0x3FF);
    ins.d16[1] |= (j1 << 13) | (j2 << 11) | ((immd >> 1) & 0x7FF);

    return ins.d32;
}

void btdrv_function_patch_init_common(uint32_t * patch, uint32_t patch_size)
{
    uint8_t *bt_patch_data = NULL;

    patch_sec_num_t *patch_info_ptr = NULL;
    patch_entry_t *patch_table = NULL;
    patch_entry_t *patch_entry_ptr = NULL;
    uint32_t i = 0;
    uint32_t rom_addr;
    uint32_t jmp_ins;
    patch_text_t *patch_text_ptr;
    patch_bss_t *patch_bss_ptr;
    uint32_t size;

    bt_patch_data = (uint8_t *)patch;

    patch_table = (patch_entry_t *)bt_patch_data;

    patch_info_ptr = (patch_sec_num_t *)((uint32_t *)bt_patch_data + sizeof(patch_entry_t) / sizeof(uint32_t) * BT_PATCH_ENTRY_NUM);

    patch_text_ptr = (patch_text_t *)((uint32_t *)patch_info_ptr + sizeof(patch_sec_num_t) / sizeof(uint32_t));

    for (i = 0; i <  patch_info_ptr->copy_section_num; i ++)
    {
        if(CO_BIT(31) & patch_text_ptr->size)
        {
            size = patch_text_ptr->size & ~CO_BIT(31);
            if(size > sizeof(uint32_t) || size == 3)
            {
                 ASSERT(0, "%s: error size,0x%x,0x%x,0x%x",
                        __func__, patch_text_ptr->ram_text_start, patch_text_ptr->flash_text_start, patch_text_ptr->size);
            }
            else
            {
                //BT_DRV_TRACE(3,"patch copy dest start addr=0x%x, src data=0x%x size=0x%x",patch_text_ptr->ram_text_start, patch_text_ptr->flash_text_start, patch_text_ptr->size);
                memcpy((uint8_t *)patch_text_ptr->ram_text_start, &patch_text_ptr->flash_text_start, size);
            }
        }
        else
        {
            //BT_DRV_TRACE(3,"patch copy dest start addr=0x%x, src addr=0x%x size=0x%x",patch_text_ptr->ram_text_start, patch_text_ptr->flash_text_start, patch_text_ptr->size);
            btdrv_memcpy_word((uint8_t *)patch_text_ptr->ram_text_start, bt_patch_data + patch_text_ptr->flash_text_start, patch_text_ptr->size);
        }
        patch_text_ptr += 1;
    }

    patch_bss_ptr = (patch_bss_t *)patch_text_ptr;

    for (i = 0; i <  patch_info_ptr->memset_section_num; i ++)
    {
        //BT_DRV_TRACE(2,"patch bss start addr=0x%x,size=0x%x",patch_bss_ptr->bss_start,patch_bss_ptr->size);
        memset((uint8_t *)patch_bss_ptr->bss_start, 0x00, patch_bss_ptr->size);

        patch_bss_ptr += 1;
    }

    //set patch entry
    for (i = 0; i < BT_PATCH_ENTRY_NUM; i++)
    {
        patch_entry_ptr = (patch_table + i);

        if (patch_entry_ptr->active == 1)
        {
            rom_addr = BTDRV_PATCH_THUMB_INS(patch_entry_ptr->source_addr) - BTDRV_PATCH_ROM_ADDRESS_OFFSET;
            BT_PATCH_WR(BTDRV_PATCH_INS_COMP_ADDR_START + i*sizeof(uint32_t), rom_addr);
            jmp_ins = func_b_jump_ins(BTDRV_PATCH_ARM_ADDR(patch_entry_ptr->source_addr),BTDRV_PATCH_ARM_ADDR(patch_entry_ptr->patch_addr) - BTDRV_PATCH_RAM_ADDRESS_OFFSET);
            BT_PATCH_WR(BTDRV_PATCH_INS_REMAP_ADDR_START + i*sizeof(uint32_t), jmp_ins);

            /*
            BT_DRV_TRACE(3,"patch idx=%d actived,rom origin addr(t)=0x%x,rom mapping addr(r)=0x%x, \n",i,rom_addr,
                         BTDRV_PATCH_ARM_ADDR(patch_entry_ptr->source_addr));
            BT_DRV_TRACE(3,"patch origin addr(r)=%x,patch mapping addr(r)=0x%x,jmp ins=0x%x \n",BTDRV_PATCH_ARM_ADDR(patch_entry_ptr->patch_addr),
                         BTDRV_PATCH_ARM_ADDR(patch_entry_ptr->patch_addr) - BTDRV_PATCH_RAM_ADDRESS_OFFSET,jmp_ins);
           */
        }
        else if(patch_entry_ptr->active == 2)
        {
            BT_PATCH_WR(BTDRV_PATCH_INS_REMAP_ADDR_START + i*sizeof(uint32_t), patch_entry_ptr->patch_addr);
            rom_addr = BTDRV_PATCH_THUMB_INS(patch_entry_ptr->source_addr) - BTDRV_PATCH_ROM_ADDRESS_OFFSET;
            BT_PATCH_WR(BTDRV_PATCH_INS_COMP_ADDR_START + i*sizeof(uint32_t), rom_addr);

            //BT_DRV_TRACE(3,"patch idx=%d actived,rom origin addr(t)=0x%x, substitute instruction=0x%x, \n",i,rom_addr,
           //              patch_entry_ptr->patch_addr);
        }
    }

}

/*
 * function patch entry init
 */
void btdrv_function_patch_init(void)
{
    enum HAL_CHIP_METAL_ID_T metal_id = hal_get_chip_metal_id();
    if (metal_id < HAL_CHIP_METAL_ID_6)
    {
        btdrv_function_patch_init_common((uint32_t *)bt_patch_2003_t0,sizeof(bt_patch_2003_t0));
        BT_DRV_TRACE(1,"BTC:2003 t0 work mode patch version:%08x",*(uint32_t *)(WORKMODE_PATCH_VERSION_T0_ADDR));
    }
    else if (metal_id == HAL_CHIP_METAL_ID_6)
    {
        btdrv_function_patch_init_common((uint32_t *)bt_patch_2003_t1,sizeof(bt_patch_2003_t1));
        BT_DRV_TRACE(1,"BTC:2003 t1 work mode patch version:%08x",*(uint32_t *)(WORKMODE_PATCH_VERSION_T1_ADDR));
    }
    else if(metal_id == HAL_CHIP_METAL_ID_7)
    {
        btdrv_function_patch_init_common((uint32_t *)bt_patch_2003_t2,sizeof(bt_patch_2003_t2));
        BT_DRV_TRACE(1,"BTC:2003 t2 work mode patch version:%08x",*(uint32_t *)(WORKMODE_PATCH_VERSION_T2_ADDR));
    }
    else
    {
        btdrv_function_patch_init_common((uint32_t *)bt_patch_2006_t0,sizeof(bt_patch_2006_t0));
        BT_DRV_TRACE(1,"BTC:2006 t0 work mode patch version:%08x",*(uint32_t *)(WORKMODE_PATCH_VERSION_2006_T0_ADDR));
    }
    //else
    //{
    //    ASSERT(0, "%s current metal id=%d, only support ramrun", __func__, metal_id);
    //}
}

/*
 * function testmode patch entry init
 */
void btdrv_function_testmode_patch_init(void)
{
    enum HAL_CHIP_METAL_ID_T metal_id = hal_get_chip_metal_id();
    if (metal_id < HAL_CHIP_METAL_ID_6)
    {
        btdrv_set_pacth_filter_testmode(&bes2003_t0_filter_tbl_testmode);
        btdrv_function_patch_init_common((uint32_t *)bt_patch_2003_t0_testmode,sizeof(bt_patch_2003_t0_testmode));
        btdrv_patch_usr_param_init_testmode();
        BT_DRV_TRACE(1,"BTC:2003 test mode patch version:%08x",*(uint32_t *)(TESTMODE_PATCH_VERSION_T0_TESTMODE_ADDR));
    }
    else
    {
        ASSERT(0, "%s:error metal id=%d", __func__, metal_id);
    }
}

void btdrv_patch_usr_param_init(void)
{
}

void btdrv_patch_usr_param_init_testmode(void)
{

}


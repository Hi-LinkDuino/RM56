/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "string.h"

#define INS_PATCH_MAX_NUM  (64)

#define BTDRV_PATCH_ACT     0x1
#define BTDRV_PATCH_INACT   0x0

typedef struct
{
    uint8_t patch_index;            //patch position
    uint8_t patch_state;            //is patch active
    uint16_t patch_length;          //patch length 0:one instrution replace  other:jump to ram to run more instruction
    uint32_t patch_remap_address;   //patch occured address
    uint32_t patch_remap_value;     //patch replaced instuction
    uint32_t patch_start_address;   //ram patch address for lenth>0
    uint8_t *patch_data;            //ram patch date for length >0

} BTDRV_PATCH_STRUCT;


//filter none patch
BEST_FILTER_PATCH_TBL_T bes2003_t0_filter_tbl =
{
    .u.filter_tbl[0] = 0xffffffff,
    .u.filter_tbl[1] = 0xffffffff,
};

//set patch entry filter
void btdrv_set_pacth_filter(BEST_FILTER_PATCH_TBL_T* p_filter_tbl)
{

}

bool btdrv_patch_is_filter(uint32_t idx)
{
    return !!(bes2003_t0_filter_tbl.u.filter_tbl[idx/32] & (1<<(idx%32) ));
}


//filter none patch in testmode
BEST_FILTER_PATCH_TBL_T bes2003_t0_filter_tbl_testmode =
{
    .u.filter_tbl[0] = 0xffffffff,
    .u.filter_tbl[1] = 0xffffffff,
};

void btdrv_set_pacth_filter_testmode(BEST_FILTER_PATCH_TBL_T* p_filter_tbl)
{

}

bool btdrv_patch_is_filter_testmode(uint32_t idx)
{
    return !!(bes2003_t0_filter_tbl_testmode.u.filter_tbl[idx/32] & (1<<(idx%32) ));
}

/*****************************************************************************
 Prototype    : btdrv_ins_patch_write
 Description  : bt driver instruction patch write
 Input        : BTDRV_PATCH_STRUCT *ins_patch_p
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History      :
 Date         : 2018/12/6
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/
void btdrv_ins_patch_write(BTDRV_PATCH_STRUCT *ins_patch_p)
{
    BT_PATCH_WR(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*sizeof(uint32_t), ins_patch_p->patch_remap_value);
    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
        memcpy((uint8_t *)ins_patch_p->patch_start_address,ins_patch_p->patch_data,ins_patch_p->patch_length);
    }

    uint32_t remap_addr =  ins_patch_p->patch_remap_address | 1;
    BT_PATCH_WR(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*sizeof(uint32_t), remap_addr);
}
/*****************************************************************************
 Prototype    : btdrv instruction patch init, a

 Description  : btdrv_ins_patch_init
 Input        : void
 Output       : None
 Return Value :
 Calls        :
 Called By    :

 History        :
 Date         : 2018/12/6
 Author       : bestechnic
 Modification : Created function

*****************************************************************************/

extern void btdrv_function_patch_init(void);

void btdrv_ins_patch_init(void)
{
    btdrv_function_patch_init();
}

//////////////////////////////patch enable////////////////////////

void btdrv_patch_en(uint8_t en)
{
    if(en)
    {
        *(volatile uint32_t *)BTDRV_PATCH_EN_REG |= 0x80000000;
    }
    else
    {
        *(volatile uint32_t *)BTDRV_PATCH_EN_REG &= ~0x80000000;
    }
}

void btdrv_ins_patch_disable(uint8_t index)
{
    //disable patch, clear remap address and jump instruction
    BT_PATCH_WR(BTDRV_PATCH_INS_COMP_ADDR_START+index*sizeof(uint32_t), 0);
    BT_PATCH_WR(BTDRV_PATCH_INS_REMAP_ADDR_START + index*sizeof(uint32_t), 0);
}

extern void btdrv_function_testmode_patch_init(void);
extern void btdrv_config_init(void);

void btdrv_ins_patch_test_init(void)
{
#ifndef __BT_RAMRUN__
    g_metal_id = hal_get_chip_metal_id();

    if(g_metal_id < HAL_CHIP_METAL_ID_6)
    {
        btdrv_patch_en(0);

        for(uint8_t i=0; i< INS_PATCH_MAX_NUM; i++)
        {
            btdrv_ins_patch_disable(i);
        }
        bt_drv_reg_op_for_test_mode_disable();

        btdrv_function_testmode_patch_init();

        btdrv_patch_en(1);
    }
#endif
}

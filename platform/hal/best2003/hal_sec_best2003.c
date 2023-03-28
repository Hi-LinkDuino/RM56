/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "plat_addr_map.h"
#include "hal_chipid.h"

#include "hal_sec.h"

#include CHIP_SPECIFIC_HDR(reg_sec)
#include CHIP_SPECIFIC_HDR(reg_mpc_spy)

//these sec regs can only be accessed in security state
static struct HAL_SEC_T *const sec = (struct HAL_SEC_T *)TRUST_ZONE_BASE;

void hal_sec_set_gdma_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= (SEC_CFG_NONSEC_GDMA1 | SEC_CFG_NONSEC_GDMA2);
    else
        sec->REG_00C &= ~(SEC_CFG_NONSEC_GDMA1 | SEC_CFG_NONSEC_GDMA2);
}

void hal_sec_set_adma_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= (SEC_CFG_NONSEC_ADMA1 | SEC_CFG_NONSEC_ADMA2);
    else
        sec->REG_00C &= ~(SEC_CFG_NONSEC_ADMA1 | SEC_CFG_NONSEC_ADMA2);
}

void hal_sec_set_bcm_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_BCM;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_BCM;
}

void hal_sec_set_cksum_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_CKSUM;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_CKSUM;
}

void hal_sec_set_crc_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_CRC;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_CRC;
}

void hal_sec_set_usb_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_USB;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_USB;
}

void hal_sec_set_i2c_slv_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_I2C_SLV;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_I2C_SLV;
}

void hal_sec_set_bt2mcu_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_BT2MCU;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_BT2MCU;
}

void hal_sec_set_wf2mcu_nonsec(bool nonsec)
{
    if (nonsec)
        sec->REG_00C |= SEC_CFG_NONSEC_WF2MCU;
    else
        sec->REG_00C &= ~SEC_CFG_NONSEC_WF2MCU;
}

void hal_mpc_spy_nonsec_bypass(uint32_t addr, bool bypass)
{
    struct HAL_MPC_SPY_T *base = (struct HAL_MPC_SPY_T *)addr;
    if (bypass)
        base->REG_000 |= MPC_SPY_MPC_CFG_NONSEC_BYPASS;
    else
        base->REG_000 &= ~MPC_SPY_MPC_CFG_NONSEC_BYPASS;
}

void hal_sec_set_flashctrl_nonsec(bool nonsec)
{
    sec->REG_010 &= ~(1 << (4 + SEC_CFG_NONSEC_BYPASS_PROT_AHB1_SHIFT));
    if (nonsec) {
        sec->REG_010 |= (1<<4);
    } else {
        sec->REG_010 &= ~(1<<4);
    }
}

static void hal_page_spy_init()
{
    uint32_t *vector_table = NVIC_GetVectorTab();
    NVIC_SetVector(PAGESPY_IRQn, (uint32_t)vector_table[3]); //HardFault Handler
    NVIC_SetPriority(PAGESPY_IRQn, IRQ_PRIORITY_REALTIME);
    NVIC_ClearPendingIRQ(PAGESPY_IRQn);
    NVIC_EnableIRQ(PAGESPY_IRQn);
}

void hal_page_spy_set(uint32_t spy_base, uint32_t start_addr, uint32_t len, bool write, bool read)
{
    struct HAL_MPC_SPY_T *base = (struct HAL_MPC_SPY_T *)spy_base;

    hal_page_spy_init();

    base->REG_008 = MPC_SPY_REG_STR_ADDR0(start_addr);
    base->REG_00C = MPC_SPY_REG_STR_ADDR0(start_addr + len - 1);

    base->REG_004 = MPC_SPY_REG_ENABLE0 | (read?MPC_SPY_REG_DETECT_READ0:0) | (write?MPC_SPY_REG_DETECT_WRITE0:0);
}
void hal_sec_init()
{
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4) {
#ifdef ARM_CMSE
        //dr mode, dr non sec
        sec->REG_00C &= ~(SEC_CFG_NONSEC_A7_AUTO | SEC_CFG_NONSEC_A7); //a7 non sec
        sec->REG_00C &= ~(SEC_CFG_NONSEC_CP_AUTO | SEC_CFG_NONSEC_CP); //cp non sec
#else
        //audo mode
        sec->REG_00C |= (SEC_CFG_NONSEC_CP_AUTO | SEC_CFG_NONSEC_A7_AUTO);
#endif
    }
}

void hal_sec_idau_enable(bool enable)
{
    if (enable) {
        sec->REG_034 |= SEC_IDAUEN_CORE0;
    } else {
        sec->REG_034 &= ~SEC_IDAUEN_CORE0;
    }
}

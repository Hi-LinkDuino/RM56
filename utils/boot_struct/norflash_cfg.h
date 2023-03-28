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
#ifndef __NORFLASH_CFG_H__
#define __NORFLASH_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

//------------------
// Used in ROM efuse
//------------------
enum SEC_REG_BASE_T {
    SEC_REG_BASE_0X1000,
    SEC_REG_BASE_0X0000,
    SEC_REG_BASE_0X2000,
};

//------------------
// Used in ROM efuse
//------------------
enum SEC_REG_SIZE_T {
    SEC_REG_SIZE_1024,
    SEC_REG_SIZE_512,
    SEC_REG_SIZE_256,
    SEC_REG_SIZE_4096,
};

//------------------
// Used in ROM efuse
//------------------
enum SEC_REG_OFFSET_T {
    SEC_REG_OFFSET_0X1000,
    SEC_REG_OFFSET_0X0100,
};

enum SEC_REG_CNT_T {
    SEC_REG_CNT_3,
    SEC_REG_CNT_4,
    SEC_REG_CNT_2,
    SEC_REG_CNT_1,
};

enum SEC_REG_PP_T {
    SEC_REG_PP_256,
    SEC_REG_PP_1024,
};

enum SEC_REG_LB_T {
    SEC_REG_LB_S11_S13,
    SEC_REG_LB_S10,
    SEC_REG_LB_S10_S13,
    SEC_REG_LB_S12_S13,
};

enum NORFLASH_CFG_MOD_CLK_T {
    NORFLASH_CFG_MOD_CLK_32K,
    NORFLASH_CFG_MOD_CLK_26M,
    NORFLASH_CFG_MOD_CLK_52M,
    NORFLASH_CFG_MOD_CLK_78M,
    NORFLASH_CFG_MOD_CLK_104M,
    NORFLASH_CFG_MOD_CLK_208M,

    NORFLASH_CFG_MOD_CLK_QTY
};

struct norflash_cfg_struct_t {
    uint8_t neg_phase:1;
    uint8_t pos_neg:1;
    uint8_t reserved_2:1;
    uint8_t samdly:3;

    uint8_t div; /* least 2 */

    uint8_t dualmode:1;
    uint8_t holdpin:1;
    uint8_t wprpin:1;
    uint8_t quadmode:1;
    uint8_t mod_clk:4;

    uint8_t spiruen:3;
    uint8_t spirden:3;

    uint8_t dualiocmd;
    uint8_t rdcmd;
    uint8_t frdcmd;
    uint8_t qrdcmd; /* quad io cmd */
};

#ifdef __cplusplus
}
#endif

#endif

